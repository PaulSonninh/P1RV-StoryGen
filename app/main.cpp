#include <cstdio>
#include <array>
#include <iostream>
#include <string>
#include <filesystem>
#include <windows.h>
#include <vector>
#include <sstream>
#include <shellapi.h>

struct Options {
    int seed = 42;
    float temp = 0.7f;
    float top_p = 0.9f;
    int n_predict = 200;
    bool verbose = false;
};

static void print_help() {
    std::cout
        << "Usage: storygen.exe [options]\n\n"
        << "Options:\n"
        << "  -h, --help              Show this help\n"
        << "  -s, --seed <int>        Seed (default: 42)\n"
        << "  -t, --temp <float>      Temperature (default: 0.7)\n"
        << "  -p, --top-p <float>     Top-p (default: 0.9)\n"
        << "  -n, --n-predict <int>   Tokens to generate (default: 200)\n"
        << "  --verbose               Print resolved paths + command\n";
}

static bool parse_options(const std::vector<std::wstring>& args, Options& opt) {
    for (size_t i = 1; i < args.size(); ++i) {
        const auto& a = args[i];

        auto need = [&](const wchar_t* name) -> const std::wstring& {
            if (i + 1 >= args.size()) throw std::runtime_error("Missing value after option");
            return args[++i];
        };

        if (a == L"-h" || a == L"--help") {
            print_help();
            return false; // stop program
        } else if (a == L"-s" || a == L"--seed") {
            opt.seed = std::stoi(need(L"seed").c_str());
        } else if (a == L"-t" || a == L"--temp") {
            opt.temp = std::stof(need(L"temp").c_str());
        } else if (a == L"-p" || a == L"--top-p") {
            opt.top_p = std::stof(need(L"top-p").c_str());
        } else if (a == L"-n" || a == L"--n-predict") {
            opt.n_predict = std::stoi(need(L"n-predict").c_str());
        } else if (a == L"--verbose") {
            opt.verbose = true;
        } else {
            // option inconnue -> message clair
            std::wcerr << L"Unknown option: " << a << L"\n";
            std::cout << "Use --help\n";
            return false;
        }
    }

    // Validation simple
    if (opt.top_p <= 0.0f || opt.top_p > 1.0f) throw std::runtime_error("top-p must be in (0, 1]");
    if (opt.temp <= 0.0f || opt.temp > 5.0f) throw std::runtime_error("temp must be > 0 (typical: 0.1..2.0)");
    if (opt.n_predict <= 0) throw std::runtime_error("n-predict must be > 0");

    return true;
}

static std::string run_cmd(const std::string& cmd) {
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "";
    std::array<char, 4096> buffer{};
    std::string result;
    while (fgets(buffer.data(), (int)buffer.size(), pipe)) result += buffer.data();
    _pclose(pipe);
    return result;
}

static std::vector<std::wstring> get_args_w() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return {};

    std::vector<std::wstring> args(argv, argv + argc);
    LocalFree(argv);
    return args;
}


int main() {

    namespace fs = std::filesystem;
    
    fs::path root = fs::current_path();

    fs::path llama = root / "llama.cpp" / "build" / "bin" / "Release" / "llama-cli.exe";
    fs::path model = root / "models" / "mistral-7b-instruct-v0.2.Q4_0.gguf";
    fs::path prompt = root / "prompt.txt";
    
    try {
        if (!fs::exists(llama)) {
            throw std::runtime_error("ERREUR: llama-cli introuvable.");
        }
        if (!fs::exists(model)) {
            throw std::runtime_error("ERREUR: modèle GGUF introuvable.");
        }
        if (!fs::exists(prompt)) {
            throw std::runtime_error("ERREUR: prompt.txt introuvable.");
        }

        // Récupère la ligne de commande complète
        std::vector<std::wstring> args = get_args_w();

        // Affiche les arguments
        for (const auto& arg : args) {
            std::wcout << arg << std::endl;
        }

        // Exemple d'utilisation des arguments : parse les valeurs
        int seed = 42;
        float temp = 0.7;
        float top_p = 0.9;

        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == L"-s" && i + 1 < args.size()) {
                seed = std::stoi(args[i + 1]);
            }
            else if (args[i] == L"-t" && i + 1 < args.size()) {
                temp = std::stof(args[i + 1]);
            }
            else if (args[i] == L"-p" && i + 1 < args.size()) {
                top_p = std::stof(args[i + 1]);
            }
        }

        // Utilisation des variables parsees
        std::wcout << L"Seed: " << seed << L", Temp: " << temp << L", Top-p: " << top_p << std::endl;

        std::string inner =
            "\"" + llama.string() + "\" "
            "-m \"" + model.string() + "\" "
            "-f \"" + prompt.string() + "\" ";

        std::string cmd = "cmd.exe /S /C \"start \"\" " + inner + "--seed " + std::to_string(seed) +
            " --temp " + std::to_string(temp) +
            " --top-p " + std::to_string(top_p) + " && echo exit\" 2>&1\"";

        std::cout << "[CMD] " << cmd << "\n\n";

        std::string out = run_cmd(cmd);
        if (out.empty()) {
            std::cerr << "Erreur: aucune sortie. Verifie les chemins (llama-cli / model / prompt).\n";
            return 1;
        }

        std::cout << out << "\n";
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
