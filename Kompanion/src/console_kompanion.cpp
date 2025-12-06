#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <sqlite3.h>
using namespace std;

// --- ANSI Colors ---
static const string RESET  = "\033[0m";
static const string CYAN   = "\033[36m";
static const string GREEN  = "\033[32m";
static const string RED    = "\033[31m";
static const string YELLOW = "\033[33m";

// --- Globals ---
string preferredEditor = "nano";
string defaultFolder   = "~";
vector<string> favorites;
string currentUser     = "anonymous";

// --- Helpers ---
string timestamp() {
    time_t t = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return buf;
}

string generateID() {
    srand(time(nullptr));
    return to_string(rand() % 1000000); // 6-digit ID
}

// --- Audit Logging ---
void audit(const string& user, const string& action) {
    bool newFile = !ifstream("kompanion_audit.log").good();
    ofstream out("kompanion_audit.log", ios::app);
    if (newFile) {
        out << left << setw(20) << "TIMESTAMP"
        << " | " << setw(12) << "USER"
        << " | " << "ACTION" << "\n";
        out << string(60, '-') << "\n";
    }
    out << left << setw(20) << timestamp()
    << " | " << setw(12) << user
    << " | " << action << "\n";
}

// --- SQLite Setup ---
void initDB(sqlite3* &db) {
    if (sqlite3_open("users.db", &db)) {
        cerr << "Can't open DB\n";
        exit(1);
    }
    const char* sql = "CREATE TABLE IF NOT EXISTS users("
    "username TEXT PRIMARY KEY,"
    "id TEXT NOT NULL);";
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}

// --- Sign Up / Login ---
void signUp(sqlite3* db) {
    cout << CYAN << "Enter new username: " << RESET;
    string username; cin >> username;
    string id = generateID();

    string sql = "INSERT INTO users(username,id) VALUES('" + username + "','" + id + "');";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK) {
        cout << GREEN << "Signup successful! Your ID is: " << id << RESET << "\n";
    } else {
        cout << RED << "Username already exists.\n" << RESET;
    }
}

bool login(sqlite3* db) {
    cout << CYAN << "Enter username: " << RESET;
    string username; cin >> username;
    cout << CYAN << "Enter ID: " << RESET;
    string id; cin >> id;

    string sql = "SELECT * FROM users WHERE username='" + username + "' AND id='" + id + "';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << GREEN << "Login successful!\n" << RESET;
            currentUser = username;
            sqlite3_finalize(stmt);
            return true;
        }
    }
    cout << RED << "Login failed.\n" << RESET;
    sqlite3_finalize(stmt);
    return false;
}

// --- Config / Favorites ---
void loadConfig() {
    ifstream in("kompanion.conf");
    if (!in.is_open()) return;
    string key, value;
    while (in >> key >> value) {
        if (key == "editor") preferredEditor = value;
        else if (key == "folder") defaultFolder = value;
        else if (key == "favorite") favorites.push_back(value);
    }
}

void saveConfig() {
    ofstream out("kompanion.conf");
    out << "editor " << preferredEditor << "\n";
    out << "folder " << defaultFolder << "\n";
    for (auto& fav : favorites) {
        out << "favorite " << fav << "\n";
    }
}

void addFavorite() {
    cout << CYAN << "\n=== Add Favorite ===\n" << RESET;
    cout << "Enter favorite command: ";
    string fav;
    cin.ignore();
    getline(cin, fav);
    favorites.push_back(fav);
    saveConfig();
    audit(currentUser, "Added favorite: " + fav);
    cout << GREEN << "Favorite saved!\n" << RESET;
}

void listFavorites() {
    cout << CYAN << "\n=== Favorites ===\n" << RESET;
    if (favorites.empty()) {
        cout << YELLOW << "No favorites saved yet.\n" << RESET;
        return;
    }
    for (size_t i = 0; i < favorites.size(); ++i) {
        cout << GREEN << i + 1 << ") " << favorites[i] << RESET << "\n";
    }
}

void favoritesMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== Favorites Menu ===\n" << RESET;
        cout << "1) Add a favorite\n";
        cout << "2) List favorites\n";
        cout << "0) Back to main menu\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice;
        switch (choice) {
            case 1: addFavorite(); break;
            case 2: listFavorites(); break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while (choice != 0);
}

// --- System Commands ---
void systemInfo() { system("uname -a"); system("uptime"); audit(currentUser,"System info"); }
void processManager() { system("ps aux --sort=-%mem | head -20"); audit(currentUser,"Process manager"); }
void resourceGraphs() { system("vmstat 1 5"); audit(currentUser,"Resource graphs"); }
void batteryStatus() { system("upower -i $(upower -e | grep BAT)"); audit(currentUser,"Battery status"); }
void systemHealth() { system("df -h"); system("uptime"); audit(currentUser,"System health"); }

void systemMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== System Commands ===\n" << RESET;
        cout << "1) System Info\n2) Process Manager\n3) Resource Graphs\n4) Battery Status\n5) System Health\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: systemInfo(); break;
            case 2: processManager(); break;
            case 3: resourceGraphs(); break;
            case 4: batteryStatus(); break;
            case 5: systemHealth(); break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}

// --- Network Commands ---
void networkBasics() { system("ip addr show"); audit(currentUser,"Network basics"); }
void networkInfo() { system("ip a"); audit(currentUser,"Network info"); }
void networkSpeedTest() { system("speedtest-cli"); audit(currentUser,"Network speed test"); }
void ipGeolocation() { system("curl -s ipinfo.io"); audit(currentUser,"IP geolocation"); }
void portScanner() { string host; cout << "Enter host: "; getline(cin, host); system(("nmap " + host).c_str()); audit(currentUser,"Port scanner"); }

void networkMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== Network Commands ===\n" << RESET;
        cout << "1) Network Basics\n2) Network Info\n3) Speed Test\n4) IP Geolocation\n5) Port Scanner\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: networkBasics(); break;
            case 2: networkInfo(); break;
            case 3: networkSpeedTest(); break;
            case 4: ipGeolocation(); break;
            case 5: portScanner(); break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}

// --- Utilities ---
void backupManager() { string src, dest; cout << "Source folder: "; getline(cin, src); cout << "Destination archive: "; getline(cin, dest); system(("tar -czf " + dest + " " + src).c_str()); audit(currentUser,"Backup manager"); }
void serviceRestarter() {
    cout << CYAN << "\n=== Service Restarter ===\n" << RESET;
    string service;
    cout << "Enter service name to restart: ";
    getline(cin, service);

    // Gentoo/OpenRC friendly: try systemctl, fallback to rc-service
    string cmd = "systemctl restart " + service + " 2>/dev/null || rc-service " + service + " restart";
    system(cmd.c_str());

    audit(currentUser, "Service restarter: " + service);
}

// --- Clipboard Utility ---
void clipboardUtility() {
    cout << CYAN << "\n=== Clipboard Utility ===\n" << RESET;
    cout << "1) Copy text\n2) Paste text\nChoice: ";
    int choice; cin >> choice; cin.ignore();
    if (choice == 1) {
        string text;
        cout << "Enter text to copy: ";
        getline(cin, text);
        string cmd = "echo \"" + text + "\" | xclip -selection clipboard";
        system(cmd.c_str());
        audit(currentUser, "Clipboard copy");
    } else if (choice == 2) {
        system("xclip -selection clipboard -o");
        audit(currentUser, "Clipboard paste");
    }
}

// --- Download Helper ---
void downloadHelper() {
    cout << CYAN << "\n=== Download Helper ===\n" << RESET;
    string url, dest;
    cout << "Enter URL: "; getline(cin, url);
    cout << "Enter destination filename: "; getline(cin, dest);
    string cmd = "wget -O " + dest + " " + url;
    system(cmd.c_str());
    audit(currentUser, "Download helper: " + url);
}


// --- Password Generator ---
void passwordGenerator() {
    cout << CYAN << "\n=== Password Generator ===\n" << RESET;
    int length;
    cout << "Enter desired length: "; cin >> length; cin.ignore();
    const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    string pass;
    srand(time(nullptr));
    for (int i = 0; i < length; i++) {
        pass += chars[rand() % chars.size()];
    }
    cout << GREEN << "Generated password: " << pass << RESET << "\n";
    audit(currentUser, "Password generator");
}

// --- Extras ---
void calendarPeek() { system("cal"); audit(currentUser,"Calendar peek"); }
void weatherSnapshot() { system("curl -s wttr.in?format=3"); audit(currentUser,"Weather snapshot"); }
void describeCommand() { cout << "ls: Lists directory contents.\n"; audit(currentUser,"Describe command"); }

// --- Menus ---
void showMainMenu() {
    cout << CYAN << "\n=== Kompanion Main Menu ===\n" << RESET;
    cout << "1) System Commands\n";
    cout << "2) Network Commands\n";
    cout << "3) Utilities\n";
    cout << "4) Extras\n";
    cout << "5) Favorites\n";
    cout << "6) Exit\n";
    cout << YELLOW << "Choice: " << RESET;
}


void utilitiesMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== Utilities ===\n" << RESET;
        cout << "1) Backup Manager\n2) Service Restarter\n3) Clipboard Utility\n4) Download Helper\n5) Password Generator\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: backupManager(); break;
            case 2: serviceRestarter(); break;
            case 3: clipboardUtility(); break;
            case 4: downloadHelper(); break;
            case 5: passwordGenerator(); break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}

void extrasMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== Extras ===\n" << RESET;
        cout << "1) Calendar Peek\n2) Weather Snapshot\n3) Describe Command\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: calendarPeek(); break;
            case 2: weatherSnapshot(); break;
            case 3: describeCommand(); break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}

// --- Main ---
int main() {
    sqlite3* db;
    initDB(db);

    cout << CYAN << "Welcome to Kompanion\n" << RESET;
    cout << "1) Sign Up\n2) Login\nChoice: ";
    int choice;
    cin >> choice;

    bool loggedIn = false;
    if (choice == 1) {
        signUp(db);
        cout << YELLOW << "Now login to continue.\n" << RESET;
        loggedIn = login(db);
    } else if (choice == 2) {
        loggedIn = login(db);
    }

    if (!loggedIn) {
        cout << RED << "Exiting...\n" << RESET;
        sqlite3_close(db);
        return 0;
    }

    loadConfig();

    int menuChoice;
    do {
        showMainMenu();
        cin >> menuChoice; cin.ignore();
        switch(menuChoice) {
            case 1: systemMenu(); break;
            case 2: networkMenu(); break;
            case 3: utilitiesMenu(); break;
            case 4: extrasMenu(); break;
            case 5: favoritesMenu(); break;
            case 6: saveConfig(); cout << GREEN << "Goodbye!\n" << RESET; break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(menuChoice != 6);

    sqlite3_close(db);
    return 0;
}
