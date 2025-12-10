// Hospital Management System - Full Version with Admin, Export CSV, Search by Date
// Compile: g++ main.cpp -o hospital
// Run: ./hospital

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <limits>

using namespace std;

/* ----------------------- Helper Utils ----------------------- */

string nowDateTime() {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);
    return string(buf);
}

vector<string> split(const string &s, char delim) {
    vector<string> parts;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) parts.push_back(item);
    return parts;
}

/* ----------------------- Data Models ----------------------- */

class Patient {
public:
    int id;
    string name;
    int age;
    string disease;

    Patient() : id(0), age(0) {}
    Patient(int i, string n, int a, string d) : id(i), name(n), age(a), disease(d) {}

    string serialize() const {
        stringstream ss;
        ss << id << "|" << name << "|" << age << "|" << disease;
        return ss.str();
    }

    static Patient deserialize(const string &line) {
        auto p = split(line, '|');
        Patient pt;
        if (p.size() >= 4) {
            pt.id = stoi(p[0]);
            pt.name = p[1];
            pt.age = stoi(p[2]);
            pt.disease = p[3];
        }
        return pt;
    }

    void display() const {
        cout << "\nPatient ID: " << id;
        cout << "\nName: " << name;
        cout << "\nAge: " << age;
        cout << "\nDisease: " << disease << endl;
    }
};

class Appointment {
public:
    int appointmentID;
    int patientID;
    string doctor;
    string date;   // yyyy-mm-dd
    string time;   // hh:mm
    string status; // "Booked" / "Cancelled"

    Appointment() : appointmentID(0), patientID(0), status("Booked") {}
    Appointment(int aid, int pid, string doc, string d, string t)
        : appointmentID(aid), patientID(pid), doctor(doc), date(d), time(t), status("Booked") {}

    string serialize() const {
        stringstream ss;
        ss << appointmentID << "|" << patientID << "|" << doctor << "|" << date << "|" << time << "|" << status;
        return ss.str();
    }

    static Appointment deserialize(const string &line) {
        auto p = split(line, '|');
        Appointment ap;
        if (p.size() >= 6) {
            ap.appointmentID = stoi(p[0]);
            ap.patientID = stoi(p[1]);
            ap.doctor = p[2];
            ap.date = p[3];
            ap.time = p[4];
            ap.status = p[5];
        }
        return ap;
    }
};

class Bill {
public:
    int billID;
    int patientID;
    double amount;
    string details;
    string datetime;

    Bill() : billID(0), patientID(0), amount(0.0) {}
    Bill(int bid, int pid, double amt, string det, string dt)
        : billID(bid), patientID(pid), amount(amt), details(det), datetime(dt) {}

    string serialize() const {
        stringstream ss;
        ss << billID << "|" << patientID << "|" << amount << "|" << details << "|" << datetime;
        return ss.str();
    }

    static Bill deserialize(const string &line) {
        auto p = split(line, '|');
        Bill b;
        if (p.size() >= 5) {
            b.billID = stoi(p[0]);
            b.patientID = stoi(p[1]);
            b.amount = stod(p[2]);
            b.details = p[3];
            b.datetime = p[4];
        }
        return b;
    }
};

/* ----------------------- Global Data ----------------------- */

vector<Patient> patients;
vector<Appointment> appointments;
vector<Bill> bills;

/* ----------------------- Persistence ----------------------- */

const string PATIENTS_FILE = "patients.txt";
const string APPOINTMENTS_FILE = "appointments.txt";
const string BILLS_FILE = "bills.txt";

int nextPatientID() {
    int mx = 0;
    for (auto &p : patients) if (p.id > mx) mx = p.id;
    return mx + 1;
}

int nextAppointmentID() {
    int mx = 0;
    for (auto &a : appointments) if (a.appointmentID > mx) mx = a.appointmentID;
    return mx + 1;
}

