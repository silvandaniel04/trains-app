#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

// Vigenère encryption class
class VigenereCipher {
private:
    static const string key;

public:
    static string encrypt(const string text) {
        string result = text;
        for (size_t i = 0; i < text.length(); i++) {
            if (isalpha(text[i])) {
                char offset = isupper(text[i]) ? 'A' : 'a';
                result[i] = (text[i] + key[i % key.length()] - 2 * offset) % 26 + offset;
            }
        }
        return result;
    }

    static string decrypt(const string text) {
        string result = text;
        for (size_t i = 0; i < text.length(); i++) {
            if (isalpha(text[i])) {
                char offset = isupper(text[i]) ? 'A' : 'a';
                result[i] = (text[i] - key[i % key.length()] + 26) % 26 + offset;
            }
        }
        return result;
    }
};

const string VigenereCipher::key = "STERNOCLEIDOMASTOIDIAN";

// Password strength checking class
class PasswordStrengthChecker {
public:
    typedef enum {
        WEAK,
        GOOD,
        STRONG
    } Strength;

    static Strength checkStrength(const string password) {
        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        
        if (password.length() < 8) return WEAK;

        for (size_t i = 0; i < password.length(); ++i) {
            char c = password[i];
            if (isupper(c)) hasUpper = true;
            else if (islower(c)) hasLower = true;
            else if (isdigit(c)) hasDigit = true;
            else hasSpecial = true;
        }

        int score = hasUpper + hasLower + hasDigit + hasSpecial;
        
        if (score <= 2) return WEAK;
        if (score == 3) return GOOD;
        return STRONG;
    }
};

// Reservation class for instantiating a reservation (ticket)
class Reservation {
public:
    string user;
    string trainId;
    string reservationDate;
    string reservationTime;
    int classType; // 1 for first class, 2 for second class
    int seatNumber;
};

// Train class for instantiating trains
class Train {
public: 
    string trainId;
    string departureLocation;
    string arrivalLocation;
    string departureTime;
    string departureDate;
    string arrivalTime;
    string arrivalDate;
    int firstClassSeats;
    int secondClassSeats;
    double firstClassPrice;
    double secondClassPrice;

    // Function to check seat availability
    bool isSeatAvailable(int classType, const int& seatNumber, vector<Reservation> reservations) {
        if (classType == 1) {
            if (firstClassSeats <= 0) return false; // No seats available
        } else if (classType == 2) {
            if (secondClassSeats <= 0) return false; // No seats available
        }

        // Check if the seat number is already reserved
       for (size_t i = 0; i < reservations.size(); ++i) {
        Reservation reservation = reservations[i];
        if (reservation.trainId == trainId && reservation.classType == classType && reservation.seatNumber == seatNumber) {
            return false; // Seat is already reserved
        } else if (classType == 1) {
            if (seatNumber > firstClassSeats || seatNumber < 1) {
                throw runtime_error("The entered seat doesn't exist!");
                return false;
            }
        } else if (classType == 2) {
            if (seatNumber > secondClassSeats || seatNumber < 1) {
                throw runtime_error("The entered seat doesn't exist!");
                return false;
            }
        }
       }
        return true; // Seat is available
    }
};

// User management class
class UserManager {
private:
    vector<pair<string, string> > users; // email, encrypted password
    string usersFile;

public:
    UserManager() {
        usersFile = "users.csv";
        loadUsers();
    }

    bool login(string email, string password) {
        loadUsers();
        string encryptedPassword = VigenereCipher::encrypt(password);

    ifstream file(usersFile);
        if (!file.is_open()) {
        return false;
        }
    
    string line;
        while (getline(file, line)) {
        istringstream ss(line);
        string fileEmail, filePassword;

        if (getline(ss, fileEmail, ',') && getline(ss, filePassword, ',')) {
            fileEmail.erase(fileEmail.find_last_not_of(" \n\r\t") + 1);
            filePassword.erase(filePassword.find_last_not_of(" \n\r\t") + 1);

            if (fileEmail == email && filePassword == encryptedPassword) {
                return true;
            }
        }
    }
    return false;
}

