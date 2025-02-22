#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <ctime>    // For logging date/time

using namespace std;

// -----------------------------------------------------------------------------
// Structure representing a supply record
// -----------------------------------------------------------------------------
struct Supply {
    string city;      // For city datasets; for the consolidated Metro Manila dataset, this is a placeholder.
    string itemName;  // Supply item name (e.g., "canned_goods")
    int quantity;     // Quantity available
};

// Comparator used for sorting supplies by itemName.
bool compareSupply(const Supply &a, const Supply &b) {
    if (a.city != b.city)
        return a.city < b.city;
    return a.itemName < b.itemName;
}

// Global data structures
map<string, vector<Supply>> cityData;   // Each city's dataset.
vector<Supply> metroManilaData;           // Consolidated Metro Manila dataset.

// -----------------------------------------------------------------------------
// Quick Sort (for sorting an individual city's dataset by itemName)
// -----------------------------------------------------------------------------
int partition(vector<Supply>& arr, int low, int high) {
    Supply pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].itemName < pivot.itemName) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<Supply>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// -----------------------------------------------------------------------------
// Merge Sort (for sorting the consolidated Metro Manila dataset)
// -----------------------------------------------------------------------------
void merge(vector<Supply>& arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    vector<Supply> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (compareSupply(L[i], R[j])) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) {
        arr[k++] = L[i++];
    }
    while (j < n2) {
        arr[k++] = R[j++];
    }
}

void mergeSort(vector<Supply>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// -----------------------------------------------------------------------------
// Binary Search: Searches a sorted vector of Supply records for a given item name.
// Returns the index if found, or -1 if not found.
// -----------------------------------------------------------------------------
int binarySearch(const vector<Supply>& arr, const string &key) {
    int left = 0;
    int right = arr.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid].itemName == key)
            return mid;
        else if (arr[mid].itemName < key)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

// -----------------------------------------------------------------------------
// Utility: Update the "registered_cities.txt" file based on the in-memory data.
// -----------------------------------------------------------------------------
void updateRegisteredCitiesFile() {
    ofstream outfile("registered_cities.txt");
    if (outfile) {
        for (auto &entry : cityData) {
            outfile << entry.first << "\n";
        }
        outfile.close();
    }
}

// -----------------------------------------------------------------------------
// Utility: Consolidate all city supplies into a single Metro Manila dataset.
// For each unique item, sums quantities from all cities, then sorts and saves the data.
// -----------------------------------------------------------------------------
void updateMetroManilaData() {
    map<string, int> consolidated;
    for (auto &entry : cityData) {
        for (auto &supply : entry.second) {
            consolidated[supply.itemName] += supply.quantity;
        }
    }
    metroManilaData.clear();
    for (auto &entry : consolidated) {
        Supply s;
        s.city = "MetroManila"; // Placeholder.
        s.itemName = entry.first;
        s.quantity = entry.second;
        metroManilaData.push_back(s);
    }
    if (!metroManilaData.empty())
        mergeSort(metroManilaData, 0, metroManilaData.size() - 1);

    ofstream outfile("metro_manila.txt");
    if (outfile) {
        for (auto &s : metroManilaData) {
            outfile << s.itemName << " " << s.quantity << "\n";
        }
        outfile.close();
    }
}

// -----------------------------------------------------------------------------
// Log a successful allocation transaction to "historical_transactions.txt".
// -----------------------------------------------------------------------------
void logTransaction(const string &donor, const string &recipient, const string &item, int quantity) {
    time_t now = time(0);
    char* dt = ctime(&now);
    string datetime(dt);
    if (!datetime.empty() && datetime.back() == '\n')
        datetime.pop_back();

    ofstream outfile("historical_transactions.txt", ios::app);
    if (outfile) {
        outfile << datetime << " - Allocated " << quantity << " of \""
                << item << "\" from " << donor << " to " << recipient << "\n";
    }
}

