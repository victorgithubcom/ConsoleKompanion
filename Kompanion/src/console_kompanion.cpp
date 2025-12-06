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

// --- Sign Up ---
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

// --- Login ---
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

// --- Config ---
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

// --- Favorites ---
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

// --- Features ---
void systemInfo() { system("uname -a"); system("uptime"); audit(currentUser,"System info"); }
void fileManager() { system("ls -lh"); audit(currentUser,"File manager"); }
void packageManager() { system("emerge -pvuD @world"); audit(currentUser,"Package manager"); }
void networkBasics() { system("ip addr show"); audit(currentUser,"Network basics"); }
void openEditor() { system((preferredEditor + " test.txt").c_str()); audit(currentUser,"Opened editor"); }
void searchFiles() { system(("find " + defaultFolder + " -name \"*.txt\"").c_str()); audit(currentUser,"Search files"); }
void processManager() { system("ps aux --sort=-%mem | head -20"); audit(currentUser,"Process manager"); }
void diskCleanup() { system(("du -sh " + defaultFolder + "/* | sort -h | tail -20").c_str()); audit(currentUser,"Disk cleanup"); }
void userManagement() { system("id"); audit(currentUser,"User management"); }
void systemServices() { system("rc-status"); audit(currentUser,"System services"); }
void systemMonitoring() { system("htop"); audit(currentUser,"System monitoring"); }
void customization() { preferredEditor="vim"; saveConfig(); audit(currentUser,"Customization"); }
void updateChecker() { system("emerge -pvuD @world"); audit(currentUser,"Update checker"); }
void logViewer() { system("less /var/log/syslog"); audit(currentUser,"Log viewer"); }
void systemHealth() { system("df -h"); system("uptime"); audit(currentUser,"System health"); }
void sessionTimer() { cout << "Session running...\n"; audit(currentUser,"Session timer"); }
void networkInfo() { system("ip a"); audit(currentUser,"Network info"); }
void calendarPeek() { system("cal"); audit(currentUser,"Calendar peek"); }
void weatherSnapshot() { system("curl -s wttr.in?format=3"); audit(currentUser,"Weather snapshot"); }
void memoryUsageSummary() { system("free -h"); audit(currentUser,"Memory usage"); }
void describeCommand() { cout << "ls: Lists directory contents.\n"; audit(currentUser,"Describe command"); }

// --- Menu ---
void showMenu() {
    cout << CYAN << "\n=== Console Kompanion Menu ===\n" << RESET;
    cout << "1) Show system info\n2) Manage files\n3) Package management\n4) Network basics\n";
    cout << "5) Open editor\n6) Search files\n7) Process manager\n8) Disk cleanup helper\n";
    cout << "9) User management\n10) System services\n11) System monitoring\n12) Customization\n";
    cout << "13) Update Checker\n14) Log View\n15) System Health\n16) Session Timer\n";
    cout << "17) Network Info\n18) Calendar Peek\n19) Weather Snapshot\n20) Memory Usage\n";
    cout << "21) Describe a Command\n22) Favorites\n23) Exit\n";
    cout << YELLOW << "Choice: " << RESET;
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
        // Run sign‑up, then immediately ask the user to log in
        signUp(db);
        cout << YELLOW << "Now login to continue.\n" << RESET;
        loggedIn = login(db);
    } else if (choice == 2) {
        // Direct login
        loggedIn = login(db);
    }

    if (!loggedIn) {
        cout << RED << "Exiting...\n" << RESET;
        sqlite3_close(db);
        return 0;
    }

    // Load config after login
    loadConfig();

    // Main menu loop
    int menuChoice;
    do {
        showMenu();
        if (!(cin >> menuChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(menuChoice) {
            case 1: systemInfo(); break;
            case 2: fileManager(); break;
            case 3: packageManager(); break;
            case 4: networkBasics(); break;
            case 5: openEditor(); break;
            case 6: searchFiles(); break;
            case 7: processManager(); break;
            case 8: diskCleanup(); break;
            case 9: userManagement(); break;
            case 10: systemServices(); break;
            case 11: systemMonitoring(); break;
            case 12: customization(); break;
            case 13: updateChecker(); break;
            case 14: logViewer(); break;
            case 15: systemHealth(); break;
            case 16: sessionTimer(); break;
            case 17: networkInfo(); break;
            case 18: calendarPeek(); break;
            case 19: weatherSnapshot(); break;
            case 20: memoryUsageSummary(); break;
            case 21: describeCommand(); break;
            case 22: favoritesMenu(); break;
            case 23: saveConfig(); cout << GREEN << "Goodbye!\n" << RESET; break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(menuChoice != 23);

    sqlite3_close(db);
    return 0;
}
