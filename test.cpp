# include <iostream>
# include <iomanip>
# include <fstream>
using namespace std;

// ---------- BASE CLASS ----------
class Vehicle {
protected:
	string number;
	int hours;
	int slot;
	bool vip;

public:
	Vehicle(string n, int h, int s, bool v) {
		number = n;
		hours = h;
		slot = s;
		vip = v;
	}

	virtual double calculateFee() = 0;   // implemented in child classes
    virtual string getType() = 0;

	string getNumber() {
        return number;
	}

	int getSlot() {
        return slot;
	}

	int getHours() {
		return hours;
	}

	bool isVIP() {
        return vip;
	}




	// display vehicle details in table format

	virtual void display() {

		cout << left << setw(8) << slot
		     << setw(12) << number
		     << setw(10) << getType()
		     << setw(8) << hours
		     << setw(6) << (vip ? "VIP" : "NO")
		     << setw(10) << calculateFee() << endl;
	}

	virtual ~Vehicle() {}
};

//    derived cassss

class Car : public Vehicle {
public:


	Car(string n, int h, int s, bool v) : Vehicle(n,h,s,v) {}

	// VIP cars cost more per 

	double calculateFee() {
		return vip ? hours * 30 : hours * 20;
	}

	string getType() {
		return "Car";
	}
};


class Bike : public Vehicle {
public:
	Bike(string n, int h, int s, bool v) : Vehicle(n,h,s,v) {}


	double calculateFee() {
		return vip ? hours * 20 : hours * 10;
	}



	string getType() {
		return "Bike";
	}
};



class Truck : public Vehicle {
public:
	Truck(string n, int h, int s, bool v) : Vehicle(n,h,s,v) {}

	double calculateFee() {
		return vip ? hours * 60 : hours * 40;
	}

	string getType() {
		return "Truck";
	}
};



//  Parking  System 

class ParkingSystem {
private:

	Vehicle* vehicles[100];             // store vehicles
	bool occupied[101];         // track slots
	int count;
	double collectedRevenue;         // revenue from removed vehicles

	// check duplicate vehicle number

	bool isDuplicate(string num) {
		for (int i = 0; i < count; i++)

			if (vehicles[i]->getNumber() == num)
				return true;
		return false;
	}

	// find vehicle by number

	int findVehicle(string num) {
		for (int i = 0; i < count; i++)

			if (vehicles[i]->getNumber() == num)
				return i;
		return -1;
	}

	// find vehicle using slot number

	int findBySlot(int slot) {
		for (int i = 0; i < count; i++)

			if (vehicles[i]->getSlot() == slot)
				return i;

		return -1;
	}

	// giving slot based on VIP

	int assignSlot(bool vip) {
		if (vip) {

			for (int i = 1; i <= 20; i++)
				if (!occupied[i]) {
					occupied[i] = true;
					return i;
				}

			cout << "VIP Full\n";
			return -1;


		} else {
			for (int i = 21; i <= 100; i++)
				if (!occupied[i]) {
					occupied[i] = true;
					return i;
				}

			cout << "Normal Full\n";
			return -1;
		}
	}

public:
	ParkingSystem() {
		count = 0;
		collectedRevenue = 0;

		for (int i = 1; i <= 100; i++)

			occupied[i] = false;
	}

	// destructor clears memory

	~ParkingSystem() {

		clearAll();
	}

	//   PARK VEHICLE

	void parkVehicle() {
		string num;
		int type, hours;
		bool vip;

		cout << "Enter number: ";
		cin >> num;

		if (isDuplicate(num)) {
			cout << "Already exists!\n";
			return;
		}

		cout << "Type (1-Car 2-Bike 3-Truck): ";
		cin >> type;

		cout << "Hours: ";
		cin >> hours;

		cout << "VIP (1/0): ";
		cin >> vip;

		int slot = assignSlot(vip);
		if (slot == -1) return;

		// create object based on type

		if (type == 1)
			vehicles[count] = new Car(num, hours, slot, vip);

		else if (type == 2)
			vehicles[count] = new Bike(num, hours, slot, vip);

		else if (type == 3)
			vehicles[count] = new Truck(num, hours, slot, vip);

		else {
			cout << "Invalid type\n";
			return;
		}
		count++;
		cout << "Parked at slot " << slot << endl;
	}



