// eduquest_final_complete_corrected.cpp
// Complete Edutainment System with Enhanced Typing Test - CORRECTED VERSION

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <map>

using namespace std;

// ========== COLOR CONSTANTS ==========
const int COLOR_RESET = 7;
const int COLOR_RED = 12;
const int COLOR_GREEN = 10;
const int COLOR_YELLOW = 14;
const int COLOR_BLUE = 9;
const int COLOR_PURPLE = 13;
const int COLOR_CYAN = 11;
const int COLOR_WHITE = 15;

// Forward declarations
class User;
class Utility;

// ========== UTILITY CLASS ==========
class Utility {
private:
    static HANDLE hConsole;
    
public:
    static void init() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    
    static void setColor(int color) {
        SetConsoleTextAttribute(hConsole, color);
    }
    
    static void clearScreen() {
        system("cls");
    }
    
    static void pauseScreen(const string& message = "Press Enter to continue...") {
        cout << "\n" << message;
        cin.ignore();
        cin.get();
    }
    
    static bool checkExit() {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27 || ch == 'q' || ch == 'Q') {
                return true;
            }
        }
        return false;
    }
    
    static string getPasswordInput() {
        string password = "";
        char ch;
        while (true) {
            ch = _getch();
            if (ch == 13) break;
            else if (ch == 8) {
                if (!password.empty()) {
                    cout << "\b \b";
                    password.pop_back();
                }
            }
            else if (ch == 27) {
                return "";
            }
            else {
                cout << '*';
                password += ch;
            }
        }
        return password;
    }
    
    static void openInBrowser(const string& filename) {
        if (fileExists(filename)) {
            string command = "start \"\" \"" + filename + "\"";
            system(command.c_str());
        } else {
            setColor(COLOR_RED);
            cout << "\nError: File '" << filename << "' not found!\n";
            setColor(COLOR_RESET);
        }
    }
    
    static bool fileExists(const string& filename) {
        ifstream file(filename);
        return file.good();
    }
    
    static void createDirectory(const string& dir) {
        system(("mkdir \"" + dir + "\" 2>nul").c_str());
    }
    
    static vector<string> split(const string& s, char delim) {
        vector<string> result;
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim)) {
            result.push_back(item);
        }
        return result;
    }
    
    static void printHeader(const string& title) {
        clearScreen();
        setColor(COLOR_PURPLE);
        cout << "========================================\n";
        cout << "           " << title << "\n";
        cout << "========================================\n";
        setColor(COLOR_RESET);
    }
    
    static void printSuccess(const string& msg) {
        setColor(COLOR_GREEN);
        cout << "[+] " << msg << "\n";
        setColor(COLOR_RESET);
    }
    
    static void printError(const string& msg) {
        setColor(COLOR_RED);
        cout << "[-] " << msg << "\n";
        setColor(COLOR_RESET);
    }
    
    static void printWarning(const string& msg) {
        setColor(COLOR_YELLOW);
        cout << "[!] " << msg << "\n";
        setColor(COLOR_RESET);
    }
    
    static void printInfo(const string& msg) {
        setColor(COLOR_CYAN);
        cout << "[i] " << msg << "\n";
        setColor(COLOR_RESET);
    }
    
    static string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        if (string::npos == first) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    
    static string toLower(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    static void displayExitHint() {
        cout << "\n";
        setColor(COLOR_YELLOW);
        cout << "Press ESC or Q to go back\n";
        setColor(COLOR_RESET);
    }
};

HANDLE Utility::hConsole;

// ========== USER CLASS ==========
class User {
private:
    string username;
    string password;
    string role;
    int points;
    
public:
    User() : points(0) {}
    
    User(const string& uname, const string& pwd, const string& r = "user") 
        : username(uname), password(pwd), role(r), points(0) {}
    
    string getUsername() const { return username; }
    string getRole() const { return role; }
    int getPoints() const { return points; }
    
    void setPoints(int p) { points = p; }
    void addPoints(int p) { points += p; }
    
    bool authenticate(const string& pwd) const {
        return password == pwd;
    }
    
    void displayInfo() const {
        Utility::setColor(COLOR_YELLOW);
        cout << "User: " << username << " | Role: " << role << " | Points: " << points << "\n";
        Utility::setColor(COLOR_RESET);
    }
    
    string toString() const {
        return username + "," + password + "," + role + "," + to_string(points);
    }
    
    static User fromString(const string& data) {
        vector<string> parts = Utility::split(data, ',');
        if (parts.size() >= 3) {
            User user(parts[0], parts[1], parts[2]);
            if (parts.size() > 3) user.points = stoi(parts[3]);
            return user;
        }
        return User();
    }
};

// ========== USER MANAGER CLASS ==========
class UserManager {
private:
    vector<User> users;
    const string USER_FILE = "users.txt";
    const string ADMIN_FILE = "admin.txt";
    
public:
    UserManager() {
        loadUsers();
    }
    
    bool registerUser(const string& username, const string& password, const string& role = "user") {
        if (userExists(username)) {
            return false;
        }
        
        User newUser(username, password, role);
        users.push_back(newUser);
        saveUser(newUser);
        return true;
    }
    
    User* login(const string& username, const string& password) {
        for (auto& user : users) {
            if (user.getUsername() == username && user.authenticate(password)) {
                return &user;
            }
        }
        return nullptr;
    }
    
    bool userExists(const string& username) {
        for (const auto& user : users) {
            if (user.getUsername() == username) {
                return true;
            }
        }
        return false;
    }
    
    void saveUser(const User& user) {
        string filename = (user.getRole() == "admin") ? ADMIN_FILE : USER_FILE;
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << user.toString() << endl;
            file.close();
        }
    }
    
    void updateUser(const User& user) {
        string filename = (user.getRole() == "admin") ? ADMIN_FILE : USER_FILE;
        vector<string> lines;
        
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            if (line.find(user.getUsername() + ",") == 0) {
                lines.push_back(user.toString());
            } else {
                lines.push_back(line);
            }
        }
        file.close();
        
        ofstream outFile(filename);
        for (const auto& l : lines) {
            outFile << l << endl;
        }
        outFile.close();
    }
    
    void displayAllUsers() {
        Utility::printHeader("REGISTERED USERS");
        
        int count = 1;
        for (const auto& user : users) {
            cout << count++ << ". " << user.getUsername() 
                 << " (" << user.getRole() 
                 << ", Points: " << user.getPoints() << ")\n";
        }
    }
    
    void deleteUser(const string& username) {
        users.erase(remove_if(users.begin(), users.end(),
            [&username](const User& u) { return u.getUsername() == username; }),
            users.end());
        updateAllUsers();
    }
    
    int getUserCount() const {
        return users.size();
    }
    
