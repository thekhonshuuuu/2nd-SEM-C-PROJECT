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

const int COLOR_RESET = 7;
const int COLOR_RED = 12;
const int COLOR_GREEN = 10;
const int COLOR_YELLOW = 14;
const int COLOR_BLUE = 9;
const int COLOR_PURPLE = 13;
const int COLOR_CYAN = 11;
const int COLOR_WHITE = 15;

class User;
class Utility;

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
};

HANDLE Utility::hConsole;

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
            
            if (Utility::checkExit() || input.empty()) return;
            
            try {
                int choice = stoi(input);
                if (choice == subjects.size() + 1) return;
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
        
        if (Utility::checkExit() || input.empty()) return;
        
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
        
        if (Utility::checkExit() || input.empty()) return;
        
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
        
        if (Utility::checkExit() || input.empty()) return;
        
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

class TypingTestManager {
private:
    const string TYPING_DIR = "typing_content/";
    vector<string> subjects = {"Physics", "Chemistry", "English", "Mathematics", "Biology", "General"};
    
    struct TypingResult {
        int wordsTyped;
        int correctChars;
        int totalChars;
        double timeTaken;
        double wpm;
        double accuracy;
    };
    
public:
    TypingTestManager() {
        Utility::createDirectory(TYPING_DIR);
        initializeDefaultContent();
    }
    
    void showTypingMenu(User* user) {
        while (true) {
            Utility::clearScreen();
            Utility::setColor(COLOR_PURPLE);
            cout << "================================================\n";
            cout << "          EDU-TAINMENT TYPING CHALLENGE        \n";
            cout << "          1-MINUTE TIMED TEST                 \n";
            cout << "================================================\n\n";
            Utility::setColor(COLOR_RESET);
            
            cout << "SELECT SUBJECT:\n";
            for (size_t i = 0; i < subjects.size(); i++) {
                cout << i + 1 << ". " << subjects[i] << "\n";
            }
            cout << subjects.size() + 1 << ". Back\n";
            
            cout << "\nChoice: ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) return;
            
            try {
                int choice = stoi(input);
                if (choice == subjects.size() + 1) return;
                if (choice >= 1 && choice <= subjects.size()) {
                    runTypingTest(subjects[choice - 1], user);
                }
            } catch (...) {
                Utility::printError("Invalid input!");
                Utility::pauseScreen();
            }
        }
    }
    
    void runTypingTest(const string& subject, User* user) {
        string content = loadContent(subject);
        if (content.empty()) {
            Utility::printError("No content available!");
            Utility::pauseScreen();
            return;
        }
        
        vector<string> words = splitIntoWords(content);
        
        showInstructions();
        
        cout << "\nPress ENTER to start...";
        cin.get();
        
        TypingResult result = startTest(words, subject);
        showResults(result, user);
    }
    
private:
    string loadContent(const string& subject) {
        string filename = TYPING_DIR + subject + ".txt";
        ifstream file(filename);
        if (!file.is_open()) {
            createDefaultContent(subject);
            file.open(filename);
        }
        
        string content, line;
        while (getline(file, line)) {
            content += line + " ";
        }
        file.close();
        return content;
    }
    
    vector<string> splitIntoWords(const string& text) {
        vector<string> words;
        stringstream ss(text);
        string word;
        while (ss >> word) {
            words.push_back(word);
        }
        return words;
    }
    
    void showInstructions() {
        Utility::clearScreen();
        Utility::setColor(COLOR_CYAN);
        cout << "================================================\n";
        cout << "                INSTRUCTIONS                   \n";
        cout << "================================================\n\n";
        Utility::setColor(COLOR_RESET);
        cout << "1. Type the words as they appear\n";
        cout << "2. Press SPACE after each word\n";
        cout << "3. Test duration: 60 seconds\n";
        cout << "4. Focus on both speed and accuracy\n";
        cout << "5. Press ESC to quit early\n";
        cout << "\nWPM = (Words Typed / Time in Minutes)\n";
        cout << "Accuracy = (Correct Characters / Total) * 100%\n";
    }
    
    TypingResult startTest(const vector<string>& words, const string& subject) {
        int currentWord = 0;
        int correctChars = 0;
        int totalChars = 0;
        string currentInput = "";
        
        time_t startTime = time(nullptr);
        time_t endTime = startTime + 60;
        
        // Initial display
        Utility::clearScreen();
        displayTestScreen(words, currentWord, currentInput, 60, subject, correctChars, totalChars, true);
        
        while (currentWord < min(150, (int)words.size())) {
            time_t currentTime = time(nullptr);
            int timeLeft = endTime - currentTime;
            
            if (timeLeft <= 0) break;
            
            // Update only the changing parts
            if (displayTestScreen(words, currentWord, currentInput, timeLeft, subject, correctChars, totalChars, false)) {
                // Screen was redrawn
            }
            
            if (_kbhit()) {
                char ch = _getch();
                
                if (ch == 27) { // ESC
                    return {currentWord, correctChars, totalChars, 
                           difftime(currentTime, startTime), 0, 0};
                }
                
                if (ch == 8) { // Backspace
                    if (!currentInput.empty()) {
                        currentInput.pop_back();
                    }
                }
                else if (ch == 32) { // Space
                    if (currentWord < words.size()) {
                        string target = words[currentWord];
                        totalChars += target.length() + 1;
                        
                        if (currentInput == target) {
                            correctChars += target.length();
                        }
                        else {
                            // Count matching characters
                            int match = 0;
                            int len = min(currentInput.length(), target.length());
                            for (int i = 0; i < len; i++) {
                                if (currentInput[i] == target[i]) match++;
                            }
                            correctChars += match;
                        }
                        
                        currentWord++;
                        currentInput = "";
                    }
                }
                else if (isprint(ch)) {
                    currentInput += ch;
                }
            }
            
            Sleep(10);
        }
        
        time_t finishTime = time(nullptr);
        double timeTaken = difftime(finishTime, startTime);
        double minutes = timeTaken / 60.0;
        double wpm = (minutes > 0) ? (currentWord / minutes) : 0;
        double accuracy = (totalChars > 0) ? (correctChars * 100.0 / totalChars) : 0;
        
        return {currentWord, correctChars, totalChars, timeTaken, wpm, accuracy};
    }
    
    bool displayTestScreen(const vector<string>& words, int currentIndex,
                         const string& input, int timeLeft, const string& subject,
                         int correctChars, int totalChars, bool initial = false) {
        static int lastCurrentIndex = -1;
        static string lastInput = "";
        static int lastTimeLeft = -1;
        
        // Only redraw if something changed
        if (!initial && currentIndex == lastCurrentIndex && 
            input == lastInput && timeLeft == lastTimeLeft) {
            return false;
        }
        
        lastCurrentIndex = currentIndex;
        lastInput = input;
        lastTimeLeft = timeLeft;
        
        // Clear screen for initial display, otherwise update in place
        if (initial) {
            Utility::clearScreen();
        } else {
            // Move cursor to top
            COORD cursorPos = {0, 0};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
        }
        
        Utility::setColor(COLOR_PURPLE);
        cout << "================================================\n";
        cout << "  " << subject << " TYPING TEST | Time: " 
             << setw(2) << timeLeft << " sec\n";
        cout << "================================================\n\n";
        Utility::setColor(COLOR_RESET);
        
        // Display words (8-10 words at a time)
        int wordsToShow = 8;
        int start = currentIndex;
        int end = min(start + wordsToShow, (int)words.size());
        
        for (int i = start; i < end; i++) {
            if (i == currentIndex) {
                // Current word with highlighting
                string target = words[i];
                
                // Show typed part
                int correct = 0;
                int len = min(input.length(), target.length());
                for (int j = 0; j < len; j++) {
                    if (input[j] == target[j]) correct++;
                    else break;
                }
                
                if (correct > 0) {
                    Utility::setColor(COLOR_GREEN);
                    cout << target.substr(0, correct);
                }
                
                if (correct < target.length()) {
                    if (input.length() > correct) {
                        Utility::setColor(COLOR_RED);
                        cout << target[correct];
                        if (correct + 1 < target.length()) {
                            Utility::setColor(COLOR_CYAN);
                            cout << target.substr(correct + 1);
                        }
                    } else {
                        Utility::setColor(COLOR_CYAN);
                        cout << target.substr(correct);
                    }
                }
            } else {
                Utility::setColor(COLOR_CYAN);
                cout << words[i];
            }
            
            if (i < end - 1) cout << " ";
        }
        
        // Clear the "Your typing:" line and rewrite it
        cout << "\n\nYour typing: ";
        Utility::setColor(COLOR_YELLOW);
        
        // Display only the current word being typed, not previous words
        cout << input;
        
        // Display underscores for remaining characters in current word
        if (currentIndex < words.size()) {
            string target = words[currentIndex];
            int remaining = target.length() - input.length();
            if (remaining > 0) {
                for (int i = 0; i < remaining; i++) {
                    cout << "_";
                }
            }
        }
        
        cout << "\n\n";
        Utility::setColor(COLOR_RESET);
        
        // Calculate stats
        double accuracy = (totalChars > 0) ? (correctChars * 100.0 / totalChars) : 100.0;
        double timeElapsed = 60 - timeLeft;
        double minutesElapsed = timeElapsed / 60.0;
        double wpm = (minutesElapsed > 0) ? (currentIndex / minutesElapsed) : 0;
        
        cout << "Words: " << currentIndex;
        cout << " | WPM: " << fixed << setprecision(1) << wpm;
        cout << " | Accuracy: " << fixed << setprecision(1) << accuracy << "%";
        cout << "\n" << string(50, '-') << "\n";
        cout << "Press SPACE for next word | ESC to quit";
        
        // Clear any remaining lines
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        COORD cursorPos = {0, 10};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
        
        // Clear lines below if needed
        DWORD written;
        FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 
                                   csbi.dwSize.X * 10, cursorPos, &written);
        
        // Move cursor back to typing position
        cursorPos = {14, 6}; // Position after "Your typing: "
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
        
        return true;
    }
    
    void showResults(const TypingResult& result, User* user) {
        Utility::clearScreen();
        Utility::setColor(COLOR_PURPLE);
        cout << "================================================\n";
        cout << "               TEST RESULTS                    \n";
        cout << "================================================\n\n";
        Utility::setColor(COLOR_RESET);
        
        cout << fixed << setprecision(1);
        cout << "Words Typed:  " << result.wordsTyped << "\n";
        cout << "Time Taken:   " << result.timeTaken << " seconds\n";
        cout << "WPM:          " << result.wpm << "\n";
        cout << "Accuracy:     " << result.accuracy << "%\n";
        
        // Calculate points
        int points = 0;
        if (result.wpm >= 60 && result.accuracy >= 95) {
            points = 50;
            Utility::setColor(COLOR_GREEN);
            cout << "\nRating: EXPERT\n";
        } else if (result.wpm >= 40 && result.accuracy >= 90) {
            points = 30;
            Utility::setColor(COLOR_CYAN);
            cout << "\nRating: ADVANCED\n";
        } else if (result.wpm >= 25 && result.accuracy >= 85) {
            points = 20;
            Utility::setColor(COLOR_YELLOW);
            cout << "\nRating: INTERMEDIATE\n";
        } else if (result.wpm >= 15 && result.accuracy >= 80) {
            points = 10;
            Utility::setColor(COLOR_WHITE);
            cout << "\nRating: BEGINNER\n";
        } else {
            points = 5;
            Utility::setColor(COLOR_RED);
            cout << "\nRating: NEEDS PRACTICE\n";
        }
        
        if (user) {
            user->addPoints(points);
            cout << "Points Earned: " << points << "\n";
            cout << "Total Points: " << user->getPoints() << "\n";
        }
        
        Utility::setColor(COLOR_RESET);
        cout << "\n" << string(50, '-') << "\n";
        Utility::pauseScreen();
    }
    
    void createDefaultContent(const string& subject) {
        string filename = TYPING_DIR + subject + ".txt";
        ofstream file(filename);
        
        if (subject == "Physics") {
            file << "Physics studies matter energy and their interactions through fundamental forces. ";
            file << "Newton's laws describe motion with inertia force and action reaction principles. ";
            file << "Energy conservation states total energy remains constant in isolated systems forever. ";
            file << "Electromagnetism involves electric charges magnetic fields and electromagnetic waves. ";
            file << "Thermodynamics deals with heat temperature and their relation to energy and work. ";
            file << "Quantum mechanics explains particle behavior at atomic and subatomic tiny scales. ";
            file << "Relativity theory describes physics in non inertial frames and gravity effects. ";
            file << "Optics studies light behavior including reflection refraction and diffraction. ";
            file << "Nuclear physics examines atomic nuclei their structure and radioactive decay. ";
            file << "Astrophysics applies physics principles to astronomical objects and phenomena. ";
            file << "Kinematics analyzes motion without considering forces or masses of objects. ";
            file << "Dynamics studies motion causes considering forces acting upon moving bodies. ";
            file << "Mechanics branch deals with bodies motion under action of various forces. ";
            file << "Acoustics science explores sound properties including production transmission. ";
            file << "Fluid mechanics examines liquids gases behavior in motion and rest states. ";
            file << "Statistical mechanics connects microscopic properties to macroscopic observations. ";
            file << "Solid state physics investigates rigid matter crystals and their properties. ";
            file << "Plasma physics studies ionized gases and their collective behavior patterns. ";
            file << "Condensed matter physics explores matter phases with strong particle interactions. ";
            file << "Particle physics investigates elementary constituents of matter and forces. ";
            file << "Gravitation describes attraction between objects with mass in universe. ";
            file << "Momentum product of mass velocity remains constant without external forces. ";
            file << "Torque rotational force causes angular acceleration in rotating systems. ";
            file << "Work energy transfer occurs when force moves object through distance. ";
            file << "Power rate work done measures energy transfer speed over time. ";
        }
        else if (subject == "Chemistry") {
            file << "Chemistry explores matter properties composition structure and chemical transformations. ";
            file << "Elements consist of identical atoms distinguished by their atomic number protons. ";
            file << "Chemical bonds form through electron sharing transfer creating molecules compounds. ";
            file << "Periodic table organizes elements based on recurring chemical properties trends. ";
            file << "Chemical reactions involve breaking forming bonds yielding new different substances. ";
            file << "Acids bases and salts exhibit characteristic properties in aqueous solutions. ";
            file << "Organic chemistry studies carbon compounds including hydrocarbons functional groups. ";
            file << "Thermochemistry examines energy changes during chemical reactions and processes. ";
            file << "Electrochemistry involves chemical reactions producing electricity or vice versa. ";
            file << "Chemical kinetics studies reaction rates and factors influencing their speed. ";
            file << "Stoichiometry calculates reactant product quantities in chemical reactions. ";
            file << "Atomic structure includes protons neutrons electrons in specific arrangements. ";
            file << "Molecular geometry describes three dimensional shapes of chemical molecules. ";
            file << "Chemical equilibrium occurs when forward reverse reaction rates equalize. ";
            file << "Solutions homogeneous mixtures contain solute dissolved in solvent medium. ";
            file << "Colligative properties depend on solute concentration not its identity. ";
            file << "Chemical thermodynamics studies energy transformations during chemical processes. ";
            file << "Coordination chemistry examines complexes containing central metal atom. ";
            file << "Analytical chemistry identifies measures chemical components in samples. ";
            file << "Biochemistry studies chemical processes within living biological organisms. ";
            file << "Polymer chemistry investigates large molecules composed repeating subunits. ";
            file << "Nuclear chemistry examines changes occurring within atomic nuclei structures. ";
            file << "Environmental chemistry studies chemical phenomena occurring in nature. ";
            file << "Materials science explores properties applications of various substances. ";
            file << "Spectroscopy analyzes interaction between matter and electromagnetic radiation. ";
        }
        else if (subject == "Mathematics") {
            file << "Mathematics uses logical reasoning to study quantities structures patterns changes. ";
            file << "Algebra manipulates symbols variables equations to solve mathematical problems. ";
            file << "Geometry analyzes shapes sizes positions angles and dimensions of figures. ";
            file << "Calculus explores rates of change through derivatives and accumulation via integrals. ";
            file << "Statistics collects analyzes interprets presents and organizes numerical data. ";
            file << "Trigonometry examines relationships between angles and sides of triangles. ";
            file << "Probability quantifies likelihood of events occurring in random experiments. ";
            file << "Number theory studies integers and their properties relationships patterns. ";
            file << "Linear algebra investigates vectors matrices linear equations transformations. ";
            file << "Differential equations describe relationships involving functions and derivatives. ";
            file << "Set theory foundation mathematics deals with collections of distinct objects. ";
            file << "Combinatorics studies counting arrangement and combination possibilities. ";
            file << "Topology explores properties preserved through continuous deformations stretching. ";
            file << "Complex analysis examines functions of complex variables with applications. ";
            file << "Numerical analysis develops algorithms for solving mathematical problems. ";
            file << "Graph theory studies networks vertices edges and their connections. ";
            file << "Mathematical logic explores reasoning validity proof and computability concepts. ";
            file << "Optimization finds best solutions from available feasible alternative options. ";
            file << "Cryptography secures communication through encryption decryption techniques. ";
            file << "Game theory analyzes strategic interactions between rational decision makers. ";
            file << "Fractal geometry studies infinitely complex patterns repeating at different scales. ";
            file << "Mathematical modeling represents real world systems using mathematical concepts. ";
            file << "Abstract algebra studies algebraic structures groups rings and fields. ";
            file << "Real analysis rigorously examines real numbers sequences series functions. ";
            file << "Functional analysis studies vector spaces endowed with limit related structures. ";
        }
        else if (subject == "Biology") {
            file << "Biology studies living organisms including their structure function growth origin. ";
            file << "Cells represent basic structural functional units of all known living organisms. ";
            file << "DNA molecules carry genetic instructions used in development functioning of life. ";
            file << "Evolution explains diversity of life through natural selection genetic variation. ";
            file << "Ecosystems comprise biological communities interacting with physical environments. ";
            file << "Genetics examines heredity gene variation in living organisms and their traits. ";
            file << "Physiology explores mechanical physical biochemical functions of living systems. ";
            file << "Taxonomy classifies organisms into hierarchical groups based on shared characteristics. ";
            file << "Ecology investigates interactions among organisms and their physical surroundings. ";
            file << "Biochemistry studies chemical processes within and relating to living organisms. ";
            file << "Microbiology examines microscopic organisms including bacteria viruses fungi. ";
            file << "Botany scientific study of plants including their physiology structure genetics. ";
            file << "Zoology branch biology dealing with animal kingdom structure and behavior. ";
            file << "Anatomy studies structure organisms and their parts tissues organs systems. ";
            file << "Embryology examines development embryos from fertilization until birth. ";
            file << "Immunology explores immune system responses to pathogens and foreign substances. ";
            file << "Neuroscience studies nervous system structure development function disorders. ";
            file << "Marine biology examines organisms ecosystems in oceans and saltwater environments. ";
            file << "Molecular biology investigates molecular basis biological activity within cells. ";
            file << "Cell biology examines structure function physiological properties of cells. ";
            file << "Developmental biology studies process growth and development of organisms. ";
            file << "Evolutionary biology explores evolutionary processes producing diversity life. ";
            file << "Population biology examines dynamics species populations and interactions. ";
            file << "Conservation biology studies biodiversity protection endangered species habitats. ";
            file << "Biotechnology uses living systems organisms develop make useful products. ";
        }
        else if (subject == "English") {
            file << "English language proficiency involves grammar vocabulary reading writing skills. ";
            file << "Grammar establishes structural rules governing composition of clauses phrases words. ";
            file << "Vocabulary represents set of familiar words within person's particular language. ";
            file << "Literature encompasses written works especially those considered artistic value. ";
            file << "Comprehension involves understanding interpreting written or spoken information. ";
            file << "Communication transmits information through various verbal and nonverbal methods. ";
            file << "Punctuation marks clarify meaning by indicating pauses separating sentence elements. ";
            file << "Syntax studies arrangement of words phrases to create well formed sentences. ";
            file << "Phonetics examines physical sounds of human speech including their production. ";
            file << "Semantics explores meaning in language including word sense and sentence logic. ";
            file << "Phonology studies sound patterns within particular languages and systems. ";
            file << "Morphology analyzes word structure formation and meaningful components. ";
            file << "Pragmatics examines how context contributes to language meaning understanding. ";
            file << "Discourse analysis studies written spoken language use beyond sentence level. ";
            file << "Stylistics explores linguistic style variation in different contexts uses. ";
            file << "Rhetoric studies principles rules governing effective persuasive communication. ";
            file << "Poetry literary form uses aesthetic rhythmic qualities language evoke meaning. ";
            file << "Prose written spoken language follows natural flow speech grammatical structure. ";
            file << "Fiction narrative literature describes imaginary events and people characters. ";
            file << "Nonfiction prose writing based on facts real events and actual people. ";
            file << "Drama literature intended performance by actors on stage theater. ";
            file << "Essay short piece writing on particular subject expressing author views. ";
            file << "Biography detailed description person's life written by someone else. ";
            file << "Autobiography account person's life written by that individual themselves. ";
            file << "Journalism collection editing presentation news information newspapers media. ";
            file << "Translation process rendering text from one language into another accurately. ";
        }
        else { // General
            file << "The quick brown fox jumps over the lazy dog repeatedly during practice sessions. ";
            file << "Typing speed improves through consistent practice and proper finger positioning. ";
            file << "Accuracy remains crucial for effective communication and professional documents. ";
            file << "Regular exercises build muscle memory enhancing both speed and precision over time. ";
            file << "Proper posture and ergonomic equipment prevent strain during extended typing periods. ";
            file << "Keyboard shortcuts increase productivity by reducing reliance on mouse navigation. ";
            file << "Touch typing involves typing without looking at keyboard keys for faster input. ";
            file << "Practice sessions should include varied texts to adapt to different word patterns. ";
            file << "Monitoring words per minute helps track progress and identify improvement areas. ";
            file << "Consistent practice yields better results than occasional intensive typing sessions. ";
            file << "Typing proficiency enhances job opportunities and academic performance significantly. ";
            file << "Different keyboard layouts exist including QWERTY DVORAK and Colemak arrangements. ";
            file << "Finger placement home row keys provide reference points for efficient typing. ";
            file << "Typing tests measure speed accuracy helping set realistic improvement goals. ";
            file << "Online resources offer free typing lessons games for skill development. ";
            file << "Professional typists often exceed hundred words per minute with high accuracy. ";
            file << "Typing competitions showcase incredible speeds reaching two hundred words. ";
            file << "Learning typing young creates lasting skills benefiting entire lifetime career. ";
            file << "Mobile devices require different typing techniques than traditional keyboards. ";
            file << "Voice recognition software complements but does not replace typing skills. ";
            file << "Programming typing involves special characters symbols requiring practice. ";
            file << "Data entry jobs demand both high speed and perfect accuracy standards. ";
            file << "Transcription work requires excellent listening and fast typing abilities. ";
            file << "Closed captioning realtime typing needs exceptional speed and concentration. ";
            file << "Court reporters use specialized machines for verbatim transcription. ";
        }
        
        file.close();
    }
    
    void initializeDefaultContent() {
        for (const auto& subject : subjects) {
            string filename = TYPING_DIR + subject + ".txt";
            if (!Utility::fileExists(filename)) {
                createDefaultContent(subject);
            }
        }
    }
};