    void registerUser(string email, string password) {
        if (!validateEmail(email)) {
            throw runtime_error("Invalid email format!");
        }

        PasswordStrengthChecker::Strength strength = PasswordStrengthChecker::checkStrength(password);
        if (strength == PasswordStrengthChecker::WEAK) {
            throw runtime_error("Password is too weak!");
        }

        string encryptedPassword = VigenereCipher::encrypt(password);
        pair<string, string> user(email, encryptedPassword);
        users.push_back(user);

        ofstream file(usersFile.c_str() , ios::app);
        file << email << "," << encryptedPassword << "\n";
    }

     void loadUsers() {
        ifstream file(usersFile.c_str());
        string line, email, password;
        
        while (getline(file, line)) {
            size_t comma = line.find(',');
            if (comma != string::npos) {
                email = line.substr(0, comma);
                password = line.substr(comma + 1);
                pair<string, string> user(email, password);
                users.push_back(user);
            }
        }
    }

    bool validateEmail(string email) {
        return email.find('@') != string::npos && email.find('.') != string::npos;
    }
};

// Train and reservation management class
class TrainManager {
private:
    vector<Train> trains;
    vector<Reservation> reservations;
    string trainsFile;
    string reservationsFile;

public:
    TrainManager() {
        trainsFile = "trains.csv";
        reservationsFile = "reservations.csv";
        loadTrains();
        loadReservations();
    }

    void loadTrains() {
    ifstream file(trainsFile.c_str());
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;

        Train train;
        size_t pos = 0;
        string delimiter = ",";

        pos = line.find(delimiter);
        train.trainId = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.departureLocation = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.arrivalLocation = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.departureDate = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.departureTime = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.arrivalDate = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.arrivalTime = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.firstClassSeats = stoi(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.secondClassSeats = stoi(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.firstClassPrice = stod(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        train.secondClassPrice = stod(line);

        trains.push_back(train);
    }

    file.close();
}

    void loadReservations() {
    ifstream file(reservationsFile.c_str());
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        Reservation reservation;
        size_t pos = 0;
        string delimiter = ",";

        pos = line.find(delimiter);
        reservation.user = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        reservation.trainId = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        reservation.reservationDate = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        reservation.reservationTime = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());

        pos = line.find(delimiter);
        reservation.classType = stoi(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());

        reservation.seatNumber = stoi(line);

        reservations.push_back(reservation);
    }

    file.close();
    }

    void showReservations(string user) {
        for (size_t i = 0; i < reservations.size(); ++i) {
        Reservation reservation = reservations[i];
        if (reservation.user == user) {
            cout << i+1 << "." 
            << " Train ID: " << reservation.trainId
            << "\n   Reservation date: " << reservation.reservationDate
            << "\n   Reservation time: " << reservation.reservationTime
            << "\n   Class: " << reservation.classType
            << "\n   Seat number: " << reservation.seatNumber << endl << "\n";
        }
        }

        if (reservations.empty()) {
           throw runtime_error("You don't have any bookings!");
        }
    }

    void addReservation(Reservation reservation) {
    if (!validateDateTime(reservation.reservationDate, reservation.reservationTime)) {
        throw runtime_error("Reservation date or time cannot be in the past!");
    }
    validateReservation(reservation, trains);
    
    reservations.push_back(reservation);

    ofstream file(reservationsFile.c_str());
    for (size_t i = 0; i < reservations.size(); ++i) {
        const Reservation reservation = reservations[i];
        file << reservation.user << "," 
             << reservation.trainId << "," 
             << reservation.reservationDate << "," 
             << reservation.reservationTime << "," 
             << reservation.classType << ","
             << reservation.seatNumber << "\n";
    }
}

void deleteReservation(string user, string trainId, int seatNumber, int classType) {
    bool found = false;
    vector<Reservation> updatedReservations;

    // search for the reservation that we want to delete
    for (size_t i = 0; i < reservations.size(); ++i) {
        const Reservation& reservation = reservations[i];
        if (reservation.user == user && 
            reservation.trainId == trainId && 
            reservation.seatNumber == seatNumber && 
            reservation.classType == classType) {
            found = true;
            continue; // skip the reservation that was found
        }
        updatedReservations.push_back(reservation);
    }

    if (!found) {
        throw runtime_error("Reservation not found!");
    }

    // update the reservations vector
    reservations = updatedReservations;

    // save the new reservations list in the reservations file
    ofstream file(reservationsFile);
    if (!file.is_open()) {
        throw runtime_error("Could not open reservations file for updating!");
    }

    for (size_t i = 0; i < reservations.size(); ++i) {
        Reservation reservation = reservations[i];
        file << reservation.user << "," 
             << reservation.trainId << "," 
             << reservation.reservationDate << "," 
             << reservation.reservationTime << "," 
             << reservation.classType << ","
             << reservation.seatNumber << "\n";
    }
    file.close();
}


