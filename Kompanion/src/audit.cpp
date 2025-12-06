// audit.cpp
#include <fstream>
#include <ctime>
#include <string>

std::string timestamp() {
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

void audit(const std::string& user, const std::string& action) {
    std::ofstream out("kompanion_audit.log", std::ios::app);
    out << "[" << timestamp() << "] user=" << user << " action=\"" << action << "\"\n";
}