int nextBillID() {
    int mx = 0;
    for (auto &b : bills) if (b.billID > mx) mx = b.billID;
    return mx + 1;
}

void savePatients() {
    ofstream fout(PATIENTS_FILE);
    for (auto &p : patients) fout << p.serialize() << "\n";
    fout.close();
}

void loadPatients() {
    patients.clear();
    ifstream fin(PATIENTS_FILE);
    string line;
    while (getline(fin, line)) {
        if (line.size() == 0) continue;
        patients.push_back(Patient::deserialize(line));
    }
    fin.close();
}

void saveAppointments() {
    ofstream fout(APPOINTMENTS_FILE);
    for (auto &a : appointments) fout << a.serialize() << "\n";
    fout.close();
}

void loadAppointments() {
    appointments.clear();
    ifstream fin(APPOINTMENTS_FILE);
    string line;
    while (getline(fin, line)) {
        if (line.size() == 0) continue;
        appointments.push_back(Appointment::deserialize(line));
    }
    fin.close();
}

void saveBills() {
    ofstream fout(BILLS_FILE);
    for (auto &b : bills) fout << b.serialize() << "\n";
    fout.close();
}

void loadBills() {
    bills.clear();
    ifstream fin(BILLS_FILE);
    string line;
    while (getline(fin, line)) {
        if (line.size() == 0) continue;
        bills.push_back(Bill::deserialize(line));
    }
    fin.close();
}

void saveAll() {
    savePatients();
    saveAppointments();
    saveBills();
}

void loadAll() {
    loadPatients();
    loadAppointments();
    loadBills();
}

/* ----------------------- Patient Operations ----------------------- */

void addPatient() {
    cout << "\n--- Add New Patient ---\n";
    string name, disease;
    int age;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter name: ";
    getline(cin, name);
    cout << "Enter age: ";
    cin >> age;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter disease/condition: ";
    getline(cin, disease);

    int id = nextPatientID();
    patients.push_back(Patient(id, name, age, disease));
    savePatients();

    cout << "Patient added with ID: " << id << "\n";
}

void viewPatients() {
    cout << "\n--- All Patients ---\n";
    if (patients.empty()) { cout << "No patients found.\n"; return; }
    for (auto &p : patients) {
        p.display();
        cout << "----------------------\n";
    }
}

Patient* findPatientByID(int id) {
    for (auto &p : patients) if (p.id == id) return &p;
    return nullptr;
}

void searchPatient() {
    cout << "\n--- Search Patient ---\n";
    cout << "Search by: 1) ID  2) Name\nChoice: ";
    int c; cin >> c;
    if (c == 1) {
        int id; cout << "Enter Patient ID: "; cin >> id;
        Patient* p = findPatientByID(id);
        if (p) p->display();
        else cout << "Patient not found.\n";
    } else if (c == 2) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string q; cout << "Enter full or partial name: ";
        getline(cin, q);
        bool found = false;
        for (auto &p : patients) {
            string nameLower = p.name; string qLower = q;
            transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            transform(qLower.begin(), qLower.end(), qLower.begin(), ::tolower);
            if (nameLower.find(qLower) != string::npos) {
                p.display(); cout << "----------------------\n";
                found = true;
            }
        }
        if (!found) cout << "No matching patients.\n";
    } else cout << "Invalid choice.\n";
}

void updatePatient() {
    cout << "\n--- Update Patient ---\n";
    int id; cout << "Enter Patient ID to update: "; cin >> id;
    Patient* p = findPatientByID(id);
    if (!p) { cout << "Patient not found.\n"; return; }

    cout << "Current details:\n"; p->display();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string name, disease;
    int age;
    cout << "Enter new name (leave blank to keep): ";
    getline(cin, name);
    cout << "Enter new age (0 to keep): ";
    cin >> age;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter new disease (leave blank to keep): ";
    getline(cin, disease);

    if (!name.empty()) p->name = name;
    if (age > 0) p->age = age;
    if (!disease.empty()) p->disease = disease;

    savePatients();
    cout << "Patient updated.\n";
}

