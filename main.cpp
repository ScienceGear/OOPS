#include <windows.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cctype>
using namespace std;


//remove f and b spaces
string getLine() {
    string s;
    getline(cin, s);
    // remove spaces from front and back
    int a = 0, b = s.size() - 1;
    while (a <= b && s[a] == ' ') a++;
    while (b >= a && s[b] == ' ') b--;
    if (a > b) return "";
    return s.substr(a, b - a + 1);
}

// asks for a number ask until valid
int getInt(string msg, int lo, int hi) {
    while (true) {
        cout << msg;
        string s = getLine();
        if (s.empty()) { cout << "  Please enter a number.\n"; continue; }

        bool ok = true;
        for (char c : s)
            if (c < '0' || c > '9') { ok = false; break; }

        if (!ok || s.size() > 9) { cout << "  Invalid. Try again.\n"; continue; }

        int n = stoi(s);
        if (n < lo || n > hi) {
            cout << "  Enter between " << lo << " and " << hi << ".\n";
            continue;
        }
        return n;
    }
}

// asks for vehicle number letters, digits, hyphens
string getVehicleNum(string msg) {
    while (true) {
        cout << msg;
        string s = getLine();
        if (s.empty()) { cout << "  Can't be empty.\n"; continue; }
        if (s.size() > 15) { cout << "  Too long (max 15 chars).\n"; continue; }

        bool ok = true;
        for (char c : s)
            if (!isalnum(c) && c != '-') { ok = false; break; }//cctype lib but working without it

        if (!ok) { cout << "  Only letters, digits, hyphens. Example: MH12-AB-1234\n"; continue; }
        return s;
    }
}



class Chargeable {
public:
    virtual double getTax() = 0;
    virtual ~Chargeable() {}
};


class Vehicle {
protected:
    string number; 
    int    hours;
    int    slot;
    bool   vip;

public:
    Vehicle(string n, int h, int s, bool v) {
        this->number = n; 
        this->hours  = h;
        this->slot   = s;
        this->vip    = v;
    }

    Vehicle(const Vehicle& other) {
        this->number = other.number;
        this->hours  = other.hours;
        this->slot   = other.slot;
        this->vip    = other.vip;
    }

    string getNumber() { return number; }
    int    getSlot()   { return slot; }
    int    getHours()  { return hours; }
    bool   isVIP()     { return vip; }


    void setHours(int h) {
        if (h <= 0) throw invalid_argument("Hours must be > 0");
        this->hours = h;
    }

    bool operator==(const Vehicle& other) {
        return this->number == other.number;
    }


    virtual double calculateFee() = 0;
    virtual string getType()      = 0;

    virtual void display() {
        cout << left
             << setw(6)  << slot
             << setw(14) << number
             << setw(8)  << getType()
             << setw(8)  << hours
             << setw(6)  << (vip ? "VIP" : "NO")
             << "Rs. " << fixed << setprecision(2) << calculateFee()
             << "\n";
    }

    virtual ~Vehicle() {}
};


class Car : public Vehicle, public Chargeable {
public:
    Car(string n, int h, int s, bool v) : Vehicle(n, h, s, v) {}

    Car(const Car& other) : Vehicle(other) {}

    double calculateFee() override { return vip ? hours * 30.0 : hours * 20.0; }
    double getTax()       override { return calculateFee() * 0.10; }  // 10%
    string getType()      override { return "Car"; }
};

class Bike : public Vehicle, public Chargeable {
public:
    Bike(string n, int h, int s, bool v) : Vehicle(n, h, s, v) {}
    Bike(const Bike& other) : Vehicle(other) {}

    double calculateFee() override { return vip ? hours * 20.0 : hours * 10.0; }
    double getTax()       override { return calculateFee() * 0.05; }  // 5%
    string getType()      override { return "Bike"; }
};

class Truck : public Vehicle, public Chargeable {
public:
    Truck(string n, int h, int s, bool v) : Vehicle(n, h, s, v) {}
    Truck(const Truck& other) : Vehicle(other) {}

    double calculateFee() override { return vip ? hours * 60.0 : hours * 40.0; }
    double getTax()       override { return calculateFee() * 0.15; }  // 15%
    string getType()      override { return "Truck"; }
};



