#include "finalmouse.hpp"
#include <cstdlib>
#include <optional>
#include <string_view>
#include <charconv>
#include <iostream>

struct Args {
    std::optional<unsigned> hz;
    std::optional<unsigned> vid;
    std::optional<unsigned> pid;
    std::optional<std::string> path;
    bool list = false;
    bool help = false;
    bool silent = false;
    bool verbose = true;
};

static bool parse_hex16(std::string_view s, unsigned &out){
    if (s.size() > 2 && s[0]=='0' && (s[1]=='x'||s[1]=='X')) s.remove_prefix(2);
    unsigned v=0;
    auto res = std::from_chars(s.data(), s.data()+s.size(), v, 16);
    if (res.ec != std::errc{} || v>0xFFFF) return false;
    out = v;
    return true;
}

static std::optional<Args> parse_args(int argc, char** argv){
    Args a{};
    for(int i=1;i<argc;++i){
        std::string_view t=argv[i];
        if(t=="--help"||t=="-h") a.help=true;
        else if(t=="--list") a.list=true;
        else if(t=="--silent"||t=="-s") a.silent=true;
        else if(t=="--hz"&&i+1<argc) a.hz=std::strtoul(argv[++i],nullptr,10);
        else if(t=="--vid"&&i+1<argc){ unsigned v=0; if(!parse_hex16(argv[++i],v)) return {}; a.vid=v; }
        else if(t=="--pid"&&i+1<argc){ unsigned p=0; if(!parse_hex16(argv[++i],p)) return {}; a.pid=p; }
        else if(t=="--path"&&i+1<argc) a.path=argv[++i];
        else return {};
    }
    if(!(a.help||a.list||a.hz)) return {};
    return a;
}

struct HzParams { uint8_t a,b; };
static std::optional<HzParams> map_hz(unsigned hz){
    switch(hz){
        case 500: return HzParams{244,1};
        case 1000: return HzParams{232,3};
        case 2000: return HzParams{208,7};
        case 4000: return HzParams{160,15};
        case 8000: return HzParams{64,31};
        default: return {};
    }
}

static void dump(const AppState &app, const DeviceInfo &d){
    app.log("path: %s", d.path.c_str());
    app.log("vid: 0x%04X pid: 0x%04X iface: %d", d.vid,d.pid,d.iface);
    app.log("manufacturer: %s", d.manufacturer.c_str());
    app.log("product: %s", d.product.c_str());
    app.log("serial: %s", d.serial.c_str());
}

static std::vector<DeviceInfo> pick_candidates(const std::vector<DeviceInfo> &all, const std::optional<unsigned> &vid,
                                               const std::optional<unsigned> &pid, const std::optional<std::string> &path){
    if(path){
        auto it = std::find_if(all.begin(), all.end(), [&](const DeviceInfo &d){ return d.path==*path; });
        return it!=all.end()?std::vector<DeviceInfo>{*it}:std::vector<DeviceInfo>{};
    }
    if(vid && pid){
        std::vector<DeviceInfo> v;
        for(auto &d:all) if(d.vid==*vid && d.pid==*pid) v.push_back(d);
        return v;
    }
    std::vector<DeviceInfo> v;
    for(auto &d:all) if(icontains(d.manufacturer,"finalmouse")||icontains(d.product,"finalmouse")) v.push_back(d);
    return v;
}

static void usage(const AppState &app){
    app.log("Usage:");
    app.log("  --help | -h");
    app.log("  --list");
    app.log("  --hz 500|1000|2000|4000|8000 [--vid 0xVVVV --pid 0xPPPP]");
    app.log("  --hz HZ --path \"<hid path>\"");
    app.log("  --silent | -s");
}

static int run_main(int argc, char** argv){
    auto parsed=parse_args(argc,argv);
    if(!parsed){
        AppState tmp;
        usage(tmp);
        return 1;
    }

    Args args=*parsed;
    AppState app;
    app.silent=args.silent;
    app.verbose=args.verbose;

    if(args.help){ usage(app); return 0; }

    HidSession hid(app);
    auto all=hid.enumerate_all();

    if(args.list){
        int idx=0;
        for(auto &d:all){
            app.log("[%d]", idx++);
            dump(app,d);
        }
        return 0;
    }

    if(!args.hz){ app.log("Missing --hz"); usage(app); return 3; }
    auto hp=map_hz(*args.hz);
    if(!hp){ app.log("Unsupported Hz"); return 4; }

    auto candidates=pick_candidates(all,args.vid,args.pid,args.path);
    if(candidates.empty()){ app.log("No candidate HID device."); return 5; }

    int successes=0;
    for(auto &d:candidates){
        app.log("Opening: %s", d.path.c_str());
        auto h=HidSession::open_path(d.path.c_str());
        if(!h){ app.log("open failed (busy?)"); continue; }
        int r=hid.send_report_any(h.get(),hp->a,hp->b);
        if(r>=0){ app.log("Success %d bytes on iface %d",r,d.iface); successes++; break; }
        else app.log("Failed to send report");
    }

    if(successes==0){ app.log("No interface accepted the report."); return 6; }
    app.log("Done. Requested %u Hz.",*args.hz);
    return 0;
}

int main(int argc,char** argv){
    return run_main(argc,argv);
}