private:
    void loadUsers() {
        ifstream userFile(USER_FILE);
        string line;
        while (getline(userFile, line)) {
            if (!line.empty()) {
                users.push_back(User::fromString(line));
            }
        }
        userFile.close();
        
        ifstream adminFile(ADMIN_FILE);
        while (getline(adminFile, line)) {
            if (!line.empty()) {
                users.push_back(User::fromString(line));
            }
        }
        adminFile.close();
    }
    
    void updateAllUsers() {
        ofstream userFile(USER_FILE);
        ofstream adminFile(ADMIN_FILE);
        
        for (const auto& user : users) {
            if (user.getRole() == "admin") {
                adminFile << user.toString() << endl;
            } else {
                userFile << user.toString() << endl;
            }
        }
        
        userFile.close();
        adminFile.close();
    }
};

// ========== NOTES MANAGER CLASS ==========
class NotesManager {
private:
    const string NOTES_DIR = "notes/";
    vector<string> subjects = {"Physics", "Chemistry", "English", "Mathematics", "Biology"};
    
public:
    NotesManager() {
        Utility::createDirectory(NOTES_DIR);
        initializeDefaultNotes();
    }
    
    vector<string> getSubjects() const {
        return subjects;
    }
    
    void viewNotes() {
        while (true) {
            Utility::printHeader("STUDY NOTES");
            
            for (size_t i = 0; i < subjects.size(); i++) {
                cout << i + 1 << ". " << subjects[i] << "\n";
            }
            cout << subjects.size() + 1 << ". Back\n";
            
            cout << "\nSelect subject: ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) {
                return;
            }
            
            try {
                int choice = stoi(input);
                if (choice == subjects.size() + 1) {
                    return;
                }
                if (choice >= 1 && choice <= subjects.size()) {
                    displayNote(subjects[choice - 1]);
                }
            } catch (...) {
                Utility::printError("Invalid input!");
                Utility::pauseScreen();
            }
        }
    }
    
    void displayNote(const string& subject) {
        string filename = NOTES_DIR + subject + ".txt";
        
        ifstream file(filename);
        if (!file.is_open()) {
            Utility::printError("No notes found!");
            Utility::pauseScreen();
            return;
        }
        
        Utility::printHeader(subject + " NOTES");
        
        string line;
        int lineCount = 0;
        while (getline(file, line)) {
            cout << line << "\n";
            lineCount++;
            
            if (lineCount % 20 == 0) {
                cout << "\n--- Press Enter to continue --- ";
                if (Utility::checkExit()) {
                    file.close();
                    return;
                }
                cin.get();
            }
        }
        file.close();
        
        Utility::pauseScreen();
    }
    
    void uploadNote() {
        Utility::printHeader("UPLOAD NOTES");
        
        cout << "Select subject:\n";
        for (size_t i = 0; i < subjects.size(); i++) {
            cout << i + 1 << ". " << subjects[i] << "\n";
        }
        cout << subjects.size() + 1 << ". New Subject\n";
        
        cout << "\nChoice: ";
        string input;
        getline(cin, input);
        
        if (Utility::checkExit() || input.empty()) {
            return;
        }
        
        try {
            int choice = stoi(input);
            string subject;
            
            if (choice == subjects.size() + 1) {
                cout << "Enter new subject: ";
                getline(cin, subject);
                subject = Utility::trim(subject);
                
                if (subject.empty()) {
                    Utility::printError("Invalid subject!");
                    Utility::pauseScreen();
                    return;
                }
                subjects.push_back(subject);
            } else if (choice >= 1 && choice <= subjects.size()) {
                subject = subjects[choice - 1];
            } else {
                Utility::printError("Invalid choice!");
                Utility::pauseScreen();
                return;
            }
            
            cout << "\nEnter content (type END on new line):\n";
            string content, line;
            while (true) {
                getline(cin, line);
                if (line == "END") break;
                content += line + "\n";
            }
            
            string filename = NOTES_DIR + subject + ".txt";
            ofstream file(filename, ios::app);
            if (file.is_open()) {
                file << "\n=== " << __DATE__ << " ===\n";
                file << content;
                file.close();
                Utility::printSuccess("Notes uploaded!");
            } else {
                Utility::printError("Failed to upload!");
            }
            
            Utility::pauseScreen();
            
        } catch (...) {
            Utility::printError("Invalid input!");
            Utility::pauseScreen();
        }
    }
    
    void updateNote() {
        Utility::printHeader("UPDATE NOTES");
        
        cout << "Select subject:\n";
        for (size_t i = 0; i < subjects.size(); i++) {
            cout << i + 1 << ". " << subjects[i] << "\n";
        }
        
        cout << "\nChoice: ";
        string input;
        getline(cin, input);
        
        if (Utility::checkExit() || input.empty()) {
            return;
        }
        
        try {
            int choice = stoi(input);
            if (choice >= 1 && choice <= subjects.size()) {
                string subject = subjects[choice - 1];
                string filename = NOTES_DIR + subject + ".txt";
                
                cout << "\nEnter new content (type END on new line):\n";
                string content, line;
                while (true) {
                    getline(cin, line);
                    if (line == "END") break;
                    content += line + "\n";
                }
                
                ofstream file(filename);
                if (file.is_open()) {
                    file << content;
                    file.close();
                    Utility::printSuccess("Notes updated!");
                } else {
                    Utility::printError("Failed to update!");
                }
                
                Utility::pauseScreen();
            }
        } catch (...) {
            Utility::printError("Invalid input!");
            Utility::pauseScreen();
        }
    }
    
    void deleteNote() {
        Utility::printHeader("DELETE NOTES");
        
        cout << "Select subject:\n";
        for (size_t i = 0; i < subjects.size(); i++) {
            cout << i + 1 << ". " << subjects[i] << "\n";
        }
        
        cout << "\nChoice: ";
        string input;
        getline(cin, input);
        
        if (Utility::checkExit() || input.empty()) {
            return;
        }
        
        try {
            int choice = stoi(input);
            if (choice >= 1 && choice <= subjects.size()) {
                string subject = subjects[choice - 1];
                string filename = NOTES_DIR + subject + ".txt";
                
                cout << "Delete " << subject << " notes? (y/n): ";
                string confirm;
                getline(cin, confirm);
                
                if (Utility::toLower(confirm) == "y") {
                    if (remove(filename.c_str()) == 0) {
                        subjects.erase(subjects.begin() + (choice - 1));
                        Utility::printSuccess("Notes deleted!");
                    } else {
                        Utility::printError("Failed to delete!");
                    }
                }
                
                Utility::pauseScreen();
            }
        } catch (...) {
            Utility::printError("Invalid input!");
            Utility::pauseScreen();
        }
    }
    
