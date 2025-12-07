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
string currentUser = "anonymous";


// --- Helpers ---
string timestamp() {
    time_t t = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

string generateID() {
    srand(time(nullptr));
    return to_string(rand() % 1000000);
}

// --- Audit Logging ---
void audit(const string& user, const string& action) {
    ofstream log("kompanion_audit.log", ios::app);  // append mode
    if (log.is_open()) {
        log << "[" << user << "] " << action << "\n";
        log.close();
    } else {
        cerr << "Could not open audit log file.\n";
    }
}

// --- Sign Up / Login ---


static inline string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a == string::npos) return "";
    return s.substr(a, b - a + 1);
}

bool isLessonComplete(sqlite3* db, const string& user, const string& lesson) {
    string sql = "SELECT completed FROM progress WHERE user='" + user +
    "' AND lesson='" + lesson + "';";
    sqlite3_stmt* stmt;
    bool result = false;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = sqlite3_column_int(stmt, 0) == 1;
        }
    }
    sqlite3_finalize(stmt);
    return result;
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

// --- Tutorials ---
// --- Lesson: Filesystem Basics ---
void filesystemBasicsLesson() {
    string dummy;

    cout << CYAN << "\n=== Lesson: Filesystem Basics ===\n" << RESET;

    cout << GREEN << "Step 1: Listing files\n" << RESET;
    cout << "Explanation: The 'ls' command lists files in the current directory.\n";
    cout << "Try running: ls\nPress Enter when ready...";
    getline(cin, dummy);
    system("ls");
    audit(currentUser, "Lesson step: ls");

    cout << GREEN << "\nStep 2: Current directory\n" << RESET;
    cout << "Explanation: The 'pwd' command shows your current location.\n";
    cout << "Try running: pwd\nPress Enter when ready...";
    getline(cin, dummy);
    system("pwd");
    audit(currentUser, "Lesson step: pwd");

    cout << GREEN << "\nStep 3: Changing directories\n" << RESET;
    cout << "Explanation: The 'cd' command moves you into another directory.\n";
    cout << "Try running: cd /tmp && pwd\nPress Enter when ready...";
    getline(cin, dummy);
    system("cd /tmp && pwd");
    audit(currentUser, "Lesson step: cd");

    cout << YELLOW << "\nQuiz: Which command shows your current directory?\n" << RESET;
    cout << "a) ls\nb) pwd\nc) cd\nChoice: ";
    string ans;
    getline(cin, ans);

    if (ans == "b" || ans == "B" || ans == "pwd" || ans == "PWD") {
        cout << GREEN << "Correct!\n" << RESET;
    } else {
        cout << RED << "Not quite — it's 'pwd'.\n" << RESET;
    }

    cout << CYAN << "\nLesson complete!\n" << RESET;
    audit(currentUser, "Completed lesson: Filesystem Basics");

    cout << YELLOW << "Press Enter to return to Tutorials Menu..." << RESET;
    getline(cin, dummy);
    audit(currentUser, "Completed lesson: Filesystem Basics");


}

// --- Lesson: Text Editing Basics ---
void textEditingLesson() {
    string dummy;

    cout << CYAN << "\n=== Lesson: Text Editing Basics ===\n" << RESET;

    cout << GREEN << "Step 1: Creating a file\n" << RESET;
    cout << "Explanation: Use 'echo' to quickly create a file with text.\n";
    cout << "Try running: echo \"Hello World\" > hello.txt\nPress Enter when ready...";
    getline(cin, dummy);
    system("echo \"Hello World\" > hello.txt");
    audit(currentUser, "Lesson step: echo > file");

    cout << GREEN << "\nStep 2: Viewing a file\n" << RESET;
    cout << "Explanation: Use 'cat' to display the contents of a file.\n";
    cout << "Try running: cat hello.txt\nPress Enter when ready...";
    getline(cin, dummy);
    system("cat hello.txt");
    audit(currentUser, "Lesson step: cat file");

    cout << GREEN << "\nStep 3: Editing a file\n" << RESET;
    cout << "Explanation: Use 'nano' to edit files interactively.\n";
    cout << "Try running: nano hello.txt (then exit with Ctrl+X)\nPress Enter when ready...";
    getline(cin, dummy);
    audit(currentUser, "Lesson step: nano file");

    cout << YELLOW << "\nQuiz: Which command displays file contents?\n" << RESET;
    cout << "a) nano\nb) cat\nc) echo\nChoice: ";
    string ans;
    getline(cin, ans);

    if (ans == "b" || ans == "B" || ans == "cat" || ans == "CAT") {
        cout << GREEN << "Correct!\n" << RESET;
    } else {
        cout << RED << "Not quite — it's 'cat'.\n" << RESET;
    }

    cout << CYAN << "\nLesson complete!\n" << RESET;
    audit(currentUser, "Completed lesson: Text Editing Basics");

    cout << YELLOW << "Press Enter to return to Tutorials Menu..." << RESET;
    getline(cin, dummy);
    audit(currentUser, "Text Editing Basics");

}

