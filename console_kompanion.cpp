#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <string>
#include <ctime>
#include <map>
#include <chrono>
#include <vector>
using namespace std;

// Test (For Discord webhook)
// Global defaults
string preferredEditor = "nano";
string defaultFolder = "~";

// --- Config helpers ---
void loadConfig() {
    ifstream in("kompanion.conf");
    if (!in.is_open()) return;
    string key, value;
    while (in >> key >> value) {
        if (key == "editor") preferredEditor = value;
        else if (key == "folder") defaultFolder = value;
    }
    in.close();
}

void saveConfig() {
    ofstream out("kompanion.conf");
    out << "editor " << preferredEditor << "\n";
    out << "folder " << defaultFolder << "\n";
    out.close();
}

void showMenu() {
    cout << "\n=== Console Kompanion ===\n";
    cout << "1) Show system info\n";
    cout << "2) Manage files\n";
    cout << "3) Package management\n";
    cout << "4) Network basics\n";
    cout << "5) Open editor\n";
    cout << "6) Search files\n";
    cout << "7) Process manager\n";
    cout << "8) Disk cleanup helper\n";
    cout << "9) User management\n";
    cout << "10) System services (OpenRC)\n";
    cout << "11) System monitoring\n";
    cout << "12) Customization\n";
    cout << "13) Update Checker\n";
    cout << "14) Log View\n";
    cout << "15) System Health (Disk + Uptime)\n";
    cout << "16) Session Timer\n";
    cout << "17) Network Info\n";
    cout << "18) Calendar Peek\n";
    cout << "19) Weather Snapshot\n";
    cout << "20) Memory Usage Summmary\n";
    cout << "21) Describe a Command\n";
    cout << "22) Favorites\n";
    cout << "23) Exit\n";
    cout << "Choice: ";
}

void networkInfo() {
    cout << "\n=== Network Info ===" << endl;
    int result = system("ip a"); // or "ifconfig" if preferred
    if (result != 0) {
        cout << "Error: Could not retrieve network info." << endl;
    }
}

void calendarPeek() {
    cout << "\n=== Calendar Peek ===" << endl;
    int result = system("cal");
    if (result != 0) {
        cout << "Error: Could not display calendar." << endl;
    }
}

void weatherSnapshot() {
    cout << "\n=== Weather Snapshot ===" << endl;
    int result = system("curl -s wttr.in?format=3");
    if (result != 0) {
        cout << "Error: Could not retrieve weather." << endl;
    }
}

void memoryUsageSummary() {
    cout << "\n=== Memory Usage Summary ===" << endl;
    int result = system("free -h");
    if (result != 0) {
        cout << "Error: Could not retrieve memory usage." << endl;
    }
}


auto startTime = chrono::steady_clock::now();

void sessionTimer() {
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - startTime).count();

    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;

    cout << "\n=== Session Timer ===" << endl;
    cout << "Kompanion has been running for "
    << hours << "h " << minutes << "m " << seconds << "s." << endl;
}

void describeCommand() {
    cout << "\n=== Describe Command ===" << endl;
    cout << "Enter a command to describe: ";
    string cmd;
    cin >> cmd;

    // Dictionary of common commands
    static map<string, string> descriptions = {
        {"ls", "Lists directory contents."},
        {"cd", "Changes the current directory."},
        {"cp", "Copies files or directories."},
        {"mv", "Moves or renames files or directories."},
        {"rm", "Removes files or directories."},
        {"grep", "Searches text using patterns."},
        {"cat", "Displays file contents."},
        {"echo", "Prints text to the terminal."},
        {"pwd", "Prints the current working directory."},
        {"man", "Displays the manual page for a command."}
    };

    auto it = descriptions.find(cmd);
    if (it != descriptions.end()) {
        cout << cmd << ": " << it->second << endl;
    } else {
        cout << "No built-in description found. Trying 'whatis'...\n";
        string sysCmd = "whatis " + cmd;
        system(sysCmd.c_str());
    }
}


void systemHealth() {
    cout << "\n=== System Health ===" << endl;

    // Show disk usage
    cout << "\n-- Disk Usage --" << endl;
    int diskResult = system("df -h");
    if (diskResult != 0) {
        cout << "Error: Could not retrieve disk usage." << endl;
    }

    // Show uptime
    cout << "\n-- System Uptime --" << endl;
    int uptimeResult = system("uptime");
    if (uptimeResult != 0) {
        cout << "Error: Could not retrieve system uptime." << endl;
    }
}


