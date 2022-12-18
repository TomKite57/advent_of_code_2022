
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <tuple>
#include <unordered_set>

using coord = std::tuple<int, int, int>;
std::ostream& operator<<(std::ostream& os, const coord& c) { return os << "(" << std::get<0>(c) << "," << std::get<1>(c) << "," << std::get<2>(c) << ")"; }
coord operator+(const coord& c1, const coord& c2)
{
    return { std::get<0>(c1) + std::get<0>(c2), std::get<1>(c1) + std::get<1>(c2), std::get<2>(c1) + std::get<2>(c2) };
}
coord operator-(const coord& c1, const coord& c2)
{
    return { std::get<0>(c1) - std::get<0>(c2), std::get<1>(c1) - std::get<1>(c2), std::get<2>(c1) - std::get<2>(c2) };
}
struct coord_hash
{
    std::size_t operator()(const coord& c) const { return std::get<0>(c) ^ std::get<1>(c) ^ std::get<2>(c); }
};
const coord forward  = {+1, 0, 0};
const coord backward = {-1, 0, 0};
const coord up       = {0, +1, 0};
const coord down     = {0, -1, 0};
const coord right    = {0, 0, +1};
const coord left     = {0, 0, -1};

coord parse_line(const std::string& line) {
    std::regex pattern("^(\\d+),(\\d+),(\\d+)$");
    std::smatch matches;
    if (!std::regex_match(line, matches, pattern)) {
        return {};
    }

    return std::make_tuple<int, int, int>(std::stoi(matches[1]), std::stoi(matches[2]), std::stoi(matches[3]));
}

std::unordered_set<coord, coord_hash> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::unordered_set<coord, coord_hash> data;
    for (std::string line; std::getline(file, line);)
        data.insert(parse_line(line));

    return data;
}
bool is_in(const coord& key, const std::unordered_set<coord, coord_hash>& set)
{
    return set.find(key) != set.end();
}

int part_1(const std::unordered_set<coord, coord_hash>& cubes)
{
    int faces = 0;
    for (const auto& c : cubes)
    {
        if (!is_in(c+forward, cubes)) ++faces;
        if (!is_in(c+backward, cubes)) ++faces;
        if (!is_in(c+up, cubes)) ++faces;
        if (!is_in(c+down, cubes)) ++faces;
        if (!is_in(c+right, cubes)) ++faces;
        if (!is_in(c+left, cubes)) ++faces;
    }
    return faces;
}

int part_2(const std::unordered_set<coord, coord_hash>& cubes)
{
    int xmin, xmax, ymin, ymax, zmin, zmax;
    xmin = xmax = ymin = ymax = zmin = zmax = 0;
    int exterior_faces = 0;
    std::vector<coord> smoke_explorers;
    std::unordered_set<coord, coord_hash> exploration_history;
    for (const auto& c : cubes)
    {
        xmin = std::min(xmin, std::get<0>(c));
        xmax = std::max(xmax, std::get<0>(c));
        ymin = std::min(ymin, std::get<1>(c));
        ymax = std::max(ymax, std::get<1>(c));
        zmin = std::min(zmin, std::get<2>(c));
        zmax = std::max(zmax, std::get<2>(c));
    }

    // initial explorer
    smoke_explorers.push_back( {xmin-1, ymin-1, zmin-1} );

    while (!smoke_explorers.empty())
    {
        auto explorer = std::move(smoke_explorers.back());
        smoke_explorers.pop_back();

        if (is_in(explorer, exploration_history))
            continue;

        int x, y, z;
        x = std::get<0>(explorer);
        y = std::get<1>(explorer);
        z = std::get<2>(explorer);

        if (x < xmin-1 || x > xmax+1)
            continue;
        if (y < ymin-1 || y > ymax+1)
            continue;
        if (z < zmin-1 || z > zmax+1)
            continue;
        
        if (is_in(explorer, cubes))
        {
            ++exterior_faces;
            continue;
        }

        smoke_explorers.push_back(explorer+forward);
        smoke_explorers.push_back(explorer+backward);
        smoke_explorers.push_back(explorer+up);
        smoke_explorers.push_back(explorer+down);
        smoke_explorers.push_back(explorer+right);
        smoke_explorers.push_back(explorer+left);

        exploration_history.insert(explorer);
    }

    return exterior_faces;
}

int main()
{
    auto data = read_file("data/day_18.dat");

    std::cout << "Part 1: " << part_1(data) << "\n";
    std::cout << "Part 2: " << part_2(data) << "\n";
    
    return 0;
}