// --- Lesson: Permissions Basics ---
void permissionsLesson() {
    string dummy;

    cout << CYAN << "\n=== Lesson: Permissions Basics ===\n" << RESET;

    cout << GREEN << "Step 1: Viewing permissions\n" << RESET;
    cout << "Explanation: Use 'ls -l' to see file permissions.\n";
    cout << "Try running: ls -l hello.txt\nPress Enter when ready...";
    getline(cin, dummy);
    system("ls -l hello.txt");
    audit(currentUser, "Lesson step: ls -l");

    cout << GREEN << "\nStep 2: Changing permissions\n" << RESET;
    cout << "Explanation: Use 'chmod' to change file permissions.\n";
    cout << "Try running: chmod 644 hello.txt\nPress Enter when ready...";
    getline(cin, dummy);
    system("chmod 644 hello.txt");
    audit(currentUser, "Lesson step: chmod");

    cout << GREEN << "\nStep 3: Ownership\n" << RESET;
    cout << "Explanation: Use 'chown' to change file ownership.\n";
    cout << "Try running: sudo chown $USER hello.txt\nPress Enter when ready...";
    getline(cin, dummy);
    audit(currentUser, "Lesson step: chown");

    cout << YELLOW << "\nQuiz: Which command changes file permissions?\n" << RESET;
    cout << "a) ls -l\nb) chmod\nc) chown\nChoice: ";
    string ans;
    getline(cin, ans);

    if (ans == "b" || ans == "B" || ans == "chmod" || ans == "CHMOD") {
        cout << GREEN << "Correct!\n" << RESET;
    } else {
        cout << RED << "Not quite — it's 'chmod'.\n" << RESET;
    }

    cout << CYAN << "\nLesson complete!\n" << RESET;
    audit(currentUser, "Completed lesson: Permissions Basics");

    cout << YELLOW << "Press Enter to return to Tutorials Menu..." << RESET;
    getline(cin, dummy);
    audit(currentUser, "Permission Basics");

}


void tutorialsMenu() {
    cout << "\n=== Tutorials Menu ===\n";
    cout << "1) Filesystem Basics\n";
    cout << "2) Text Editing Basics\n";
    cout << "3) Permissions Basics\n";
    cout << "0) Back\nChoice: ";

    string input;
    getline(cin, input);

    if (input == "1") filesystemBasicsLesson();
    else if (input == "2") textEditingLesson();
    else if (input == "3") permissionsLesson();
    else if (input == "0") return;
    else cout << "Invalid choice.\n";
}





// --- Menus ---
void showMainMenu() {
    cout << CYAN << "\n=== Kompanion Main Menu ===\n" << RESET;
    cout << "1) System Commands\n";
    cout << "2) Network Commands\n";
    cout << "3) Utilities\n";
    cout << "4) Extras\n";
    cout << "5) Favorites\n";
    cout << "6) Tutorials\n";
    cout << "7) Exit\n";
    cout << YELLOW << "Choice: " << RESET;
}
// --- System Menu ---
void systemMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== System Commands ===\n" << RESET;
        cout << "1) System Info\n2) Process Manager\n3) Resource Graphs\n4) Battery Status\n5) System Health\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: system("uname -a"); system("uptime"); audit(currentUser,"System info"); break;
            case 2: system("ps aux --sort=-%mem | head -20"); audit(currentUser,"Process manager"); break;
            case 3: system("vmstat 1 5"); audit(currentUser,"Resource graphs"); break;
            case 4: system("upower -i $(upower -e | grep BAT)"); audit(currentUser,"Battery status"); break;
            case 5: system("df -h"); system("uptime"); audit(currentUser,"System health"); break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}