void showQuoteOfTheDay() {
    const char* quotes[] = {
        "Keep pushing forward, no matter what.",
        "Small steps every day lead to big results.",
        "Your system is strong — and so are you.",
        "Learning never stops, even in the console.",
        "Stay curious, stay sharp, stay Kompanion."
    };
    int totalQuotes = sizeof(quotes) / sizeof(quotes[0]);

    srand(time(nullptr));
    int index = rand() % totalQuotes;

    cout << "\nQuote of the Day:\n" << quotes[index] << endl;
}
// Store favorite commands as strings
std::vector<std::string> favorites;

void addFavorite() {
    cout << "\n=== Add Favorite ===" << endl;
    cout << "Enter the command or description to save: ";
    string fav;
    cin.ignore(); // clear leftover newline
    getline(cin, fav);
    favorites.push_back(fav);
    cout << "Favorite added!" << endl;
}

void listFavorites() {
    cout << "\n=== Favorites ===" << endl;
    if (favorites.empty()) {
        cout << "No favorites saved yet." << endl;
        return;
    }
    for (size_t i = 0; i < favorites.size(); ++i) {
        cout << i + 1 << ") " << favorites[i] << endl;
    }
}

void favoritesMenu() {
    int choice;
    do {
        cout << "\n=== Favorites Menu ===" << endl;
        cout << "1) Add a favorite" << endl;
        cout << "2) List favorites" << endl;
        cout << "0) Back to main menu" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addFavorite(); break;
            case 2: listFavorites(); break;
            case 0: cout << "Returning to main menu..." << endl; break;
            default: cout << "Invalid choice." << endl;
        }
    } while (choice != 0);
}
void logViewer() {
    cout << "\n=== Log Viewer ===" << endl;
    cout << "Select a log to view:" << endl;
    cout << "1) System log (/var/log/syslog)" << endl;
    cout << "2) Messages log (/var/log/messages)" << endl;
    cout << "3) Kernel log (/var/log/kern.log)" << endl;
    cout << "4) Authentication log (/var/log/auth.log)" << endl;
    cout << "5) Back to main menu" << endl;

    int choice;
    cout << "Enter choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            system("less /var/log/syslog");
            break;
        case 2:
            system("less /var/log/messages");
            break;
        case 3:
            system("less /var/log/kern.log");
            break;
        case 4:
            system("less /var/log/auth.log");
            break;
        case 5:
            cout << "Returning to main menu..." << endl;
            break;
        default:
            cout << "Invalid choice." << endl;
    }
}
void updateChecker() {
    cout << "\n=== Update Checker ===" << endl;
    cout << "Previewing available updates...\n" << endl;

    // Run emerge pretend update
    int result = system("emerge -pvuD @world");

    if (result != 0) {
        cout << "Error: Could not run update check. "
        << "Make sure you have emerge installed and proper permissions."
        << endl;
    }
}
static const std::string RESET = "\033[0m";
static const std::string CYAN  = "\033[36m";

void showSplashScreen() {
    std::cout << CYAN;
    std::cout << "=====================================\n";
    std::cout << "        Welcome to Kompanion         \n";
    std::cout << "   A friendly console menu tool      \n";
    std::cout << "=====================================\n";
    std::cout << RESET << std::endl;
}

// --- System Info ---
void systemInfo() {
    cout << "\n--- System Info ---\n";
    system("uname -a");
    system("uptime");
    system("free -h");
    system("df -h");
}

// --- File Manager ---
void fileManager() {
    cout << "\n--- File Manager ---\n";
    cout << "1) List files\n2) Copy file\n3) Move file\n4) Delete file\nChoice: ";
    int c; cin >> c; cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string src, dest;
    switch(c) {
        case 1: system(("ls -lh " + defaultFolder).c_str()); break;
        case 2: cout << "Source: "; getline(cin, src);
        cout << "Destination: "; getline(cin, dest);
        system(("cp \""+src+"\" \""+dest+"\"").c_str()); break;
        case 3: cout << "Source: "; getline(cin, src);
        cout << "Destination: "; getline(cin, dest);
        system(("mv \""+src+"\" \""+dest+"\"").c_str()); break;
        case 4: cout << "File: "; getline(cin, src);
        system(("rm \""+src+"\"").c_str()); break;
    }
}