private:
    void initializeDefaultNotes() {
        for (const auto& subject : subjects) {
            string filename = NOTES_DIR + subject + ".txt";
            if (!Utility::fileExists(filename)) {
                ofstream file(filename);
                file << "=== " << subject << " Notes ===\n\n";
                file << "Add your notes here.\n";
                file.close();
            }
        }
    }
};

// ========== TYPING TEST MANAGER CLASS ==========
class TypingTestManager {
private:
    const string TYPING_DIR = "typing_content/";
    vector<string> subjects = {"Physics", "Chemistry", "English", "Mathematics", "Biology", "General"};
    
public:
    TypingTestManager() {
        Utility::createDirectory(TYPING_DIR);
        initializeDefaultContent();
    }
    
    vector<string> getSubjects() const {
        return subjects;
    }
    
    string getTypingContent(const string& subject) {
        string filename = TYPING_DIR + subject + ".txt";
        ifstream file(filename);
        
        if (!file.is_open()) {
            createDefaultContentForSubject(subject);
            file.open(filename);
        }
        
        string content;
        string line;
        while (getline(file, line)) {
            content += line + " ";
        }
        file.close();
        
        return content;
    }
    
    void showTypingTestMenu() {
        while (true) {
            Utility::clearScreen();
            
            // Cool ASCII art header
            Utility::setColor(COLOR_PURPLE);
            cout << "==========================================================\n";
            cout << "          EEEEE  DDDD   U   U   QQQ   U   U  EEEEE  SSSS T\n";
            cout << "          E      D   D  U   U  Q   Q  U   U  E      S     \n";
            cout << "          EEEE   D   D  U   U  Q   Q  U   U  EEEE    SSS  \n";
            cout << "          E      D   D  U   U  Q  QQ  U   U  E          S \n";
            cout << "          EEEEE  DDDD    UUU    QQQQ   UUU   EEEEE  SSSS  \n";
            cout << "                                                          \n";
            cout << "          1-MINUTE TYPING CHALLENGE                       \n";
            cout << "          MONKEYTYPE STYLE - IMPROVE YOUR SPEED!          \n";
            cout << "                                                          \n";
            cout << "==========================================================\n";
            Utility::setColor(COLOR_RESET);
            
            cout << "\nSELECT SUBJECT FOR TYPING TEST:\n";
            cout << "═══════════════════════════════════════════════════\n\n";
            
            for (size_t i = 0; i < subjects.size(); i++) {
                cout << "  " << i + 1 << ". " << subjects[i] << "\n";
            }
            cout << "  " << subjects.size() + 1 << ". Back to Main Menu\n";
            
            cout << "\n═══════════════════════════════════════════════════\n";
            Utility::setColor(COLOR_YELLOW);
            cout << "\n  Choice (1-" << subjects.size() + 1 << "): ";
            Utility::setColor(COLOR_RESET);
            
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) {
                return;
            }
            
            try {
                int choice = stoi(input);
                if (choice == subjects.size() + 1) {
                    return;
                }
                if (choice >= 1 && choice <= subjects.size()) {
                    return; // Subject selected
                }
            } catch (...) {
                Utility::printError("Invalid input!");
                Utility::pauseScreen();
            }
        }
    }
    
    void runTimedTypingTest(const string& subject, User* currentUser) {
        string content = getTypingContent(subject);
        
        if (content.empty()) {
            Utility::printError("No typing content available!");
            Utility::pauseScreen();
            return;
        }
        
        // Split into words
        vector<string> allWords = splitIntoWords(content);
        if (allWords.size() < 20) {
            Utility::printError("Not enough words for test!");
            Utility::pauseScreen();
            return;
        }
        
        // Take first 100 words or all if less
        int maxWords = min(100, (int)allWords.size());
        vector<string> testWords(allWords.begin(), allWords.begin() + maxWords);
        
        // Display instructions
        Utility::clearScreen();
        Utility::setColor(COLOR_PURPLE);
        cout << "==========================================================\n";
        cout << "          EEEEE  DDDD   U   U   QQQ   U   U  EEEEE  SSSS T\n";
        cout << "          E      D   D  U   U  Q   Q  U   U  E      S     \n";
        cout << "          EEEE   D   D  U   U  Q   Q  U   U  EEEE    SSS  \n";
        cout << "          E      D   D  U   U  Q  QQ  U   U  E          S \n";
        cout << "          EEEEE  DDDD    UUU    QQQQ   UUU   EEEEE  SSSS  \n";
        cout << "                                                          \n";
        cout << "          TYPING TEST - " << left << setw(23) << subject << "\n";
        cout << "          1-MINUTE CHALLENGE                             \n";
        cout << "                                                          \n";
        cout << "==========================================================\n";
        Utility::setColor(COLOR_RESET);
        
        cout << "\nINSTRUCTIONS:\n";
        cout << "1. Type the text as it appears\n";
        cout << "2. Press SPACE after each word\n";
        cout << "3. Test lasts 1 minute\n";
        cout << "4. Press ESC to quit early\n";
        cout << "5. Focus on accuracy AND speed!\n\n";
        
        Utility::setColor(COLOR_YELLOW);
        cout << "Press ENTER to start the 1-minute timer...";
        Utility::setColor(COLOR_RESET);
        cin.ignore();
        cin.get();
        
        // Start the timed test
        runTypingTestCore(subject, testWords, currentUser);
    }
    
    void runTypingTestCore(const string& subject, const vector<string>& testWords, User* currentUser) {
        int totalWords = testWords.size();
        int currentWord = 0;
        int correctChars = 0;
        int totalTypedChars = 0;
        string currentInput = "";
        bool testActive = true;
        
        // Start timer (60 seconds)
        time_t startTime = time(nullptr);
        time_t endTime = startTime + 60; // 1 minute
        
        // Clear screen for test
        Utility::clearScreen();
        
        while (testActive && currentWord < totalWords) {
            // Check time
            time_t currentTime = time(nullptr);
            int timeLeft = endTime - currentTime;
            
            if (timeLeft <= 0) {
                testActive = false;
                break;
            }
            
            // Display test interface
            displayTestInterface(subject, testWords, currentWord, currentInput, 
                               timeLeft, correctChars, totalTypedChars);
            
            // Get input
            if (_kbhit()) {
                char ch = _getch();
                
                if (ch == 27) { // ESC key to quit
                    cout << "\n\n";
                    Utility::setColor(COLOR_YELLOW);
                    cout << "[!] Test cancelled by user!\n";
                    Utility::setColor(COLOR_RESET);
                    Utility::pauseScreen();
                    return;
                }
                
                if (ch == 8) { // Backspace
                    if (!currentInput.empty()) {
                        currentInput.pop_back();
                    }
                }
                else if (ch == 32) { // Space - check word
                    if (currentWord < totalWords) {
                        string targetWord = testWords[currentWord];
                        totalTypedChars += targetWord.length() + 1; // +1 for space
                        
                        if (currentInput == targetWord) {
                            correctChars += targetWord.length();
                        }
                        
                        currentWord++;
                        currentInput = "";
                    }
                }
                else if (isprint(ch)) { // Printable character
                    currentInput += ch;
                }
            }
            
            // Small delay to make it smooth
            Sleep(50);
        }
        
        // Calculate final results
        time_t finalTime = time(nullptr);
        double actualTime = difftime(finalTime, startTime);
        double minutes = actualTime / 60.0;
        
        // Words per minute
        double wpm = (minutes > 0) ? (currentWord / minutes) : 0;
        
        // Accuracy calculation
        double accuracy = (totalTypedChars > 0) ? (correctChars * 100.0 / totalTypedChars) : 0;
        
        // Show results
        showFinalResults(subject, currentWord, actualTime, wpm, accuracy, currentUser);
    }
    
    void updateTypingContent(const string& subject) {
        string filename = TYPING_DIR + subject + ".txt";
        
        ifstream file(filename);
        if (file.is_open()) {
            Utility::printHeader("UPDATE TYPING CONTENT - " + subject);
            cout << "Current content preview:\n";
            cout << "========================\n";
            
            string line;
            int lines = 0;
            while (getline(file, line) && lines < 5) {
                cout << line << "\n";
                lines++;
            }
            file.close();
            
            cout << "\n========================\n";
            cout << "\nEnter new content (type END on new line):\n";
            
            string newContent, lineInput;
            while (true) {
                getline(cin, lineInput);
                if (lineInput == "END") break;
                newContent += lineInput + "\n";
            }
            
            ofstream outFile(filename);
            if (outFile.is_open()) {
                outFile << newContent;
                outFile.close();
                Utility::printSuccess("Content updated!");
            }
        }
    }
    
    void deleteTypingContent(const string& subject) {
        string filename = TYPING_DIR + subject + ".txt";
        
        cout << "Delete typing content for " << subject << "? (y/n): ";
        string confirm;
        getline(cin, confirm);
        
        if (Utility::toLower(confirm) == "y") {
            if (remove(filename.c_str()) == 0) {
                Utility::printSuccess("Content deleted!");
            } else {
                Utility::printError("Failed to delete!");
            }
        }
    }
    
