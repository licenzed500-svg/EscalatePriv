#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <cstdlib>

// Структура для хранения информации о библиотеке
struct Library {
    std::string name;
    std::string version;
};

// Функция для загрузки базы данных CVE
void downloadCveDatabase(const std::string& url, const std::string& outputFile) {
    std::string command = "wget -O " + outputFile + " " + url;
    std::system(command.c_str());
}

// Функция для чтения базы данных CVE из локального файла
std::vector<std::pair<std::string, std::string>> readCveDatabase(const std::string& filePath) {
    std::vector<std::pair<std::string, std::string>> cves;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string name, version;
        std::getline(lineStream, name, ',');
        std::getline(lineStream, version, ',');
        cves.emplace_back(name, version);
    }
    return cves;
}

// Функция для поиска библиотек на системе
std::vector<Library> findLibraries() {
    std::vector<Library> libraries;
    for (const auto& entry : std::filesystem::recursive_directory_iterator("/")) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::regex versionRegex("([0-9]+\\.[0-9]+(\\.[0-9]+)?)");
            std::smatch versionMatch;
            std::string libraryPath = entry.path().string();
            if (std::regex_search(libraryPath, versionMatch, versionRegex)) {
                libraries.push_back({ libraryPath, versionMatch[0] });
            }
        }
    }
    return libraries;
}

// Функция для проверки уязвимостей и вывода только уязвимых библиотек
void checkForVulnerabilities(const std::vector<Library>& libraries, const std::vector<std::pair<std::string, std::string>>& cveDatabase) {
    for (const auto& lib : libraries) {
        for (const auto& cve : cveDatabase) {
            if (lib.name.find(cve.first) != std::string::npos && lib.version == cve.second) {
                std::cout << "Уязвимая библиотека: " << lib.name << " Версия: " << lib.version << std::endl;
            }
        }
    }
}

int main() {
    std::string cveUrl = "https://example.com/cve-database.csv"; // Укажите URL базы данных CVE
    std::string cveFilePath = "cve-database.csv";

    downloadCveDatabase(cveUrl, cveFilePath);
    auto cveDatabase = readCveDatabase(cveFilePath);
    auto libraries = findLibraries();
    checkForVulnerabilities(libraries, cveDatabase);

    return 0;
}

