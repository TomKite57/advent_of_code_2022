
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <regex>
#include <set>
#include <numeric>
#include <algorithm>

using coord = std::pair<int, int>;
coord operator+(const coord& a, const coord& b){ return {a.first+b.first, a.second+b.second}; }
int dist(const coord& a, const coord& b){ return abs(a.first-b.first) + abs(a.second-b.second); }
int dist(const std::tuple<int, int, int, int>& c){ return dist({std::get<0>(c), std::get<1>(c)}, {std::get<2>(c), std::get<3>(c)}); }

std::tuple<int, int, int, int> parse_line(const std::string& str)
{
    std::regex pattern("Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at x=(-?\\d+), y=(-?\\d+)");
    std::smatch match;

    std::regex_search(str, match, pattern);

    if (match.size() != 5)
        throw("parse_line: Invalid input string.");
    
    int sensor_x = std::stoi(match[1]);
    int sensor_y = std::stoi(match[2]);
    int beacon_x = std::stoi(match[3]);
    int beacon_y = std::stoi(match[4]);

    return {sensor_x, sensor_y, beacon_x, beacon_y};
}

std::vector<std::tuple<int, int, int, int>> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<std::string> lines;
    for (std::string line; std::getline(file, line);)
        lines.push_back(line);
    
    std::vector<std::tuple<int, int, int, int>> coordinates;
    std::transform(lines.begin(), lines.end(), std::back_inserter(coordinates), parse_line);

    return coordinates;
}
class sensor_beacon_pair
{
private:
    coord sensor, beacon;
    int total_distance;

public:
    sensor_beacon_pair(const std::tuple<int,int,int,int>& coords):
        sensor{std::get<0>(coords), std::get<1>(coords)}, beacon{std::get<2>(coords), std::get<3>(coords)}
        { total_distance = dist(sensor, beacon); }
    ~sensor_beacon_pair() = default;
    sensor_beacon_pair(const sensor_beacon_pair&) = default;

    coord get_beacon() const { return beacon; }
    coord get_sensor() const { return sensor; }

    int sensor_dist(const coord& pos) const { return dist(sensor, pos); }

    int excluded_range(int y_row, std::pair<int, int>& range) const
    {
        coord pos = {sensor.first, y_row};
        if (sensor_dist(pos) > total_distance)
            return 0;

        range = { sensor.first - total_distance + abs(y_row-sensor.second),
                  sensor.first + total_distance - abs(y_row-sensor.second) };
        return 1;
    }
};

int unite_ranges(std::pair<int,int>& a, std::pair<int,int>& b)
{
    // Get ordering
    if (a.first > b.first)
        return unite_ranges(b, a);
    
    // a is now always to the left
    if (a.second+1 < b.first)
        return 0;

    a.second = std::max(a.second, b.second);
    b.first = a.first;
    b.second = a.second;
    return 1;
}

int intersected_ranges(std::pair<int,int>& a, std::pair<int,int>& b)
{
    // Get ordering
    if (a.first > b.first)
        return intersected_ranges(b, a);
    
    // a is now always to the left
    if (a.second < b.first)
    {
        return 0;
    }

    a.first = std::max(a.first, b.first);
    a.second = std::min(a.second, b.second);
    b.first = a.first;
    b.second = a.second;
    return 1;
}

int part_1(const std::vector<sensor_beacon_pair>& pairs, int y_row)
{
    std::vector<std::pair<int,int>> all_ranges;
    std::set<coord> beacons_on_line;
    for (const auto& pair : pairs)
    {
        std::pair<int,int> range{0, 0};
        if (pair.excluded_range(y_row, range))
            all_ranges.push_back(range);
        
        if (pair.get_beacon().second == y_row)
            beacons_on_line.insert(pair.get_beacon());
    }

    for (auto it_a = all_ranges.begin(); it_a<all_ranges.end(); ++it_a)
    {
        for (auto it_b = it_a+1; it_b<all_ranges.end(); ++it_b)
        {
            if (unite_ranges(*it_a, *it_b))
            {
                it_b = all_ranges.erase(it_b);
                it_a = all_ranges.begin()-1;
                
                break;
            }
        }
    }

    return std::accumulate(all_ranges.begin(), all_ranges.end(), -beacons_on_line.size(), [](const int& c, const std::pair<int,int>& r){return c + r.second-r.first + 1;});
}

std::string part_2(const std::vector<sensor_beacon_pair>& pairs, int min, int max)
{
    for (int y_row=min; y_row<=max; ++y_row)
    {

        std::vector<std::pair<int,int>> all_ranges;
        for (const auto& pair : pairs)
        {
            std::pair<int,int> full_range = {min, max};
            std::pair<int,int> range{0, 0};
            if (!pair.excluded_range(y_row, range))
                continue;
            if (intersected_ranges(range, full_range))
                all_ranges.push_back(range);
        }

        for (auto it_a = all_ranges.begin(); it_a<all_ranges.end(); ++it_a)
        {
            for (auto it_b = it_a+1; it_b<all_ranges.end(); ++it_b)
            {
                if (unite_ranges(*it_a, *it_b))
                {
                    it_b = all_ranges.erase(it_b);
                    it_a = all_ranges.begin()-1;
                    
                    break;
                }
            }
        }

        if (all_ranges.size() > 1)
        {
            //int x = std::min(all_ranges[0].second, all_ranges[1].second)+1;
            int x = all_ranges[0].second+1;

            std::string x_str, y_str;

            y_str = std::to_string(y_row);
            if (y_str.size() == 7)
            {
                x_str = std::to_string(4*x + y_row/1000000);
                y_str.erase(0, 1);
            }
            else
            {
                x_str = std::to_string(4*x);
                while (y_str.size()!= 6)
                    y_str = "0" + y_str;
            }
            return x_str + y_str;
        }
    }

    return "-1";
}

int main()
{
    auto data = read_file("data/day_15.dat");
    
    std::vector<sensor_beacon_pair> all_pairs;
    for (const auto& row : data)
        all_pairs.push_back(sensor_beacon_pair{row});

    //std::cout << "Part 1: " << part_1(all_pairs, 10) << "\n";
    //std::cout << "Part 2: " << part_2(all_pairs, 0, 20) << "\n";

    std::cout << "Part 1: " << part_1(all_pairs, 2000000) << "\n";
    std::cout << "Part 2: " << part_2(all_pairs, 0, 4000000) << "\n";

    return 0;
}