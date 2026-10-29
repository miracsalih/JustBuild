#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

static std::string workdir;
static std::string lang;
static std::vector<std::string> lib;
static std::vector<std::string> arguments;

static std::string verilog;
static std::string verilogtb;
static std::string wave;

static bool run = false;

static void load(std::string path) {
    std::string cmd = "sudo cp " + path + " /usr/local/bin/";
    std::cout << "Yükleniyor: " << cmd << std::endl;
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Yükleme başarısız! Sudo yetkisi gerekli." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Kullanım: " << argv[0] << " <build.dosya> [load dosya]" << std::endl;
        return -6;
    }
    
    if (std::string(argv[1]) == "load") {
        if (argc < 3) {
            std::cerr << "Yüklenecek dosya belirtilmedi!" << std::endl;
            return 1;
        }
        load(argv[2]);
        std::cerr << "Yükleme tamamlandı." << std::endl;
        return 0;
    }

    if (std::string(argv[1]) == "load") {
        load(argv[0]);
        std::cerr << "Yükleme tamamlandı." << std::endl;
        return 0;
    }

    std::string path = argv[1];
    std::ifstream buildF(path);

    if (!buildF.is_open()) {
        std::cerr << "Build dosyası açılamadı!" << std::endl;
        return 1;
    }

    std::string build;
    path.erase(path.find_last_of('/'));
    path = path.substr(0, path.find_last_of('/') + 1);

    while (getline(buildF, build)) {
        if (build.empty()) continue;
        if (build.front() == '#') continue;

        if (build.substr(0, 9) == "workdir: ") {
            build.erase(0, 9);
            workdir = build;
            continue;
        }

        if (build.substr(0, 6) == "lang: ") {
            build.erase(0, 6);
            lang = build;
            continue;
        }

        if (build.substr(0, 5) == "lib: ") {
            build.erase(0, 5);
            bool skip = false;
            for (const std::string& lib : lib) {
                if (lib == build) skip = true;
            }
            if (skip) continue;

            if (build == "sdl2") lib.emplace_back("-lSDL2");
            else if (build == "sdl2::image") lib.emplace_back("-lSDL2_image");
            else if (build == "sdl2::tff") lib.emplace_back("-lSDL2_tff");
            else if (build == "sdl2::mixer") lib.emplace_back("-lSDL2_mixer");
            else if (build == "opengl") lib.emplace_back("-lGL");
            else if (build == "opengl::ew") lib.emplace_back("-lGLEW");
            else if (build == "opengl::u") lib.emplace_back("-lGLU");
            else if (build == "vulkan") lib.emplace_back("-lvulkan");
            else if (build == "winsock") lib.emplace_back("-lws2_32");
            else lib.emplace_back(build);
            continue;
        }

        if (build.substr(0, 18) == "verilog.filename: ") {
            build.erase(0, 18);
            verilog = build;
            continue;
        }

        if (build.substr(0, 19) == "verilog.testbench: ") {
            build.erase(0, 19);
            verilogtb = build;
            continue;
        }
            
        if (build.substr(0, 18) == "verilog.wavename: ") {
            build.erase(0, 18);
            wave = build;
            continue;
        }

        if (build == "compile") {
            int result;
            std::string command;

            if (lang == "cpp") {
                command = "g++ " + path + workdir + "/*." + lang + " -o " + path + workdir + "/code";
                for (const auto & i : lib) { command += " " + i; }

                std::cout << "Komut çalıştırılıyor..." << std::endl;
                result = system(command.c_str());

                if (result != 0) {
                    std::cerr << "Komut çalıştırıldı: Derlenemedi!" << std::endl;
                    std::cout << "Builder: -1 ile döndü." << std::endl;
                    return -1;
                }

                std::cout << "Komut çalıştırıldı: Derlendi..." << std::endl;
                std::cout << "Çalıştırılan komut: " << command << std::endl << std::endl;
            }

            if (lang == "verilog") {
                int result;
                std::string command;

                std::cout << "Verilog derleniyor..." << std::endl;
                command = "iverilog -o " + path + workdir + "/simule -s " + verilogtb + " " + path + workdir + "/" + verilogtb + ".v " + path + workdir + "/" + verilog + ".v";
                
                std::cout << "Çalıştırılan komut: " << command << std::endl;
                result = system(command.c_str());
    
                if (result != 0) {
                    std::cerr << "Verilog derlenemedi! Hata kodu: " << result << std::endl;
                    std::cerr << "Builder -3 ile döndü." << std::endl;
                    return -3;
                }

                std::cout << "Verilog başarıyla derlendi!" << std::endl;

                std::cout << "Simülasyon çalıştırılıyor ve VCD dosyası oluşturuluyor..." << std::endl;
                command = "cd " + path + workdir + "&& " + path + workdir + "/simule";
                std::cout << "Çalıştırılan komut: " << command << std::endl;
                result = system(command.c_str());

                if (result != 0) {
                    std::cerr << "Simülasyon çalıştırılamadı!" << std::endl;
                    std::cout << "Builder: -4 ile döndü." << std::endl;
                    return -4;
                }
                
                std::cout << "Simülasyon başarıyla çalıştı!" << std::endl;
                std::cout << "Oluşturulan dosya: " << path + workdir + "/" + wave + ".vcd" << std::endl;
            }

            std::cout << "Builder: " << lang << " dili derlendi." << std::endl;
            continue;
        }
            
        if (build == "run") {
            int result;
            std::string command;

            if (lang == "cpp") {
                std::cout << "Çalıştırılan komut: " << path << workdir << "/code";
                std::string arg;
                for (const auto & argument : arguments) {
                    arg += " " + argument;
                }
                std::cout << arg << std::endl;
                std::cout << "Kontrolü bu satırdan sonra bırakıyorum:" << std::endl << std::endl;
                result = system((path + workdir + "/code" + arg).c_str());

                if (result != 0) {
                    std::cerr << "Uygulama çalıştırılamadı!" << std::endl;
                    std::cerr << "Builder: -2 döndü." << std::endl;
                    return -2;
                }
            }

            if (lang == "verilog") {
                std::cout << "Dalgayı çalıştırıyorum..." << std::endl;
                result = system(("gtkwave " + path + workdir + "/" + wave + ".vcd").c_str());

                if (result != 0) {
                    std::cerr << "Komut çalıştırıldı: Dalga çalıştırılamadı!" << std::endl;
                    std::cout << "Builder: -5 ile döndü." << std::endl;
                    return -5;
                }
            }

            std::cout << "Builder: " << lang << " dili çalıştırıldı." << std::endl;
            continue;
        }

        if (build.substr(0, 11) == "arguments: ") {
            build.erase(0, 11);
            arguments.push_back(build);
            continue;
        }
    }
    return 0;
}
