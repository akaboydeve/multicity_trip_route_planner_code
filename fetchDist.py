import requests
from bs4 import BeautifulSoup
import json
import time
import os

# List of cities
cities = [
    "Delhi", "Mumbai", "Bangalore", "Hyderabad", "Chennai",
    "Kolkata", "Pune", "Ahmedabad", "Jaipur", "Lucknow",
    "Chandigarh", "Patna", "Bhopal", "Thiruvananthapuram",
    "Guwahati", "Ranchi", "Bhubaneswar", "Imphal", "Shillong",
    "Gangtok", "Dehradun", "Shimla", "Panaji", "Raipur",
    "Aizawl", "Kohima", "Agartala", "Itanagar"
]

# Load existing distances from JSON file
def load_existing_distances(filename):
    if os.path.exists(filename):
        with open(filename, "r") as json_file:
            return json.load(json_file)
    return {"cities": cities, "distances": []}

# Function to get distance using Google Search
def get_distance(city1, city2):
    query = f"road distance from {city1} to {city2}"
    url = f"https://www.google.com/search?q={query}"
    
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36"
    }
    
    try:
        response = requests.get(url, headers=headers)
        response.raise_for_status()  # Raise an error for bad responses
    except requests.RequestException as e:
        print(f"Error fetching distance from Google for {city1} to {city2}: {e}")
        return None

    # Parse the HTML
    soup = BeautifulSoup(response.text, 'html.parser')
    
    # Extract the distance value using the provided CSS selector
    try:
        distance_element = soup.select_one('#exp0 > div.mGSxre.Ra9ql > div > div > span.UdvAnf > span:nth-child(2)')
        if distance_element:
            distance_str = distance_element.text.strip()  # Get the distance as a string
            # Clean up the distance string
            distance_str = distance_str.replace("km", "").replace("\xa0", "").replace(",", "").strip()  # Remove 'km', non-breaking spaces, and commas
            distance = int(float(distance_str))  # Convert to integer (using float for cases with decimal values)
            return distance
    except Exception as e:
        print(f"Error parsing distance for {city1} to {city2}: {e}")
        return None

# Main function to calculate distances
def main():
    filename = "cities_distance.json"
    distance_data = load_existing_distances(filename)
    total_pairs = (len(cities) * (len(cities) - 1)) // 2
    success_count = len(distance_data["distances"])
    fail_count = 0
    existing_distances = {(d["source"], d["destination"]) for d in distance_data["distances"]}

    # Calculate distances between all pairs of cities
    for i in range(len(cities)):
        for j in range(i + 1, len(cities)):
            city1 = cities[i]
            city2 = cities[j]

            # Skip if distance already exists
            if (city1, city2) in existing_distances:
                print(f"Distance already exists for {city1} to {city2}. Skipping.")
                continue

            distance = get_distance(city1, city2)
            
            if distance is not None:
                distance_data["distances"].append({
                    "source": city1,
                    "destination": city2,
                    "distance": distance
                })
                print(f"Fetched distance: {distance} km from {city1} to {city2}")  # Print distance
                success_count += 1
            else:
                fail_count += 1
            
            # Log progress
            total_done = success_count + fail_count
            print(f"Distances fetched: {total_done}/{total_pairs} | Success: {success_count} | Failures: {fail_count}")

            # Time delay between requests
            #time.sleep(2)  # Adjust as needed

    # Save updated data to JSON file
    with open(filename, "w") as json_file:
        json.dump(distance_data, json_file, indent=4)

    print(f"\nDistance data updated in {filename}")
    print(f"Total successful fetches: {success_count}/{total_pairs} | Failures: {fail_count}")

if __name__ == "__main__":
    main()
