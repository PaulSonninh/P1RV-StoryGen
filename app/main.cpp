#include <windows.h>

#include <array>
#include <cstdio>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// ---------- Helpers: UTF-16 -> UTF-8 (for cmd.exe strings) ----------
static std::string w2u8(const std::wstring& ws) {
    if (ws.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(),
        nullptr, 0, nullptr, nullptr);
    std::string out(size_needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(),
        out.data(), size_needed, nullptr, nullptr);
    return out;
}

static std::string path_u8(const fs::path& p) {
    return w2u8(p.wstring());
}

static std::string quote_u8(const fs::path& p) {
    return "\"" + path_u8(p) + "\"";
}

// ---------- Helpers: run command and capture stdout+stderr ----------
static std::string run_cmd(const std::string& cmd) {
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "";
    std::array<char, 4096> buffer{};
    std::string result;
    while (fgets(buffer.data(), (int)buffer.size(), pipe)) {
        result += buffer.data();
    }
    _pclose(pipe);
    return result;
}

// ---------- Find exe dir ----------
static fs::path get_exe_dir() {
    std::wstring buf(32768, L'\0');
    DWORD len = GetModuleFileNameW(nullptr, buf.data(), (DWORD)buf.size());
    if (len == 0 || len >= buf.size()) {
        throw std::runtime_error("GetModuleFileNameW failed.");
    }
    buf.resize(len);
    return fs::path(buf).parent_path();
}

// ---------- Find repo root by climbing up until we see llama.cpp/ and models/ ----------
static fs::path find_repo_root(const fs::path& start, int max_up = 8) {
    fs::path cur = start;
    for (int i = 0; i <= max_up; ++i) {
        if (fs::exists(cur / "llama.cpp") && fs::exists(cur / "models")) {
            return cur;
        }
        if (!cur.has_parent_path()) break;
        fs::path parent = cur.parent_path();
        if (parent == cur) break;
        cur = parent;
    }
    // fallback: current working dir
    return fs::current_path();
}

// ---------- CLI Options ----------
struct Options {
    fs::path root;          // repo root
    fs::path llama_cli;     // llama-cli.exe path
    fs::path model;         // gguf path
    fs::path prompt;        // prompt file path
    fs::path out_file;      // optional output file
    int seed = 42;
    int n_predict = 200;
    float temp = 0.7f;
    float top_p = 0.9f;
    bool verbose = false;
    bool single_turn = true;      // default: exit after one turn
    bool no_conversation = false; // optional advanced toggle
};

static void print_help() {
    std::cout <<
        "Usage: storygen.exe [options]\n\n"
        "Core options:\n"
        "  -h, --help                 Show this help\n"
        "  -m, --model <path>         Model GGUF path (default: models/mistral-7b-instruct-v0.2.Q4_0.gguf)\n"
        "  -f, --prompt <path>        Prompt file path (default: prompt.txt)\n"
        "  -n, --n-predict <int>      Tokens to generate (default: 200)\n"
        "  -s, --seed <int>           Seed (default: 42)\n"
        "  -t, --temp <float>         Temperature (default: 0.7)\n"
        "  -p, --top-p <float>        Top-p (default: 0.9)\n"
        "  --out <path>               Write output to a file (optional)\n\n"
        "Execution mode:\n"
        "  --single-turn              Run one user turn then exit (default: ON)\n"
        "  --multi-turn               Allow interactive/multi-turn (may wait for input)\n"
        "  --no-conversation           Force disable conversation mode (advanced)\n\n"
        "Diagnostics:\n"
        "  --verbose                  Print resolved paths + full command\n\n"
        "Examples:\n"
        "  storygen.exe --verbose\n"
        "  storygen.exe -n 300 -t 0.9 -p 0.95\n"
        "  storygen.exe --out out.txt\n";
}

static fs::path resolve_from_root(const fs::path& p, const fs::path& root) {
    if (p.empty()) return p;
    if (p.is_absolute()) return p;
    return root / p;
}

static void validate(const Options& opt) {
    if (opt.n_predict <= 0) throw std::runtime_error("n-predict must be > 0");
    if (!(opt.top_p > 0.0f && opt.top_p <= 1.0f)) throw std::runtime_error("top-p must be in (0, 1]");
    if (!(opt.temp > 0.0f && opt.temp <= 5.0f)) throw std::runtime_error("temp must be > 0 (typical: 0.1..2.0)");
}

