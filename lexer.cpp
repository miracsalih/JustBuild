/*
 * JustBuild/lexer.cpp
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
#include <vector>
#include <string>

static std::vector<char> sytnax_operator = {
    ':',
    '{',
    '}',
    ';'
};

static bool isOperator(char& op) {
    bool equal = false;
    for (const char& sytnax : sytnax_operator) if (sytnax == op) { equal = true; break; }
    return equal;
}

std::vector<Lexer> lexer(std::string& code)
 {
    std::vector<std::string> lexerlayer1;
    std::string token;
    enum Mode {
        normal,
        string,
        comment1,
        comment2
    };
    Mode mode = Mode::normal;

    for (size_t size = 0; size < code.size(); size++) {
        char i = code[size];

        if (i == '\"' && (mode != Mode::comment1 && mode != Mode::comment2)) {
            if (mode == Mode::string) {  
                token += i;
                if (!token.empty()) {
                    lexerlayer1.push_back(token);
                    token = "";
                }
                mode = Mode::normal;
            } else {
                if (!token.empty()) {
                    lexerlayer1.push_back(token);
                    token = "";
                }
                token += i;
                mode = Mode::string;
            }
            continue;
        }

        if (mode == Mode::comment1 || mode == Mode::comment2) {
            if (i == '\n' && mode == Mode::comment1) {
                mode = Mode::normal;
                continue;
            }
            else if (i == '*' && mode == Mode::comment2) {
                size++;
                if (code[size] == '/' && code.size() > 1 + size) {
                    mode = Mode::normal;
                    continue;
                }
                else size--;
            }
        }
        
        if (mode == Mode::string) {
            if (i == '\\' && code.size() > 1 + size) {
                size++;
                switch (code[size]) {
                    case 'n': token += '\n'; break;
                    case 't': token += '\t'; break;
                    case 'r': token += '\r'; break;
                    case '0': token += '\0'; break;
                    case '\\': token += '\\'; break;
                    case '"': token += '"'; break;
                    default: token += code[size]; break;
                }
            } else token += i;
        }
        
        if (mode == Mode::normal) {
            if (mode != Mode::comment1 && mode != Mode::comment2) {
                if (i == '/' && code.size() > 1 + size) {
                    size++;
                    if (code[size] == '/') {
                        mode = Mode::comment1;
                        continue;
                    } else if (code[size] == '*') {
                        mode = Mode::comment2;
                        continue;
                    } else {
                        size--;
                    }
                }
            } 

            if (i != ' ' && i != '\n' && i != '\t') token += i;
            else {
                if (!token.empty()) {
                    lexerlayer1.push_back(token);
                    token = "";
                }
            }
        }
    }

    if (!token.empty()) {
        if (mode == Mode::string) token += '\"';
        lexerlayer1.push_back(token);
        token = "";
    }

    std::vector<Lexer> lexerlayer2;

    for (const std::string& i : lexerlayer1) {
        bool op = false;
        enum Mode {
            normal,
            string,
            comment1,
            comment2
        };
        Mode mode = Mode::normal;
        std::string k;

        for (size_t idx = 0; idx < i.size(); idx++) {
            bool newop = op;
            char j = i[idx];

            if (isOperator(j)) newop = true;
            else newop = false;

            if (mode == Mode::normal) {
                if (op != newop) {
                    if (!k.empty()) {
                        lexerlayer2.push_back({.lexerop = op, .lexerdata = k});
                        k = "";
                    }
                    op = newop;
                } else {
                    if (newop) {
                        if (!k.empty()) {
                            lexerlayer2.push_back({.lexerop = op, .lexerdata = k});
                            k = "";
                        }
                    }
                }
            }

            if (j == '\"') {
                if (mode == Mode::normal) mode = Mode::string;
                else mode = Mode::normal;
            }

            k += j;
        }

        if (!k.empty()) lexerlayer2.push_back({.lexerop = op, .lexerdata = k});
    }

    return lexerlayer2;
}

Command parser(std::vector<Lexer>) {
    // here...
    return {};
}
