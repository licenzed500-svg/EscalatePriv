#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <curl/curl.h>

// Структура для хранения информации о библиотеке
struct Library {
    std::string name;
    std::string version;
};

// Функция для загрузки базы данных CVE
std::string downloadCveDatabase() {
    CURL* curl;
    CURLcode res;
    std::ostringstream oss;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/cve-database.csv"); // URL базы данных CVE
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char* ptr, size_t size, size_t nmemb, void* userdata) {
            std::ostringstream* oss = static_cast<std::ostringstream*>(userdata);
            size_t total_size = size * nmemb;
            oss->write(ptr, total_size);
            return total_size;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &oss);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return oss.str();
}

// Функция для парсинга базы данных CVE
std::vector<std::pair<std::string, std::string>> parseCveDatabase(const std::string& data) {
    std::vector<std::pair<std::string, std::string>> cves;
    std::istringstream iss(data);
    std::string line;
    while (std::getline(iss, line)) {
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
    std::string cveData = downloadCveDatabase();
    auto cveDatabase = parseCveDatabase(cveData);
    auto libraries = findLibraries();
    checkForVulnerabilities(libraries, cveDatabase);
    return 0;
}
