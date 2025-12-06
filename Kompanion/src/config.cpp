// config.cpp
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> favorites;

void loadConfig() {
    std::ifstream in("kompanion.conf");
    std::string key, value;
    while (in >> key >> value) {
        if (key == "editor") preferredEditor = value;
        else if (key == "folder") defaultFolder = value;
        else if (key == "favorite") favorites.push_back(value);
    }
}

void saveConfig() {
    std::ofstream out("kompanion.conf");
    out << "editor " << preferredEditor << "\n";
    out << "folder " << defaultFolder << "\n";
    for (auto& fav : favorites) {
        out << "favorite " << fav << "\n";
    }
}