    void addTrain(Train train) {
    if (!validateDateTime(train.departureDate, train.departureTime)) {
        throw runtime_error("Invalid date format!");
    }
    if (!validateLocation(train.departureLocation) || !validateLocation(train.arrivalLocation)) {
        throw runtime_error("City name contains invalid characters!");
    }

    trains.push_back(train);

    ofstream file(trainsFile.c_str(), ios::app);
    file << train.trainId << "," 
         << train.departureLocation << "," 
         << train.arrivalLocation << "," 
         << train.departureDate << "," 
         << train.departureTime << "," 
         << train.arrivalDate << "," 
         << train.arrivalTime << "," 
         << train.firstClassSeats << "," 
         << train.secondClassSeats << "," 
         << train.firstClassPrice << "," 
         << train.secondClassPrice << "\n";
}

void removeTrain(string trainId) {
    bool found = false;

    if (trains.empty()) {
        throw runtime_error("There are no trains to remove!");
    }

    for (size_t i = 0; i < trains.size(); ++i) {
        if (trains[i].trainId == trainId) {
            trains.erase(trains.begin() + i);
            found = true;
            break;
        }
    }

    if (!found) {
        throw runtime_error("Train with specified ID not found!");
    }

    saveTrains();
}

void saveTrains() {
    ofstream file(trainsFile.c_str());
    for (size_t i = 0; i < trains.size(); ++i) {
    const Train train = trains[i];
    file << train.trainId << "," 
         << train.departureLocation << "," 
         << train.arrivalLocation << "," 
         << train.departureDate << ","
         << train.departureTime << "," 
         << train.arrivalDate << "," 
         << train.arrivalTime << "," 
         << train.firstClassSeats << "," 
         << train.secondClassSeats << "," 
         << train.firstClassPrice << "," 
         << train.secondClassPrice << "\n";
}
}

void getAllTrains() {
    if (trains.empty()) {
        cout << "No registered routes!" << endl << "\n";
    } else {
        for (size_t i = 0; i < trains.size(); ++i) {
        Train train = trains[i];
        cout << i+1 << "." 
            << " Train ID: " << train.trainId
            << "\n   Departure city: " << train.departureLocation
            << "\n   Arrival city: " << train.arrivalLocation 
            << "\n   Departure date (DD/MM/YYYY): " << train.departureDate
            << "\n   Departure time (HH:MM): " << train.departureTime
            << "\n   Arrival date (DD/MM/YYYY): " << train.arrivalDate
            << "\n   Arrival time (HH:MM): " << train.arrivalTime
            << "\n   First class seats: " << train.firstClassSeats
            << "\n   Second class seats: " << train.secondClassSeats
            << "\n   First class price: " << train.firstClassPrice
            << "\n   Second class price: " << train.secondClassPrice << "\n\n";
    }
    }
}

vector<Train> searchTrains(string departure, string arrival, string date) {
    vector<Train> results;
    loadTrains();

    for (size_t i = 0; i < trains.size(); ++i) {
        if (trains[i].departureLocation == departure &&
            trains[i].arrivalLocation == arrival &&
            trains[i].departureDate == date) {

            bool alreadyAdded = false;
            for (size_t j = 0; j < results.size(); ++j) {
                if (results[j].departureLocation == trains[i].departureLocation &&
                    results[j].arrivalLocation == trains[i].arrivalLocation &&
                    results[j].departureDate == trains[i].departureDate) {
                    alreadyAdded = true;
                    break;
                }
            }

            if (!alreadyAdded) {
                results.push_back(trains[i]);
            }
        }
    }

    if (results.empty()) {
    throw runtime_error("No trains available for the specified route!");
}

return results;
}

private:
bool validateDateTime(string date, string timp) {
    // Get current date and time
    time_t now = time(0);
    tm *currentTime = localtime(&now);

    int currentYear = currentTime->tm_year + 1900;
    int currentMonth = currentTime->tm_mon + 1;
    int currentDay = currentTime->tm_mday;
    int currentHour = currentTime->tm_hour;
    int currentMinutes = currentTime->tm_min;

    // Extract day, month, and year
    size_t pos = date.find('/');
    if (pos == string::npos) {
        throw runtime_error("Invalid date format!");
    }
    int day = stoi(date.substr(0, pos));
    date.erase(0, pos + 1);

    pos = date.find('/');
    if (pos == string::npos) {
        throw runtime_error("Invalid date format!");
    }
    int month = stoi(date.substr(0, pos));
    date.erase(0, pos + 1);

    int year = stoi(date);

    // Validate date limits
    if (month < 1 || month > 12) {
        throw runtime_error("Month must be between 1 and 12!");
    }

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29; // Leap year
    }

