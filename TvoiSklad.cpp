#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
using namespace std;

class Logger {
public:
    void logAction(const string& username, const string& action) {
        ofstream log("Logs.txt", ios_base::app);
        log << "User: " << username << "\nAction: " << action << "\nTime: " << getCurrentTime() << "\n\n";
        log.close();
    }
protected:
    string getCurrentTime() {
        time_t now = time(0);
        char buf[26];
        ctime_s(buf, sizeof(buf), &now);
        return string(buf);
    }
};

class Account : public Logger {
protected:
    string username;
    string password;
public:
    Account(const string& username, const string& password)
        : username(username), password(password) {}
    virtual void login() = 0;
    virtual void displayInfo() = 0;
    virtual void addItem() { cout << "Action not allowed\n"; }
    virtual void deleteItem() { cout << "Action not allowed\n"; }
    virtual void editItem() { cout << "Action not allowed\n"; }
    virtual void getUserInfo() { cout << "Action not allowed\n"; }
    virtual ~Account() {}
};

class Worker : public Account {
public:
    Worker(const string& username, const string& password)
        : Account(username, password) {}
    void login() override {
        cout << "Logged in as warehouse worker" << endl;
        logAction(username, "Login");
    }
    void displayInfo() override {
        string itemID;
        cout << "Enter item ID to display information: ";
        cin.ignore();
        getline(cin, itemID);
        ifstream file(itemID + ".txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
            logAction(username, "Displayed information for item ID: " + itemID);
        }
        else {
            cout << "Item with the specified ID not found" << endl;
            logAction(username, "Failed to find item with ID: " + itemID);
        }
    }
};

class Manager : public Worker {
public:
    Manager(const string& username, const string& password)
        : Worker(username, password) {}
    void login() override {
        cout << "Logged in as warehouse manager" << endl;
        logAction(username, "Login as manager");
    }
    void editItem() override {
        string newName, newCategory, newQuantity, newItemID;
        cin.ignore();
        cout << "Enter new item name: ";
        getline(cin, newName);
        cout << "Enter new item category: ";
        getline(cin, newCategory);
        cout << "Enter new item quantity: ";
        getline(cin, newQuantity);
        cout << "Enter new item ID: ";
        getline(cin, newItemID);
        ofstream file(newItemID + ".txt");
        file << "Name: " << newName << "\nCategory: " << newCategory << "\nQuantity: " << newQuantity << "\nID: " << newItemID;
        file.close();
        cout << "Item information updated successfully" << endl;
        logAction(username, "Edited item with new ID: " + newItemID);
    }
};

class Administrator : public Manager {
public:
    Administrator(const string& username, const string& password)
        : Manager(username, password) {}
    void login() override {
        cout << "Logged in as warehouse administrator" << endl;
        logAction(username, "Login as administrator");
    }
    void addItem() override {
        string name, category, quantity, itemID;
        cin.ignore();
        cout << "Enter item name: ";
        getline(cin, name);
        cout << "Enter item category: ";
        getline(cin, category);
        cout << "Enter item quantity: ";
        getline(cin, quantity);
        cout << "Enter item ID: ";
        getline(cin, itemID);
        ofstream file(itemID + ".txt");
        file << "Name: " << name << "\nCategory: " << category << "\nQuantity: " << quantity << "\nID: " << itemID;
        file.close();
        cout << "Item added successfully" << endl;
        logAction(username, "Added item with ID: " + itemID);
    }
    void deleteItem() override {
        string itemID;
        cin.ignore();
        cout << "Enter item ID to delete: ";
        getline(cin, itemID);
        if (remove((itemID + ".txt").c_str()) != 0) {
            cout << "Error deleting item" << endl;
            logAction(username, "Error deleting item with ID: " + itemID);
        }
        else {
            cout << "Item deleted successfully" << endl;
            logAction(username, "Deleted item with ID: " + itemID);
        }
    }
    void getUserInfo() override {
        string userID;
        cin.ignore();
        cout << "Enter user ID: ";
        getline(cin, userID);
        ifstream file("users.txt");
        if (file.is_open()) {
            string line;
            bool userFound = false;
            while (getline(file, line)) {
                if (line.find("ID: " + userID) != string::npos) {
                    userFound = true;
                    cout << "User information: " << endl;
                    do {
                        cout << line << endl;
                    } while (getline(file, line) && !line.empty());
                    logAction(username, "Retrieved information for user ID: " + userID);
                    break;
                }
            }
            if (!userFound) {
                cout << "User with the specified ID not found" << endl;
                logAction(username, "Failed to find user with ID: " + userID);
            }
            file.close();
        }
        else {
            cout << "Error opening users file" << endl;
            logAction(username, "Error opening users.txt for user ID: " + userID);
        }
    }
};

struct AccountInfo {
    string username;
    string password;
    string role;
};

bool loadAccounts(AccountInfo accounts[], int& count) {
    ifstream file("accounts.txt");
    string line;
    count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, password, role;
        if (getline(ss, username, ':') && getline(ss, password, ':') && getline(ss, role)) {
            accounts[count++] = { username, password, role };
        }
    }
    return count > 0;
}

void performActions(Account* account) {
    int action;
    while (true) {
        cout << "Choose an action:\n[1] Add Item\n[2] Delete Item\n[3] Edit Item\n[4] Get User Data\n[5] Display Item Info\n[6] Exit\n";
        cin >> action;
        if (action == 6) {
            break;
        }
        switch (action) {
        case 1:
            account->addItem();
            break;
        case 2:
            account->deleteItem();
            break;
        case 3:
            account->editItem();
            break;
        case 4:
            account->getUserInfo();
            break;
        case 5:
            account->displayInfo();
            break;
        default:
            cout << "Invalid action" << endl;
            break;
        }
    }
}

int main() {
    system("chcp 1251");
    const int maxAccounts = 100;
    AccountInfo accounts[maxAccounts];
    int accountCount = 0;
    if (!loadAccounts(accounts, accountCount)) {
        cout << "Error loading accounts" << endl;
        return 1;
    }
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    Account* account = nullptr;
    for (int i = 0; i < accountCount; ++i) {
        if (accounts[i].username == username && accounts[i].password == password) {
            string role = accounts[i].role;
            if (role == "admin") {
                account = new Administrator(username, password);
            }
            else if (role == "manager") {
                account = new Manager(username, password);
            }
            else if (role == "worker") {
                account = new Worker(username, password);
            }
            else {
                cout << "Unknown role" << endl;
                return 1;
            }
            break;
        }
    }
    if (account != nullptr) {
        account->login();
        performActions(account);
        delete account;
    }
    else {
        cout << "Invalid username or password" << endl;
        return 1;
    }
    return 0;
}