private:
    void initializeDefaultContent() {
        for (const auto& subject : subjects) {
            string filename = TYPING_DIR + subject + ".txt";
            if (!Utility::fileExists(filename)) {
                createDefaultContentForSubject(subject);
            }
        }
    }
    
    void createDefaultContentForSubject(const string& subject) {
        string filename = TYPING_DIR + subject + ".txt";
        ofstream file(filename);
        
        if (subject == "Physics") {
            file << "Motion is defined as the change in position of an object with respect to time.\n";
            file << "Force is any interaction that when unopposed will change the motion of an object.\n";
            file << "Newton's first law states that an object at rest stays at rest unless acted upon.\n";
            file << "Velocity is the speed of something in a given direction measured in meters per second.\n";
            file << "Acceleration is the rate of change of velocity per unit of time in physics.\n";
            file << "Gravity is a force that attracts a body toward the center of the earth or other mass.\n";
            file << "Energy exists in many forms such as kinetic potential thermal electrical chemical.\n";
            file << "Momentum is the product of the mass and velocity of an object in classical mechanics.\n";
            file << "Work is done when a force moves an object through a distance in the direction of force.\n";
            file << "Power is the rate at which work is done or energy is transferred over time.\n";
        }
                else if (subject == "Chemistry") {
            file << "Elements are pure substances that cannot be broken down into simpler substances.\n";
            file << "Atoms are the basic building blocks of all matter consisting of protons neutrons.\n";
            file << "Compounds are formed when two or more different elements chemically combine.\n";
            file << "Chemical reactions occur when substances interact to form new substances.\n";
            file << "The periodic table organizes elements according to their atomic number properties.\n";
            file << "Molecules are groups of atoms bonded together representing the smallest unit.\n";
            file << "Chemical bonds include ionic covalent and metallic bonds between atoms.\n";
            file << "Acids are substances that donate hydrogen ions while bases accept them.\n";
            file << "pH is a measure of how acidic or basic a solution is on a logarithmic scale.\n";
            file << "Chemical equations show reactants and products in a balanced format.\n";
        }
        else if (subject == "English") {
            file << "Grammar is the system and structure of a language including syntax morphology.\n";
            file << "Vocabulary refers to the set of words within a language that someone knows.\n";
            file << "Pronunciation is the way in which a word is spoken using correct sounds.\n";
            file << "Literature is written works especially those considered of superior quality.\n";
            file << "Punctuation marks help clarify meaning in written English sentences.\n";
            file << "Communication involves transmitting information through various methods.\n";
            file << "Comprehension is the ability to understand something complex or abstract.\n";
            file << "Writing skills include proper sentence structure paragraph organization.\n";
            file << "Reading fluency involves reading quickly accurately with proper expression.\n";
            file << "Critical thinking evaluates information and arguments in a logical manner.\n";
        }
        else if (subject == "Mathematics") {
            file << "Algebra uses letters and symbols to represent numbers in equations formulas.\n";
            file << "Geometry studies shapes sizes relative positions of figures and properties.\n";
            file << "Calculus is the mathematical study of continuous change through derivatives.\n";
            file << "Statistics involves collecting analyzing interpreting presenting data sets.\n";
            file << "Trigonometry deals with relationships between angles and sides of triangles.\n";
            file << "Arithmetic is the branch of mathematics dealing with numbers and operations.\n";
            file << "Probability measures the likelihood that an event will occur in mathematics.\n";
            file << "Linear equations represent straight lines when graphed on coordinate plane.\n";
            file << "Functions relate inputs to outputs with each input having exactly one output.\n";
            file << "Mathematical proofs provide logical arguments establishing truth of theorems.\n";
        }
        else if (subject == "Biology") {
            file << "Cells are the basic structural and functional units of all living organisms.\n";
            file << "DNA contains genetic instructions used in development functioning of organisms.\n";
            file << "Evolution is the change in characteristics of species over many generations.\n";
            file << "Photosynthesis converts light energy into chemical energy in green plants.\n";
            file << "Ecosystems consist of biological communities interacting with their environment.\n";
            file << "Genetics is the study of genes genetic variation and heredity in organisms.\n";
            file << "Metabolism includes all chemical reactions that occur within living cells.\n";
            file << "Homeostasis is the state of steady internal conditions maintained by organisms.\n";
            file << "Classification organizes organisms into hierarchical groups based on features.\n";
            file << "Physiology studies how living organisms their systems organs and cells function.\n";
        }
        else { // General
            file << "Practice makes perfect when it comes to improving typing speed and accuracy.\n";
            file << "The quick brown fox jumps over the lazy dog contains all alphabet letters.\n";
            file << "Typing skills are essential in today's digital world for communication work.\n";
            file << "Accuracy is more important than speed when beginning to learn typing skills.\n";
            file << "Regular practice sessions help develop muscle memory for keyboard positions.\n";
            file << "Proper posture and hand placement can prevent strain and improve efficiency.\n";
            file << "Touch typing involves typing without looking at the keyboard using muscle memory.\n";
            file << "Typing tests measure words per minute and accuracy percentage for assessment.\n";
            file << "Consistent practice over time yields better results than occasional sessions.\n";
            file << "Keyboard shortcuts can significantly improve productivity in computer usage.\n";
        }
        
        file.close();
    }
    
    vector<string> splitIntoWords(const string& text) {
        vector<string> words;
        string currentWord;
        
        for (char ch : text) {
            if (isalpha(ch) || isdigit(ch)) {
                currentWord += ch;
            } else if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        }
        
        if (!currentWord.empty()) {
            words.push_back(currentWord);
        }
        
        return words;
    }
    
    void displayTestInterface(const string& subject, const vector<string>& words, 
                             int currentWord, const string& currentInput, int timeLeft,
                             int correctChars, int totalTypedChars) {
        // Clear previous display
        cout << "\033[H"; // Move cursor to home
        
        Utility::setColor(COLOR_PURPLE);
        cout << "==========================================================\n";
        cout << "          TYPING TEST - " << left << setw(33) << subject << "\n";
        cout << "          TIME LEFT: " << setw(2) << timeLeft << " seconds                \n";
        cout << "==========================================================\n";
        Utility::setColor(COLOR_RESET);
        
        // Display current word and upcoming words
        cout << "\nTYPE THIS TEXT:\n";
        cout << "═══════════════════════════════════════════════════\n";
        
        Utility::setColor(COLOR_CYAN);
        // Show previous words (greyed out)
        int start = max(0, currentWord - 3);
        for (int i = start; i < currentWord; i++) {
            if (i > start) cout << " ";
            Utility::setColor(COLOR_WHITE);
            cout << words[i];
        }
        
        if (currentWord < words.size()) {
            if (currentWord > 0) cout << " ";
            
            // Show current word with highlighting
            string targetWord = words[currentWord];
            int matchLength = 0;
            
            // Check how many characters match
            for (size_t i = 0; i < min(currentInput.length(), targetWord.length()); i++) {
                if (currentInput[i] == targetWord[i]) {
                    matchLength++;
                } else {
                    break;
                }
            }
            
            // Display matched part in green
            Utility::setColor(COLOR_GREEN);
            cout << targetWord.substr(0, matchLength);
            
            // Display incorrect part in red
            if (currentInput.length() > matchLength) {
                Utility::setColor(COLOR_RED);
                if (matchLength < targetWord.length()) {
                    cout << targetWord[matchLength];
                }
            } else {
                // Display remaining part in cyan
                Utility::setColor(COLOR_CYAN);
                cout << targetWord.substr(matchLength);
            }
            
            // Display upcoming words
            Utility::setColor(COLOR_CYAN);
            int wordsToShow = min(10, (int)words.size() - currentWord - 1);
            for (int i = 1; i <= wordsToShow; i++) {
                cout << " " << words[currentWord + i];
            }
        }
        
        cout << "\n═══════════════════════════════════════════════════\n\n";
        
        // Display current input
        Utility::setColor(COLOR_YELLOW);
        cout << "YOUR TYPING: ";
        Utility::setColor(COLOR_RESET);
        cout << currentInput;
        
        if (currentInput.length() < words[currentWord].length()) {
            int remaining = words[currentWord].length() - currentInput.length();
            for (int i = 0; i < remaining; i++) cout << "_";
        }
        
        cout << "\n\n";
        
        // Display stats
        Utility::setColor(COLOR_CYAN);
        cout << "WORDS TYPED: " << currentWord;
        cout << "   |   ";
        
        // Calculate current WPM
        double minutesElapsed = (60 - timeLeft) / 60.0;
        double currentWPM = (minutesElapsed > 0) ? (currentWord / minutesElapsed) : 0;
        
        cout << "CURRENT WPM: " << fixed << setprecision(1) << currentWPM;
        cout << "   |   ";
        
        // Calculate current accuracy
        double currentAccuracy = (totalTypedChars > 0) ? 
            (correctChars * 100.0 / totalTypedChars) : 100.0;
        
        cout << "ACCURACY: " << fixed << setprecision(1) << currentAccuracy << "%\n";
        Utility::setColor(COLOR_RESET);
        
        cout << "\n═══════════════════════════════════════════════════\n";
        Utility::setColor(COLOR_YELLOW);
        cout << "Tip: Type the word and press SPACE. Press ESC to quit.\n";
        Utility::setColor(COLOR_RESET);
    }
    
    void showFinalResults(const string& subject, int wordsTyped, double timeTaken,
                         double wpm, double accuracy, User* currentUser) {
        Utility::clearScreen();
        
        Utility::setColor(COLOR_PURPLE);
        cout << "==========================================================\n";
        cout << "          TYPING TEST COMPLETE!                          \n";
        cout << "          SUBJECT: " << left << setw(30) << subject << "\n";
        cout << "==========================================================\n\n";
        Utility::setColor(COLOR_RESET);
        
        // Calculate performance rating
        string rating;
        int pointsEarned = 0;
        
        if (wpm >= 70 && accuracy >= 98) {
            rating = "EXPERT";
            pointsEarned = 50;
            Utility::setColor(COLOR_GREEN);
        } else if (wpm >= 50 && accuracy >= 95) {
            rating = "ADVANCED";
            pointsEarned = 30;
            Utility::setColor(COLOR_CYAN);
        } else if (wpm >= 30 && accuracy >= 90) {
            rating = "INTERMEDIATE";
            pointsEarned = 20;
            Utility::setColor(COLOR_YELLOW);
        } else if (wpm >= 15 && accuracy >= 80) {
            rating = "BEGINNER";
            pointsEarned = 10;
            Utility::setColor(COLOR_WHITE);
        } else {
            rating = "NEEDS PRACTICE";
            pointsEarned = 5;
            Utility::setColor(COLOR_RED);
        }
        
        // Display results
        cout << "RESULTS:\n";
        cout << "═══════════════════════════════════════════════════\n";
        cout << "  Words Typed:  " << wordsTyped << "\n";
        cout << "  Time Taken:   " << fixed << setprecision(1) << timeTaken << " seconds\n";
        cout << "  WPM:          " << fixed << setprecision(1) << wpm << "\n";
        cout << "  Accuracy:     " << fixed << setprecision(1) << accuracy << "%\n";
        cout << "  Rating:       " << rating << "\n";
        
        if (currentUser) {
            currentUser->addPoints(pointsEarned);
            cout << "  Points Earned:" << pointsEarned << "\n";
            cout << "  Total Points: " << currentUser->getPoints() << "\n";
        }
        
        cout << "═══════════════════════════════════════════════════\n\n";
        
        Utility::setColor(COLOR_RESET);
        
        // Display motivational message
        Utility::setColor(COLOR_CYAN);
        cout << "FEEDBACK:\n";
        if (wpm < 30) {
            cout << "  Keep practicing! Focus on accuracy first, then speed.\n";
        } else if (wpm < 50) {
            cout << "  Good progress! Try to maintain accuracy while increasing speed.\n";
        } else if (wpm < 70) {
            cout << "  Excellent! You're becoming a proficient typist.\n";
        } else {
            cout << "  Outstanding! You're among the fastest typists!\n";
        }
        Utility::setColor(COLOR_RESET);
        
        Utility::pauseScreen();
    }
};

