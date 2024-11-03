#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include "include/json.hpp" // Make sure to include this library in your project.

using json = nlohmann::json;
using namespace std;

class TripPlanner
{
private:
    vector<string> cities;
    unordered_map<string, unordered_map<string, int>> distances;

public:
    void loadData(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Could not open the file: " << filename << endl;
            return;
        }

        json j;
        file >> j;
        cities = j["cities"].get<vector<string>>();

        for (const auto &d : j["distances"])
        {
            string src = d["source"];
            string dest = d["destination"];
            int distance = d["distance"];
            distances[src][dest] = distance;
            distances[dest][src] = distance; // Assume undirected distances
        }
    }

    void printLoadedCities() const
    {
        cout << "Cities loaded in the dataset:\n";
        for (const auto &city : cities)
        {
            cout << city << endl;
        }
    }

    // Dijkstra's algorithm to find the shortest path from 'start' to 'end'
    int dijkstra(const string &start, const string &end, vector<string> &path)
    {
        unordered_map<string, int> minDistance;
        unordered_map<string, string> previous;
        for (const auto &city : cities)
        {
            minDistance[city] = numeric_limits<int>::max();
        }
        minDistance[start] = 0;

        auto compare = [&minDistance](const string &left, const string &right)
        {
            return minDistance[left] > minDistance[right];
        };
        priority_queue<string, vector<string>, decltype(compare)> pq(compare);
        pq.push(start);

        while (!pq.empty())
        {
            string current = pq.top();
            pq.pop();

            if (current == end)
                break;

            for (const auto &neighbor : distances[current])
            {
                int newDist = minDistance[current] + neighbor.second;
                if (newDist < minDistance[neighbor.first])
                {
                    minDistance[neighbor.first] = newDist;
                    previous[neighbor.first] = current;
                    pq.push(neighbor.first);
                }
            }
        }

        // Reconstruct the path
        path.clear();
        if (minDistance[end] == numeric_limits<int>::max())
            return -1; // No path found

        for (string at = end; at != ""; at = previous[at])
        {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        return minDistance[end];
    }

    void planRoute(const string &start, const vector<string> &visitCities)
    {
        int totalDistance = 0;
        const int avgSpeed = 60; // Average speed in Km/h
        vector<string> pathSegment;

        cout << "\nThe best route will be: " << start;

        string prevCity = start;

        for (const auto &city : visitCities)
        {
            int segmentDistance = dijkstra(prevCity, city, pathSegment);
            if (segmentDistance == -1)
            {
                cout << "\nNo route found between " << prevCity << " and " << city << ".\n";
                return;
            }

            for (size_t i = 1; i < pathSegment.size(); ++i)
            {
                cout << " -> " << pathSegment[i] << " that is " << distances[pathSegment[i - 1]][pathSegment[i]] << " Km";
            }
            totalDistance += segmentDistance;
            prevCity = city;
        }

        cout << "\n\nTotal distance: " << totalDistance << " Km" << endl;
        cout << "Estimated time: " << (totalDistance / avgSpeed) << " hours (assuming avg speed " << avgSpeed << " Km/h)" << endl;
    }
};

int main()
{
    TripPlanner planner;
    planner.loadData("cities.json");

    // Print loaded cities before asking for user input
    planner.printLoadedCities();

    string start;
    int numVisit;
    cout << "\nEnter source city: ";
    cin >> start;

    cout << "Enter number of cities you want to visit: ";
    cin >> numVisit;

    vector<string> visitCities(numVisit);
    for (int i = 0; i < numVisit; i++)
    {
        cout << "Enter city " << i + 1 << ": ";
        cin >> visitCities[i];
    }

    planner.planRoute(start, visitCities);

    return 0;
}
