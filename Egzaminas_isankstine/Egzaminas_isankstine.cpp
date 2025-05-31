#include "Header.h"


int main() {

    auto programStart = std::chrono::high_resolution_clock::now();

    // Nustatome lokalę į UTF-8
    try {
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8); // Nustato konsolės išvestį į UTF-8
        locale::global(std::locale("")); // Naudoja sistemos lokalę
        wcout.imbue(std::locale());      // Įgalina lokalę wcout srautui
    }
    catch (const std::runtime_error&) {
        wcout << L"Lokacijos nustatyti nepavyko, naudojama numatytoji.\n";
    }

    ifstream inFile("tekstas.txt");
    ofstream wordCountOut("zodziu_pasikartojimai.txt");
    ofstream vietaOut("zodziu_vietos.txt");
    ofstream urlOut("url_adresai.txt");

    if (!inFile) {
        cerr << "Nepavyko atidaryti failo tekstas.txt\n";
        return 1;
    }

    // Bendresnis URL radimas (uŽskaitytų ir ne tik URL, pvz. T.XVII, nors ir yra galimas toks URL variantas)
    std::regex URL(R"(\b((https?:\/\/|www\.)?[\w\-.]+\.[a-z]{2,}([\/?#][^\s]*)?)\b)", std::regex::icase);

    // URL radimas - neužskaito bet kokių "raidės"+"."+"raidės", tačiau limituoja, nes nenuskaitytų pvz.: "pvz.lv"
    // std::regex URL(R"(\b((https?:\/\/|www\.)?[\w\-.]+\.(lt|gov|com|org|net|edu)(\/[^\s]*)?)\b)", std::regex::icase);

    auto readStart = std::chrono::high_resolution_clock::now();

    while (std::getline(inFile, line)) {
        ++lineNumber;
        std::istringstream iss(line);
        string word;

        // Ieškom URL'
        auto words_begin = std::sregex_iterator(line.begin(), line.end(), URL);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            urlSet.insert(i->str());
        }

        // Valomi nereikalingi ženklai
        while (iss >> word) {
            string normalized = normalizeWord(word);
            if (!normalized.empty()) {
                wordCount[normalized]++;
                wordLines[normalized].insert(lineNumber);
            }
        }
    }

    auto readEnd = std::chrono::high_resolution_clock::now();
    auto writeStart = std::chrono::high_resolution_clock::now();

    // PASIKARTOJIMAI
    if (!wordCountOut) {
        cerr << "Nepavyko sukurti failo zodziu_pasikartojimai.txt\n";
    }
    else {
        // Rašomas žodžių pasikartojimų sąrašas
        wordCountOut << "Žodžių, pasikartojusių dažniau nei 1 kartą, sąrašas:\n" << string(21, '-') << "|" << string(35, '-') << endl;
        bool found = false;
        for (const auto& [word, count] : wordCount) {
            if (count > 1) {
                wordCountOut << setw(20) << std::left << word << " : " << count << " kartus (-ų)\n";
                found = true;
            }
        }
        if (!found) {
            wordCountOut << "(nieko nerasta.)\n";
        }
    }

    // VIETA TEKSTE
    if (!vietaOut) {
        cerr << "Nepavyko sukurti failo zodziu_vietos.txt\n";
    }
    else {
        // Rašoma cross-reference lentelė su nurodytomis žodžių vietomis tekste
        vietaOut << "Žodžių, pasikartojusių dažniau nei 1 kartą, vietų sąrašas (kuriose jis teksto eilutėse):\n" << string(19, '-') << "|" << string(19, '-') << "|" << string(50, '-') << endl;
        if (wordCount.empty()) vietaOut << "(nieko nerasta.)";
        else {
            for (const auto& [word, lines] : wordLines) {
                if (wordCount[word] > 1) {
                    vietaOut << setw(20) << std::left << word << " pasirodė eilutėse:  ";
                    for (int lineNum : lines) {
                        vietaOut << lineNum << " ";
                    }
                    vietaOut << "\n";
                }
            }
        }
    }

    // URL
    if (!urlOut) {
        cerr << "Nepavyko sukurti failo url_adresai.txt\n";
    }
    else {
        // Rašomi URL'ai
        urlOut << "Rastų URL sąrašas:\n" << string(80, '-') << endl;
        if (urlSet.empty()) urlOut << "(nieko nerasta.)";
        else {
            for (const auto& url : urlSet) { urlOut << url << "\n"; }
        }
    }

    auto writeEnd = std::chrono::high_resolution_clock::now();
    auto programEnd = std::chrono::high_resolution_clock::now();

    auto readTime = std::chrono::duration_cast<std::chrono::milliseconds>(readEnd - readStart).count();
    auto writeTime = std::chrono::duration_cast<std::chrono::milliseconds>(writeEnd - writeStart).count();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(programEnd - programStart).count();

    wcout << L"\n Įvykdymo laiko analizė:\n";
    wcout << L" - Nuskaitymas          : " << readTime << " ms\n";
    wcout << L" - Rezultatų išvedimas  : " << writeTime << " ms\n";
    wcout << L" - Bendra programa      : " << totalTime << " ms\n";

    wcout << L"\nAnalizė baigta. \nRezultatai išsaugoti failuose: zodziu_pasikartojimai.txt, zodziu_vietos.txt, url_adresai.txt.\n";
    return 0;
}
