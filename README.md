# 🔐 Vault Shell

A simple, secure, and minimal password vault written in modern C++.

---

## ⚙️ Build & Setup Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/vault-shell.git
cd vault-shell
```
### 2. Build the project
```bash
mkdir build
cd build
cmake ..
make
```
### 3. Find the build executable
```bash
./vaultShell
```
### 4. Install the executable globally
```bash
sudo mv vaultShell /usr/local/bin/vaultShell
```
### 5. Run the Vault Shell
```bash
vaultShell
```

### Available commands in the Vault Shell
| Command         | Description                             |
|-----------------|-----------------------------------------|
| `ls`            | List all entries in the vault           |
| `add <name>`    | Add a new entry                         |
| `delete <name>` | Delete an existing entry (if it exists) |
| `change`        | Change the master password              |
| `change <name>` | Change the password for an entry        |
| `search <name>` | Search for entries                      |
| `exit`          | Exit the vault shell                    |
