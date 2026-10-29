/*
 * JustBuild/main.cpp
 * Copyright (C) 2026  Miraç Salih İşler
 *
 * Bu program özgür yazılımdır: GNU Genel Kamu Lisansı'nın (GNU General
 * Public License) Free Software Foundation tarafından yayımlanan koşulları
 * altında (Lisansın 3. sürümü veya (isteğinize göre) daha sonraki bir
 * sürümü) yeniden dağıtabilir ve/veya değiştirebilirsiniz.
 *
 * Bu program, işe yarayacağı umuduyla dağıtılmaktadır, ancak HİÇBİR GARANTİ
 * VERİLMEZ; hatta TİCARİ ELVERİŞLİLİK veya BELİRLİ BİR AMACA UYGUNLUK
 * garantisi bile verilmez. Daha fazla ayrıntı için GNU Genel Kamu
 * Lisansı'na bakın.
 *
 * Bu programla birlikte GNU Genel Kamu Lisansı'nın bir kopyasını almış
 * olmalısınız. Eğer almamışsanız, <https://www.gnu.org/licenses/> adresine
 * bakın.
 */

#include "lexer.hpp"
#include <ostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Kullanım: " << argv[0] << " <girdi_dosyası> <çıktı_dosyası>" << std::endl;
        return -1;
    }

    std::ifstream input(argv[1], std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "Girdi dosyası açılamadı!" << std::endl;
        return -1;
    }
    
    std::ofstream output(argv[2], std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Çıktı dosyası açılamadı!" << std::endl;
        return -1;
    }

    std::stringstream ss;
    ss << input.rdbuf();
    std::string str = ss.str();

    std::vector<Lexer> code1 = lexer(str);

    for (const auto& [op, data] : code1) {
        output << "\"" << data << "\" : " << (op ? "YesOP" : "NoOP") << "\n";
    }

    input.close();
    output.close();
}