// -----------------------------------------------------------------------------
// View historical transactions by reading "historical_transactions.txt".
// -----------------------------------------------------------------------------
void viewTransactions() {
    ifstream infile("historical_transactions.txt");
    if (!infile) {
        cout << "\nNo historical transactions found.\n";
        return;
    }
    cout << "\nHistorical Transactions:\n";
    string line;
    while(getline(infile, line)) {
        cout << line << "\n";
    }
    infile.close();
}

// -----------------------------------------------------------------------------
// Initialization: Create sample files for each city if they do not exist.
// -----------------------------------------------------------------------------
void initializeSampleFiles() {
    vector<pair<string, vector<pair<string, int>>>> samples = {
        {"Mandaluyong", {
            {"canned_goods",150}, {"water_bottles",200}, {"rice",250},
            {"noodles",180}, {"medicine",120}, {"blankets",130},
            {"clothes",170}, {"diapers",140}, {"fuel",110}, {"first_aid",160},
            {"flashlights",230}, {"batteries",150}, {"solar_lamps",170}, {"tents",180},
            {"sleeping_bags",230}, {"mosquito_nets",230}, {"lighters",120}, {"umbrella",110},
            {"plastic_buckets",230}, {"tarpaulins",100}, {"plastic_sheets",160}, {"hygiene_kits",220},
            {"wet_wipes",150}, {"gloves",180}, {"trash_bags",170}, {"disposable_plates",190},
            {"contact_list",200}, {"whistles",180}, {"helmets",120}, {"candles",230},
            {"sandbags",200}, {"boots",150}, {"baby_formula",220}, {"notebooks",140},
            {"hand_fans",210}, {"plastic_cups",140}, {"plastic_utensil",160}, {"safety_pins",200},
            {"rubber_bands",100}, {"slippers",180}, {"raincoats",180}, {"plastic_containers",220},
            {"tissue",220}, {"towels",140}, {"face_masks",140}, {"ziplock_bag",100},
            {"biscuits",230}, {"powdered_milk",160}, {"energy_bars",230}, {"trail_mix",200}
        }},
        {"Caloocan", {
            {"canned_goods",220}, {"water_bottles",210}, {"rice",300},
            {"noodles",190}, {"medicine",130}, {"blankets",150},
            {"clothes",200}, {"diapers",160}, {"fuel",180}, {"first_aid",170},
            {"flashlights",180}, {"batteries",180}, {"solar_lamps",220}, {"tents",250},
            {"sleeping_bags",190}, {"mosquito_nets",150}, {"lighters",200}, {"umbrella",210},
            {"plastic_buckets",230}, {"tarpaulins",190}, {"plastic_sheets",230}, {"hygiene_kits",210},
            {"wet_wipes",220}, {"gloves",190}, {"trash_bags",210}, {"disposable_plates",160},
            {"contact_list",200}, {"whistles",200}, {"helmets",220}, {"candles",220},
            {"sandbags",190}, {"boots",230}, {"baby_formula",160}, {"notebooks",230},
            {"hand_fans",230}, {"plastic_cups",180}, {"plastic_utensil",240}, {"safety_pins",160},
            {"rubber_bands",150}, {"slippers",200}, {"raincoats",170}, {"plastic_containers",160},
            {"tissue",160}, {"towels",170}, {"face_masks",190}, {"ziplock_bag",240},
            {"biscuits",230}, {"powdered_milk",170}, {"energy_bars",150}, {"trail_mix",150}
        }},
        {"Manila", {
            {"canned_goods",230}, {"water_bottles",250}, {"rice",280},
            {"noodles",210}, {"medicine",150}, {"blankets",170},
            {"clothes",190}, {"diapers",180}, {"fuel",200}, {"first_aid",190},
            {"flashlights",200}, {"batteries",170}, {"solar_lamps",190}, {"tents",160},
            {"sleeping_bags",210}, {"mosquito_nets",150}, {"lighters",230}, {"umbrella",180},
            {"plastic_buckets",240}, {"tarpaulins",190}, {"plastic_sheets",240}, {"hygiene_kits",180},
            {"wet_wipes",180}, {"gloves",170}, {"trash_bags",180}, {"disposable_plates",150},
            {"contact_list",170}, {"whistles",210}, {"helmets",240}, {"candles",210},
            {"sandbags",210}, {"boots",250}, {"baby_formula",220}, {"notebooks",160},
            {"hand_fans",240}, {"plastic_cups",210}, {"plastic_utensil",200}, {"safety_pins",250},
            {"rubber_bands",150}, {"slippers",200}, {"raincoats",200}, {"plastic_containers",180},
            {"tissue",190}, {"towels",170}, {"face_masks",230}, {"ziplock_bag",200},
            {"biscuits",210}, {"powdered_milk",170}, {"energy_bars",230}, {"trail_mix",180}
        }},
        {"Paranaque", {
            {"canned_goods",200}, {"water_bottles",300}, {"rice",260},
            {"noodles",220}, {"medicine",180}, {"blankets",190},
            {"clothes",210}, {"diapers",230}, {"fuel",240}, {"first_aid",250},
            {"flashlights",170}, {"batteries",150}, {"solar_lamps",190}, {"tents",160},
            {"sleeping_bags",140}, {"mosquito_nets",130}, {"lighters",150}, {"umbrella",120},
            {"plastic_buckets",210}, {"tarpaulins",230}, {"plastic_sheets",110}, {"hygiene_kits",170},
            {"wet_wipes",100}, {"gloves",180}, {"trash_bags",100}, {"disposable_plates",130},
            {"contact_list",190}, {"whistles",220}, {"helmets",190}, {"candles",150},
            {"sandbags",110}, {"boots",200}, {"baby_formula",100}, {"notebooks",120},
            {"hand_fans",110}, {"plastic_cups",160}, {"plastic_utensil",140}, {"safety_pins",140},
            {"rubber_bands",190}, {"slippers",210}, {"raincoats",190}, {"plastic_containers",150},
            {"tissue",160}, {"towels",160}, {"face_masks",170}, {"ziplock_bag",230},
            {"biscuits",160}, {"powdered_milk",140}, {"energy_bars",190}, {"trail_mix",160}
        }},
        {"Pasay", {
            {"canned_goods",210}, {"water_bottles",310}, {"rice",270},
            {"noodles",230}, {"medicine",190}, {"blankets",200},
            {"clothes",220}, {"diapers",240}, {"fuel",250}, {"first_aid",260},
            {"flashlights",230}, {"batteries",260}, {"solar_lamps",190}, {"tents",220},
            {"sleeping_bags",260}, {"mosquito_nets",240}, {"lighters",210}, {"umbrella",190},
            {"plastic_buckets",240}, {"tarpaulins",210}, {"plastic_sheets",240}, {"hygiene_kits",240},
            {"wet_wipes",190}, {"gloves",270}, {"trash_bags",240}, {"disposable_plates",260},
            {"contact_list",250}, {"whistles",190}, {"helmets",180}, {"candles",200},
            {"sandbags",200}, {"boots",180}, {"baby_formula",190}, {"notebooks",200},
            {"hand_fans",260}, {"plastic_cups",230}, {"plastic_utensil",180}, {"safety_pins",230},
            {"rubber_bands",210}, {"slippers",230}, {"raincoats",190}, {"plastic_containers",200},
            {"tissue",270}, {"towels",190}, {"face_masks",190}, {"ziplock_bag",220},
            {"biscuits",210}, {"powdered_milk",220}, {"energy_bars",200}, {"trail_mix",280}
        }},
        {"QuezonCity", {
            {"canned_goods",240}, {"water_bottles",320}, {"rice",290},
            {"noodles",250}, {"medicine",210}, {"blankets",220},
            {"clothes",230}, {"diapers",260}, {"fuel",270}, {"first_aid",280},
            {"flashlights",230}, {"batteries",210}, {"solar_lamps",210}, {"tents",230},
            {"sleeping_bags",190}, {"mosquito_nets",250}, {"lighters",180}, {"umbrella",250},
            {"plastic_buckets",230}, {"tarpaulins",260}, {"plastic_sheets",190}, {"hygiene_kits",180},
            {"wet_wipes",190}, {"gloves",190}, {"trash_bags",190}, {"disposable_plates",190},
            {"contact_list",250}, {"whistles",200}, {"helmets",220}, {"candles",210},
            {"sandbags",240}, {"boots",250}, {"baby_formula",180}, {"notebooks",190},
            {"hand_fans",270}, {"plastic_cups",270}, {"plastic_utensil",200}, {"safety_pins",200},
            {"rubber_bands",250}, {"slippers",230}, {"raincoats",220}, {"plastic_containers",230},
            {"tissue",210}, {"towels",210}, {"face_masks",200}, {"ziplock_bag",270},
            {"biscuits",260}, {"powdered_milk",220}, {"energy_bars",180}, {"trail_mix",190}
        }},
        {"Pasig", {
            {"canned_goods",250}, {"water_bottles",330}, {"rice",300},
            {"noodles",260}, {"medicine",220}, {"blankets",230},
            {"clothes",240}, {"diapers",270}, {"fuel",280}, {"first_aid",290},
            {"flashlights",210}, {"batteries",190}, {"solar_lamps",260}, {"tents",220},
            {"sleeping_bags",260}, {"mosquito_nets",180}, {"lighters",210}, {"umbrella",270},
            {"plastic_buckets",190}, {"tarpaulins",250}, {"plastic_sheets",230}, {"hygiene_kits",270},
            {"wet_wipes",180}, {"gloves",260}, {"trash_bags",220}, {"disposable_plates",180},
            {"contact_list",200}, {"whistles",200}, {"helmets",240}, {"candles",240},
            {"sandbags",230}, {"boots",190}, {"baby_formula",220}, {"notebooks",190},
            {"hand_fans",250}, {"plastic_cups",180}, {"plastic_utensil",240}, {"safety_pins",180},
            {"rubber_bands",210}, {"slippers",200}, {"raincoats",220}, {"plastic_containers",220},
            {"tissue",180}, {"towels",200}, {"face_masks",230}, {"ziplock_bag",200},
            {"biscuits",210}, {"powdered_milk",240}, {"energy_bars",230}, {"trail_mix",190}
        }}
    };

    for (auto &city : samples) {
        // Use the city name as the filename (adjust naming if needed)
        string filename = city.first + ".txt";
        ifstream infile(filename);
        if (!infile) { // File does not exist – create it.
            ofstream outfile(filename);
            if (outfile) {
                for (auto &supply : city.second) {
                    outfile << supply.first << " " << supply.second << "\n";
                }
                outfile.close();
                cout << "Created sample file: " << filename << "\n";
            }
        }
    }
}