class ParkingStats {
public:
    static int    totalParked;     
    static double totalTax;     

    static void add(double tax) {
        totalParked++;
        totalTax += tax;
    }

    static void show() {
        cout << "Total ever parked  : " << totalParked << "\n";
        cout << "Total tax collected: Rs. " << fixed << setprecision(2) << totalTax << "\n";
    }
};


int    ParkingStats::totalParked = 0;
double ParkingStats::totalTax    = 0.0;


class ParkingSystem;
void printReceipt(Vehicle* v, double fee, double tax);

string buildVehicleRow(Vehicle* v) {
    ostringstream out;
    out << left
        << setw(6)  << v->getSlot()
        << setw(14) << v->getNumber()
        << setw(8)  << v->getType()
        << setw(8)  << v->getHours()
        << setw(6)  << (v->isVIP() ? "VIP" : "NO")
        << "Rs. " << fixed << setprecision(2) << v->calculateFee();
    return out.str();
}

string buildReceiptText(Vehicle* v, double fee, double tax) {
    ostringstream out;
    out << "====== RECEIPT ======\n";
    out << "Number : " << v->getNumber() << "\n";
    out << "Type   : " << v->getType()   << "\n";
    out << "Slot   : " << v->getSlot()   << "\n";
    out << "Hours  : " << v->getHours()  << "\n";
    out << "Fee    : Rs. " << fixed << setprecision(2) << fee << "\n";
    out << "Tax    : Rs. " << tax << "\n";
    out << "Total  : Rs. " << (fee + tax) << "\n";
    out << "=====================\n";
    return out.str();
}



class ParkingSystem {
private:
    Vehicle* slots[100];    
    bool     taken[101];    
    int      count;        
    double   earned;        

    friend void printReceipt(Vehicle* v, double fee, double tax);

    bool exists(string num) {
        for (int i = 0; i < count; i++)
            if (slots[i]->getNumber() == num) return true;
        return false;
    }

    int findByNum(string num) {
        for (int i = 0; i < count; i++)
            if (slots[i]->getNumber() == num) return i;
        return -1;
    }

    int findBySlot(int s) {
        for (int i = 0; i < count; i++)
            if (slots[i]->getSlot() == s) return i;
        return -1;
    }

    int assignSlot(bool vip) {
        int from = vip ? 1  : 21;
        int to   = vip ? 20 : 100;
        for (int i = from; i <= to; i++)
            if (!taken[i]) { taken[i] = true; return i; }
        return -1;
    }

public:
    ParkingSystem() {
        count  = 0;
        earned = 0;
        for (int i = 0; i <= 100; i++) taken[i] = false;
    }

    ~ParkingSystem() {
        for (int i = 0; i < count; i++) delete slots[i];
    }

    string parkVehicleDirect(const string& num, int type, int hours, bool vip) {
        if (exists(num))
            throw runtime_error(num + " is already parked!");
        if (type < 1 || type > 3)
            throw invalid_argument("Invalid vehicle type.");
        if (hours < 1 || hours > 72)
            throw invalid_argument("Hours must be between 1 and 72.");

        int s = assignSlot(vip);
        if (s == -1) throw runtime_error("No slots available!");

        Vehicle* v;
        if      (type == 1) v = new Car(num, hours, s, vip);
        else if (type == 2) v = new Bike(num, hours, s, vip);
        else                v = new Truck(num, hours, s, vip);

        slots[count++] = v;

        Chargeable* c = dynamic_cast<Chargeable*>(v);
        if (c) ParkingStats::add(c->getTax());

        ostringstream out;
        out << "Done! Parked " << num << " at slot " << s << " (" << v->getType() << ").\n";
        return out.str();
    }

    string removeVehicleDirect(const string& num) {
        int idx = findByNum(num);
        if (idx == -1) throw runtime_error(num + " not found!");

        Vehicle* v  = slots[idx];
        double fee  = v->calculateFee();
        double tax  = dynamic_cast<Chargeable*>(v)->getTax();

        earned += fee;

        string receipt = buildReceiptText(v, fee, tax);

        ofstream f("receipt.txt", ios::app);
        if (f) {
            f << v->getNumber() << " " << v->getType()
              << " Slot:" << v->getSlot()
              << " Fee:Rs." << fixed << setprecision(2) << fee
              << " Tax:Rs." << tax << "\n";
            f.close();
        }

        taken[v->getSlot()] = false;
        delete v;

        for (int i = idx; i < count - 1; i++)
            slots[i] = slots[i + 1];
        count--;

        ostringstream out;
        out << receipt;
        out << "Removed " << num << " from the parking area.\n";
        return out.str();
    }

