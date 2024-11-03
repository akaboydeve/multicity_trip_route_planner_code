#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <nlohmann/json.hpp> // Make sure to include this library in your project.

using json = nlohmann::json;

class TripPlanner
{
private:
    std::vector<std::string> cities;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> distances;

public:
    void loadData(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Could not open the file: " << filename << std::endl;
            return;
        }

        json j;
        file >> j;

        cities = j["cities"].get<std::vector<std::string>>();

        for (const auto &d : j["distances"])
        {
            std::string src = d["source"];
            std::string dest = d["destination"];
            int distance = d["distance"];
            distances[src][dest] = distance;
        }
    }

    void findShortestTrip(const std::string &start, const std::string &end)
    {
        std::unordered_map<std::string, int> min_distance;
        std::unordered_map<std::string, bool> visited;
        std::unordered_map<std::string, std::string> previous;

        for (const auto &city : cities)
        {
            min_distance[city] = std::numeric_limits<int>::max();
            visited[city] = false;
        }
        min_distance[start] = 0;

        for (size_t i = 0; i < cities.size(); ++i)
        {
            std::string current_city;
            int min_dist = std::numeric_limits<int>::max();

            for (const auto &city : cities)
            {
                if (!visited[city] && min_distance[city] < min_dist)
                {
                    min_dist = min_distance[city];
                    current_city = city;
                }
            }

            if (current_city.empty() || current_city == end)
                break;
            visited[current_city] = true;

            for (const auto &neighbor : distances[current_city])
            {
                int new_distance = min_distance[current_city] + neighbor.second;
                if (new_distance < min_distance[neighbor.first])
                {
                    min_distance[neighbor.first] = new_distance;
                    previous[neighbor.first] = current_city;
                }
            }
        }

        if (min_distance[end] == std::numeric_limits<int>::max())
        {
            std::cout << "No path found from " << start << " to " << end << std::endl;
            return;
        }

        std::cout << "Shortest distance from " << start << " to " << end << " is " << min_distance[end] << " km." << std::endl;

        std::vector<std::string> path;
        for (std::string at = end; at != ""; at = previous[at])
        {
            path.push_back(at);
        }
        std::reverse(path.begin(), path.end());

        std::cout << "Path: ";
        for (const auto &city : path)
        {
            std::cout << city << " ";
        }
        std::cout << std::endl;
    }
};

int main()
{
    TripPlanner planner;
    planner.loadData("cities.json");

    std::string start, end;
    std::cout << "Enter the starting city: ";
    std::cin >> start;
    std::cout << "Enter the destination city: ";
    std::cin >> end;

    planner.findShortestTrip(start, end);

    return 0;
}
