#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <regex>
#include <algorithm>
#include <locale>
#include <string>
#include <iomanip>
#include <codecvt> 
#include <cwctype> // Funkcijos kaip iswalpha, towlower
#include <Windows.h> // Naudojama simbolių koduotės konvertavimui
#include <chrono>

using std::string;
using std::wcout;
using std::endl;
using std::cerr;
using std::locale;
using std::ofstream;
using std::ifstream;
using std::wstring;
using std::setw;


/* Pagalbinė funkcija žodžiams valyti nuo skyrybos ženklų, skaičių, kitokių ne raidžių simbolių,
    kuri užtikrina lietuviškas ir kitokias raides, suvienodina mažosiomis raidėmis: */

string normalizeWord(const string& word) {

    // Konvertuoja UTF-8 eilutę į platų simbolių rinkinį (UTF-16, naudojamą Windows)
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, word.c_str(), -1, nullptr, 0);
    wstring wword(wideLen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, word.c_str(), -1, &wword[0], wideLen);

    wstring clean;
    // Iteruojama per kiekvieną simbolį ir patikrinama, ar tai raidė
    for (wchar_t c : wword) {
        if (iswalpha(c)) {          // Tik jei simbolis yra raidė
            clean += towlower(c);   // Konvertuojama į mažąją raidę
        }
    }

    // Konvertuoja plačią eilutę atgal į UTF-8 eilutę
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, clean.c_str(), -1, nullptr, 0, nullptr, nullptr);
    string result(utf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, clean.c_str(), -1, &result[0], utf8Len, nullptr, nullptr);

    // Pašalinamas papildomas '\0' simbolis, jei buvo pridėtas pabaigoje
    if (!result.empty() && result.back() == '\0') {
        result.pop_back();
    }
    return result;
}


std::map<string, int> wordCount;
std::map<string, std::set<int>> wordLines;
std::set<string> urlSet;

string line;
int lineNumber = 0;

