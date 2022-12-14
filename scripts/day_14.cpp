
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <windows.h>

using coord = std::pair<int, int>;

void sleep(int milli)
{
    Sleep(milli);
}

std::vector<coord> parse_line(const std::string& str)
{
    std::vector<coord> rval;
    std::stringstream ss{str};
    std::string substr1, substr2;

    while (ss)
    {
        std::getline(ss, substr1, ',');
        ss >> substr2;
        rval.push_back({std::stoi(substr1), std::stoi(substr2)});
        
        if (!ss.eof())
            ss >> substr1;
    }

    return rval;
}

std::vector<std::vector<coord>> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::vector<std::vector<coord>> rval;
    for (std::string str1; std::getline(file, str1);)
        rval.push_back(parse_line(str1));

    file.close();

    return rval;
}

class sandfall_model
{
    friend std::ostream& operator<<(std::ostream& os, const sandfall_model&);
private:
    std::map<coord, char> _map;
    int sand_count=0;
    int xmin, xmax, ymin, ymax;
    coord sand_entry;

    char operator[](const coord& c) const { if (_map.find(c)!=_map.end()) return _map.at(c); return '.'; }

    void add_rock(const coord& c)
    {
        _map[c] = '#';
        xmin = std::min(xmin, c.first);
        xmax = std::max(xmax, c.first);
        ymin = std::min(ymin, c.second);
        ymax = std::max(ymax, c.second);
    }

public:
    sandfall_model(const coord& sand): sand_entry{sand}, xmin{sand.first}, xmax{sand.first}, ymin{sand.second}, ymax{sand.second} {}
    ~sandfall_model() = default;
    sandfall_model(const sandfall_model&) = default;

    int get_sand_count(){ return sand_count; }

    void add_rock_row(const coord& c1, const coord& c2)
    {
        int dx = 0;
        int dy = 0;

        if (c1.first == c2.first)
            dy = (c1.second < c2.second) ? +1 : -1;
        
        if (c1.second == c2.second)
            dx = (c1.first < c2.first) ? +1 : -1;
        
        coord moving = c1;
        add_rock(moving);
        while (moving != c2)
        {
            moving = {moving.first + dx, moving.second + dy};
            add_rock(moving);
        }
    }

    int add_sand_p1()
    {
        coord sand_loc{sand_entry};

        while(true)
        {
            coord down = {sand_loc.first, sand_loc.second+1};
            coord down_left = {sand_loc.first-1, sand_loc.second+1};
            coord down_right = {sand_loc.first+1, sand_loc.second+1};

            if (sand_loc.second > ymax)
                return 0;

            if (operator[](down) == '.')
            {
                sand_loc = down;
                continue;
            }
            else if (operator[](down_left) == '.')
            {
                sand_loc = down_left;
                continue;
            }
            else if (operator[](down_right) == '.')
            {
                sand_loc = down_right;
                continue;
            }

            _map[sand_loc] = 'o';
            ++sand_count;
            return 1;
        }
    }

    int add_sand_p2()
    {
        coord sand_loc{sand_entry};
        int yfloor = ymax+2;

        while(true)
        {
            coord down = {sand_loc.first, sand_loc.second+1};
            coord down_left = {sand_loc.first-1, sand_loc.second+1};
            coord down_right = {sand_loc.first+1, sand_loc.second+1};

            if (sand_loc.second == yfloor-1)
            {
                _map[sand_loc] = 'o';
                ++sand_count;
                return 1;
            }

            if (operator[](down) == '.')
            {
                sand_loc = down;
                continue;
            }
            else if (operator[](down_left) == '.')
            {
                sand_loc = down_left;
                continue;
            }
            else if (operator[](down_right) == '.')
            {
                sand_loc = down_right;
                continue;
            }

            if (sand_loc == sand_entry)
            {
                _map[sand_loc] = 'o';
                ++sand_count;
                return 0;
            }

            _map[sand_loc] = 'o';
            ++sand_count;
            return 1;
        }
    }
};

std::ostream& operator<<(std::ostream& os, const sandfall_model& mod)
{
    for (int y=mod.ymin-1; y<mod.ymax+2; ++y)
    {
        for (int x=mod.xmin-1; x<mod.xmax+2; ++x)
        {
            os << mod[{x, y}];
        }
        os << "\n";
    }

    return os;
}

int part_1(sandfall_model cave, bool animate=false)
{
    if (animate)
        do { std::cout << cave << "\n"; sleep(250); } while (cave.add_sand_p1());
    else
    {
        while (cave.add_sand_p1())
        continue;
    }
    
    return cave.get_sand_count();
}

int part_2(sandfall_model cave, bool animate=false)
{
    if (animate)
        do { std::cout << cave << "\n"; sleep(250); } while (cave.add_sand_p2());
    else
    {
        while (cave.add_sand_p2())
        continue;
    }
    
    return cave.get_sand_count();
}

int main()
{
    auto data = read_file("data/day_14.dat");
    sandfall_model cave{{500,0}};
    for (const auto& row : data)
        for (auto it=row.begin()+1; it!=row.end(); ++it)
            cave.add_rock_row(*(it-1), *it);
    
    std::cout << "Part 1: " << part_1(cave) << "\n";
    std::cout << "Part 2: " << part_2(cave) << "\n";    

    return 0;
}