#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// -----------------------------------------------------------------------------
// Structure representing a supply record
// -----------------------------------------------------------------------------
struct Supply {
    string city;      // For city datasets; for the consolidated Metro Manila dataset, this is a placeholder.
    string itemName;  // Supply item name (e.g., "canned_goods")
    int quantity;     // Quantity available
};

// Comparator used for merge sort (and used in quick sort when comparing items).
// For a single city dataset (or a consolidated dataset), the sorting is based on itemName.
bool compareSupply(const Supply &a, const Supply &b) {
    if (a.city != b.city)
        return a.city < b.city;
    return a.itemName < b.itemName;
}

// Global data structures
// Holds the dataset for each city (key: city name, value: vector of supplies)
map<string, vector<Supply>> cityData;
// Holds the consolidated Metro Manila dataset (each unique item appears once, with the summed quantity)
vector<Supply> metroManilaData;

// -----------------------------------------------------------------------------
// Quick Sort (used for sorting an individual city's dataset by itemName)
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
// Merge Sort (used for sorting the consolidated Metro Manila dataset)
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
// This function ignores the originating city. For each unique item, it sums the
// quantities across all cities. The consolidated list is then sorted using merge sort
// and saved to "metro_manila.txt".
// -----------------------------------------------------------------------------
void updateMetroManilaData() {
    // Use a map to consolidate items (key: itemName, value: summed quantity)
    map<string, int> consolidated;
    for (auto &entry : cityData) {
        for (auto &supply : entry.second) {
            consolidated[supply.itemName] += supply.quantity;
        }
    }
    metroManilaData.clear();
    for (auto &entry : consolidated) {
        Supply s;
        s.city = "MetroManila";  // Placeholder value; not used in display.
        s.itemName = entry.first;
        s.quantity = entry.second;
        metroManilaData.push_back(s);
    }
    // Sort the consolidated list using merge sort.
    if (!metroManilaData.empty())
        mergeSort(metroManilaData, 0, metroManilaData.size() - 1);

    // Save the consolidated Metro Manila dataset to "metro_manila.txt"
    ofstream outfile("metro_manila.txt");
    if (outfile) {
        for (auto &s : metroManilaData) {
            outfile << s.itemName << " " << s.quantity << "\n";
        }
        outfile.close();
    }
}