// --- Network Menu ---
void networkMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== Network Commands ===\n" << RESET;
        cout << "1) Network Basics\n2) Network Info\n3) Speed Test\n4) IP Geolocation\n5) Port Scanner\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: system("ip addr show"); audit(currentUser,"Network basics"); break;
            case 2: system("ip a"); audit(currentUser,"Network info"); break;
            case 3: system("speedtest-cli"); audit(currentUser,"Network speed test"); break;
            case 4: system("curl -s ipinfo.io"); audit(currentUser,"IP geolocation"); break;
            case 5: { string host; cout << "Enter host: "; getline(cin, host); system(("nmap " + host).c_str()); audit(currentUser,"Port scanner"); } break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}

// --- Utilities Menu ---
void utilitiesMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== Utilities ===\n" << RESET;
        cout << "1) Backup Manager\n2) Service Restarter\n3) Clipboard Utility\n4) Download Helper\n5) Password Generator\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: { string src, dest; cout << "Source folder: "; getline(cin, src); cout << "Destination archive: "; getline(cin, dest); system(("tar -czf " + dest + " " + src).c_str()); audit(currentUser,"Backup manager"); } break;
            case 2: { string service; cout << "Enter service name: "; getline(cin, service); string cmd = "systemctl restart " + service + " 2>/dev/null || rc-service " + service + " restart"; system(cmd.c_str()); audit(currentUser,"Service restarter"); } break;
            case 3: { cout << "Enter text to copy: "; string text; getline(cin, text); string cmd = "echo \"" + text + "\" | xclip -selection clipboard"; system(cmd.c_str()); audit(currentUser,"Clipboard copy"); } break;
            case 4: { string url, dest; cout << "Enter URL: "; getline(cin, url); cout << "Enter destination filename: "; getline(cin, dest); string cmd = "wget -O " + dest + " " + url; system(cmd.c_str()); audit(currentUser,"Download helper"); } break;
            case 5: { int length; cout << "Enter desired length: "; cin >> length; cin.ignore(); const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*"; string pass; srand(time(nullptr)); for (int i=0;i<length;i++) pass += chars[rand()%chars.size()]; cout << GREEN << "Generated password: " << pass << RESET << "\n"; audit(currentUser,"Password generator"); } break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}

// --- Extras Menu ---
void extrasMenu() {
    int choice;
    do {
        cout << CYAN << "\n=== Extras ===\n" << RESET;
        cout << "1) Calendar Peek\n2) Weather Snapshot\n3) Describe Command\n0) Back\n";
        cout << YELLOW << "Choice: " << RESET;
        cin >> choice; cin.ignore();
        switch(choice) {
            case 1: system("cal"); audit(currentUser,"Calendar peek"); break;
            case 2: system("curl -s wttr.in?format=3"); audit(currentUser,"Weather snapshot"); break;
            case 3: cout << "ls: Lists directory contents.\n"; audit(currentUser,"Describe command"); break;
            case 0: break;
            default: cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(choice != 0);
}


// --- Main ---
// --- Main ---
int main() {
    cout << CYAN << "----------------------\n" << RESET;
    cout << CYAN << "Welcome to Kompanion\n" << RESET;
    cout << CYAN << "----------------------\n" << RESET;

    // Load user preferences (editor, folder, favorites)
    loadConfig();

    int menuChoice;
    do {
        showMainMenu();
        cin >> menuChoice;
        cin.ignore();

        switch(menuChoice) {
            case 1: systemMenu(); break;
            case 2: networkMenu(); break;
            case 3: utilitiesMenu(); break;
            case 4: extrasMenu(); break;
            case 5: favoritesMenu(); break;
            case 6: tutorialsMenu(); break;   // Tutorials integration
            case 7:
                saveConfig();
                cout << GREEN << "Goodbye!\n" << RESET;
                break;
            default:
                cout << RED << "Invalid choice.\n" << RESET;
        }
    } while(menuChoice != 7);

    return 0;
}