// -----------------------------------------------------------------------------
// Loads a single city's dataset from its file and registers it in the system.
// -----------------------------------------------------------------------------
void loadCityDataset(const string &city) {
    string filename = city + ".txt";
    ifstream infile(filename);
    if (!infile) {
        cout << "Error: File \"" << filename << "\" not found!\n";
        return;
    }
    map<string, int> suppliesMap;
    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string item;
        int qty;
        iss >> item >> qty;
        suppliesMap[item] += qty;
    }
    infile.close();

    vector<Supply> supplies;
    for (auto &entry : suppliesMap) {
        Supply s;
        s.city = city;
        s.itemName = entry.first;
        s.quantity = entry.second;
        supplies.push_back(s);
    }
    if (!supplies.empty())
        quickSort(supplies, 0, supplies.size() - 1);

    cityData[city] = supplies;
    cout << "City \"" << city << "\" dataset loaded.\n";
}

// -----------------------------------------------------------------------------
// Automatically register (load) all cities from the sample files.
// -----------------------------------------------------------------------------
void registerAllCities() {
    vector<string> cities = {"Mandaluyong", "Caloocan", "Manila", "Paranaque", "Pasay", "QuezonCity", "Pasig"};
    for (auto &city : cities) {
        loadCityDataset(city);
    }
    updateRegisteredCitiesFile();
    updateMetroManilaData();
}