    if (day < 1 || day > daysInMonth[month - 1]) {
        throw runtime_error("Invalid day for the selected month!");
    }

    // Compare date with current date
    if (year < currentYear || (year == currentYear && month < currentMonth) ||
        (year == currentYear && month == currentMonth && day < currentDay)) {
        throw runtime_error("Cannot enter a past date!");
    }

    // Extract hour and minutes
    pos = timp.find(':');
    if (pos == string::npos) {
        throw runtime_error("Invalid time format!");
    }
    int hour = stoi(timp.substr(0, pos));
    timp.erase(0, pos + 1);

    int minutes = stoi(timp);

    // Validate time limits
    if (hour < 0 || hour > 23) {
        throw runtime_error("Hour must be between 0 and 23!");
    }

    if (minutes < 0 || minutes > 59) {
        throw runtime_error("Minutes must be between 0 and 59!");
    }

    // Compare time if the date matches the current date
    if (year == currentYear && month == currentMonth && day == currentDay) {
        if (hour < currentHour || (hour == currentHour && minutes <= currentMinutes)) {
            throw runtime_error("Cannot enter a past time!");
        }
    }

    return true; // Date and time are valid
}

    bool validateReservation(Reservation& reservation, vector<Train> trains) {
    for (size_t i = 0; i < trains.size(); ++i) {
        Train train = trains[i];
        if (train.trainId == reservation.trainId) {
            // Validate seat availability
            if (!train.isSeatAvailable(reservation.classType, reservation.seatNumber, reservations)) {
                throw runtime_error("The selected seat is not available!");
                return false;
            }
            return true;
        }
    }
    throw runtime_error("The specified train was not found!");
}

bool validateLocation(const string location) {
    for (size_t i = 0; i < location.length(); ++i) {
        if (!isalpha(location[i]) && location[i] != ' ') {
            return false;
        }
    }
    return true;
}
};

