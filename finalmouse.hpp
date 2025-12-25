#pragma once

#define _HIDAPI_STATIC
#include "hidapi.h"

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cctype>
#include <cwchar>
#include <cstdarg>
#include <cstdio>

using std::string;

struct AppState {
    bool silent = false;
    bool verbose = true;

    void log(const char* fmt, ...) const {
        if (silent) return;
        va_list ap;
        va_start(ap, fmt);
        std::vprintf(fmt, ap);
        std::printf("\n");
        std::fflush(stdout);
        va_end(ap);
    }
};

struct DeviceInfo {
    const hid_device_info* raw = nullptr;
    string path;
    unsigned vid = 0, pid = 0;
    int iface = -1;
    unsigned usage_page = 0, usage = 0;
    string manufacturer, product, serial;
};

// Convert wchar_t* (HIDAPI) to UTF-8 string
inline string hid_string_to_utf8(const wchar_t* ws) {
    if (!ws) return {};
    size_t len = std::wcslen(ws);
    if (len == 0) return {};
    std::vector<char> buf(len * 4 + 1, 0);
    std::wcstombs(buf.data(), ws, buf.size());
    return string(buf.data());
}

inline bool icontains(std::string_view h, std::string_view n) {
    auto low = [](unsigned char c){ return char(std::tolower(c)); };
    string H(h.size(), 0), N(n.size(), 0);
    std::transform(h.begin(), h.end(), H.begin(), low);
    std::transform(n.begin(), n.end(), N.begin(), low);
    return H.find(N) != string::npos;
}

struct HidCloser {
    void operator()(hid_device* d) const noexcept {
        if (d) hid_close(d);
    }
};
using HidHandle = std::unique_ptr<hid_device, HidCloser>;

class HidSession {
public:
    explicit HidSession(const AppState& s) : app(s) { hid_init(); }
    ~HidSession() { hid_exit(); }

    std::vector<DeviceInfo> enumerate_all() const {
        std::vector<DeviceInfo> v;
        if (auto* head = hid_enumerate(0,0)) {
            for (auto* d=head; d; d=d->next){
                DeviceInfo x;
                x.raw = d;
                x.path = d->path ? d->path : "";
                x.vid = d->vendor_id;
                x.pid = d->product_id;
                x.iface = d->interface_number;
                x.usage_page = d->usage_page;
                x.usage = d->usage;
                x.manufacturer = hid_string_to_utf8(d->manufacturer_string);
                x.product = hid_string_to_utf8(d->product_string);
                x.serial = hid_string_to_utf8(d->serial_number);
                v.push_back(std::move(x));
            }
            hid_free_enumeration(head);
        }
        return v;
    }

    static HidHandle open_path(const char* p) {
        return HidHandle{hid_open_path(p)};
    }

    int send_report_any(hid_device* dev, uint8_t a, uint8_t b) const {
        unsigned char r[65] = {0};
        r[0] = 0; // report ID for Linux
        r[1] = 0x04;
        r[2] = 0x04;
        r[3] = 0x91;
        r[4] = 0x02;
        r[5] = a;
        r[6] = b;

        int w = hid_write(dev, r, sizeof(r));
        if (app.verbose) app.log("hid_write -> %d", w);
        return w;
    }

private:
    const AppState& app;
};