// ========== LEARNING RESOURCES CLASS ==========
class LearningResources {
private:
    const string RESOURCES_DIR = "resources/";
    
public:
    LearningResources() {
        Utility::createDirectory(RESOURCES_DIR);
        initializeResources();
    }
    
    void showResourcesMenu() {
        while (true) {
            Utility::printHeader("LEARNING RESOURCES");
            
            cout << "1. Open YouTube Educational Channels\n";
            cout << "2. Open Khan Academy\n";
            cout << "3. Open Coursera\n";
            cout << "4. Open edX\n";
            cout << "5. Open Crash Course\n";
            cout << "6. Open MIT OpenCourseWare\n";
            cout << "7. Open TED Ed\n";
            cout << "8. Open Physics Classroom\n";
            cout << "9. Open Wolfram Alpha\n";
            cout << "10. Back to Main Menu\n\n";
            
            cout << "Choice: ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) {
                return;
            }
            
            try {
                int choice = stoi(input);
                
                switch (choice) {
                    case 1:
                        openYouTubeChannels();
                        break;
                    case 2:
                        system("start https://www.khanacademy.org");
                        break;
                    case 3:
                        system("start https://www.coursera.org");
                        break;
                    case 4:
                        system("start https://www.edx.org");
                        break;
                    case 5:
                        system("start https://www.youtube.com/c/crashcourse");
                        break;
                    case 6:
                        system("start https://ocw.mit.edu");
                        break;
                    case 7:
                        system("start https://ed.ted.com");
                        break;
                    case 8:
                        system("start https://www.physicsclassroom.com");
                        break;
                    case 9:
                        system("start https://www.wolframalpha.com");
                        break;
                    case 10:
                        return;
                    default:
                        Utility::printError("Invalid choice!");
                }
            } catch (...) {
                Utility::printError("Invalid input!");
            }
        }
    }
    