void deletePatient() {
    cout << "\n--- Delete Patient ---\n";
    int id; cout << "Enter Patient ID to delete: "; cin >> id;
    auto it = find_if(patients.begin(), patients.end(), [&](const Patient &p){ return p.id == id; });
    if (it == patients.end()) { cout << "Patient not found.\n"; return; }

    // remove appointments and bills associated
    appointments.erase(remove_if(appointments.begin(), appointments.end(), [&](const Appointment &a){ return a.patientID == id; }), appointments.end());
    bills.erase(remove_if(bills.begin(), bills.end(), [&](const Bill &b){ return b.patientID == id; }), bills.end());

    patients.erase(it);
    saveAll();
    cout << "Patient and related data deleted.\n";
}

/* ----------------------- Appointment Operations ----------------------- */

void bookAppointment() {
    cout << "\n--- Book Appointment ---\n";
    int pid;
    cout << "Enter Patient ID: "; cin >> pid;
    Patient* p = findPatientByID(pid);
    if (!p) { cout << "Patient not found. Add patient first.\n"; return; }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string doctor, date, time;
    cout << "Doctor name: "; getline(cin, doctor);
    cout << "Date (YYYY-MM-DD): "; getline(cin, date);
    cout << "Time (HH:MM): "; getline(cin, time);

    int aid = nextAppointmentID();
    appointments.push_back(Appointment(aid, pid, doctor, date, time));
    saveAppointments();
    cout << "Appointment booked with ID: " << aid << "\n";
}

void viewAppointments() {
    cout << "\n--- All Appointments ---\n";
    if (appointments.empty()) { cout << "No appointments found.\n"; return; }
    for (auto &a : appointments) {
        cout << "Appointment ID: " << a.appointmentID << "\n";
        cout << "Patient ID: " << a.patientID << "\n";
        cout << "Doctor: " << a.doctor << "\n";
        cout << "Date: " << a.date << " Time: " << a.time << "\n";
        cout << "Status: " << a.status << "\n";
        cout << "----------------------\n";
    }
}

void cancelAppointment() {
    cout << "\n--- Cancel Appointment ---\n";
    int aid; cout << "Enter Appointment ID: "; cin >> aid;
    for (auto &a : appointments) {
        if (a.appointmentID == aid) {
            if (a.status == "Cancelled") { cout << "Already cancelled.\n"; return; }
            a.status = "Cancelled";
            saveAppointments();
            cout << "Appointment cancelled.\n";
            return;
        }
    }
    cout << "Appointment not found.\n";
}

void searchAppointmentsByDate() {
    cout << "\n--- Search Appointments by Date ---\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string date; cout << "Enter date (YYYY-MM-DD): "; getline(cin, date);
    bool found = false;
    for (auto &a : appointments) {
        if (a.date == date) {
            cout << "Appointment ID: " << a.appointmentID << " | Patient ID: " << a.patientID
                 << " | Doctor: " << a.doctor << " | Time: " << a.time << " | Status: " << a.status << "\n";
            found = true;
        }
    }
    if (!found) cout << "No appointments on " << date << "\n";
}

/* ----------------------- Billing Operations ----------------------- */

void generateBill() {
    cout << "\n--- Generate Bill ---\n";
    int pid; cout << "Enter Patient ID: "; cin >> pid;
    Patient* p = findPatientByID(pid);
    if (!p) { cout << "Patient not found.\n"; return; }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string details; double amount;
    cout << "Enter bill details/description: "; getline(cin, details);
    cout << "Enter amount: "; cin >> amount;

    int bid = nextBillID();
    string dt = nowDateTime();
    bills.push_back(Bill(bid, pid, amount, details, dt));
    saveBills();
    cout << "Bill generated with ID: " << bid << "\n";
}