// -----------------------------------------------------------------------------
// Initialization: Create sample files for each city if they do not exist.
// -----------------------------------------------------------------------------
void initializeSampleFiles() {
    // Each entry: city name and its list of supplies (item, quantity)
    vector<pair<string, vector<pair<string, int>>>> samples = {
        {"Mandaluyong", { {"canned_goods",150}, {"water_bottles",200}, {"rice",250},
                          {"noodles",180}, {"medicine",120}, {"blankets",130},
                          {"clothes",170}, {"diapers",140}, {"fuel",110}, {"first_aid",160} }},

        {"Caloocan",    { {"canned_goods",220}, {"water_bottles",210}, {"rice",300},
                          {"noodles",190}, {"medicine",130}, {"blankets",150},
                          {"clothes",200}, {"diapers",160}, {"fuel",180}, {"first_aid",170} }},

        {"Manila",      { {"canned_goods",230}, {"water_bottles",250}, {"rice",280},
                          {"noodles",210}, {"medicine",150}, {"blankets",170},
                          {"clothes",190}, {"diapers",180}, {"fuel",200}, {"first_aid",190} }},

        {"Paranaque",   { {"canned_goods",200}, {"water_bottles",300}, {"rice",260},
                          {"noodles",220}, {"medicine",180}, {"blankets",190},
                          {"clothes",210}, {"diapers",230}, {"fuel",240}, {"first_aid",250} }},

        {"Pasay",       { {"canned_goods",210}, {"water_bottles",310}, {"rice",270},
                          {"noodles",230}, {"medicine",190}, {"blankets",200},
                          {"clothes",220}, {"diapers",240}, {"fuel",250}, {"first_aid",260} }},

        {"QuezonCity",  { {"canned_goods",240}, {"water_bottles",320}, {"rice",290},
                          {"noodles",250}, {"medicine",210}, {"blankets",220},
                          {"clothes",230}, {"diapers",260}, {"fuel",270}, {"first_aid",280} }},

        {"Pasig",       { {"canned_goods",250}, {"water_bottles",330}, {"rice",300},
                          {"noodles",260}, {"medicine",220}, {"blankets",230},
                          {"clothes",240}, {"diapers",270}, {"fuel",280}, {"first_aid",290} }}
    };

    for (auto &city : samples) {
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
// Option 1: Add a city dataset.
// The program reads the file <city>.txt, combines duplicate items (summing quantities),
// sorts the dataset using quick sort, and adds it to the system.
// -----------------------------------------------------------------------------
void addCityDataset() {
    cout << "\nEnter city name to add (the program will look for <city>.txt): ";
    string city;
    getline(cin, city);
    string filename = city + ".txt";
    ifstream infile(filename);
    if (!infile) {
        cout << "Error: File \"" << filename << "\" not found!\n";
        return;
    }
    // Combine duplicate items using a map.
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

    // Transfer the combined data into a vector of Supply records.
    vector<Supply> supplies;
    for (auto &entry : suppliesMap) {
        Supply s;
        s.city = city;
        s.itemName = entry.first;
        s.quantity = entry.second;
        supplies.push_back(s);
    }
    // Sort the city's dataset using quick sort (by itemName)
    if (!supplies.empty())
        quickSort(supplies, 0, supplies.size() - 1);

    // Add/update the system’s record.
    cityData[city] = supplies;
    cout << "\nCity \"" << city << "\" dataset loaded and sorted (using quick sort):\n";
    for (auto &s : supplies) {
        cout << "  " << s.itemName << " : " << s.quantity << "\n";
    }
    updateRegisteredCitiesFile();
    updateMetroManilaData();
}

// -----------------------------------------------------------------------------
// Option 2: Show the consolidated Metro Manila dataset.
// This displays a single list of unique supplies (item names with their summed quantities)
// sorted using merge sort and saved to "metro_manila.txt".
// -----------------------------------------------------------------------------
void showMetroManilaDataset() {
    if (metroManilaData.empty()) {
        cout << "\nMetro Manila dataset is empty. Add some city datasets first.\n";
        return;
    }
    cout << "\nConsolidated Metro Manila dataset (sorted using merge sort):\n";
    for (auto &s : metroManilaData) {
        cout << "  " << s.itemName << " : " << s.quantity << "\n";
    }
    cout << "The consolidated dataset has also been saved to \"metro_manila.txt\".\n";
}

// -----------------------------------------------------------------------------
// Option 3: Allocate resources from one city to another.
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
    cin.ignore(); // remove newline

    // Find donor cities (other than the recipient) that have enough supply.
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
    // Validate selection.
    bool valid = false;
    for (auto &d : donorCities) {
        if (d == donorCity) { valid = true; break; }
    }
    if (!valid) {
        cout << "Invalid donor city selection.\n";
        return;
    }
    // Deduct the quantity from the donor city.
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
    // Add (or update) the item in the recipient city's dataset.
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
    updateMetroManilaData();
}

// -----------------------------------------------------------------------------
// Option 4: Show a specific city's dataset (sorted using quick sort).
// -----------------------------------------------------------------------------
void showCityDataset() {
    cout << "\nEnter city name to display its dataset: ";
    string city;
    getline(cin, city);
    if (cityData.find(city) == cityData.end()) {
        cout << "City \"" << city << "\" is not registered in the system.\n";
        return;
    }
    // Make a copy and sort it.
    vector<Supply> citySupplies = cityData[city];
    if (!citySupplies.empty())
        quickSort(citySupplies, 0, citySupplies.size() - 1);
    cout << "\nDataset for \"" << city << "\" (sorted using quick sort):\n";
    for (auto &s : citySupplies) {
        cout << "  " << s.itemName << " : " << s.quantity << "\n";
    }
}

// -----------------------------------------------------------------------------
// Option 5: Search for a specific item using Binary Search.
// The search is performed on a sorted dataset (either a specific city's dataset or
// the consolidated Metro Manila dataset).
// -----------------------------------------------------------------------------
void searchItem() {
    cout << "\nSearch in (1) Specific City or (2) Metro Manila? Enter 1 or 2: ";
    int choice;
    cin >> choice;
    cin.ignore();  // clear newline from input buffer

    if (choice == 1) {
        cout << "Enter city name: ";
        string city;
        getline(cin, city);
        if (cityData.find(city) == cityData.end()) {
            cout << "City \"" << city << "\" is not registered in the system.\n";
            return;
        }
        // Ensure the city's dataset is sorted.
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
        // metroManilaData is already consolidated and sorted.
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
    // Create sample data files if they do not already exist.
    initializeSampleFiles();

    int option;
    do {
        cout << "\n=== Disaster Relief Allocation System ===\n";
        cout << "1. Add city dataset\n";
        cout << "2. Show consolidated Metro Manila dataset\n";
        cout << "3. Allocate resources\n";
        cout << "4. Show city dataset\n";
        cout << "5. Search for item (using Binary Search)\n";
        cout << "6. Exit\n";
        cout << "Enter option: ";
        cin >> option;
        cin.ignore(); // clear newline
        switch (option) {
            case 1:
                addCityDataset();
                break;
            case 2:
                showMetroManilaDataset();
                break;
            case 3:
                allocateResource();
                break;
            case 4:
                showCityDataset();
                break;
            case 5:
                searchItem();
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