    string searchText(int s) {
        if (s < 1 || s > 100)
            throw invalid_argument("Slot must be between 1 and 100.");
        int idx = findBySlot(s);
        if (idx == -1) {
            ostringstream out;
            out << "Slot " << s << " is empty.\n";
            return out.str();
        }
        ostringstream out;
        out << "--- Slot " << s << " ---\n";
        out << buildVehicleRow(slots[idx]) << "\n";
        return out.str();
    }

    string displayAllText() {
        ostringstream out;
        if (count == 0) {
            out << "No vehicles parked.\n";
            return out.str();
        }

        out << left
            << setw(6)  << "Slot"
            << setw(14) << "Number"
            << setw(8)  << "Type"
            << setw(8)  << "Hours"
            << setw(6)  << "VIP"
            << "Fee\n"
            << string(52, '-') << "\n";

        for (int i = 0; i < count; i++)
            out << buildVehicleRow(slots[i]) << "\n";

        return out.str();
    }

    string summaryText() {
        double pending = 0;
        for (int i = 0; i < count; i++)
            pending += slots[i]->calculateFee();

        ostringstream out;
        out << "--- Summary ---\n";
        out << "Currently parked : " << count << "\n";
        out << "Collected so far : Rs. " << fixed << setprecision(2) << earned << "\n";
        out << "Pending (parked) : Rs. " << pending << "\n";
        out << "Grand Total      : Rs. " << (earned + pending) << "\n";
        out << "Total ever parked : " << ParkingStats::totalParked << "\n";
        out << "Total tax collected: Rs. " << fixed << setprecision(2) << ParkingStats::totalTax << "\n";
        return out.str();
    }

   
    void park() {
        string num = getVehicleNum("Vehicle number : ");
        int type  = getInt("Type (1=Car 2=Bike 3=Truck): ", 1, 3);
        int hours = getInt("Hours (1-72)              : ", 1, 72);
        int vip   = getInt("VIP? (1=Yes  0=No)        : ", 0, 1);

        cout << parkVehicleDirect(num, type, hours, vip);
    }

    void remove() {
        string num = getVehicleNum("Vehicle number: ");
        cout << removeVehicleDirect(num);
    }

    void search() {
        int s   = getInt("Slot number (1-100): ", 1, 100);
        cout << searchText(s);
    }


    void displayAll() {
        cout << "\n" << displayAllText();
    }


    void summary() {
        cout << "\n" << summaryText();
    }

    void clearAll() {
        for (int i = 0; i < count; i++) {
            taken[slots[i]->getSlot()] = false;
            delete slots[i];
        }
        count = 0;
        cout << "All cleared.\n";
    }

    void saveToFile() {
        ofstream f("parking.txt");
        if (!f) throw runtime_error("Cannot open parking.txt");
        for (int i = 0; i < count; i++)
            f << slots[i]->getNumber() << " "
              << slots[i]->getType()   << " "
              << slots[i]->getHours()  << " "
              << slots[i]->getSlot()   << " "
              << slots[i]->isVIP()     << "\n";
        f.close();
    }
    int loadFromFile() {
        ifstream f("parking.txt");
        if (!f) return 0;

        string num, type;
        int hours, s, vip;
        int loaded = 0;
        while (f >> num >> type >> hours >> s >> vip) {
            taken[s] = true;
            if      (type == "Car")   slots[count++] = new Car(num, hours, s, vip);
            else if (type == "Bike")  slots[count++] = new Bike(num, hours, s, vip);
            else if (type == "Truck") slots[count++] = new Truck(num, hours, s, vip);
            loaded++;
        }
        f.close();
        if (loaded > 0) cout << loaded << " vehicle(s) loaded.\n";
        return loaded;
    }