// -----------------------------------------------------------------------------
// Option: Allocate resources from one city to another.
// -----------------------------------------------------------------------------
void allocateResource() {
    cout << "\nEnter your city (recipient): ";
    string recipientCity;
    getline(cin, recipientCity);
    if (cityData.find(recipientCity) == cityData.end()) {
        cout << "Error: City \"" << recipientCity << "\" is not registered in the system.\n";
        return;
    }
    cout << "Enter the item you need: ";
    string itemNeeded;
    getline(cin, itemNeeded);
    cout << "Enter the quantity needed: ";
    int qtyNeeded;
    cin >> qtyNeeded;
    cin.ignore(); // Remove newline

    vector<string> donorCities;
    for (auto &entry : cityData) {
        string donor = entry.first;
        if (donor == recipientCity)
            continue;
        for (auto &s : entry.second) {
            if (s.itemName == itemNeeded && s.quantity >= qtyNeeded) {
                donorCities.push_back(donor);
                break;
            }
        }
    }
    if (donorCities.empty()) {
        cout << "No donor city has enough \"" << itemNeeded << "\" available.\n";
        return;
    }
    cout << "\nDonor cities with available \"" << itemNeeded << "\":\n";
    for (auto &donor : donorCities) {
        for (auto &s : cityData[donor]) {
            if (s.itemName == itemNeeded) {
                cout << "  " << donor << " - Available: " << s.quantity << "\n";
                break;
            }
        }
    }
    cout << "Enter the donor city you want to allocate from: ";
    string donorCity;
    getline(cin, donorCity);
    bool valid = false;
    for (auto &d : donorCities) {
        if (d == donorCity) { valid = true; break; }
    }
    if (!valid) {
        cout << "Invalid donor city selection.\n";
        return;
    }
    bool donorUpdated = false;
    for (auto &s : cityData[donorCity]) {
        if (s.itemName == itemNeeded) {
            if (s.quantity < qtyNeeded) {
                cout << "Error: Donor city \"" << donorCity << "\" does not have enough \""
                     << itemNeeded << "\".\n";
                return;
            }
            s.quantity -= qtyNeeded;
            donorUpdated = true;
            break;
        }
    }
    if (!donorUpdated) {
        cout << "Unexpected error: Donor city \"" << donorCity << "\" does not have the item.\n";
        return;
    }
    bool found = false;
    for (auto &s : cityData[recipientCity]) {
        if (s.itemName == itemNeeded) {
            s.quantity += qtyNeeded;
            found = true;
            break;
        }
    }
    if (!found) {
        Supply newSupply;
        newSupply.city = recipientCity;
        newSupply.itemName = itemNeeded;
        newSupply.quantity = qtyNeeded;
        cityData[recipientCity].push_back(newSupply);
        quickSort(cityData[recipientCity], 0, cityData[recipientCity].size() - 1);
    }
    cout << "Allocation successful! " << qtyNeeded << " of \"" << itemNeeded << "\" allocated from \""
         << donorCity << "\" to \"" << recipientCity << "\".\n";

    // Log the transaction.
    logTransaction(donorCity, recipientCity, itemNeeded, qtyNeeded);
    updateMetroManilaData();
}

