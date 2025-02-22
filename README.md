# Disaster Relief Allocation and Inventory Management System 🚑
## Overview
The Disaster Relief Allocation and Inventory Management System is a C++ command-line application designed to streamline the management and allocation of disaster relief supplies across multiple cities. It consolidates individual city datasets into a unified Metro Manila dataset, making it easier to monitor, search, and allocate resources efficiently during emergencies.

## Features 🦾
### City Dataset Management: 🏙️
Automatically loads or creates sample supply data for various cities (e.g., Mandaluyong, Caloocan, Manila, etc.).
Reads supply data from text files (e.g., <city>.txt) and consolidates duplicate entries by summing quantities.

### Sorting Algorithms: 📊
#### Quick Sort: Used to sort individual city datasets by supply item name.

#### Merge Sort: Applied to the consolidated Metro Manila dataset for efficient organization.

### Search Functionality:
Implements Binary Search to quickly locate specific supply items within both individual city datasets and the consolidated dataset.

### Resource Allocation: 💰
Enables allocation of resources from one city to another by deducting quantities from a donor city and adding them to a recipient city.
Automatically updates the consolidated dataset after each allocation.

### File I/O Integration: 🗃️
Maintains persistent data by generating/updating files such as registered_cities.txt and metro_manila.txt.
Creates sample data files automatically if they do not exist.

## User Manual
### Installation Guide:
#### The user should have a C++ IDE (The most recommended IDE is Code::Blocks, as the program might experience some issues with other IDE’s).
#### Have the .cpp file downloaded.
#### Open the .cpp file with the IDE.
#### Compile and run the file.
#### All the text files needed should already be generated when the program is compiled and run.
