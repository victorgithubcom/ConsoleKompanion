#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <string>
using namespace std;

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
    cout << "13) Exit\n";
    cout << "Choice: ";
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
            case 13:
                cout << "Goodbye from Console Kompanion!\n";
                break;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    } while (choice != 13);
    return 0;
}