private:
    void initializeResources() {
        // Create resource files with links
        ofstream ytFile(RESOURCES_DIR + "youtube_channels.txt");
        if (ytFile.is_open()) {
            ytFile << "Top Educational YouTube Channels:\n\n";
            ytFile << "1. Crash Course - Comprehensive courses on various subjects\n";
            ytFile << "   https://www.youtube.com/c/crashcourse\n\n";
            ytFile << "2. Khan Academy - Free lessons on math, science, and more\n";
            ytFile << "   https://www.youtube.com/c/khanacademy\n\n";
            ytFile << "3. Numberphile - Mathematics videos\n";
            ytFile << "   https://www.youtube.com/user/numberphile\n\n";
            ytFile << "4. Physics Girl - Physics demonstrations and explanations\n";
            ytFile << "   https://www.youtube.com/c/physicswoman\n\n";
            ytFile << "5. Veritasium - Science education channel\n";
            ytFile << "   https://www.youtube.com/c/veritasium\n\n";
            ytFile << "6. 3Blue1Brown - Animated math explanations\n";
            ytFile << "   https://www.youtube.com/c/3blue1brown\n\n";
            ytFile << "7. SciShow - Answers to science questions\n";
            ytFile << "   https://www.youtube.com/c/scishow\n\n";
            ytFile << "8. TED-Ed - Educational animations\n";
            ytFile << "   https://www.youtube.com/c/TEDEducation\n\n";
            ytFile << "9. SmarterEveryDay - Science and engineering\n";
            ytFile << "   https://www.youtube.com/c/smartereveryday\n\n";
            ytFile << "10. MIT OpenCourseWare - University lectures\n";
            ytFile << "    https://www.youtube.com/c/mitocw\n";
            ytFile.close();
        }
    }
    
    void openYouTubeChannels() {
        Utility::printHeader("YOUTUBE EDUCATIONAL CHANNELS");
        
        ifstream file(RESOURCES_DIR + "youtube_channels.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                cout << line << "\n";
            }
            file.close();
        }
        
        cout << "\nPress Enter to open these channels in browser...";
        cin.get();
        
        // Open multiple educational channels
        string channels[] = {
            "https://www.youtube.com/c/crashcourse",
            "https://www.youtube.com/c/khanacademy",
            "https://www.youtube.com/c/veritasium",
            "https://www.youtube.com/c/3blue1brown",
            "https://www.youtube.com/c/TEDEducation"
        };
        
        for (const auto& channel : channels) {
            string command = "start " + channel;
            system(command.c_str());
            Sleep(100); // Small delay between openings
        }
        
        Utility::printInfo("Opening educational channels in browser...");
        Sleep(2000); // Wait a bit before returning
    }
};