// -----------------------------------------------------------------------------
// Option: Show a specific city's dataset (sorted using quick sort).
// -----------------------------------------------------------------------------
void showCityDataset() {
    cout << "\nEnter city name to display its dataset: ";
    string city;
    getline(cin, city);
    if (cityData.find(city) == cityData.end()) {
        cout << "City \"" << city << "\" is not registered in the system.\n";
        return;
    }
    vector<Supply> citySupplies = cityData[city];
    if (!citySupplies.empty())
        quickSort(citySupplies, 0, citySupplies.size() - 1);
    cout << "\nDataset for \"" << city << "\" (sorted using quick sort):\n";
    for (auto &s : citySupplies) {
        cout << "  " << s.itemName << " : " << s.quantity << "\n";
    }
}

// -----------------------------------------------------------------------------
// Option: Show the consolidated Metro Manila dataset.
// -----------------------------------------------------------------------------
void showMetroManilaDataset() {
    if (metroManilaData.empty()) {
        cout << "\nMetro Manila dataset is empty. Check that cities are registered.\n";
        return;
    }
    cout << "\nConsolidated Metro Manila dataset (sorted using merge sort):\n";
    for (auto &s : metroManilaData) {
        cout << "  " << s.itemName << " : " << s.quantity << "\n";
    }
    cout << "The consolidated dataset has been saved to \"metro_manila.txt\".\n";
}

