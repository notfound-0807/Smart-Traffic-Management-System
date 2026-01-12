#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
using namespace std;

/* ================= ENUMS ================= */

enum class SignalState { RED, YELLOW, GREEN };
enum class VehicleType { NORMAL, EMERGENCY };
enum class SystemMode { AUTOMATIC, MANUAL, EMERGENCY_OVERRIDE };

/* ================= VEHICLE ================= */

class Vehicle {
    int id;
    VehicleType type;

public:
    Vehicle(int id, VehicleType type) : id(id), type(type) {}
    int getPriority() const {
        return (type == VehicleType::EMERGENCY) ? 100 : 10;
    }
    bool isEmergency() const {
        return type == VehicleType::EMERGENCY;
    }
};

/* ================= TRAFFIC LIGHT (FSM) ================= */

class TrafficLight {
    SignalState state;

public:
    TrafficLight() : state(SignalState::RED) {}

    void setState(SignalState s) {
        state = s;
    }

    string getState() const {
        if (state == SignalState::RED) return "RED";
        if (state == SignalState::GREEN) return "GREEN";
        return "YELLOW";
    }
};

/* ================= ROAD ================= */

class Road {
    string name;
    vector<Vehicle> vehicles;
    bool accident;
    bool pedestrian;
    TrafficLight signal;
    int waitTime;

public:
    Road(string name)
        : name(name), accident(false), pedestrian(false), waitTime(0) {}

    void addVehicle(const Vehicle& v) {
        vehicles.push_back(v);
    }

    int density() const {
        return vehicles.size();
    }

    bool hasEmergency() const {
        for (auto &v : vehicles)
            if (v.isEmergency()) return true;
        return false;
    }

    void setAccident(bool status) {
        accident = status;
    }

    bool hasAccident() const {
        return accident;
    }

    void requestPedestrian() {
        pedestrian = true;
    }

    bool hasPedestrian() const {
        return pedestrian;
    }

    void incrementWait() {
        waitTime++;
    }

    int getWaitTime() const {
        return waitTime;
    }

    void resetCycle() {
        vehicles.clear();
        pedestrian = false;
        waitTime = 0;
    }

    string getName() const { return name; }
    TrafficLight& getSignal() { return signal; }
};

/* ================= AI SIGNAL CONTROLLER ================= */

class SignalController {
public:
    Road* decideGreen(vector<Road>& roads) {
        double bestScore = -1;
        Road* selected = nullptr;

        for (auto &r : roads) {
            if (r.hasAccident()) continue;

            double score =
                r.density() * 0.4 +
                r.getWaitTime() * 0.3 +
                (r.hasEmergency() ? 50 : 0) +
                (r.hasPedestrian() ? 5 : 0);

            if (score > bestScore) {
                bestScore = score;
                selected = &r;
            }
        }
        return selected;
    }

    int calculateGreenTime(Road& r) {
        return 20 + r.density() * 2 + (r.hasEmergency() ? 30 : 0);
    }
};

/* ================= INTERSECTION ================= */

class Intersection {
    int id;
    vector<Road> roads;
    SignalController controller;

public:
    Intersection(int id) : id(id) {
        roads.emplace_back("North");
        roads.emplace_back("South");
        roads.emplace_back("East");
        roads.emplace_back("West");
    }

    vector<Road>& getRoads() { return roads; }

    bool hasEmergency() {
        for (auto &r : roads)
            if (r.hasEmergency()) return true;
        return false;
    }

    void simulate() {
        for (auto &r : roads) r.incrementWait();

        Road* green = controller.decideGreen(roads);
        if (!green) return;

        for (auto &r : roads)
            r.getSignal().setState(SignalState::RED);

        green->getSignal().setState(SignalState::GREEN);
        int time = controller.calculateGreenTime(*green);

        display(green->getName(), time);
        green->resetCycle();
    }

    void display(string greenRoad, int time) {
        cout << "\n--- Intersection " << id << " ---\n";
        cout << "GREEN: " << greenRoad << " | Time: " << time << "s\n";

        cout << left << setw(10) << "Road"
             << setw(10) << "Density"
             << setw(10) << "Signal" << endl;

        for (auto &r : roads) {
            cout << setw(10) << r.getName()
                 << setw(10) << r.density()
                 << setw(10) << r.getSignal().getState()
                 << endl;
        }
    }
};

/* ================= CENTRAL TRAFFIC SERVER ================= */

class CentralTrafficServer {
public:
    void monitor(vector<Intersection>& intersections) {
        cout << "\n=== CENTRAL TRAFFIC SERVER ===\n";
        for (auto &i : intersections) {
            if (i.hasEmergency()) {
                cout << "Emergency detected → GREEN WAVE ACTIVATED\n";
            }
        }
    }
};

/* ================= CITY ================= */

class City {
    vector<Intersection> intersections;
    CentralTrafficServer server;
    SystemMode mode;

public:
    City(int count) : mode(SystemMode::AUTOMATIC) {
        for (int i = 1; i <= count; i++)
            intersections.emplace_back(i);
    }

    void populateTraffic() {
        intersections[0].getRoads()[1].addVehicle(Vehicle(1, VehicleType::EMERGENCY));
        intersections[1].getRoads()[2].addVehicle(Vehicle(2, VehicleType::NORMAL));
        intersections[1].getRoads()[2].addVehicle(Vehicle(3, VehicleType::NORMAL));
        intersections[2].getRoads()[0].addVehicle(Vehicle(4, VehicleType::NORMAL));
        intersections[2].getRoads()[0].requestPedestrian();
    }

    void run() {
        populateTraffic();
        server.monitor(intersections);

        for (auto &i : intersections)
            i.simulate();
    }
};

/* ================= MAIN ================= */

int main() {
    City smartCity(3);   // 3 intersections
    smartCity.run();
    return 0;
}