void operatorLogin(TrainManager trainManager) {
    string email, password;
    cout << "\nOperator Login\n";
    cout << "Email: ";
    cin >> email;
    cout << "Password: ";
    cin >> password;
    
    try {
    ifstream operatorFile("operators.csv"); // Read from operators.csv file
    if (!operatorFile.is_open()) {
        throw runtime_error("Error: Cannot access operator database!");
    }

    bool isOperator = false;
    string line;
    while (getline(operatorFile, line)) {
        size_t comma = line.find(',');
        if (comma != string::npos) {
            string fileEmail = line.substr(0, comma);
            string filePassword = line.substr(comma + 1);
            
            // Remove any trailing whitespace
            fileEmail.erase(fileEmail.find_last_not_of(" \n\r\t") + 1);
            filePassword.erase(filePassword.find_last_not_of(" \n\r\t") + 1);

            if (fileEmail == email && filePassword == password) {
                isOperator = true;
                break;
            }
        }
    }
    operatorFile.close();
    if (!isOperator) {
        throw runtime_error("Access denied: Invalid operator credentials!");
    }
    cout << "\nLogin successful as operator!\n";
    
    int option;
    do {
        cout << "\nOperator Options:\n";
        cout << "1. Add new train\n";
        cout << "2. Remove train\n";
        cout << "3. View all trains\n";
        cout << "4. Exit to main menu\n";
        cout << "Choose an option: ";
        cin >> option;

        switch (option) {
            case 1: {
                cout << "\nAdd New Train\n";
                Train train;
                cout << "Train ID: ";
                cin >> train.trainId;
                cout << "Departure city: ";
                cin >> train.departureLocation;
                cout << "Arrival city: ";
                cin >> train.arrivalLocation;
                cout << "Departure date (DD/MM/YYYY): ";
                cin >> train.departureDate;
                cout << "Departure time (HH:MM): ";
                cin >> train.departureTime;
                cout << "Arrival date (DD/MM/YYYY): ";
                cin >> train.arrivalDate;
                cout << "Arrival time (HH:MM): ";
                cin >> train.arrivalTime;
                cout << "Number of first-class seats: ";
                cin >> train.firstClassSeats;
                cout << "Number of second-class seats: ";
                cin >> train.secondClassSeats;
                cout << "Price for first-class: ";
                cin >> train.firstClassPrice;
                cout << "Price for second-class: ";
                cin >> train.secondClassPrice;

                try {
                    trainManager.addTrain(train);
                    cout << "The train has been successfully added!\n";
                } catch (runtime_error e) {
                    cerr << "Error adding train: " << e.what() << endl;
                }
                break;
            }
            case 2: {
                cout << "\nRemove Train\n";
                try {
                    string trainId;
                    cout << "Enter the train ID to remove: ";
                    cin >> trainId;
                    trainManager.removeTrain(trainId);
                    cout << "Train " << trainId << " has been removed.\n";
                } catch (runtime_error e) {
                    cerr << e.what() << endl;
                }
                break;
            }
            case 3: {
                cout << "\nAll Trains List\n";
                trainManager.getAllTrains();
                break;
            }
            case 4:
                cout << "\nExiting to main menu...\n";
                break;
            default:
                cout << "\nInvalid option! Please try again.\n";
        }
    } while (option != 4);
    } catch (runtime_error e) {
        cerr << e.what() << endl;
    }
}


