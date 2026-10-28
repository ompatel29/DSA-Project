/*
    THE EXAM SEAT ALLOCATION SYSTEM
    DS Mini Project - C++
    Concepts: Structures, Arrays, Queue, Sorting, Searching, File Handling

    Compile:
        g++ exam_seat_allocation.cpp -o exam_seat_allocation

    Run:
        ./exam_seat_allocation
    On Windows:
        exam_seat_allocation.exe
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

const int MAX_STUDENTS = 500;
const int MAX_ROOMS = 50;

struct Student {
    string enrollmentNo;
    string name;
    string department;
    string roomId;
    int seatNo;
    bool allocated;
};

struct Room {
    string roomId;
    int capacity;
    int occupied;
};

Student students[MAX_STUDENTS];
Room rooms[MAX_ROOMS];
int studentCount = 0;
int roomCount = 0;

// ---------- Utility functions ----------

string cleanField(string value) {
    // Keep the simple CSV format safe by replacing commas with semicolons.
    replace(value.begin(), value.end(), ',', ';');
    return value;
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int findStudentIndex(const string& enrollmentNo) {
    for (int i = 0; i < studentCount; ++i) {
        if (students[i].enrollmentNo == enrollmentNo) {
            return i;
        }
    }
    return -1;
}

int findRoomIndex(const string& roomId) {
    for (int i = 0; i < roomCount; ++i) {
        if (rooms[i].roomId == roomId) {
            return i;
        }
    }
    return -1;
}

// ---------- File handling ----------

void saveData() {
    ofstream studentFile("students.csv");
    studentFile << "EnrollmentNo,Name,Department,RoomID,SeatNo,Allocated\n";

    for (int i = 0; i < studentCount; ++i) {
        studentFile << cleanField(students[i].enrollmentNo) << ','
                    << cleanField(students[i].name) << ','
                    << cleanField(students[i].department) << ','
                    << cleanField(students[i].roomId) << ','
                    << students[i].seatNo << ','
                    << (students[i].allocated ? 1 : 0) << '\n';
    }
    studentFile.close();

    ofstream roomFile("rooms.csv");
    roomFile << "RoomID,Capacity,Occupied\n";
    for (int i = 0; i < roomCount; ++i) {
        roomFile << cleanField(rooms[i].roomId) << ','
                 << rooms[i].capacity << ','
                 << rooms[i].occupied << '\n';
    }
    roomFile.close();
}

void loadData() {
    ifstream roomFile("rooms.csv");
    string line;

    if (roomFile.is_open()) {
        getline(roomFile, line); // Skip heading.
        while (getline(roomFile, line) && roomCount < MAX_ROOMS) {
            stringstream ss(line);
            string roomId, capacityText, occupiedText;

            if (!getline(ss, roomId, ',') ||
                !getline(ss, capacityText, ',') ||
                !getline(ss, occupiedText, ',')) {
                continue;
            }

            try {
                int capacity = stoi(capacityText);
                int occupied = stoi(occupiedText);
                if (!roomId.empty() && capacity > 0 &&
                    findRoomIndex(roomId) == -1) {
                    rooms[roomCount++] = {roomId, capacity, occupied};
                }
            } catch (...) {
                // Ignore malformed records.
            }
        }
        roomFile.close();
    }

    ifstream studentFile("students.csv");
    if (studentFile.is_open()) {
        getline(studentFile, line); // Skip heading.
        while (getline(studentFile, line) && studentCount < MAX_STUDENTS) {
            stringstream ss(line);
            string enrollment, name, department, roomId, seatText, allocatedText;

            if (!getline(ss, enrollment, ',') ||
                !getline(ss, name, ',') ||
                !getline(ss, department, ',') ||
                !getline(ss, roomId, ',') ||
                !getline(ss, seatText, ',') ||
                !getline(ss, allocatedText, ',')) {
                continue;
            }

            try {
                int seat = stoi(seatText);
                bool allocated = (stoi(allocatedText) == 1);

                if (!enrollment.empty() && findStudentIndex(enrollment) == -1) {
                    students[studentCount++] = {
                        enrollment, name, department, roomId, seat, allocated
                    };
                }
            } catch (...) {
                // Ignore malformed records.
            }
        }
        studentFile.close();
    }
}

// ---------- Student management ----------

void addStudent() {
    if (studentCount >= MAX_STUDENTS) {
        cout << "\nStudent storage is full.\n";
        return;
    }

    Student s;
    cout << "\nEnter enrollment number: ";
    getline(cin >> ws, s.enrollmentNo);

    if (findStudentIndex(s.enrollmentNo) != -1) {
        cout << "This enrollment number already exists.\n";
        return;
    }

    cout << "Enter student name: ";
    getline(cin, s.name);
    cout << "Enter department/division: ";
    getline(cin, s.department);

    if (s.enrollmentNo.empty() || s.name.empty() || s.department.empty()) {
        cout << "Fields cannot be empty. Student not added.\n";
        return;
    }

    s.roomId = "";
    s.seatNo = 0;
    s.allocated = false;
    students[studentCount++] = s;

    saveData();
    cout << "Student added successfully.\n";
}

void displayStudents() {
    if (studentCount == 0) {
        cout << "\nNo students registered.\n";
        return;
    }

    cout << "\n---------------- REGISTERED STUDENTS ----------------\n";
    cout << "Enrollment\tName\t\tDepartment\tStatus\n";
    cout << "-----------------------------------------------------\n";

    for (int i = 0; i < studentCount; ++i) {
        cout << students[i].enrollmentNo << '\t'
             << students[i].name << '\t'
             << students[i].department << '\t'
             << (students[i].allocated ? "Allocated" : "Pending") << '\n';
    }
}

// ---------- Room management ----------

void addRoom() {
    if (roomCount >= MAX_ROOMS) {
        cout << "\nRoom storage is full.\n";
        return;
    }

    Room r;
    cout << "\nEnter room ID/name: ";
    getline(cin >> ws, r.roomId);

    if (findRoomIndex(r.roomId) != -1) {
        cout << "This room already exists.\n";
        return;
    }

    cout << "Enter room seat capacity: ";
    if (!(cin >> r.capacity)) {
        cout << "Invalid capacity.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (r.capacity <= 0) {
        cout << "Capacity must be a positive number.\n";
        return;
    }

    r.occupied = 0;
    rooms[roomCount++] = r;
    saveData();
    cout << "Room added successfully.\n";
}

void displayRooms() {
    if (roomCount == 0) {
        cout << "\nNo rooms registered.\n";
        return;
    }

    cout << "\n------------- EXAMINATION ROOMS -------------\n";
    cout << "Room ID\tCapacity\tOccupied\tAvailable\n";
    for (int i = 0; i < roomCount; ++i) {
        int available = rooms[i].capacity - rooms[i].occupied;
        cout << rooms[i].roomId << '\t'
             << rooms[i].capacity << '\t'
             << rooms[i].occupied << '\t'
             << available << '\n';
    }
}

// ---------- DSA: Queue-based automatic allocation ----------

void allocateSeats() {
    if (studentCount == 0 || roomCount == 0) {
        cout << "\nPlease add students and rooms first.\n";
        return;
    }

    int totalCapacity = 0;
    for (int i = 0; i < roomCount; ++i) {
        totalCapacity += rooms[i].capacity;
    }

    if (studentCount > totalCapacity) {
        cout << "\nNot enough seats. Students: " << studentCount
             << ", total capacity: " << totalCapacity << '\n';
        return;
    }

    // Reset the previous seating plan before making a new one.
    for (int i = 0; i < studentCount; ++i) {
        students[i].allocated = false;
        students[i].roomId = "";
        students[i].seatNo = 0;
    }
    for (int i = 0; i < roomCount; ++i) {
        rooms[i].occupied = 0;
    }

    // Sort students by enrollment number for a predictable allocation order.
    sort(students, students + studentCount,
         [](const Student& a, const Student& b) {
             return a.enrollmentNo < b.enrollmentNo;
         });

    // Queue holds student array indexes waiting for a seat.
    queue<int> waitingQueue;
    for (int i = 0; i < studentCount; ++i) {
        waitingQueue.push(i);
    }

    for (int r = 0; r < roomCount && !waitingQueue.empty(); ++r) {
        for (int seat = 1; seat <= rooms[r].capacity && !waitingQueue.empty(); ++seat) {
            int studentIndex = waitingQueue.front();
            waitingQueue.pop();

            students[studentIndex].roomId = rooms[r].roomId;
            students[studentIndex].seatNo = seat;
            students[studentIndex].allocated = true;
            rooms[r].occupied++;
        }
    }

    saveData();
    cout << "\nSeat allocation completed successfully for "
         << studentCount << " student(s).\n";
}

// ---------- Searching and seating-plan display ----------

void searchStudent() {
    string enrollment;
    cout << "\nEnter enrollment number to search: ";
    getline(cin >> ws, enrollment);

    int index = findStudentIndex(enrollment);
    if (index == -1) {
        cout << "Student not found.\n";
        return;
    }

    const Student& s = students[index];
    cout << "\nStudent Details\n";
    cout << "Enrollment: " << s.enrollmentNo << '\n';
    cout << "Name: " << s.name << '\n';
    cout << "Department: " << s.department << '\n';

    if (s.allocated) {
        cout << "Room: " << s.roomId << '\n';
        cout << "Seat number: " << s.seatNo << '\n';
    } else {
        cout << "Seat status: Not allocated yet\n";
    }
}

void displaySeatingPlan() {
    bool anyAllocated = false;
    cout << "\n---------------- SEATING PLAN ----------------\n";
    cout << "Enrollment\tName\t\tRoom\tSeat No.\n";

    // Display allocations room by room, then by seat number.
    for (int r = 0; r < roomCount; ++r) {
        for (int seat = 1; seat <= rooms[r].capacity; ++seat) {
            for (int i = 0; i < studentCount; ++i) {
                if (students[i].allocated &&
                    students[i].roomId == rooms[r].roomId &&
                    students[i].seatNo == seat) {
                    cout << students[i].enrollmentNo << '\t'
                         << students[i].name << '\t'
                         << students[i].roomId << '\t'
                         << students[i].seatNo << '\n';
                    anyAllocated = true;
                }
            }
        }
    }

    if (!anyAllocated) {
        cout << "No seats have been allocated yet.\n";
    }
}

// ---------- Main menu ----------

void showMenu() {
    cout << "\n=============================================\n";
    cout << "     EXAM SEAT ALLOCATION SYSTEM\n";
    cout << "=============================================\n";
    cout << "1. Add Student\n";
    cout << "2. Display Students\n";
    cout << "3. Add Examination Room\n";
    cout << "4. Display Rooms\n";
    cout << "5. Allocate Seats Automatically\n";
    cout << "6. Display Seating Plan\n";
    cout << "7. Search Student / Seat\n";
    cout << "8. Save Data\n";
    cout << "0. Exit\n";
    cout << "---------------------------------------------\n";
    cout << "Enter your choice: ";
}


// ---------- Demo data for project presentation ----------

void addDemoDataIfEmpty() {
    // Add sample records only when no saved records exist.
    if (studentCount == 0 && roomCount == 0) {
        students[0] = {"12502120601001", "Aarav Patel", "AIDS", "", 0, false};
        students[1] = {"12502120601002", "Diya Shah", "AIDS", "", 0, false};
        students[2] = {"12502120601003", "Rohan Mehta", "AIDS", "", 0, false};
        students[3] = {"12502120601004", "Anaya Desai", "AIDS", "", 0, false};
        students[4] = {"12502120601005", "Krish Patel", "AIDS", "", 0, false};
        students[5] = {"12502120601006", "Ishita Shah", "AIDS", "", 0, false};
        students[6] = {"12502120601007", "Dev Joshi", "AIDS", "", 0, false};
        students[7] = {"12502120601008", "Mira Trivedi", "AIDS", "", 0, false};
        students[8] = {"12502120601009", "Arjun Parmar", "AIDS", "", 0, false};
        students[9] = {"12502120601010", "Kavya Patel", "AIDS", "", 0, false};
        studentCount = 10;

        rooms[0] = {"R101", 4, 0};
        rooms[1] = {"R102", 3, 0};
        rooms[2] = {"R103", 5, 0};
        roomCount = 3;

        saveData();
        cout << "\\nDemo data loaded: 10 students and 3 rooms.\\n";
    }
}

int main() {
    loadData();
    addDemoDataIfEmpty();

    int choice;
    do {
        showMenu();

        if (!(cin >> choice)) {
            cout << "Please enter a valid menu number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                addStudent();
                break;
            case 2:
                displayStudents();
                break;
            case 3:
                addRoom();
                break;
            case 4:
                displayRooms();
                break;
            case 5:
                allocateSeats();
                break;
            case 6:
                displaySeatingPlan();
                break;
            case 7:
                searchStudent();
                break;
            case 8:
                saveData();
                cout << "Data saved successfully.\n";
                break;
            case 0:
                saveData();
                cout << "Data saved. Exiting the system. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Select a number from the menu.\n";
        }
    } while (choice != 0);

    return 0;
}