class QuizManager {
private:
    struct QuizQuestion {
        string question;
        vector<string> options;
        int correctAnswer;
    };
    
    map<string, vector<QuizQuestion>> subjectQuizzes;
    
public:
    QuizManager() {
        initializeQuizzes();
    }
    
    void showQuizMenu(User* user) {
        while (true) {
            Utility::printHeader("QUIZ CHALLENGE");
            cout << "Select Subject:\n";
            cout << "1. Physics\n";
            cout << "2. Chemistry\n";
            cout << "3. English\n";
            cout << "4. Mathematics\n";
            cout << "5. Biology\n";
            cout << "6. Back\n\n";
            
            cout << "Choice: ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) return;
            
            try {
                int choice = stoi(input);
                string subject;
                
                switch (choice) {
                    case 1: subject = "Physics"; break;
                    case 2: subject = "Chemistry"; break;
                    case 3: subject = "English"; break;
                    case 4: subject = "Mathematics"; break;
                    case 5: subject = "Biology"; break;
                    case 6: return;
                    default: 
                        Utility::printError("Invalid choice!");
                        Utility::pauseScreen();
                        continue;
                }
                
                runQuiz(subject, user);
                
            } catch (...) {
                Utility::printError("Invalid input!");
                Utility::pauseScreen();
            }
        }
    }
    
private:
    void runQuiz(const string& subject, User* user) {
        if (subjectQuizzes.find(subject) == subjectQuizzes.end()) {
            Utility::printError("No quizzes available!");
            Utility::pauseScreen();
            return;
        }
        
        vector<QuizQuestion> questions = subjectQuizzes[subject];
        int score = 0;
        int totalQuestions = min(10, (int)questions.size());
        
        Utility::printHeader(subject + " QUIZ");
        cout << "You will answer " << totalQuestions << " questions.\n";
        cout << "Each correct answer gives 5 points.\n\n";
        Utility::pauseScreen("Press Enter to start...");
        
        for (int i = 0; i < totalQuestions; i++) {
            Utility::clearScreen();
            cout << "Question " << (i + 1) << " of " << totalQuestions << "\n";
            cout << "Score: " << score << "/" << (i * 5) << "\n\n";
            
            QuizQuestion q = questions[i];
            cout << q.question << "\n\n";
            
            for (size_t j = 0; j < q.options.size(); j++) {
                cout << (j + 1) << ". " << q.options[j] << "\n";
            }
            
            cout << "\nYour answer (1-" << q.options.size() << "): ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) {
                return;
            }
            
            try {
                int answer = stoi(input);
                if (answer == q.correctAnswer) {
                    Utility::setColor(COLOR_GREEN);
                    cout << "\nCorrect! +5 points\n";
                    score += 5;
                    if (user) user->addPoints(5);
                } else {
                    Utility::setColor(COLOR_RED);
                    cout << "\nIncorrect! Correct answer: " << q.correctAnswer << "\n";
                }
                Utility::setColor(COLOR_RESET);
                Utility::pauseScreen();
            } catch (...) {
                Utility::printError("Invalid input! Skipping...");
                Utility::pauseScreen();
            }
        }
        