void userLogin(UserManager userManager, TrainManager trainManager) {
    string email, password;
    cout << "\nUser Login\n";
    cout << "Email: ";
    cin >> email;
    cout << "Password: ";
    cin >> password;
    int option;
    
    try {
        if (userManager.login(email, password)) {
            cout << "\nLogin successful!\n";
            do { 
                cout << "\nUser options:\n";
                cout << "1. Search for a train\n";
                cout << "2. Reserve a seat\n";
                cout << "3. View my reservations\n";
                cout << "4. Delete a reservation\n";
                cout << "5. Return to main menu\n";
                cout << "Choose an option: ";
                cin >> option;
            
                if (option == 1) {
                    string departure, arrival, date;
                    cout << "Departure city: ";
                    cin >> departure;
                    cout << "Arrival city: ";
                    cin >> arrival;
                    cout <<  "Departure date (DD/MM/YYYY): ";
                    cin >> date;
                    
                    try {
                        vector<Train> trains = trainManager.searchTrains(departure, arrival, date);
                        cout << "\nAvailable trains:\n";
                        for (size_t i = 0; i < trains.size(); ++i) {
                            cout << i+1 << ". " << trains[i].trainId 
                                 << " " << trains[i].departureLocation
                                 << " -> " << trains[i].arrivalLocation
                                 << "\nDeparture date: " << trains[i].departureDate
                                 << "\nDeparture time: " << trains[i].departureTime
                                 << "\nArrival date: " << trains[i].arrivalDate
                                 << "\nArrival time: " << trains[i].arrivalTime 
                                 << "\nAvailable first-class seats: " << trains[i].firstClassSeats
                                 << "\nAvailable second-class seats: " << trains[i].secondClassSeats << endl << "\n";
                        }
                    } catch (runtime_error e) {
                        cerr << "Search error: " << e.what() << endl;
                    }
                } else if (option == 2) {
                    Reservation reservation;
                    cout << "\nSeat reservation:\n";
                    reservation.user = email;

                    cout << "Train ID: ";
                    cin >> reservation.trainId;
                    cout << "Date (DD/MM/YYYY): ";
                    cin >> reservation.reservationDate;
                    cout << "Time (HH:MM): ";
                    cin >> reservation.reservationTime;
                    cout << "Class type (1 or 2): ";
                    cin >> reservation.classType;
                    cout << "Seat number: ";
                    cin >> reservation.seatNumber;

                    try {
                        trainManager.addReservation(reservation);
                        cout << "\nYou successfully reserved seat " << reservation.seatNumber << " in class " << reservation.classType << "!" << endl;
                    } catch (runtime_error e) {
                        cerr << "Reservation error: " << e.what() << endl;
                    }
                } else if (option == 3) {
                    try {
                        cout << "\nMy Reservations\n";
                        trainManager.showReservations(email);
                        cout << "\n\n";
                    } catch (runtime_error e) {
                        cerr << e.what() << endl;
                    }
                } else if (option == 4) {
                    string trainId;
                    int seatNumber, classType;
                    
                    cout << "\nDelete Reservation\n";
                    cout << "Current reservations:\n";
                    try {
                        trainManager.showReservations(email);
                    
                        cout << "\nEnter the details of the reservation to delete:\n";
                        cout << "Train ID: ";
                        cin >> trainId;
                        cout << "Seat number: ";
                        cin >> seatNumber;
                        cout << "Class type (1 or 2): ";
                        cin >> classType;

                        trainManager.deleteReservation(email, trainId, seatNumber, classType);
                        cout << "Reservation successfully deleted!\n";
                    } catch (runtime_error e) {
                        cerr << e.what() << endl;
                    }
                } else if (option == 5) {
                    cout << "\nReturning to main menu...\n";
                } else {
                    cout << "\nInvalid option!";
                }
            } while (option != 5);
        } else {
            throw runtime_error("Incorrect email or password!\n");
        }    
    } catch (runtime_error e) {
        cerr << "\nLogin error: " << e.what() << "If you do not have an account, please register!" << endl;
    }
}

void registerUser(UserManager userManager) {
    string email, password, confirmPassword;
    cout << "\nNew User Registration\n";
    cout << "Email: ";
    cin >> email;
    cout << "Password: ";
    cin >> password;
    cout << "Confirm password: ";
    cin >> confirmPassword;
    
    try {
        if (password != confirmPassword) {
            throw runtime_error("Passwords do not match!");
        }
        userManager.registerUser(email, password);
        cout << "\nRegistration successful! You can now log in.\n\n";
    } catch (runtime_error e) {
        cerr << "Registration error: " << e.what() << endl;
    }
}

int main() {
    try {
        UserManager userManager;
        TrainManager trainManager;
        int option;
        
        do {
            cout << "\nTrain Travel Reservation System\n";
            cout << "1. Operator Login\n"
                 << "2. User Login\n"
                 << "3. User Registration\n"
                 << "4. Exit\n"
                 << "Choose an option: ";
            cin >> option;

            switch (option) {
                case 1:
                    operatorLogin(trainManager);
                    break;
                case 2:
                    userLogin(userManager, trainManager);
                    break;
                case 3:
                    registerUser(userManager);
                    break;
                case 4:
                    cout << "Goodbye!\n";
                    break;
                default:
                    cout << "Invalid option!\n";
            }
            
        } while (option != 4);

    } catch (runtime_error e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
