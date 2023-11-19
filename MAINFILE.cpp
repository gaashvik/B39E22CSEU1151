#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <functional>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <set>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <iomanip>
using namespace std;



// ANSI escape codes for text colors
const std::string reset = "\033[0m"; // Reset to default
const std::string blackText = "\033[30m"; // Black text
const std::string redText = "\033[31m"; // Red text
const std::string greenText = "\033[32m"; // Green text
const std::string yellowText = "\033[33m"; // Yellow text
const std::string blueText = "\033[34m"; // Blue text
const std::string magentaText = "\033[35m"; // Magenta text
const std::string cyanText = "\033[36m"; // Cyan text
const std::string whiteText = "\033[97m"; // White text

// ANSI escape codes for background colors
const std::string blackBackground = "\033[40m"; // Black background
const std::string redBackground = "\033[41m"; // Red background
const std::string greenBackground = "\033[42m"; // Green background
const std::string yellowBackground = "\033[43m"; // Yellow background
const std::string blueBackground = "\033[44m"; // Blue background
const std::string magentaBackground = "\033[45m"; // Magenta background
const std::string cyanBackground = "\033[46m"; // Cyan background
const std::string whiteBackground = "\033[107m"; // White background



// issues with the project, add friend can duplicate friend data ,new user data is not being stored,add course is not implemeneted,if freind not in database he is still being added
//goals-:coursetocourse interaction,gui,grph automations.
class User{
    public:
    string username;
    string password;
    vector<unique_ptr<User>> connections;
    User()=default;
    User(string name,string pwd):username(name),password(pwd){}
    User(const User& other) : username(other.username), password(other.password) {
        for (const auto& connection : other.connections) {
            connections.push_back(std::make_unique<User>(*connection));
        }
    }
};
class Course{
    public:
    string courseName;
    string genre;
    Course(string name,string b):courseName(name),genre(b){}
    bool operator==(const Course& other) const {
        return this->courseName == other.courseName;
    }
};
struct UserHash {
    std::size_t operator()(const User& user) const {
        return std::hash<std::string>()(user.username);
    }
};

struct UserEqual {
    bool operator()(const User& lhs, const User& rhs) const {
        return lhs.username == rhs.username;
    }
};
struct CourseHash {
    std::size_t operator()(const Course& course) const {
        return std::hash<std::string>()(course.courseName);
    }
};

struct CourseEqual {
    bool operator()(const Course& lhs, const Course& rhs) const {
        return lhs.courseName == rhs.courseName;
    }
};
using UserCourseMap = std::unordered_map<User, std::vector<Course>, UserHash, UserEqual>;
void animate_text(const std::string &text, unsigned int delay_us = 100000) {
    for (char c : text) {
        std::cout << c;
        usleep(delay_us);
    }
}
void animate_text(const std::vector<std::string> &lines, unsigned int delay_us = 100000) {
    std::vector<std::string> current_lines(lines.size());

    bool still_printing = true;
    while (still_printing) {
        still_printing = false;
        for (std::size_t i = 0; i < lines.size(); ++i) {
            if (current_lines[i].length() < lines[i].length()) {
                current_lines[i] += lines[i][current_lines[i].length()];
                still_printing = true;
            }
            std::cout << current_lines[i] << std::endl;
        }
        usleep(delay_us);
        if (still_printing) {
            // Move the cursor up to the start of the first line.
            for (std::size_t i = 0; i < lines.size(); ++i) {
                std::cout << "\033[A";
            }
        }
    }
}


class RecommendationSystem{
    private:
        unique_ptr<User> currentUser;
    public:
     UserCourseMap userCourseInteraction;
      vector<Course> availableCourses;
     std::unordered_map<Course, std::vector<Course>, CourseHash, CourseEqual> courseInteractions;
    void addUser(User user){
        userCourseInteraction[user]=vector<Course>();
    }
    void addCourseToUser(User user,Course course){
        if (userCourseInteraction.find(user)!=userCourseInteraction.end()){
            userCourseInteraction[user].push_back(course);
        }
    }
    void displayUserCourseInteractions(const User& user){
        if(userCourseInteraction.find(user) != userCourseInteraction.end()){
            cout<<"User: "<<user.username<<endl;
            cout<<"Courses: ";
            for (const Course& course:userCourseInteraction[user]){
                cout<<course.courseName<<",";
            }
            cout<<endl;
        }
    }
    vector<Course> loadAvailableCourses(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return availableCourses;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string c_name, genre;
        getline(ss, c_name, ',');
        getline(ss, genre, ',');
        Course course(c_name, genre);
        availableCourses.push_back(course);
    }