static void parse_args(int argc, wchar_t* argv[], Options& opt) {
    for (int i = 1; i < argc; ++i) {
        std::wstring a = argv[i];

        auto need = [&](const wchar_t* name) -> std::wstring {
            if (i + 1 >= argc) {
                std::wstring msg = L"Missing value after option: ";
                msg += name;
                throw std::runtime_error(w2u8(msg));
            }
            return argv[++i];
            };

        if (a == L"-h" || a == L"--help") {
            print_help();
            std::exit(0);
        }
        else if (a == L"-m" || a == L"--model") {
            opt.model = fs::path(need(L"--model"));
        }
        else if (a == L"-f" || a == L"--prompt") {
            opt.prompt = fs::path(need(L"--prompt"));
        }
        else if (a == L"-n" || a == L"--n-predict") {
            opt.n_predict = std::stoi(need(L"--n-predict"));
        }
        else if (a == L"-s" || a == L"--seed") {
            opt.seed = std::stoi(need(L"--seed"));
        }
        else if (a == L"-t" || a == L"--temp") {
            opt.temp = std::stof(need(L"--temp"));
        }
        else if (a == L"-p" || a == L"--top-p") {
            opt.top_p = std::stof(need(L"--top-p"));
        }
        else if (a == L"--out") {
            opt.out_file = fs::path(need(L"--out"));
        }
        else if (a == L"--verbose") {
            opt.verbose = true;
        }
        else if (a == L"--single-turn") {
            opt.single_turn = true;
        }
        else if (a == L"--multi-turn") {
            opt.single_turn = false;
        }
        else if (a == L"--no-conversation") {
            opt.no_conversation = true;
        }
        else {
            std::wcerr << L"Unknown option: " << a << L"\n";
            std::cout << "Use --help\n";
            std::exit(2);
        }
    }
}

int wmain(int argc, wchar_t* argv[]) {
    try {
        Options opt;

        // 1) Determine root robustly
        const fs::path exe_dir = get_exe_dir();
        opt.root = find_repo_root(exe_dir);

        // 2) Defaults relative to root
        opt.llama_cli = opt.root / "llama.cpp" / "build" / "bin" / "Release" / "llama-cli.exe";
        opt.model = fs::path("models") / "mistral-7b-instruct-v0.2.Q4_0.gguf";
        opt.prompt = fs::path("prompt.txt");

        // 3) Parse CLI
        parse_args(argc, argv, opt);

        // 4) Resolve relative paths from root
        opt.llama_cli = resolve_from_root(opt.llama_cli, opt.root);
        opt.model = resolve_from_root(opt.model, opt.root);
        opt.prompt = resolve_from_root(opt.prompt, opt.root);
        if (!opt.out_file.empty()) opt.out_file = resolve_from_root(opt.out_file, opt.root);

        validate(opt);

        // 5) Existence checks
        if (!fs::exists(opt.llama_cli)) throw std::runtime_error("ERREUR: llama-cli introuvable: " + path_u8(opt.llama_cli));
        if (!fs::exists(opt.model))     throw std::runtime_error("ERREUR: modele GGUF introuvable: " + path_u8(opt.model));
        if (!fs::exists(opt.prompt))    throw std::runtime_error("ERREUR: prompt introuvable: " + path_u8(opt.prompt));

        // 6) Build command
        std::ostringstream ftemp, ftop;
        ftemp << std::fixed << std::setprecision(3) << opt.temp;
        ftop << std::fixed << std::setprecision(3) << opt.top_p;

        std::string inner =
            quote_u8(opt.llama_cli) + " " +
            "-m " + quote_u8(opt.model) + " " +
            "-f " + quote_u8(opt.prompt) + " " +
            "-n " + std::to_string(opt.n_predict) + " " +
            "--seed " + std::to_string(opt.seed) + " " +
            "--temp " + ftemp.str() + " " +
            "--top-p " + ftop.str() + " ";

        // Key: exit cleanly after one turn (prevents hanging in interactive chat)
        if (opt.single_turn) inner += "--single-turn ";
        if (opt.no_conversation) inner += "--no-conversation ";

        // Use cmd.exe so we can redirect stderr easily (+ chcp 65001 for UTF-8)
        std::string cmd;
        if (opt.out_file.empty()) {
            cmd = "cmd.exe /S /C \"chcp 65001 >NUL && " + inner + " 2>&1\"";
        }
        else {
            cmd = "cmd.exe /S /C \"chcp 65001 >NUL && " + inner +
                " > " + quote_u8(opt.out_file) + " 2>&1\"";
        }

        if (opt.verbose) {
            std::cout << "[root]     " << path_u8(opt.root) << "\n";
            std::cout << "[llama]     " << path_u8(opt.llama_cli) << "\n";
            std::cout << "[model]     " << path_u8(opt.model) << "\n";
            std::cout << "[prompt]    " << path_u8(opt.prompt) << "\n";
            if (!opt.out_file.empty()) std::cout << "[out]       " << path_u8(opt.out_file) << "\n";
            std::cout << "[CMD] " << cmd << "\n\n";
        }

        // 7) Run
        std::string out = run_cmd(cmd);

        if (!opt.out_file.empty()) {
            std::cout << "OK: output written to " << path_u8(opt.out_file) << "\n";
            return 0;
        }

        if (out.empty()) {
            std::cerr << "Erreur: sortie vide. Active --verbose et verifie la commande.\n";
            return 1;
        }

        std::cout << out << "\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