// ========== ADMIN PANEL CLASS ==========
class AdminPanel {
private:
    UserManager* userManager;
    NotesManager* notesManager;
    TypingTestManager* typingManager;
    
public:
    AdminPanel(UserManager* um, NotesManager* nm, TypingTestManager* ttm)
        : userManager(um), notesManager(nm), typingManager(ttm) {}
    
    void showAdminMenu(User* admin) {
        while (true) {
            Utility::printHeader("ADMIN PANEL");
            
            admin->displayInfo();
            
            cout << "\nADMIN OPTIONS:\n";
            cout << "═══════════════════════════════════════════════════\n";
            cout << "1. View All Users\n";
            cout << "2. Delete User\n";
            cout << "3. Add Notes\n";
            cout << "4. Update Notes\n";
            cout << "5. Delete Notes\n";
            cout << "6. Update Typing Content\n";
            cout << "7. Delete Typing Content\n";
            cout << "8. Add Admin User\n";
            cout << "9. View Statistics\n";
            cout << "10. Back to Main Menu\n";
            cout << "═══════════════════════════════════════════════════\n\n";
            
            cout << "Choice: ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) {
                return;
            }
            
            try {
                int choice = stoi(input);
                
                switch (choice) {
                    case 1:
                        userManager->displayAllUsers();
                        Utility::pauseScreen();
                        break;
                    case 2:
                        deleteUser();
                        break;
                    case 3:
                        notesManager->uploadNote();
                        break;
                    case 4:
                        notesManager->updateNote();
                        break;
                    case 5:
                        notesManager->deleteNote();
                        break;
                    case 6:
                        updateTypingContent();
                        break;
                    case 7:
                        deleteTypingContent();
                        break;
                    case 8:
                        addAdminUser();
                        break;
                    case 9:
                        showStatistics();
                        break;
                    case 10:
                        return;
                    default:
                        Utility::printError("Invalid choice!");
                }
            } catch (...) {
                Utility::printError("Invalid input!");
            }
        }
    }
    
private:
    void deleteUser() {
        Utility::printHeader("DELETE USER");
        userManager->displayAllUsers();
        
        cout << "\nEnter username to delete: ";
        string username;
        getline(cin, username);
        
        if (username.empty() || Utility::checkExit()) {
            return;
        }
        
        if (userManager->userExists(username)) {
            cout << "Are you sure you want to delete user '" << username << "'? (y/n): ";
            string confirm;
            getline(cin, confirm);
            
            if (Utility::toLower(confirm) == "y") {
                userManager->deleteUser(username);
                Utility::printSuccess("User deleted successfully!");
            }
        } else {
            Utility::printError("User not found!");
        }
        
        Utility::pauseScreen();
    }
    
    void updateTypingContent() {
        Utility::printHeader("UPDATE TYPING CONTENT");
        
        auto subjects = typingManager->getSubjects();
        for (size_t i = 0; i < subjects.size(); i++) {
            cout << i + 1 << ". " << subjects[i] << "\n";
        }
        
        cout << "\nSelect subject: ";
        string input;
        getline(cin, input);
        
        if (input.empty() || Utility::checkExit()) {
            return;
        }
        
        try {
            int choice = stoi(input);
            if (choice >= 1 && choice <= subjects.size()) {
                typingManager->updateTypingContent(subjects[choice - 1]);
                Utility::pauseScreen();
            }
        } catch (...) {
            Utility::printError("Invalid input!");
            Utility::pauseScreen();
        }
    }
    
    void deleteTypingContent() {
        Utility::printHeader("DELETE TYPING CONTENT");
        
        auto subjects = typingManager->getSubjects();
        for (size_t i = 0; i < subjects.size(); i++) {
            cout << i + 1 << ". " << subjects[i] << "\n";
        }
        
        cout << "\nSelect subject: ";
        string input;
        getline(cin, input);
        
        if (input.empty() || Utility::checkExit()) {
            return;
        }
        
        try {
            int choice = stoi(input);
            if (choice >= 1 && choice <= subjects.size()) {
                typingManager->deleteTypingContent(subjects[choice - 1]);
                Utility::pauseScreen();
            }
        } catch (...) {
            Utility::printError("Invalid input!");
            Utility::pauseScreen();
        }
    }
    
    void addAdminUser() {
        Utility::printHeader("ADD ADMIN USER");
        
        cout << "Enter new admin username: ";
        string username;
        getline(cin, username);
        
        if (username.empty() || Utility::checkExit()) {
            return;
        }
        
        if (userManager->userExists(username)) {
            Utility::printError("Username already exists!");
            Utility::pauseScreen();
            return;
        }
        
        cout << "Enter password: ";
        string password = Utility::getPasswordInput();
        cout << "\n";
        
        if (password.empty()) {
            Utility::printError("Password cannot be empty!");
            Utility::pauseScreen();
            return;
        }
        
        cout << "Confirm password: ";
        string confirmPass = Utility::getPasswordInput();
        cout << "\n";
        
        if (password != confirmPass) {
            Utility::printError("Passwords don't match!");
            Utility::pauseScreen();
            return;
        }
        
        if (userManager->registerUser(username, password, "admin")) {
            Utility::printSuccess("Admin user created successfully!");
        } else {
            Utility::printError("Failed to create admin user!");
        }
        
        Utility::pauseScreen();
    }
    
    void showStatistics() {
        Utility::printHeader("SYSTEM STATISTICS");
        
        cout << "Users Statistics:\n";
        cout << "─────────────────────────────────────\n";
        cout << "Total Users: " << userManager->getUserCount() << "\n";
        
        // Note: In a real system, you would track more statistics
        cout << "\nSystem Status:\n";
        cout << "─────────────────────────────────────\n";
        cout << "Notes Module: ✓ Active\n";
        cout << "Typing Test:  ✓ Active\n";
        cout << "Resources:    ✓ Active\n";
        cout << "Admin Panel:  ✓ Active\n";
        
        cout << "\nStorage Information:\n";
        cout << "─────────────────────────────────────\n";
        cout << "Notes Directory: notes/\n";
        cout << "Typing Content: typing_content/\n";
        cout << "User Data: users.txt, admin.txt\n";
        
        Utility::pauseScreen();
    }
};