    file.close();
    return availableCourses;
}

    void populateFromCsv(const string& filename){
        ifstream file(filename);
        if (!file.is_open()){
            cerr<<"Error opening file: "<<filename<<endl;
            return;
        }
        string line;
        while (getline(file,line)){
            stringstream ss(line);
            string username,c_name,password,genre;
            getline(ss,username,',');
            getline(ss,password,',');
            getline(ss,c_name,',');
            getline(ss,genre,',');
            User user(username,password);
            Course course(c_name,genre);
        if (userCourseInteraction.find(user) != userCourseInteraction.end()) {
            userCourseInteraction[user].push_back(course);
        } else {
            addUser(user);
            addCourseToUser(user, course);
        }


        }
        file.close();
    }
    void populateConnectionsFromCsv(const string& filename){
        ifstream file(filename);
        if (!file.is_open()){
            cerr<<"Error opening file "<<filename<<endl;
            return;
        }
        string line;
        while(getline(file,line)){
            stringstream ss(line);
            string u_name,f_name;
            getline(ss,u_name,',');
            getline(ss,f_name,',');
            User user(u_name,"");
            unique_ptr<User> friendUser = make_unique<User>(f_name, "");
            auto useriter=userCourseInteraction.find(user);
            if (useriter !=userCourseInteraction.end()){
                User& userobject=const_cast<User&>(useriter->first);
                userobject.connections.push_back(move(friendUser));
            }




        }
    }
    void executePythonScript(const char* pythonInterpreter="python3", const char* pythonScript="p1.py") {
    // Set up an array to hold the command and its arguments
    const char* args[] = {pythonInterpreter, pythonScript, NULL};

    // Execute the Python script using execvp
    execvp(pythonInterpreter, const_cast<char* const*>(args));

    // If execvp returns, there was an error
    std::cerr << "Failed to execute the Python script." << std::endl;
}

void displayUserConnections(const User& user) {
    auto userIter = userCourseInteraction.find(user);
    if (userIter != userCourseInteraction.end()) {
        const User& userObject = userIter->first;
        cout << "Connections for user " << userObject.username << ": ";
        for (const unique_ptr<User>& friendUser : userObject.connections) {
            cout << friendUser->username << ", ";
        }
        cout << endl;
    } else {
        cout << "User not found in the system." << endl;
    }
}

void addConnection(User user, unique_ptr<User> friendUser) {
    auto userIter = userCourseInteraction.find(user);
    if (userIter != userCourseInteraction.end()) {
        // Make sure that 'userIter->second' is a 'User' object, not a vector
        User& userObject = const_cast<User&>(userIter->first);
        userObject.connections.push_back(std::move(friendUser));
        cout << "Friend added successfully!" << endl;
    } else {
        cout << "User not found in the system. Make sure the user is registered." << endl;
    }
}
    void saveToCSV(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        for (const auto& entry : userCourseInteraction) {
            for (const Course& course : entry.second) {
                file << entry.first.username << "," << entry.first.password << "," << course.courseName <<course.genre<< endl;
            }
        }
        file.close();
    }
bool loginUser(const string& username, const string& password) {
    User inputUser(username, password);
    auto userIter = userCourseInteraction.find(inputUser);
    if (userIter != userCourseInteraction.end()) {
        const User& storedUser = userIter->first;
        if (storedUser.password == password) {
            currentUser = std::make_unique<User>(storedUser);
            return true;
        }
    }
    return false;
}
void saveUserCourseInteractionToCSV(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }
     file << "Username,Password,Course,Genre" << endl;
    // Write user and course data, excluding entries with username "Course"
    for (const auto& entry : userCourseInteraction) {
        if (entry.first.username != "Username") {
            for (const Course& course : entry.second) {
                file << entry.first.username << "," << entry.first.password << ","
                     << course.courseName << "," << course.genre << endl;
            }
        }
    }

    file.close();
}


void displayAvailableCourses() {
    cout << "Available Courses:" << endl;
    cout << left << setw(5) << "No." << setw(20) << "Course" << "Genre" << endl;
    
    int courseNumber = 1;
    for (const Course& course : availableCourses) {
        cout << left << setw(5) << courseNumber << setw(20) << course.courseName << course.genre << endl;
        courseNumber++;
    }
}