        // Show final results
        Utility::clearScreen();
        Utility::setColor(COLOR_PURPLE);
        cout << "================================================\n";
        cout << "               QUIZ RESULTS                     \n";
        cout << "================================================\n\n";
        Utility::setColor(COLOR_RESET);
        
        cout << "Subject: " << subject << "\n";
        cout << "Questions: " << totalQuestions << "\n";
        cout << "Score: " << score << "/" << (totalQuestions * 5) << "\n";
        cout << "Percentage: " << (score * 100 / (totalQuestions * 5)) << "%\n\n";
        
        if (score >= 40) {
            Utility::setColor(COLOR_GREEN);
            cout << "Excellent! You're an expert!\n";
        } else if (score >= 30) {
            Utility::setColor(COLOR_YELLOW);
            cout << "Good job! Keep learning!\n";
        } else {
            Utility::setColor(COLOR_RED);
            cout << "Keep studying! You can do better!\n";
        }
        
        Utility::setColor(COLOR_RESET);
        if (user) {
            cout << "\nTotal points earned: " << score << "\n";
            cout << "Your total points: " << user->getPoints() << "\n";
        }
        
        cout << "\n" << string(50, '-') << "\n";
        Utility::pauseScreen();
    }
    
    void initializeQuizzes() {
        // Physics Quiz
        vector<QuizQuestion> physicsQuiz = {
            {"What is the SI unit of force?", {"Newton", "Joule", "Watt", "Pascal"}, 1},
            {"Which law states that every action has an equal and opposite reaction?", 
             {"Newton's First Law", "Newton's Second Law", "Newton's Third Law", "Law of Gravity"}, 3},
            {"What does 'm' represent in E=mc²?", {"Mass", "Momentum", "Matter", "Magnitude"}, 1},
            {"Which type of energy is stored in a compressed spring?", 
             {"Kinetic", "Thermal", "Potential", "Electrical"}, 3},
            {"What is the speed of light in vacuum?", 
             {"300,000 km/s", "150,000 km/s", "450,000 km/s", "600,000 km/s"}, 1},
            {"Which particle has a positive charge?", {"Electron", "Neutron", "Proton", "Photon"}, 3},
            {"What is the study of motion called?", {"Dynamics", "Kinematics", "Statics", "Thermodynamics"}, 2},
            {"Which law states that energy cannot be created or destroyed?", 
             {"First Law of Thermodynamics", "Second Law of Thermodynamics", 
              "Law of Conservation of Energy", "Ohm's Law"}, 3},
            {"What is the unit of electrical resistance?", {"Volt", "Ampere", "Ohm", "Watt"}, 3},
            {"Which scientist developed the theory of relativity?", 
             {"Newton", "Einstein", "Galileo", "Tesla"}, 2}
        };
        subjectQuizzes["Physics"] = physicsQuiz;
        
        // Chemistry Quiz
        vector<QuizQuestion> chemistryQuiz = {
            {"What is the atomic number of oxygen?", {"6", "7", "8", "9"}, 3},
            {"Which gas is most abundant in Earth's atmosphere?", 
             {"Oxygen", "Carbon Dioxide", "Nitrogen", "Argon"}, 3},
            {"What is H2O commonly known as?", {"Hydrogen Peroxide", "Water", "Hydroxide", "Hydrate"}, 2},
            {"Which element has the chemical symbol 'Au'?", {"Silver", "Gold", "Aluminum", "Argon"}, 2},
            {"What is the pH of pure water?", {"5", "6", "7", "8"}, 3},
            {"Which process converts liquid to gas?", {"Condensation", "Evaporation", "Sublimation", "Freezing"}, 2},
            {"What is the lightest element?", {"Helium", "Hydrogen", "Lithium", "Oxygen"}, 2},
            {"Which gas do plants absorb during photosynthesis?", 
             {"Oxygen", "Carbon Dioxide", "Nitrogen", "Hydrogen"}, 2},
            {"What is the main component of natural gas?", {"Propane", "Butane", "Methane", "Ethane"}, 3},
            {"Which metal is liquid at room temperature?", {"Gallium", "Mercury", "Sodium", "Potassium"}, 2}
        };
        subjectQuizzes["Chemistry"] = chemistryQuiz;
        
        // English Quiz
        vector<QuizQuestion> englishQuiz = {
            {"Which word is a noun?", {"Run", "Beautiful", "Quickly", "House"}, 4},
            {"What is the past tense of 'go'?", {"Goed", "Went", "Gone", "Going"}, 2},
            {"Which sentence is correct?", 
             {"She don't like apples", "She doesn't likes apples", 
              "She doesn't like apples", "She not like apples"}, 3},
            {"What is the synonym of 'happy'?", {"Sad", "Joyful", "Angry", "Tired"}, 2},
            {"Which is a preposition?", {"And", "The", "In", "Quickly"}, 3},
            {"What is the plural of 'child'?", {"Childs", "Children", "Childes", "Child's"}, 2},
            {"Which word is spelled correctly?", {"Recieve", "Receive", "Recieve", "Receve"}, 2},
            {"What is the opposite of 'ancient'?", {"Old", "Modern", "Historic", "Traditional"}, 2},
            {"Which sentence is in passive voice?", 
             {"The cat chased the mouse", "The mouse was chased by the cat", 
              "Chasing the mouse, the cat", "The cat and the mouse"}, 2},
            {"What type of word is 'quickly'?", {"Noun", "Verb", "Adjective", "Adverb"}, 4}
        };
        subjectQuizzes["English"] = englishQuiz;
        
        // Mathematics Quiz
        vector<QuizQuestion> mathQuiz = {
            {"What is 15 × 6?", {"80", "90", "100", "110"}, 2},
            {"What is the value of π to two decimal places?", {"3.12", "3.14", "3.16", "3.18"}, 2},
            {"Solve: 2x + 5 = 15", {"x = 5", "x = 10", "x = 7.5", "x = 5.5"}, 1},
            {"What is the area of a rectangle with length 8 and width 5?", 
             {"13", "26", "40", "35"}, 3},
            {"What is 3/4 as a percentage?", {"25%", "50%", "75%", "100%"}, 3},
            {"What is the square root of 144?", {"11", "12", "13", "14"}, 2},
            {"How many degrees in a right angle?", {"45", "90", "180", "360"}, 2},
            {"What is 7² + 8²?", {"49", "64", "113", "15"}, 3},
            {"If a triangle has angles 60° and 70°, what is the third angle?", 
             {"40°", "50°", "60°", "70°"}, 2},
            {"What is the next number: 2, 4, 8, 16, ?", {"24", "28", "32", "36"}, 3}
        };
        subjectQuizzes["Mathematics"] = mathQuiz;
        
        // Biology Quiz
        vector<QuizQuestion> biologyQuiz = {
            {"What is the powerhouse of the cell?", 
             {"Nucleus", "Mitochondria", "Ribosome", "Cell Membrane"}, 2},
            {"How many bones are in the adult human body?", {"156", "206", "256", "306"}, 2},
            {"Which blood type is the universal donor?", {"A", "B", "AB", "O"}, 4},
            {"What gas do humans breathe out?", {"Oxygen", "Carbon Dioxide", "Nitrogen", "Helium"}, 2},
            {"Which organ pumps blood throughout the body?", {"Lung", "Liver", "Heart", "Kidney"}, 3},
            {"What is the study of living organisms called?", {"Geology", "Biology", "Chemistry", "Physics"}, 2},
            {"Which is not a type of blood cell?", {"Red blood cell", "White blood cell", "Platelet", "Neuron"}, 4},
            {"What is the largest organ in the human body?", {"Liver", "Heart", "Skin", "Brain"}, 3},
            {"Which part of the plant conducts photosynthesis?", {"Root", "Stem", "Leaf", "Flower"}, 3},
            {"How many chambers does the human heart have?", {"2", "3", "4", "5"}, 3}
        };
        subjectQuizzes["Biology"] = biologyQuiz;
    }
};