// ========== MAIN APPLICATION CLASS ==========
class EduQuestApp {
private:
    UserManager userManager;
    NotesManager notesManager;
    TypingTestManager typingManager;
    LearningResources resources;
    User* currentUser;
    AdminPanel* adminPanel;
    
public:
    EduQuestApp() : currentUser(nullptr) {
        adminPanel = new AdminPanel(&userManager, &notesManager, &typingManager);
        Utility::init();
        setupDefaultAdmin();
    }
    
    ~EduQuestApp() {
        delete adminPanel;
    }
    
    void run() {
        while (true) {
            showMainMenu();
        }
    }
    
private:
    void setupDefaultAdmin() {
        // Create default admin if not exists
        if (!userManager.userExists("admin")) {
            userManager.registerUser("admin", "admin123", "admin");
            Utility::printInfo("Default admin created: admin/admin123");
        }
    }
    
    void showMainMenu() {
        Utility::printHeader("EDUQUEST - EDUCATION & TYPING SYSTEM");
        
        if (currentUser) {
            currentUser->displayInfo();
        } else {
            Utility::setColor(COLOR_YELLOW);
            cout << "Not logged in. Please login or register.\n";
            Utility::setColor(COLOR_RESET);
        }
        
        cout << "\nMAIN MENU:\n";
        cout << "═══════════════════════════════════════════════════\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        if (currentUser) {
            cout << "3. View Study Notes\n";
            cout << "4. Typing Test Challenge\n";
            cout << "5. Learning Resources\n";
            if (currentUser->getRole() == "admin") {
                cout << "6. Admin Panel\n";
            }
            cout << "7. Logout\n";
            cout << "8. Exit\n";
        } else {
            cout << "3. Exit\n";
        }
        cout << "═══════════════════════════════════════════════════\n\n";
        
        cout << "Choice: ";
        string input;
        getline(cin, input);
        
        if (input.empty()) {
            return;
        }
        
        try {
            int choice = stoi(input);
            
            if (!currentUser) {
                // Not logged in
                switch (choice) {
                    case 1:
                        login();
                        break;
                    case 2:
                        registerUser();
                        break;
                    case 3:
                        exitApp();
                        break;
                    default:
                        Utility::printError("Invalid choice!");
                }
            } else {
                // Logged in
                switch (choice) {
                    case 1:
                        // Already logged in
                        break;
                    case 2:
                        // Already registered
                        break;
                    case 3:
                        notesManager.viewNotes();
                        break;
                    case 4:
                        typingManager.showTypingTestMenu();
                        break;
                    case 5:
                        resources.showResourcesMenu();
                        break;
                    case 6:
                        if (currentUser->getRole() == "admin") {
                            adminPanel->showAdminMenu(currentUser);
                        } else {
                            Utility::printError("Access denied!");
                            Utility::pauseScreen();
                        }
                        break;
                    case 7:
                        logout();
                        break;
                    case 8:
                        exitApp();
                        break;
                    default:
                        Utility::printError("Invalid choice!");
                }
            }
        } catch (...) {
            Utility::printError("Invalid input!");
        }
    }
    
    void login() {
        Utility::printHeader("LOGIN");
        
        cout << "Username: ";
        string username;
        getline(cin, username);
        
        if (username.empty() || Utility::checkExit()) {
            return;
        }
        
        cout << "Password: ";
        string password = Utility::getPasswordInput();
        cout << "\n";
        
        if (password.empty()) {
            return;
        }
        
        currentUser = userManager.login(username, password);
        
        if (currentUser) {
            Utility::printSuccess("Login successful!");
            Utility::pauseScreen("Press Enter to continue...");
        } else {
            Utility::printError("Invalid username or password!");
            Utility::pauseScreen();
        }
    }
    
    void registerUser() {
        Utility::printHeader("REGISTER");
        
        cout << "Username: ";
        string username;
        getline(cin, username);
        
        if (username.empty() || Utility::checkExit()) {
            return;
        }
        
        if (userManager.userExists(username)) {
            Utility::printError("Username already exists!");
            Utility::pauseScreen();
            return;
        }
        
        cout << "Password: ";
        string password = Utility::getPasswordInput();
        cout << "\n";
        
        if (password.empty()) {
            Utility::printError("Password cannot be empty!");
            Utility::pauseScreen();
            return;
        }
        
        cout << "Confirm password: ";
        string confirmPass = Utility::getPasswordInput();
        cout << "\n";
        
        if (password != confirmPass) {
            Utility::printError("Passwords don't match!");
            Utility::pauseScreen();
            return;
        }
        
        if (userManager.registerUser(username, password)) {
            Utility::printSuccess("Registration successful! Please login.");
        } else {
            Utility::printError("Registration failed!");
        }
        
        Utility::pauseScreen();
    }
    
    void logout() {
        if (currentUser) {
            // Save user data before logout
            userManager.updateUser(*currentUser);
            Utility::printSuccess("Logged out successfully!");
            currentUser = nullptr;
            Utility::pauseScreen();
        }
    }
    
    void exitApp() {
        Utility::printHeader("EXIT");
        cout << "Thank you for using EduQuest!\n";
        cout << "Goodbye!\n";
        exit(0);
    }
};

// ========== MAIN FUNCTION ==========
int main() {
    // Set console title
    system("title EduQuest - Education & Typing System");
    
    // Enable UTF-8 support for Windows
    system("chcp 65001 >nul");
    
    EduQuestApp app;
    app.run();
    
    return 0;
}
        