void convertCSVToGEXFWithoutSourceTarget(const std::string& csvFile, const std::string& gexfFile) {
    std::ifstream inputFile(csvFile);
    std::ofstream outputFile(gexfFile);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }

    // Write the GEXF file header
    outputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    outputFile << "<gexf xmlns=\"http://www.gexf.net/1.3draft\" xmlns:viz=\"http://www.gexf.net/1.3draft/viz\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.gexf.net/1.3draft http://www.gexf.net/1.3draft/gexf.xsd\">" << std::endl;
    outputFile << "  <graph defaultedgetype=\"directed\" mode=\"static\">" << std::endl;

    std::unordered_set<std::string> uniqueNodes;

    // Write node data
    outputFile << "    <attributes class=\"node\" mode=\"static\">" << std::endl;
    outputFile << "      <attribute id=\"0\" title=\"name\" type=\"string\" />" << std::endl;
    outputFile << "    </attributes>" << std::endl;

    // Write the graph data
    std::string line;
    while (std::getline(inputFile, line)) {
        std::string source, target;
        std::istringstream iss(line);
        if (std::getline(iss, source, ',') && std::getline(iss, target, ',')) {
            if (uniqueNodes.find(source) == uniqueNodes.end()) {
                if (source != "Source" && source != "Target") {
                    outputFile << "    <node id=\"" << source << "\" label=\"" << source << "\">" << std::endl;
                    outputFile << "      <attvalues>" << std::endl;
                    outputFile << "        <attvalue for=\"0\" value=\"" << source << "\" />" << std::endl;
                    outputFile << "      </attvalues>" << std::endl;
                    outputFile << "    </node>" << std::endl;
                }
                uniqueNodes.insert(source);
            }
            if (uniqueNodes.find(target) == uniqueNodes.end()) {
                if (target != "Source" && target != "Target") {
                    outputFile << "    <node id=\"" << target << "\" label=\"" << target << "\">" << std::endl;
                    outputFile << "      <attvalues>" << std::endl;
                    outputFile << "        <attvalue for=\"0\" value=\"" << target << "\" />" << std::endl;
                    outputFile << "      </attvalues>" << std::endl;
                    outputFile << "    </node>" << std::endl;
                }
                uniqueNodes.insert(target);
            }
        }
    }

    // Write the graph data (edges) after node creation
    inputFile.clear();
    inputFile.seekg(0);

    while (std::getline(inputFile, line)) {
        std::string source, target;
        std::istringstream iss(line);
        if (std::getline(iss, source, ',') && std::getline(iss, target, ',')) {
            if (source != "Source" && target != "Target") {
                outputFile << "    <edge source=\"" << source << "\" target=\"" << target << "\" />" << std::endl;
            }
        }
    }

    // Write the GEXF file footer
    outputFile << "  </graph>" << std::endl;
    outputFile << "</gexf>" << std::endl;

    inputFile.close();
    outputFile.close();
}


void exportGraphData(const UserCourseMap& userCourseInteraction, const string& filename) {
    set<pair<string, string>> writtenConnections;

    // Load existing connections from the CSV file
    ifstream existingFile(filename);
    if (existingFile.is_open()) {
        string line;
        while (getline(existingFile, line)) {
            stringstream ss(line);
            string source, target;
            getline(ss, source, ',');
            getline(ss, target, ',');
            writtenConnections.insert({source, target});
        }
        existingFile.close();
    }

    ofstream file(filename, ios::app); // Open the file in append mode
    if (!file.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }


    for (const auto& entry : userCourseInteraction) {
        const User& user = entry.first;
        for (const unique_ptr<User>& friendUser : user.connections) {
            pair<string, string> connection(user.username, friendUser->username);
            if (writtenConnections.find(connection) == writtenConnections.end()) {
                // Export the relationship between the user and their friend
                file << user.username << "," << friendUser->username << endl;
            }
        }
    }

    file.close();
}