    void saveSummary() {
        double total = earned;
        for (int i = 0; i < count; i++)
            total += slots[i]->calculateFee();

        ofstream f("summary.txt");
        if (!f) return;
        f << "=== Parking Summary ===\n";
        f << "Total Vehicles Ever Parked : " << ParkingStats::totalParked << "\n";
        f << "Total Tax Collected        : Rs. " << fixed << setprecision(2) << ParkingStats::totalTax << "\n";
        f << "Grand Total Revenue        : Rs. " << total << "\n";
        f.close();

        cout << "Summary saved to summary.txt\n";
    }
};


void printReceipt(Vehicle* v, double fee, double tax) {
    cout << "\n" << buildReceiptText(v, fee, tax);
}

struct GuiState {
    HWND hwnd = nullptr;
    HWND output = nullptr;
    HWND passEdit = nullptr;
    HWND unlockButton = nullptr;
    HWND numberEdit = nullptr;
    HWND typeCombo = nullptr;
    HWND hoursEdit = nullptr;
    HWND vipCheck = nullptr;
    HWND parkButton = nullptr;
    HWND removeEdit = nullptr;
    HWND removeButton = nullptr;
    HWND slotEdit = nullptr;
    HWND searchButton = nullptr;
    HWND displayButton = nullptr;
    HWND summaryButton = nullptr;
    HWND clearButton = nullptr;
    HWND saveButton = nullptr;
    HWND exitButton = nullptr;
    HWND banner = nullptr;
    HWND subtitle = nullptr;
    HFONT titleFont = nullptr;
    HFONT uiFont = nullptr;
    HFONT monoFont = nullptr;
    HBRUSH background = nullptr;
    bool unlocked = false;
};

enum ControlId {
    ID_PASS_EDIT = 1001,
    ID_UNLOCK,
    ID_NUMBER_EDIT,
    ID_TYPE_COMBO,
    ID_HOURS_EDIT,
    ID_VIP_CHECK,
    ID_PARK_BUTTON,
    ID_REMOVE_EDIT,
    ID_REMOVE_BUTTON,
    ID_SLOT_EDIT,
    ID_SEARCH_BUTTON,
    ID_DISPLAY_BUTTON,
    ID_SUMMARY_BUTTON,
    ID_CLEAR_BUTTON,
    ID_SAVE_BUTTON,
    ID_EXIT_BUTTON,
    ID_OUTPUT
};

GuiState gGui;
ParkingSystem gParking;
string gLog;

string trimText(string value) {
    size_t start = 0;
    while (start < value.size() && isspace(static_cast<unsigned char>(value[start]))) start++;
    size_t end = value.size();
    while (end > start && isspace(static_cast<unsigned char>(value[end - 1]))) end--;
    return value.substr(start, end - start);
}

string getWindowTextString(HWND hwnd) {
    int len = GetWindowTextLengthA(hwnd);
    string value(len, '\0');
    if (len > 0) GetWindowTextA(hwnd, &value[0], len + 1);
    return trimText(value);
}

