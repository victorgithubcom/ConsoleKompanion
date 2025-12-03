
---

## 📘 `README.md`

```markdown
# Console Kompanion

Console Kompanion is a friendly, menu‑driven tool that makes the Calculate Linux console easier to use for people coming from Windows or other GUI‑centric systems.  
It wraps common commands into simple menus, so you don’t need to memorize them.

---

## ✨ Features

- **System Info**: View CPU, RAM, disk usage, uptime, kernel version.
- **File Management**: List, copy, move, delete files.
- **Package Management**: Update, install, remove software (Gentoo/Calculate via `emerge`).
- **Network Basics**: Show IP address, test internet connection.
- **Editor Launch**: Open files in nano or vim.
- **Search Files**: Find files by name.
- **Process Manager**: View top processes, kill by PID.
- **Disk Cleanup Helper**: Show largest files in a folder.
- **User Management**: Add/remove users, change passwords.
- **System Services (OpenRC)**: Start/stop/restart/status/list services.
- **System Monitoring**: Launch `htop` or `top` for live performance.
- **Customization**: Save preferred editor and default folder in `kompanion.conf`.

---

## 🚀 Quick Start

### Prerequisites
- A Calculate Linux or Gentoo‑based system.
- Installed tools: `emerge`, `rc-service`, `rc-status`, `nano` or `vim`, `htop` (optional).
- C++17 compiler (`g++`).

### Build
```bash
g++ -std=c++17 console_kompanion.cpp -o kompanion