void establishCourseInteractions() {
    // Clear the existing course interactions
    courseInteractions.clear();

    for (const auto& userEntry : userCourseInteraction) {
        const vector<Course>& userCourses = userEntry.second;
        
        for (const Course& userCourse : userCourses) {
            for (const auto& otherUserEntry : userCourseInteraction) {
                if (userEntry.first.username != otherUserEntry.first.username) {  // Use != for comparison
                    const vector<Course>& otherUserCourses = otherUserEntry.second;
                    for (const Course& otherCourse : otherUserCourses) {
                        if (userCourse.genre == otherCourse.genre) {
                            courseInteractions[userCourse].push_back(otherCourse);
                        }
                    }
                }
            }
        }
    }
}


void displayCourseInteractions(const UserCourseMap& userCourseInteraction, const std::unordered_map<Course, std::vector<Course>, CourseHash, CourseEqual>& courseInteractions) {
    for (const auto& entry : courseInteractions) {
        const Course& course = entry.first;
        const std::vector<Course>& relatedCourses = entry.second;

        cout << "Course: " << course.courseName << " (Genre: " << course.genre << ")" << endl;
        cout << "Related Courses: ";

        for (const Course& relatedCourse : relatedCourses) {
            cout << relatedCourse.courseName << " (Genre: " << relatedCourse.genre << "), ";
        }

        cout << endl;
    }
}


vector<Course> getUserEnrolledCourses(const User& user) {
    vector<Course> enrolledCourses;

    auto userIter = userCourseInteraction.find(user);
    if (userIter != userCourseInteraction.end()) {
        enrolledCourses = userIter->second;
    }

    return enrolledCourses;
}
void displayCourseRecommendations(const User& user, const UserCourseMap& userCourseInteraction, const std::unordered_map<Course, std::vector<Course>, CourseHash, CourseEqual>& courseInteractions) {
    auto userIter = userCourseInteraction.find(user);
    if (userIter != userCourseInteraction.end()) {
        const std::vector<Course>& userEnrolledCourses = userIter->second;

        for (const Course& enrolledCourse : userEnrolledCourses) {
            cout << "Because you have enrolled in " << enrolledCourse.courseName << " (Genre: " << enrolledCourse.genre << "), we recommend the following courses:" << endl;

            auto recommendationsIter = courseInteractions.find(enrolledCourse);
            if (recommendationsIter != courseInteractions.end()) {
                const std::vector<Course>& recommendedCourses = recommendationsIter->second;
                for (const Course& recommendedCourse : recommendedCourses) {
                    cout << "  - " << recommendedCourse.courseName << " (Genre: " << recommendedCourse.genre << ")" << endl;
                }
            }
        }
    } else {
        cout << "User not found in the system or not enrolled in any courses." << endl;
    }
}