	// ---------- REMOVE VEHICLE ----------
	void removeVehicle() {

		string num;
		cout << "Enter number: ";
		cin >> num;

		int index = findVehicle(num);

		if (index == -1) {

			cout << "Not found!\n";
			return;
		}

		Vehicle* v = vehicles[index];

		double fee = v->calculateFee();

		collectedRevenue += fee;

		// display receipt

		cout << "\n===== RECEIPT =====\n";

		cout << "Number: " << v->getNumber() << endl;
		cout << "Type: " << v->getType() << endl;
		cout << "Slot: " << v->getSlot() << endl;
		cout << "Fee: " << fee << endl;
		cout << "===================\n";

		// save receipt in file

		ofstream r("receipt.txt", ios::app);
		r << "Number: " << v->getNumber()
		  << " Type: " << v->getType()
		  << " Slot: " << v->getSlot()
		  << " Fee: " << fee << endl;
		r.close();

		occupied[v->getSlot()] = false;

		delete v;

		// shift array
		for (int i = index; i < count - 1; i++)

			vehicles[i] = vehicles[i + 1];

		count--;
	}

	// ---------- SEARCH BY SLOT ----------

	void searchBySlot() {
		int slot;
		cout << "Enter slot: ";
		cin >> slot;

		int index = findBySlot(slot);

		if (index == -1) {
			cout << "Empty slot\n";
			return;
		}

		vehicles[index]->display();
	}

	// ---------- CLEAR ALL ----------

	void clearAll() {
		for (int i = 0; i < count; i++)

			delete vehicles[i];

		count = 0;

		for (int i = 1; i <= 100; i++)

			occupied[i] = false;

		cout << "All data cleared!\n";
	}



	// ---------- DISPLAY ----------


	void display() {
		cout << "\nSlot   Number      Type      Hours   VIP   Fee\n";
		cout << "-----------------------------------------\n";

		for (int i = 0; i < count; i++)

			vehicles[i]->display();
	}

	// ---------- SUMMARY ----------

	void summary() {
		double totalRevenue = 0;

		// revenue of parked vehicles
		for (int i = 0; i < count; i++)

			totalRevenue += vehicles[i]->calculateFee();

		totalRevenue += collectedRevenue;

		cout << "\nTotal Vehicles: " << count << endl;
		cout << "Collected Revenue: " << collectedRevenue << endl;
		cout << "Total Revenue (including parked): " << totalRevenue << endl;
	}

	// ---------- SAVE -------

	void saveToFile() {
		ofstream file("parking.txt");

		for (int i = 0; i < count; i++) {
			file << vehicles[i]->getNumber() << " "
			     << vehicles[i]->getType() << " "
			     << vehicles[i]->getHours() << " "
			     << vehicles[i]->getSlot() << " "
			     << vehicles[i]->isVIP() << endl;
		}

		file.close();
	}

	// ---------- LOAD ----------
	void loadFromFile() {
		ifstream file("parking.txt");

		string num, type;
		int hours, slot;
		bool vip;

		while (file >> num >> type >> hours >> slot >> vip) {
			occupied[slot] = true;

			if (type == "Car")
				vehicles[count++] = new Car(num, hours, slot, vip);
			else if (type == "Bike")
				vehicles[count++] = new Bike(num, hours, slot, vip);
			else if (type == "Truck")
				vehicles[count++] = new Truck(num, hours, slot, vip);
		}

		file.close();
	}
};

// ---------- MAIN ----------

int main() {

	string password;
	cout << "Enter admin password: ";
	cin >> password;

	if (password != "admin123") {
		cout << "Access denied!\n";
        return 0;
	}

	cout << "=====================================\n";
	cout << "   WELCOME TO PARKING SYSTEM APP\n";
	cout << "=====================================\n";

	ParkingSystem p;

	p.loadFromFile();

	int choice;

	do {
		cout << "\n1.Park 2.Remove 3.Display 4.Summary 5.SearchSlot 6.ClearAll 7.Exit\n";
		cin >> choice;

		switch (choice) {
		case 1:
			p.parkVehicle();
			break;
		case 2:
			p.removeVehicle();
			break;
		case 3:
			p.display();
			break;
		case 4:
			p.summary();
			break;
		case 5:
			p.searchBySlot();
			break;
		case 6:
			p.clearAll();
			break;
		case 7:
			p.saveToFile();

			cout << "Saved & Exiting...\n";
			break;
		}

	} while (choice != 7);

	return 0;
}