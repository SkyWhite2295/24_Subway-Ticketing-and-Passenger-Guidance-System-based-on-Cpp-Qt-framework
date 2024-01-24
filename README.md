# Subway-Ticketing-and-Passenger-Guidance-System-based-on-Cpp-Qt-framework
Data Structure Course Design

# I. Requirements Analysis
Develop a ticket and route inquiry software for use at ticket windows in each subway station. The software should provide information on the minimum ticket fare from the selected station to all other subway stations, including transfer options. It should also support route planning (including transfers) and provide information on the minimum ticket fare from the current station to stations near famous landmarks, along with station information queries.

Due to the addition of new subway lines and stations, changes in the price list may occur. Therefore, the software should automatically generate the fare table and support the accurate querying of subway routes and stations after adding new landmark information.

# II. Program Features
## 1. Map Display
The program can draw a subway route map based on information read from the configuration file. In the map, circles represent subway stations, straight lines represent subway routes, and different colors indicate different routes. Each station displays its name, and after a route query, the stations on the route are marked in red. The map supports mouse long-press dragging, and scroll wheel zooming. Additional information is displayed when the mouse pointer hovers over a station (station details) or a line segment (route information).

## 2. Normal Mode
In the normal mode interface, users can choose the starting and ending points from dropdown menus or enter them manually. They can use radio buttons to select "Shortest Path" or "Minimum Transfers" route options. After clicking the confirmation button, the program calculates and provides a suitable travel route (with emphasis on transfer stations) in text format, along with visual indications on the subway route map. It also calculates the fare, distance, and estimated time for the route.

## 3. Tourism Mode
In the tourism mode interface, users can choose the starting point and select one or more destination attractions from a dropdown menu or by manual entry. If only one attraction is selected, users can choose "Shortest Path" or "Minimum Transfers" route options similar to the normal mode. If multiple attractions are selected, a discounted tourism ticket is applied, allowing passengers to visit attractions near a subway station and return to that station for the next attraction (charged at 90% of the total distance). After clicking the confirmation button, the program calculates and provides a suitable travel route (with emphasis on transfer stations) in text format, along with visual indications on the subway route map. It also calculates the fare, distance, and estimated time for the route.

## 4. Administrator Functions
In the "Edit" menu, there are functions for managing configuration files, accessible only after administrator login. The program provides an administrator login feature, where the administrator enters a predefined username and password. After successful login, the administrator can add, delete, or modify data in station configuration, edge configuration, and tourist attraction configuration files. Changes take effect upon restarting the program.

## 5. Other Features
The "Help" menu provides a brief introduction to the program's functionality.
The "About" menu provides information about the program.
The program's status bar displays the current time in real-time.

# III. Data Processing
The program reads data from three .txt files:

Station Configuration File (stationInfo.txt): Includes station ID (int), station name (QString), station x, and y coordinates (double). A total of 219 records.
Edge Configuration File (edgeInfo.txt): Includes edge ID (int), starting station ID (int), ending station ID (int), edge length (double), and the route to which the edge belongs (int). A total of 247 records.
Tourist Attraction Configuration File (touristattractionInfo.txt): Includes attraction ID (int), attraction name (QString), and the station ID where the attraction is located (int). A total of 14 records.

# IV. Development Environment
Device Information:

Processor: Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz 1.80 GHz
RAM: 8.00 GB
Operating System: Windows 10 Home Chinese Edition, Version: 22H2
Development Platform:

Program developed, tested, and built using Qt Creator 4.5.1 (Community) based on Qt 5.10.1 (MSVC 2015, 32 bit).