// -----------------------------------------------------------------------------
// Option: Search for a specific item using Binary Search.
// -----------------------------------------------------------------------------
void searchItem() {
    cout << "\nSearch in (1) Specific City or (2) Metro Manila? Enter 1 or 2: ";
    int choice;
    cin >> choice;
    cin.ignore();
    if (choice == 1) {
        cout << "Enter city name: ";
        string city;
        getline(cin, city);
        if (cityData.find(city) == cityData.end()) {
            cout << "City \"" << city << "\" is not registered in the system.\n";
            return;
        }
        vector<Supply> citySupplies = cityData[city];
        if (!citySupplies.empty())
            quickSort(citySupplies, 0, citySupplies.size() - 1);
        cout << "Enter item name to search: ";
        string item;
        getline(cin, item);
        int index = binarySearch(citySupplies, item);
        if (index != -1) {
            cout << "Found: \"" << citySupplies[index].itemName << "\" in \"" << city
                 << "\" with quantity " << citySupplies[index].quantity << "\n";
        } else {
            cout << "Item \"" << item << "\" not found in \"" << city << "\".\n";
        }
    } else if (choice == 2) {
        if (metroManilaData.empty()) {
            cout << "Metro Manila dataset is empty.\n";
            return;
        }
        cout << "Enter item name to search in Metro Manila: ";
        string item;
        getline(cin, item);
        int index = binarySearch(metroManilaData, item);
        if (index != -1) {
            cout << "Found: \"" << metroManilaData[index].itemName
                 << "\" with consolidated quantity " << metroManilaData[index].quantity << "\n";
        } else {
            cout << "Item \"" << item << "\" not found in the Metro Manila dataset.\n";
        }
    } else {
        cout << "Invalid choice.\n";
    }
}

// -----------------------------------------------------------------------------
// Main menu
// -----------------------------------------------------------------------------
int main() {
    // Create sample data files if they do not exist.
    initializeSampleFiles();
    // Automatically register all cities.
    registerAllCities();

    int option;
    do {
        cout << "\n=== Disaster Relief Allocation System ===\n";
        cout << "1. Show consolidated Metro Manila dataset\n";
        cout << "2. Allocate resources\n";
        cout << "3. Show city dataset\n";
        cout << "4. Search for item (using Binary Search)\n";
        cout << "5. View historical transactions\n";
        cout << "6. Exit\n";
        cout << "Enter option: ";

        if (!(cin >> option)) {
            cout << "Invalid input. Please enter a number between 1 and 6." << endl;
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits <streamsize>::max(), '\n'); // Discard invalid input
            continue; // Skip the rest of the loop and prompt again
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the newline character

        switch (option) {
            case 1:
                showMetroManilaDataset();
                break;
            case 2:
                allocateResource();
                break;
            case 3:
                showCityDataset();
                break;
            case 4:
                searchItem();
                break;
            case 5:
                viewTransactions();
                break;
            case 6:
                cout << "Exiting system.\n";
                break;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    } while (option != 6);

    return 0;
}
