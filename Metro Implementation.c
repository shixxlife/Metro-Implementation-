#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define M 20

int adj[M][M];
char station[M][100];
int stationCount = 0;

typedef struct LINES {
    char nameofLine[100];
    char stations[100][100];
    int no;
} lines;

lines* all_metro_lines;
int numberofLines;

//finding index of the specified station
int find_station_index(char *name) {
    for (int i = 0; i < stationCount; i++) {
        if (strcmp(station[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

// Recursive DFS function to find all routes between source and destination
void dfs(int u, int dest, int visited[], int path[], int path_index, int *cost, int all_routes[][M], int *route_count) {
    visited[u] = 1; //because source is the visited destination itself
    path[path_index] = u;
    path_index++;

    if (u == dest) {
        int route_cost = 0;
        for (int i = 0; i < path_index - 1; i++) {
            int src = path[i];
            int dst = path[i + 1];
            route_cost += adj[src][dst];
        }
        all_routes[*route_count][0] = route_cost;
        for (int i = 0; i < path_index; i++) {
            all_routes[*route_count][i + 1] = path[i];
        }
        (*route_count)++;
    } else {
        for (int v = 0; v < stationCount; v++) {
            if (!visited[v] && adj[u][v] != -1) {
                dfs(v, dest, visited, path, path_index, cost, all_routes, route_count);
            }
        }
    }
    path_index--;
    visited[u] = 0;
}

// Function to find all routes between two stations
void find_routes(char *src, char *dst) {
    int src_index = find_station_index(src);
    int dst_index = find_station_index(dst);

    if (src_index == -1 || dst_index == -1) {
        printf("Invalid station name(s) entered.\n");
        return;
    }

    int visited[M] = {0}; //array to store the visited stations
    int path[M]; //stores the current path
    int path_index = 0;
    int all_routes[100][M] = {0}; //stores all roots
    int route_count = 0; //stores the number of possible roots

    int cost = 0;
    //dfs function to find all possible paths
    dfs(src_index, dst_index, visited, path, path_index, &cost, all_routes, &route_count);

    if (route_count == 0) {
        printf("No route found between %s and %s.\n", src, dst);
    } else {
        printf("Routes between %s and %s:\n", src, dst);
        for (int i = 0; i < route_count; i++) {
            printf("%s", station[all_routes[i][1]]);
            int total_cost = all_routes[i][0];
            for (int j = 2; all_routes[i][j] != 0; j++) {
                printf(" -> %s", station[all_routes[i][j]]);
            }
            printf(", Cost: %d\n", total_cost);
        }
    }
}

// Display detailed information of a specific station
//used array of structres and adjancey matrix 
void display_station_details(char *name) {
    int index = find_station_index(name);
    //if the station is there then the index would be any any number other than -1 
    if (index == -1) {
        printf("Station %s not found.\n", name);
        return;
    }

    printf("Station Name: %s\n", name);

    // List lines that this station is present
    //to find , we used array of structures and arrays to find the lines where the station is present
    //lines are array of structures which has name of the line and array which contains stations in that line
    //iterating the array of structures and the array inside them to find whether the station is the line or not, if present print otherwise continue
    printf("Lines: ");
    for (int i = 0; i < numberofLines; i++) {
        for (int j = 0; j < all_metro_lines[i].no; j++) {
            if (strcmp(all_metro_lines[i].stations[j], name) == 0) {
                printf("%s ", all_metro_lines[i].nameofLine);
                break;
            }
        }
    }
    //to find adjacent stations of the station using adjency matrix 
    printf("\nAdjacent Stations: ");
    for (int i = 0; i < stationCount; i++) {
        if (adj[index][i] > 0) {  // LOGIC: if fare is not there then there is no station adjacent to it, if fare is there then the station is present 
            printf("%s (%d) ", station[i], adj[index][i]);
        }
    }
    printf("\n");
}

// Display stations that are linked to multiple lines
void display_intersections() {
    int lineCount[stationCount];
    memset(lineCount, 0, sizeof(lineCount)); //initializing all elements of the lineCount array to 0

    //counting the number of lines a particular station serves in
    for (int i = 0; i < stationCount; i++) {
        for (int j = 0; j < numberofLines; j++) {
            for (int k = 0; k < all_metro_lines[j].no; k++) {
                if (strcmp(station[i], all_metro_lines[j].stations[k]) == 0) {
                    lineCount[i]++; 
                    break;
                }
            }
        }
    }

    printf("Stations Serving Multiple Lines:\n");  
    for (int i = 0; i < stationCount; i++) {
        if (lineCount[i] > 1) {    
            printf("%s, %d\n", station[i], lineCount[i]);   //if a station serves more than 1 line, it gets printed
        }
    }
}

// Display terminal stations of each line
void display_terminal_stations() {
    printf("Terminal Stations:\n");
    for (int i = 0; i < numberofLines; i++) {
        printf("%s: %s, %s\n", all_metro_lines[i].nameofLine,
               all_metro_lines[i].stations[0],
               all_metro_lines[i].stations[all_metro_lines[i].no - 1]);
    }
}

// Finding the nearest intersection to a given station by implementing bfs search
void find_path_to_nearest_intersection(char *src) {
    int src_index = find_station_index(src);
    if (src_index == -1) {
        printf("Station %s not found.\n", src);
        return;
    }

    int visited[M] = {0}; //array to keep track of visited stations
    int queue[M],  //for BFS
    parent[M];   //keeps track of previous station to the current one to trace the path later
    int front = 0, rear = 0;  //for the queue
    queue[rear++] = src_index;
    visited[src_index] = 1;  //marks station visited
    parent[src_index] = -1;  

    while (front < rear) {
        int u = queue[front++];
        int is_intersection = 0;
        for (int i = 0; i < numberofLines; i++) {
            int count = 0;
            for (int j = 0; j < all_metro_lines[i].no; j++) {
                if (strcmp(all_metro_lines[i].stations[j], station[u]) == 0) {
                    count++;
                }
            }
            if (count > 1) {
                is_intersection = 1;
                break;
            }
        }
        if (is_intersection && u != src_index) {
            printf("Nearest Intersection from %s:\n", src);
            int path_cost = 0;
            int current = u;
            while (parent[current] != -1) {
                path_cost += adj[parent[current]][current];  //sums the cost of the whole path
                current = parent[current];
            }
            current = u;
            printf("%s", src);
            while (current != src_index) {
                printf(" -> %s", station[current]);  //finally prints the path
                current = parent[current];
            }
            printf(", Cost: %d\n", path_cost);
            return;
        }
        for (int v = 0; v < stationCount; v++) {
            if (!visited[v] && adj[u][v] != -1) {
                visited[v] = 1;  //if there is path marks as visited
                parent[v] = u;
                queue[rear++] = v; //enqueue
            }
        }
    }
}

// Function to initialize adjacency matrix and take inputs
void initialize_metro_system() {
    printf("Enter the total number of stations: ");
    scanf("%d", &stationCount);
    
    //entering all stations and the stations get stored in an array
    printf("Enter all the stations:\n"); 
    for (int i = 0; i < stationCount; i++) {
        scanf("%s", station[i]);
    }

    
    printf("Enter number of metro lines: ");
    scanf("%d", &numberofLines);
    all_metro_lines = (lines*)malloc(sizeof(lines) * numberofLines); //dynamically creating array of structures to store lines

    //iterating through array of structures to store name of the line, stations for that line and number of stations.
    for (int j = 0; j < numberofLines; j++) {
        printf("Enter the name of the line: ");
        scanf(" %[^\n]s", all_metro_lines[j].nameofLine);

        printf("Enter the number of stations in this line: ");
        scanf("%d", &all_metro_lines[j].no);

        for (int i = 0; i < all_metro_lines[j].no; i++) {
            printf("Enter station name: ");
            scanf(" %s", all_metro_lines[j].stations[i]);
        }
    }

    //initializing all the numbers in adjency matrix to 0 
    for (int i = 0; i < stationCount; i++) {
        for (int j = 0; j < stationCount; j++) {
            adj[i][j] = 0;
        }
    }
    
    //edges-> connections 
    int edges, fare;
    char src[100], dtn[100];

    printf("Enter the number of edges: ");
    scanf("%d", &edges);

    for (int i = 0; i < edges; i++) {
        printf("Enter source, destination, and fare: ");
        scanf("%s %s %d", src, dtn, &fare);
        
        int u = find_station_index(src);
        int v = find_station_index(dtn);
        
        //finding thr index of station in stations array so that that index of adjency matrix is updated to fare of that connection
        if (u != -1 && v != -1) {
            adj[u][v] = fare;
            adj[v][u] = fare;
        } else {
            printf("Invalid station names entered.\n");
        }
    }
}

int main() {
    initialize_metro_system();

    // Sample usage of functionalities
    display_station_details("A");
    display_terminal_stations();
    display_intersections();
    find_routes("A", "D");
    find_path_to_nearest_intersection("C");

    free(all_metro_lines);

    return 0;
}