void viewBillsForPatient() {
    cout << "\n--- View Bills for Patient ---\n";
    int pid; cout << "Enter Patient ID: "; cin >> pid;
    bool found = false;
    for (auto &b : bills) {
        if (b.patientID == pid) {
            cout << "Bill ID: " << b.billID << "\n";
            cout << "Amount: " << b.amount << "\n";
            cout << "Details: " << b.details << "\n";
            cout << "Datetime: " << b.datetime << "\n";
            cout << "----------------------\n";
            found = true;
        }
    }
    if (!found) cout << "No bills found for this patient.\n";
}

void viewAllBills() {
    cout << "\n--- All Bills ---\n";
    if (bills.empty()) { cout << "No bills found.\n"; return; }
    for (auto &b : bills) {
        cout << "Bill ID: " << b.billID << " | Patient ID: " << b.patientID << " | Amount: " << b.amount << " | " << b.datetime << "\n";
        cout << "Details: " << b.details << "\n";
        cout << "----------------------\n";
    }
}

void exportBillsToCSV() {
    cout << "\n--- Export Bills to CSV ---\n";
    ofstream fout("export_bills.csv");
    fout << "BillID,PatientID,Amount,Details,Datetime\n";
    for (auto &b : bills) {
        // double quotes around details in case it contains commas
        string deta = b.details;
        // replace any newline in details
        for (auto &ch : deta) if (ch == '\n' || ch == '\r') ch = ' ';
        fout << b.billID << "," << b.patientID << "," << b.amount << ",\"" << deta << "\"," << b.datetime << "\n";
    }
    fout.close();
    cout << "Bills exported to export_bills.csv\n";
}

/* ----------------------- Small Displays & Admin ----------------------- */

void showMainMenu() {
    cout << "\n===== Hospital Management System =====\n";
    cout << "1. Add Patient\n";
    cout << "2. View Patients\n";
    cout << "3. Search Patient\n";
    cout << "4. Update Patient\n";
    cout << "5. Delete Patient\n";
    cout << "6. Book Appointment\n";
    cout << "7. View Appointments\n";
    cout << "8. Cancel Appointment\n";
    cout << "9. Search Appointments by Date\n";
    cout << "10. Generate Bill\n";
    cout << "11. View Patient Bills\n";
    cout << "12. View All Bills\n";
    cout << "13. Export Bills to CSV\n";
    cout << "14. Save & Exit\n";
    cout << "Choice: ";
}

bool adminLogin() {
    const string ADMIN_USER = "admin";
    const string ADMIN_PASS = "admin123"; // you can change or load from config

    cout << "=== Admin Login Required ===\n";
    string u, p;
    cout << "Username: "; cin >> u;
    cout << "Password: "; cin >> p;
    if (u == ADMIN_USER && p == ADMIN_PASS) {
        cout << "Login successful.\n";
        return true;
    }
    cout << "Invalid credentials.\n";
    return false;
}

/* ----------------------- Main ----------------------- */

int main() {
    loadAll();
    cout << "Welcome to Hospital Management System\nData files: patients.txt, appointments.txt, bills.txt\n";

    if (!adminLogin()) {
        cout << "Exiting program (admin login failed).\n";
        return 0;
    }

    while (true) {
        showMainMenu();
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }

        switch (choice) {
            case 1: addPatient(); break;
            case 2: viewPatients(); break;
            case 3: searchPatient(); break;
            case 4: updatePatient(); break;
            case 5: deletePatient(); break;
            case 6: bookAppointment(); break;
            case 7: viewAppointments(); break;
            case 8: cancelAppointment(); break;
            case 9: searchAppointmentsByDate(); break;
            case 10: generateBill(); break;
            case 11: viewBillsForPatient(); break;
            case 12: viewAllBills(); break;
            case 13: exportBillsToCSV(); break;
            case 14:
                saveAll();
                cout << "Data saved. Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}


