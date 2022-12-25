
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <functional>

class Elf;
class Space;

class coord
{
public:
    int first, second;

    coord(const int& xx, const int& yy): first{xx}, second{yy} {};
    coord() = delete;
    ~coord() = default;
    coord(const coord&) = default;
    bool operator<(const coord& c) const { if (second != c.second) return second < c.second; return first < c.first; }
    bool operator>(const coord& c) const { if (second != c.second) return second > c.second; return first > c.first; }
    bool operator>=(const coord& c) const { return operator==(c) || operator>(c); }
    bool operator==(const coord& c) const { return first == c.first && second == c.second; }
    bool operator!=(const coord& c) const { return first != c.first || second != c.second; }
    coord operator+(const coord& c) const { return coord(first + c.first, second + c.second); }
    coord operator-(const coord& c) const { return coord(first - c.first, second - c.second); }
    coord& operator+=(const coord& c) { first += c.first; second += c.second; return *this; }
    coord& operator-=(const coord& c) { first -= c.first; second -= c.second; return *this; }
};
struct coord_hash
{
    std::size_t operator()(const coord& c) const { return std::hash<int>()(c.first) ^ std::hash<int>()(c.second); }
};
const coord north = {0, 1};
const coord south = {0, -1};
const coord east = {1, 0};
const coord west = {-1,0};
const coord northeast = {1, 1};
const coord northwest = {-1, 1};
const coord southeast = {1, -1};
const coord southwest = {-1, -1};

const std::map<coord, std::vector<coord>> adjacent_directions = 
{
    {north, {north, northeast, northwest}},
    {east, {east, northeast, southeast}},
    {south, {south, southeast, southwest}},
    {west, {west, northwest, southwest}},
};

template <typename Key, typename Container>
bool is_in(const Key& key, const Container& container)
{
    return container.find(key) != container.end();
}

std::vector<std::string> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<std::string> rval;
    for (std::string str; std::getline(file, str);)
        rval.push_back(str);

    return rval;
}

std::unordered_set<coord, coord_hash> parse_grid(const std::vector<std::string>& grid)
{
    std::unordered_set<coord, coord_hash> elves;
    for (int y=0; y<grid.size(); y++)
        for (int x = 0; x < grid[y].size(); x++)
            if (grid[y][x] == '#')
                elves.emplace(x, -y);
    return elves;
}

class Elf
{
private:
    std::vector<coord> directions{north, south, west, east};
    const Space& space;
    coord position;
    coord next_position{0, 0};

public:
    Elf(const Space& space, const coord& position) : space{space}, position{position} {}
    ~Elf() = default;
    Elf(const Elf&) = default;

    void set_position(const coord& pos_in) { position = pos_in; }
    coord get_position() const { return position; }
    coord get_next_position() const { return next_position; }

    void generate_next_position();
};

class Space
{
    friend std::ostream& operator<<(std::ostream& os, const Space& space);
private:
    std::unordered_set<coord, coord_hash> elf_locs;
    std::vector<Elf> elves;
    int rounds{0};

public:
    Space(const std::vector<std::string>& grid) : elf_locs{parse_grid(grid)}
    {
        for (const auto& loc : elf_locs)
        {
            elves.emplace_back(*this, loc);
        }
    }
    Space(const Space&) = default;
    Space(Space&&) = default;
    Space& operator=(const Space&) = default;
    Space& operator=(Space&&) = default;
    ~Space() = default;

    bool at(const coord& c) const { return elf_locs.count(c) > 0; }
    int get_rounds() const { return rounds; }

    bool evolve()
    {
        rounds++;
        // Collect the proposed new positions of the Elves
        std::unordered_map<coord, int, coord_hash> proposed_positions;
        for (auto& elf : elves)
        {
            elf.generate_next_position();
            const coord new_pos = elf.get_next_position();
            if (is_in(new_pos, proposed_positions))
            {
                proposed_positions[new_pos]++;
            }
            else
            {
                proposed_positions[new_pos] = 1;
            }
        }

        // Update the positions of the Elves
        bool someone_moved=false;
        elf_locs.clear();
        for (auto& elf : elves)
        {

            const coord old_pos = elf.get_position();
            const coord new_pos = elf.get_next_position();
            if (proposed_positions[new_pos] == 1)
            {
                elf.set_position(new_pos);
                elf_locs.emplace(new_pos);
                if (new_pos!=old_pos)
                    someone_moved=true;
            }
            else
            {
                elf_locs.emplace(old_pos);
            }
        }
        return someone_moved;
    }

    int checksum() const
    {
        int min_x = std::numeric_limits<int>::max();
        int max_x = std::numeric_limits<int>::min();
        int min_y = std::numeric_limits<int>::max();
        int max_y = std::numeric_limits<int>::min();
        for (const auto& elf : elves)
        {
            const auto c = elf.get_position();
            int x = c.first;
            int y = c.second;
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
        return (max_x - min_x + 1) * (max_y - min_y + 1) - elves.size();
    }  
};

std::ostream& operator<<(std::ostream& os, const Space& space)
{
    // Find the minimum and maximum x and y coordinates of the elves
    int xmin = std::numeric_limits<int>::max();
    int xmax = std::numeric_limits<int>::min();
    int ymin = std::numeric_limits<int>::max();
    int ymax = std::numeric_limits<int>::min();
    for (const auto& elf : space.elves)
    {
        xmin = std::min(xmin, elf.get_position().first);
        xmax = std::max(xmax, elf.get_position().first);
        ymin = std::min(ymin, elf.get_position().second);
        ymax = std::max(ymax, elf.get_position().second);
    }

    for (int y=ymax; y>=ymin; --y)
    {
        for (int x=xmin; x<=xmax; ++x)
        {
            if (is_in(coord(x, y), space.elf_locs))
                os << "#";
            else
                os << ".";
        }
        os << "\n";
    }

    return os;
}

void Elf::generate_next_position()
{
    bool space_free=true;
    for (const coord& dir : {north, east, south, west, northeast, northwest, southeast, southwest})
        if (space.at(position+dir))
        {
            space_free=false;
            break;
        }
    if (space_free)
    {
        next_position = position;
        directions.push_back(directions.front());
        directions.erase(directions.begin());
        return;
    }
    
    for (const auto& direction : directions)
    {
        bool free_space=true;
        for (const auto& direction_to_consider : adjacent_directions.at(direction))
        {
            if (space.at(position + direction_to_consider))
            {
                free_space = false;
                break;
            }
        }
        if (free_space)
        {
            next_position = position + direction;
            directions.push_back(directions.front());
            directions.erase(directions.begin());
            return;
        }
    }
    next_position = position;
    directions.push_back(directions.front());
    directions.erase(directions.begin());
    return;
}

int main()
{
    auto data = read_file("data/day_23.dat");
    Space space{data};

    //std::cout << space << "\n";
    for (int i=0; i<10; ++i)
    {
        space.evolve();
        //std::cout << space << "\n";
    }

    std::cout << "Part 1: " << space.checksum() << "\n";

    while (space.evolve())
        continue;
    
    std::cout << "Part 2: " << space.get_rounds() << "\n";

    return 0;
}