vector<Course> getRecommendations(const User& user) {
    vector<Course> recommendations;

    auto userIter = userCourseInteraction.find(user);
    if (userIter != userCourseInteraction.end()) {
        const User& userObject = userIter->first;

        for (const unique_ptr<User>& friendUser : userObject.connections) {
            // Loop through the courses of the friendUser
            auto it = userCourseInteraction.find(*friendUser);
            if (it != userCourseInteraction.end()) {
                const vector<Course>& friendCourses = it->second;
                for (const Course& recommendedCourse : friendCourses) {
                    // Check if the recommended course is not already in recommendations
                    if (find(recommendations.begin(), recommendations.end(), recommendedCourse) == recommendations.end()) {
                        recommendations.push_back(recommendedCourse);
                    }
                }
            }
        }
    }

    // Remove courses the user is already enrolled in
    auto userEnrolledCourses = userCourseInteraction[user];
    recommendations.erase(std::remove_if(recommendations.begin(), recommendations.end(),
        [&userEnrolledCourses](const Course& course) {
            return std::find(userEnrolledCourses.begin(), userEnrolledCourses.end(), course) != userEnrolledCourses.end();
        }), recommendations.end());


    return recommendations;
}

    unique_ptr<User>& getCurrentUser() {
        return currentUser;
    }

};
const std::string clearScreen = "\033[2J\033[H";
int main() {
    RecommendationSystem r;
    int choice = 0;
    r.populateFromCsv("second.csv");
    r.convertCSVToGEXFWithoutSourceTarget("user_connections.csv", "user_connections.gexf");
    r.populateConnectionsFromCsv("user_connections.csv");
    r.establishCourseInteractions();
    r.loadAvailableCourses("course.csv");
    string username, password;
    User* currentUser = nullptr; // Initialize the current user to nullptr
    bool isAdmin = false; 
    cout<<clearScreen;
    std::vector<std::string> lines = {"------------------------", "WELCOME TO COURSESCAPE.","------------------------"};
    cout << blueText ;
    animate_text(lines);

    cout<<reset;


    cout << "Choose mode:\n1. Admin Mode\n2. User Mode\nEnter your choice: ";
    cin >> choice;

    if (choice == 1) {
        isAdmin = true; // Set the isAdmin flag to true for admin mode
        cout << "Admin Mode selected." << endl;
    } else if (choice == 2) {
        cout << "User Mode selected." << endl;
    } else {
        cout << "Invalid choice. Exiting the program." << endl;
        return 0;
    }
    while (true) {
        // Registration, login, and main program loop
        while (currentUser == nullptr) {
            cout << "Menu:\n1. Register\n2. Login\n3. Exit\nEnter your choice: ";
            std::cin >> choice;
            if (choice == 1) {
                cout << "Enter username: ";
                std::cin >> username;
                cout << "Enter password: ";
                std::cin >> password;
                User newUser(username, password);
                r.addUser(newUser);
                cout << "Registration successful!" << endl;
            } else if (choice == 2) {
                cout << "Enter username: ";
                std::cin >> username;
                cout << "Enter password: ";
                std::cin >> password;
                if (r.loginUser(username, password)) {
                    currentUser = r.getCurrentUser().get(); // Save the current user
                    cout << "Login successful!" << endl;
                } else {
                    cout << "Login failed. Invalid credentials." << endl;
                }
            } else if (choice == 3) {
                cout << "Exiting the program." << endl;
                return 0;
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        }
        cout << "Menu:\n1. Recommend Courses\n2. Add friends\n3. Display enrolled courses\n4. Display friends\n5. Logout\n";
        if (isAdmin) {
            cout << "6. Visualize Graph (Admin)\n";
        }
        cout<<"7.Add Course\n";
        cout << "8. Exit\nEnter your choice: ";
        cin >> choice;

        if (choice == 1) {
       // Get genre-based recommendations and friend-based recommendations
        vector<Course> friendRecommendations = r.getRecommendations(*currentUser);
        
        cout << "\nFriend-Based Recommendations: "<<endl;
        for (const Course& course : friendRecommendations) {
            cout <<"-"<< course.courseName << endl;
        }
        cout<<endl;
        cout<<"\nGenre Based Recommendations: "<<endl;
        r.displayCourseRecommendations(*currentUser,r.userCourseInteraction, r.courseInteractions);
            
       
        } else if (choice == 2) {
            cout << "Enter your friend's username: ";
            std::cin >> username;
            unique_ptr<User> friendUser = make_unique<User>(username, "");
            r.addConnection(*currentUser, move(friendUser));
        } else if (choice == 3) {
            r.displayUserCourseInteractions(*currentUser);
        } else if (choice == 4) {
            r.displayUserConnections(*currentUser);
        } else if (choice == 5) {
            cout << "Logging out." << endl;
            currentUser = nullptr;
        } else if (choice == 8) {
            cout << "Exiting the program." << endl;
            r.saveUserCourseInteractionToCSV("second.csv");
            r.exportGraphData(r.userCourseInteraction, "user_connections.csv");
            r.convertCSVToGEXFWithoutSourceTarget("user_connections.csv", "user_connections.gexf");
            return 0;
        } 
        else if (isAdmin && choice == 6) {
            r.saveUserCourseInteractionToCSV("second.csv");
            r.exportGraphData(r.userCourseInteraction, "user_connections.csv");
            r.convertCSVToGEXFWithoutSourceTarget("user_connections.csv", "user_connections.gexf");
            r.executePythonScript("python3", "p1.py");
        }
        else if(choice==7){
        r.displayAvailableCourses();
    cout << "Enter the course number to add (e.g., 1): ";
    int courseNumber;
    cin >> courseNumber;

    if (courseNumber >= 1 && courseNumber <= r.availableCourses.size()) {
        Course selectedCourse = r.availableCourses[courseNumber - 1];
        
        if (currentUser != nullptr) {
            r.addCourseToUser(*currentUser, selectedCourse);
            cout << "Course added to enrolled courses!" << endl;
        } else {
            cout << "You must be logged in to add courses." << endl;
        }
    } else {
        cout << "Invalid course number. Please try again." << endl;
    }
}
            else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}