bool parseIntText(HWND hwnd, int lo, int hi, int& value) {
    string text = getWindowTextString(hwnd);
    if (text.empty()) return false;
    try {
        size_t pos = 0;
        int parsed = stoi(text, &pos);
        if (pos != text.size()) return false;
        if (parsed < lo || parsed > hi) return false;
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

void setOutputText(const string& text) {
    gLog = text;
    if (gGui.output) SetWindowTextA(gGui.output, gLog.c_str());
    if (gGui.output) SendMessageA(gGui.output, EM_LINESCROLL, 0, 1000);
}

void appendOutput(const string& text) {
    if (!gLog.empty() && gLog.back() != '\n') gLog += "\r\n";
    gLog += text;
    if (!gLog.empty() && gLog.back() != '\n') gLog += "\r\n";
    if (gGui.output) {
        SetWindowTextA(gGui.output, gLog.c_str());
        SendMessageA(gGui.output, EM_LINESCROLL, 0, 1000);
    }
}

void setMainControlsEnabled(bool enabled) {
    HWND controls[] = {
        gGui.numberEdit, gGui.typeCombo, gGui.hoursEdit, gGui.vipCheck, gGui.parkButton,
        gGui.removeEdit, gGui.removeButton, gGui.slotEdit, gGui.searchButton,
        gGui.displayButton, gGui.summaryButton, gGui.clearButton, gGui.saveButton
    };
    for (HWND control : controls) if (control) EnableWindow(control, enabled);
}

void applyFonts(HWND hwnd) {
    SendMessageA(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(gGui.uiFont), TRUE);
}

void createGuiControls(HWND hwnd) {
    gGui.banner = CreateWindowA("STATIC", "Parking Control Center", WS_CHILD | WS_VISIBLE,
        24, 18, 420, 32, hwnd, nullptr, nullptr, nullptr);
    gGui.subtitle = CreateWindowA("STATIC", "A cleaner dashboard for parking, receipts, and summaries.", WS_CHILD | WS_VISIBLE,
        24, 48, 520, 20, hwnd, nullptr, nullptr, nullptr);

    CreateWindowA("STATIC", "Admin password", WS_CHILD | WS_VISIBLE,
        24, 82, 120, 20, hwnd, nullptr, nullptr, nullptr);
    gGui.passEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_PASSWORD | ES_AUTOHSCROLL,
        24, 104, 180, 24, hwnd, reinterpret_cast<HMENU>(ID_PASS_EDIT), nullptr, nullptr);
    gGui.unlockButton = CreateWindowA("BUTTON", "Unlock", WS_CHILD | WS_VISIBLE,
        214, 103, 90, 26, hwnd, reinterpret_cast<HMENU>(ID_UNLOCK), nullptr, nullptr);

    CreateWindowA("BUTTON", "Vehicle Entry", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        24, 144, 420, 196, hwnd, nullptr, nullptr, nullptr);
    CreateWindowA("STATIC", "Number", WS_CHILD | WS_VISIBLE,
        40, 172, 100, 18, hwnd, nullptr, nullptr, nullptr);
    gGui.numberEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        40, 192, 180, 24, hwnd, reinterpret_cast<HMENU>(ID_NUMBER_EDIT), nullptr, nullptr);
    CreateWindowA("STATIC", "Type", WS_CHILD | WS_VISIBLE,
        236, 172, 100, 18, hwnd, nullptr, nullptr, nullptr);
    gGui.typeCombo = CreateWindowA("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        236, 192, 180, 120, hwnd, reinterpret_cast<HMENU>(ID_TYPE_COMBO), nullptr, nullptr);
    SendMessageA(gGui.typeCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Car"));
    SendMessageA(gGui.typeCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Bike"));
    SendMessageA(gGui.typeCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Truck"));
    SendMessageA(gGui.typeCombo, CB_SETCURSEL, 0, 0);
    CreateWindowA("STATIC", "Hours (1-72)", WS_CHILD | WS_VISIBLE,
        40, 226, 100, 18, hwnd, nullptr, nullptr, nullptr);
    gGui.hoursEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        40, 246, 180, 24, hwnd, reinterpret_cast<HMENU>(ID_HOURS_EDIT), nullptr, nullptr);
    gGui.vipCheck = CreateWindowA("BUTTON", "VIP parking", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        236, 244, 140, 24, hwnd, reinterpret_cast<HMENU>(ID_VIP_CHECK), nullptr, nullptr);
    gGui.parkButton = CreateWindowA("BUTTON", "Park Vehicle", WS_CHILD | WS_VISIBLE,
        236, 282, 180, 28, hwnd, reinterpret_cast<HMENU>(ID_PARK_BUTTON), nullptr, nullptr);

    CreateWindowA("BUTTON", "Quick Actions", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        24, 352, 420, 196, hwnd, nullptr, nullptr, nullptr);
    CreateWindowA("STATIC", "Remove by number", WS_CHILD | WS_VISIBLE,
        40, 380, 120, 18, hwnd, nullptr, nullptr, nullptr);
    gGui.removeEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        40, 400, 180, 24, hwnd, reinterpret_cast<HMENU>(ID_REMOVE_EDIT), nullptr, nullptr);
    gGui.removeButton = CreateWindowA("BUTTON", "Remove", WS_CHILD | WS_VISIBLE,
        236, 398, 180, 28, hwnd, reinterpret_cast<HMENU>(ID_REMOVE_BUTTON), nullptr, nullptr);
    CreateWindowA("STATIC", "Search slot", WS_CHILD | WS_VISIBLE,
        40, 434, 120, 18, hwnd, nullptr, nullptr, nullptr);
    gGui.slotEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        40, 454, 180, 24, hwnd, reinterpret_cast<HMENU>(ID_SLOT_EDIT), nullptr, nullptr);
    gGui.searchButton = CreateWindowA("BUTTON", "Search Slot", WS_CHILD | WS_VISIBLE,
        236, 452, 180, 28, hwnd, reinterpret_cast<HMENU>(ID_SEARCH_BUTTON), nullptr, nullptr);
    gGui.displayButton = CreateWindowA("BUTTON", "Display All", WS_CHILD | WS_VISIBLE,
        40, 492, 120, 28, hwnd, reinterpret_cast<HMENU>(ID_DISPLAY_BUTTON), nullptr, nullptr);
    gGui.summaryButton = CreateWindowA("BUTTON", "Summary", WS_CHILD | WS_VISIBLE,
        170, 492, 120, 28, hwnd, reinterpret_cast<HMENU>(ID_SUMMARY_BUTTON), nullptr, nullptr);
    gGui.clearButton = CreateWindowA("BUTTON", "Clear All", WS_CHILD | WS_VISIBLE,
        300, 492, 116, 28, hwnd, reinterpret_cast<HMENU>(ID_CLEAR_BUTTON), nullptr, nullptr);

    gGui.saveButton = CreateWindowA("BUTTON", "Save", WS_CHILD | WS_VISIBLE,
        24, 562, 100, 32, hwnd, reinterpret_cast<HMENU>(ID_SAVE_BUTTON), nullptr, nullptr);
    gGui.exitButton = CreateWindowA("BUTTON", "Exit", WS_CHILD | WS_VISIBLE,
        134, 562, 100, 32, hwnd, reinterpret_cast<HMENU>(ID_EXIT_BUTTON), nullptr, nullptr);

    gGui.output = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
        468, 82, 480, 512, hwnd, reinterpret_cast<HMENU>(ID_OUTPUT), nullptr, nullptr);

    HWND allControls[] = {
        gGui.banner, gGui.subtitle, gGui.passEdit, gGui.unlockButton,
        gGui.numberEdit, gGui.typeCombo, gGui.hoursEdit, gGui.vipCheck, gGui.parkButton,
        gGui.removeEdit, gGui.removeButton, gGui.slotEdit, gGui.searchButton,
        gGui.displayButton, gGui.summaryButton, gGui.clearButton, gGui.saveButton,
        gGui.exitButton, gGui.output
    };
    for (HWND control : allControls) applyFonts(control);

    SendMessageA(gGui.banner, WM_SETFONT, reinterpret_cast<WPARAM>(gGui.titleFont), TRUE);
    SendMessageA(gGui.output, WM_SETFONT, reinterpret_cast<WPARAM>(gGui.monoFont), TRUE);
    setMainControlsEnabled(false);
}