// --- Package Manager ---
void packageManager() {
    cout << "\n--- Package Manager ---\n";
    cout << "1) Update system\n2) Install package\n3) Remove package\nChoice: ";
    int c; cin >> c; cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string pkg;
    switch(c) {
        case 1: system("sudo emerge --sync && sudo emerge -uD @world"); break;
        case 2: cout << "Package name: "; getline(cin, pkg);
        system(("sudo emerge " + pkg).c_str()); break;
        case 3: cout << "Package name: "; getline(cin, pkg);
        system(("sudo emerge -C " + pkg).c_str()); break;
    }
}

// --- Network Basics ---
void networkBasics() {
    cout << "\n--- Network Basics ---\n";
    system("ip addr show");
    cout << "Testing internet connection...\n";
    system("ping -c 1 google.com");
}

// --- Editor ---
void openEditor() {
    cout << "Enter file to edit: ";
    string file; getline(cin, file);
    int rc = system((preferredEditor + " \""+file+"\"").c_str());
    if (rc != 0 && preferredEditor == "nano") {
        cout << "nano not found, trying vim...\n";
        system(("vim \""+file+"\"").c_str());
    }
}

// --- Search Files ---
void searchFiles() {
    cout << "Enter filename to search: ";
    string name; getline(cin, name);
    system(("find " + defaultFolder + " -name \""+name+"\" 2>/dev/null").c_str());
}

// --- Process Manager ---
void processManager() {
    cout << "\n--- Processes (top 20 by memory) ---\n";
    system("ps aux --sort=-%mem | head -20");
    cout << "Kill a process? Enter PID or press Enter to skip: ";
    string pid; getline(cin, pid);
    if (!pid.empty()) system(("kill -9 " + pid).c_str());
}

// --- Disk Cleanup Helper ---
void diskCleanup() {
    cout << "\n--- Largest files in current folder ---\n";
    system(("du -sh " + defaultFolder + "/* 2>/dev/null | sort -h | tail -20").c_str());
}

// --- User Management ---
void userManagement() {
    cout << "\n--- User Management ---\n";
    cout << "1) Add user\n2) Remove user\n3) Change password\nChoice: ";
    int c; cin >> c; cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string user;
    switch(c) {
        case 1: cout << "Enter new username: "; getline(cin, user);
        system(("sudo useradd " + user).c_str());
        cout << "Set password for " << user << ":\n";
        system(("sudo passwd " + user).c_str()); break;
        case 2: cout << "Enter username to remove: "; getline(cin, user);
        system(("sudo deluser " + user).c_str()); break;
        case 3: cout << "Enter username to change password: "; getline(cin, user);
        system(("sudo passwd " + user).c_str()); break;
    }
}

// --- System Services (OpenRC) ---
void systemServices() {
    cout << "\n--- System Services (OpenRC) ---\n";
    cout << "1) Start service\n2) Stop service\n3) Restart service\n4) Status of service\n5) List all services\nChoice: ";
    int c; cin >> c; cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string svc;
    if (c >= 1 && c <= 4) {
        cout << "Enter service name (e.g., sshd, apache2): ";
        getline(cin, svc);
    }
    switch(c) {
        case 1: system(("sudo rc-service " + svc + " start").c_str()); break;
        case 2: system(("sudo rc-service " + svc + " stop").c_str()); break;
        case 3: system(("sudo rc-service " + svc + " restart").c_str()); break;
        case 4: system(("rc-service " + svc + " status").c_str()); break;
        case 5: system("rc-status"); break;
    }
}

// --- System Monitoring ---
void systemMonitoring() {
    cout << "\n--- System Monitoring ---\n";
    cout << "Launching htop (if installed)...\n";
    int rc = system("htop");
    if (rc != 0) {
        cout << "htop not found, falling back to top...\n";
        system("top");
    }
}

// --- Customization ---
void customization() {
    cout << "\n--- Customization ---\n";
    cout << "1) Set preferred editor (nano/vim)\n";
    cout << "2) Set default folder\nChoice: ";
    int c; cin >> c; cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string val;
    switch(c) {
        case 1: cout << "Enter editor (nano/vim): "; getline(cin, val);
        preferredEditor = val; saveConfig(); break;
        case 2: cout << "Enter default folder path: "; getline(cin, val);
        defaultFolder = val; saveConfig(); break;
    }
    cout << "Settings updated.\n";
}

int main() {
    loadConfig();
    showSplashScreen();
    showQuoteOfTheDay();

    int choice;
    do {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(choice) {
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
            case 23:
                cout << "Goodbye from Console Kompanion!\n";
                break;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    } while (choice != 23);

    return 0;
}