class LearningResources {
public:
    void showResources() {
        while (true) {
            Utility::printHeader("LEARNING RESOURCES");
            cout << "1. Khan Academy - Free online courses\n";
            cout << "2. Coursera - University courses\n";
            cout << "3. edX - Online learning platform\n";
            cout << "4. YouTube Education - Video tutorials\n";
            cout << "5. Physics Simulations (Open main.html)\n";
            cout << "6. Back to Main Menu\n\n";
            
            cout << "Choice: ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) return;
            
            try {
                int choice = stoi(input);
                switch (choice) {
                    case 1: 
                        system("start https://www.khanacademy.org");
                        Utility::printInfo("Opening Khan Academy...");
                        Sleep(1000);
                        break;
                    case 2: 
                        system("start https://www.coursera.org");
                        Utility::printInfo("Opening Coursera...");
                        Sleep(1000);
                        break;
                    case 3: 
                        system("start https://www.edx.org");
                        Utility::printInfo("Opening edX...");
                        Sleep(1000);
                        break;
                    case 4: 
                        system("start https://www.youtube.com/education");
                        Utility::printInfo("Opening YouTube Education...");
                        Sleep(1000);
                        break;
                    case 5:
                        openPhysicsSimulations();
                        break;
                    case 6: 
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
    void openPhysicsSimulations() {
        string htmlFile = "main.html";
        
        if (Utility::fileExists(htmlFile)) {
            string command = "start \"\" \"" + htmlFile + "\"";
            system(command.c_str());
            Utility::printSuccess("Opening Physics Simulations in browser...");
        } else {
            Utility::printError("main.html file not found!");
            Utility::printInfo("Please make sure main.html is in the same directory.");
            Utility::printInfo("Creating a simple HTML file instead...");
            
            // Create a basic HTML file if main.html doesn't exist
            ofstream file(htmlFile);
            file << "<!DOCTYPE html>\n";
            file << "<html>\n";
            file << "<head><title>Physics Simulations</title></head>\n";
            file << "<body>\n";
            file << "<h1>Physics Simulations</h1>\n";
            file << "<p>This is the physics simulations page.</p>\n";
            file << "<p>Replace this file with your actual main.html file.</p>\n";
            file << "</body>\n";
            file << "</html>\n";
            file.close();
            
            string command = "start \"\" \"" + htmlFile + "\"";
            system(command.c_str());
            Utility::printInfo("Created and opened a basic HTML file.");
        }
        
        Utility::pauseScreen();
    }
};

class AdminPanel {
private:
    UserManager* userManager;
    NotesManager* notesManager;
    TypingTestManager* typingManager;
    
public:
    AdminPanel(UserManager* um, NotesManager* nm, TypingTestManager* ttm)
        : userManager(um), notesManager(nm), typingManager(ttm) {}
    
    void showMenu(User* admin) {
        while (true) {
            Utility::printHeader("ADMIN PANEL");
            cout << "1. View Users\n";
            cout << "2. Delete User\n";
            cout << "3. Manage Notes\n";
            cout << "4. Back\n\n";
            
            cout << "Choice: ";
            string input;
            getline(cin, input);
            
            if (Utility::checkExit() || input.empty()) return;
            
            try {
                int choice = stoi(input);
                switch (choice) {
                    case 1: userManager->displayAllUsers(); Utility::pauseScreen(); break;
                    case 2: deleteUser(); break;
                    case 3: manageNotes(); break;
                    case 4: return;
                    default: Utility::printError("Invalid choice!");
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
        
        cout << "\nUsername to delete: ";
        string username;
        getline(cin, username);
        
        if (username.empty()) return;
        
        if (userManager->userExists(username)) {
            cout << "Confirm delete '" << username << "'? (y/n): ";
            string confirm;
            getline(cin, confirm);
            
            if (Utility::toLower(confirm) == "y") {
                userManager->deleteUser(username);
                Utility::printSuccess("User deleted!");
            }
        } else {
            Utility::printError("User not found!");
        }
        
        Utility::pauseScreen();
    }
    
    void manageNotes() {
        NotesManager nm;
        Utility::printHeader("MANAGE NOTES");
        cout << "1. Upload Notes\n";
        cout << "2. Update Notes\n";
        cout << "3. Delete Notes\n";
        cout << "4. Back\n\n";
        
        cout << "Choice: ";
        string input;
        getline(cin, input);
        
        if (Utility::checkExit() || input.empty()) return;
        
        try {
            int choice = stoi(input);
            switch (choice) {
                case 1: nm.uploadNote(); break;
                case 2: nm.updateNote(); break;
                case 3: nm.deleteNote(); break;
                case 4: return;
                default: Utility::printError("Invalid choice!");
            }
        } catch (...) {
            Utility::printError("Invalid input!");
        }
    }
};

class EdutainmentApp {
private:
    UserManager userManager;
    NotesManager notesManager;
    TypingTestManager typingManager;
    QuizManager quizManager;
    LearningResources resources;
    AdminPanel adminPanel;
    User* currentUser;
    
public:
    EdutainmentApp() : adminPanel(&userManager, &notesManager, &typingManager), currentUser(nullptr) {
        Utility::init();
        setupAdmin();
    }
    
    void run() {
        while (true) {
            showMainMenu();
        }
    }
    
private:
    void setupAdmin() {
        if (!userManager.userExists("admin")) {
            userManager.registerUser("admin", "admin123", "admin");
        }
    }
    
    void showMainMenu() {
        Utility::printHeader("EDU-TAINMENT SYSTEM");
        
        if (currentUser) {
            currentUser->displayInfo();
            cout << "\n1. Study Notes\n";
            cout << "2. Typing Challenge\n";
            cout << "3. Subject Quizzes\n";
            cout << "4. Learning Resources\n";
            if (currentUser->getRole() == "admin") {
                cout << "5. Admin Panel\n";
                cout << "6. Logout\n";
                cout << "7. Exit\n";
            } else {
                cout << "5. Logout\n";
                cout << "6. Exit\n";
            }
        } else {
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. Exit\n";
        }
        
        cout << "\nChoice: ";
        string input;
        getline(cin, input);
        
        if (input.empty()) return;
        
        try {
            int choice = stoi(input);
            
            if (!currentUser) {
                switch (choice) {
                    case 1: login(); break;
                    case 2: registerUser(); break;
                    case 3: exit(0); break;
                }
            } else {
                switch (choice) {
                    case 1: notesManager.viewNotes(); break;
                    case 2: typingManager.showTypingMenu(currentUser); break;
                    case 3: quizManager.showQuizMenu(currentUser); break;
                    case 4: resources.showResources(); break;
                    case 5: 
                        if (currentUser->getRole() == "admin") {
                            adminPanel.showMenu(currentUser);
                        } else {
                            logout();
                        }
                        break;
                    case 6: 
                        if (currentUser->getRole() == "admin") {
                            logout();
                        } else {
                            exit(0);
                        }
                        break;
                    case 7: 
                        if (currentUser->getRole() == "admin") {
                            exit(0);
                        }
                        break;
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
        
        if (username.empty()) return;
        
        cout << "Password: ";
        string password = Utility::getPasswordInput();
        cout << "\n";
        
        currentUser = userManager.login(username, password);
        
        if (currentUser) {
            Utility::printSuccess("Login successful!");
            Utility::pauseScreen();
        } else {
            Utility::printError("Invalid credentials!");
            Utility::pauseScreen();
        }
    }
    
    void registerUser() {
        Utility::printHeader("REGISTER");
        cout << "Username: ";
        string username;
        getline(cin, username);
        
        if (username.empty()) return;
        
        if (userManager.userExists(username)) {
            Utility::printError("Username taken!");
            Utility::pauseScreen();
            return;
        }
        
        cout << "Password: ";
        string password = Utility::getPasswordInput();
        cout << "\n";
        
        if (password.empty()) {
            Utility::printError("Password required!");
            Utility::pauseScreen();
            return;
        }
        
        cout << "Confirm: ";
        string confirm = Utility::getPasswordInput();
        cout << "\n";
        
        if (password != confirm) {
            Utility::printError("Mismatch!");
            Utility::pauseScreen();
            return;
        }
        
        if (userManager.registerUser(username, password)) {
            Utility::printSuccess("Registered!");
        } else {
            Utility::printError("Failed!");
        }
        
        Utility::pauseScreen();
    }
    
    void logout() {
        if (currentUser) {
            userManager.updateUser(*currentUser);
            currentUser = nullptr;
            Utility::printSuccess("Logged out!");
            Utility::pauseScreen();
        }
    }
};

int main() {
    system("title Edu-Tainment System");
    system("chcp 65001 >nul");
    
    EdutainmentApp app;
    app.run();
    
    return 0;
}