void initializeGuiTheme(HWND hwnd) {
    gGui.background = CreateSolidBrush(RGB(245, 248, 252));
    gGui.uiFont = CreateFontA(18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    gGui.titleFont = CreateFontA(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    gGui.monoFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");

    createGuiControls(hwnd);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        initializeGuiTheme(hwnd);
        gParking.loadFromFile();
        setOutputText("Parking dashboard ready.\r\nEnter the admin password to unlock the controls.");
        break;

    case WM_COMMAND: {
        const int id = LOWORD(wParam);
        const int code = HIWORD(wParam);
        if (id == ID_UNLOCK && code == BN_CLICKED) {
            string pass = getWindowTextString(gGui.passEdit);
            if (pass != "admin123") {
                appendOutput("Access denied. Try the admin password again.");
                SetFocus(gGui.passEdit);
                SendMessageA(gGui.passEdit, EM_SETSEL, 0, -1);
                break;
            }

            gGui.unlocked = true;
            setMainControlsEnabled(true);
            EnableWindow(gGui.passEdit, false);
            EnableWindow(gGui.unlockButton, false);
            appendOutput("Access granted. Controls unlocked.");
            break;
        }

        if (!gGui.unlocked) break;

        try {
            if (id == ID_PARK_BUTTON && code == BN_CLICKED) {
                int typeIndex = static_cast<int>(SendMessageA(gGui.typeCombo, CB_GETCURSEL, 0, 0));
                int hours = 0;
                if (typeIndex < 0) throw runtime_error("Select a vehicle type.");
                if (getWindowTextString(gGui.numberEdit).empty()) throw runtime_error("Vehicle number cannot be empty.");
                if (!parseIntText(gGui.hoursEdit, 1, 72, hours)) throw runtime_error("Hours must be between 1 and 72.");
                bool vip = SendMessageA(gGui.vipCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
                string result = gParking.parkVehicleDirect(getWindowTextString(gGui.numberEdit), typeIndex + 1, hours, vip);
                appendOutput(result);
                SetWindowTextA(gGui.numberEdit, "");
                SetWindowTextA(gGui.hoursEdit, "");
                SendMessageA(gGui.vipCheck, BM_SETCHECK, BST_UNCHECKED, 0);
                SendMessageA(gGui.typeCombo, CB_SETCURSEL, 0, 0);
            } else if (id == ID_REMOVE_BUTTON && code == BN_CLICKED) {
                string num = getWindowTextString(gGui.removeEdit);
                if (num.empty()) throw runtime_error("Enter a vehicle number to remove.");
                appendOutput(gParking.removeVehicleDirect(num));
                SetWindowTextA(gGui.removeEdit, "");
            } else if (id == ID_SEARCH_BUTTON && code == BN_CLICKED) {
                int slot = 0;
                if (!parseIntText(gGui.slotEdit, 1, 100, slot)) throw runtime_error("Slot must be between 1 and 100.");
                appendOutput(gParking.searchText(slot));
            } else if (id == ID_DISPLAY_BUTTON && code == BN_CLICKED) {
                appendOutput(gParking.displayAllText());
            } else if (id == ID_SUMMARY_BUTTON && code == BN_CLICKED) {
                appendOutput(gParking.summaryText());
            } else if (id == ID_CLEAR_BUTTON && code == BN_CLICKED) {
                gParking.clearAll();
                appendOutput("All parked vehicles cleared.");
            } else if (id == ID_SAVE_BUTTON && code == BN_CLICKED) {
                gParking.saveToFile();
                gParking.saveSummary();
                appendOutput("Parking data saved to parking.txt and summary.txt.");
            } else if (id == ID_EXIT_BUTTON && code == BN_CLICKED) {
                SendMessageA(hwnd, WM_CLOSE, 0, 0);
            }
        } catch (const invalid_argument& e) {
            appendOutput(string("Input error: ") + e.what());
        } catch (const runtime_error& e) {
            appendOutput(string("Error: ") + e.what());
        } catch (...) {
            appendOutput("Something went wrong.");
        }
        break;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, RGB(31, 41, 55));
        SetBkMode(hdc, TRANSPARENT);
        return reinterpret_cast<LRESULT>(gGui.background);
    }

    case WM_CTLCOLOREDIT: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, RGB(17, 24, 39));
        SetBkColor(hdc, RGB(255, 255, 255));
        return reinterpret_cast<LRESULT>(gGui.background);
    }

    case WM_CTLCOLORDLG:
        return reinterpret_cast<LRESULT>(gGui.background);

    case WM_CLOSE:
        gParking.saveToFile();
        gParking.saveSummary();
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        if (gGui.titleFont) DeleteObject(gGui.titleFont);
        if (gGui.uiFont) DeleteObject(gGui.uiFont);
        if (gGui.monoFont) DeleteObject(gGui.monoFont);
        if (gGui.background) DeleteObject(gGui.background);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    HINSTANCE instance = GetModuleHandleA(nullptr);
    const char CLASS_NAME[] = "ParkingControlCenterWindow";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassA(&wc)) {
        MessageBoxA(nullptr, "Failed to register window class.", "Parking System", MB_ICONERROR | MB_OK);
        return 0;
    }

    RECT rect = {0, 0, 980, 650};
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Parking Control Center",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!hwnd) {
        MessageBoxA(nullptr, "Failed to create the main window.", "Parking System", MB_ICONERROR | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageA(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}