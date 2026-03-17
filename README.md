# OpenStreetMap Routing Engine (C++)
A high-performance routing engine that parses real-world map data from OpenStreetMap (OSM) to calculate shortest paths and perform network analysis.

## Core Functionalities
* **XML Map Parsing**: Uses tinyxml2 to extract nodes and highways from .osm files, building a dynamic adjacency list.
* **Advanced Routing (Dijkstra)**: Implements Dijkstra's algorithm for shortest-path estimation between any two geographic coordinates.
* **Path Optimization (Graph Compacting)**: Features a custom compact() function that simplifies the graph by merging intermediate nodes, significantly reducing search space without losing connectivity.
* **Geospatial Calculations**: Accurate distance measurement using the Haversine formula, adjusted by road-type weight factors (e.g., motorways vs. residential streets).
* **Graph Traversal**: Full support for Breadth-First Search (BFS) and Depth-First Search (DFS) for network connectivity testing.
* **Google Maps Integration**: Automatically generates a URL that visualizes the calculated path directly on Google Maps.

## Technical Stack
* **Language**: C++17 .
* **Libraries**: `tinyxml2` for XML processing.
* **Build System**: Makefile with integrated **AddressSanitizer (ASan)** to ensure memory safety and catch leaks early .
* **Data Structures**: Adjacency lists, Priority Queues, Maps, and Sets.

## How to Run
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Compile the source files: `make`
4. Run the executable ./main
5. Once the program is running, you can use the following commands:
* ```-i <filepath>```: Import a .osm file to build the graph.
* ```-c```: Compact the graph to improve performance.
* ```-p <start_id> <end_id>```: Calculate the shortest path and get the Google Maps link.
* ```-b <sid>```: Perform a BFS starting from node <sid>.
* ```-d <sid>```: Perform a DFS starting from node <sid>.
* ```-q```: Exit the program.
