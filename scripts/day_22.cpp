
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
    coord operator+(const coord& c) const { return coord(first + c.first, second + c.second); }
    coord operator-(const coord& c) const { return coord(first - c.first, second - c.second); }
    coord& operator+=(const coord& c) { first += c.first; second += c.second; return *this; }
    coord& operator-=(const coord& c) { first -= c.first; second -= c.second; return *this; }
};
struct coord_hash
{
    std::size_t operator()(const coord& c) const { return std::hash<int>()(c.first) ^ std::hash<int>()(c.second); }
};
struct coord_coord_hash
{
    std::size_t operator()(const std::pair<coord, coord>& c) const
    {
        return std::hash<int>()(c.first.first) ^ std::hash<int>()(c.first.second) ^ std::hash<int>()(c.second.first) ^ std::hash<int>()(c.second.second);
    }
};
const coord north = {0, 1};
const coord south = {0, -1};
const coord east = {1, 0};
const coord west = {-1,0};

template <typename Key, typename Container>
bool is_in(const Key& key, const Container& container)
{
    return container.find(key) != container.end();
}

std::pair<std::vector<std::string>, std::string> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<std::string> map_part;
    std::string walk_part;
    std::string tmp;
    while (std::getline(file, tmp) && !tmp.empty())
        map_part.push_back(tmp);
    std::getline(file, walk_part);

    return {map_part, walk_part};
}

class map
{
    friend map map_builder(const std::vector<std::string> &);

private:
    std::unordered_map<coord, char, coord_hash> locations;
    std::unordered_map<std::pair<coord, coord>, std::pair<coord, coord>, coord_coord_hash> wrap_locations;
    int xmin, xmax, ymin, ymax;

    void normalise_coords()
    {
        std::unordered_map<coord, char, coord_hash> new_locations;

        coord origin{xmin, ymin};
        for (const auto& c : locations)
            new_locations.insert({c.first-origin, c.second});

        locations = new_locations;
        xmax -= xmin;
        ymax -= ymin;
        xmin -= xmin;
        ymin -= ymin;
    }

    std::pair<coord, coord> smart_cube_wrap_build(const coord &pos, const coord &direc)
    {
        //   |5|6|
        //   |4|
        // |2|3|
        // |1|
        // std::cout << xmin << ", " << ymin << " | " << xmax << ", " << ymax << " | " << pos.first << ", " << pos.second << " | " << direc.first << ", " << direc.second << "\n";

        const int x = pos.first;
        const int y = pos.second;
        const int dx = x - xmin;
        const int dy = y - ymin;
        // Face 1 -> Face 5 (low == right)
        if (x == xmin && dy < 50 && direc == west)
            return {{xmin + 100 - dy -1, ymax}, south};
        // Face 1 -> Face 6 (left == right)
        if (dx < 50 && y == ymin && direc == south)
            return {{xmin + 150 - dx - 1, ymax}, south};
        // Face 2 -> Face 5 (low == high)
        if (x == xmin && dy >= 50 && direc == west)
            return {{xmin + 50, ymin + 250 - dy -1}, east};
        // Face 2 -> Face 4 (right == low)
        if (dx < 50 && y == ymin + 100 - 1 && direc == north)
            return {{xmin + 50, ymin + 150 - dx -1}, east};
        // Face 1 -> Face 3 (high == left)
        if (dx == 50 - 1 && dy < 50 && direc == east)
            return {{xmin + 100 - dy -1, ymin + 50}, north};
        // Face 3 -> Face 6 (high == low)
        if (dx == 100 - 1 && dy < 100 && direc == east)
            return {{xmax, ymin + 250 - dy -1}, west};
        // Face 4 -> Face 6 (high == left)
        if (dx == 100 - 1 && dy >= 100 && direc == east)
            return {{xmin + 250 - dy - 1, ymin + 150}, north};

        return {{-1, -1}, {-1, -1}};
    }

    void add_reverse_cube_wrap_locations()
    {
        std::cout << wrap_locations.size() << "\n";
        std::unordered_map<std::pair<coord, coord>, std::pair<coord, coord>, coord_coord_hash> tmp_copy = wrap_locations;
        for (const auto& row : wrap_locations)
        {
            const coord& before_pos = row.first.first;
            const coord& before_direc = row.first.second;
            const coord& after_pos = row.second.first;
            const coord& after_direc = row.second.second;

            tmp_copy.insert({{after_pos, coord(0, 0) - after_direc}, {before_pos, coord(0, 0) - before_direc}});
        }
        wrap_locations = tmp_copy;
        std::cout << wrap_locations.size() << "\n";
    }

public:
    map() = default;
    ~map() = default;
    map(const map&) = default;

    void build_locations(const std::vector<std::string> &map_in)
    {
        bool first_found = true;
        for (auto it_st = map_in.begin(); it_st != map_in.end(); ++it_st)
        for (auto it_ch = it_st->begin(); it_ch != it_st->end(); ++it_ch)
        {
            if (*it_ch == ' ')
                continue;

            int x = it_ch - it_st->begin();
            int y = map_in.end() - it_st;
            locations.insert({{x, y}, *it_ch});
            if (first_found)
            {
                xmin = xmax = x;
                ymin = ymax = y;
                first_found = false;
            }
            else
            {
                xmin = std::min(xmin, x);
                xmax = std::max(xmax, x);
                ymin = std::min(ymin, y);
                ymax = std::max(ymax, y);
            }
        }

        normalise_coords();
    }

    void build_cube_wrap_locations()
    {
        for (const auto &c : locations)
        {
            for (const auto &direc : {north, east, south, west})
            {
                    if (is_in(c.first + direc, locations))
                        continue;

                    std::pair<coord, coord> mapped_coord = smart_cube_wrap_build(c.first, direc);
                    if (mapped_coord.second == coord(-1, -1))
                        continue;

                    //std:: cout << c.first.first << ", " << c.first.second << " | "
                    //           << direc.first << ", " << direc.second << "\n"
                    //           << mapped_coord.first.first << ", " << mapped_coord.first.second << " | "
                    //           << mapped_coord.second.first << ", " << mapped_coord.second.second << "\n\n";
                    wrap_locations.insert({{c.first, direc}, mapped_coord});
            }
        }
        add_reverse_cube_wrap_locations();
    }

    void build_wrap_locations()
    {
        for (const auto &c : locations)
        {
            for (const auto &direc : {north, east, south, west})
            {
                if (is_in(c.first + direc, locations))
                    continue;

                auto pos = c.first;
                // Walk in opposite direction till you find an edge
                while (is_in(pos - direc, locations))
                    pos -= direc;

                wrap_locations.insert({{c.first, direc}, {pos, direc}});
            }
        }
    }

    std::pair<coord, coord> new_pos(const coord& pos, const coord& direc) const
    {
        if (is_in(pos+direc, locations))
        {
            return {pos+direc, direc};
        }
        std::cout << pos.first << ", " << pos.second << " | " << direc.first << ", " << direc.second << "\n";
        return wrap_locations.at({pos, direc});
    }

    bool will_collide(const coord& pos, const coord& direc) const
    {
        coord to_check = new_pos(pos, direc).first;
        return (locations.at(to_check) == '#');
    }

    int get_checksum(const coord& pos, const coord& direc) const 
    {
        const std::map<coord, int> direc_score = {{east, 0}, {south, 1}, {west, 2}, {north, 3}};
        return 1000*(1+ymax-pos.second) + 4*(1+pos.first-xmin) + direc_score.at(direc);
    }

    coord get_starting_pos() const
    {
        for (int x=xmin; x<=xmax; ++x)
        {
            if (is_in(coord(x, ymax), locations))
                return {x, ymax};
        }
        throw("");
    }
};

map map_builder(const std::vector<std::string>& map_in, const bool& cube=false)
{
    map new_map{};
    new_map.build_locations(map_in);
    if (!cube)
        new_map.build_wrap_locations();
    else
        new_map.build_cube_wrap_locations();
    
    return new_map;
}

class walker
{
private:
    const map space;
    coord pos{-1, -1};
    coord direc{east};

    void move(int len)
    {
        while (len!=0)
        {
            //std::cout << pos.first << ", " << pos.second << " | " << direc.first << ", " << direc.second << "\n";
            if (space.will_collide(pos, direc))
                return;
            
            auto new_pos_direc = space.new_pos(pos, direc);
            pos = new_pos_direc.first;
            direc = new_pos_direc.second;
            --len;
        }
    }

    void rotate(char L_or_R)
    {
        if (L_or_R == 'L')
        {
            direc = std::map<coord, coord>{{north, west}, {west, south}, {south, east}, {east, north}}.at(direc);
        }
        else if (L_or_R == 'R')
        {
            direc = std::map<coord, coord>{{north, east}, {east, south}, {south, west}, {west, north}}.at(direc);
        }
        else
            throw("");
    }

public:
    walker(const map& sp)
    : space{sp}
    {
        pos = space.get_starting_pos();
    }
    ~walker() = default;
    walker(const walker&) = default;

    void parse_instruction(const std::string& inst)
    {
        std::regex regex("(\\d+)([LR]|$)");

        std::sregex_iterator it(inst.begin(), inst.end(), regex);
        std::sregex_iterator end;
        while (it != end)
        {
            int len = std::stoi((*it)[1]);
            move(len);
            if ((*it)[2].length() > 0)
            {   
                char to_rot = (*it)[2].str()[0];
                rotate(to_rot);
            }
            ++it;
        }
    }

    int checksum() const
    {
        return space.get_checksum(pos, direc);
    }
};

int main()
{
    auto data = read_file("data/day_22.dat");

    walker my_walker{map_builder(data.first)};
    my_walker.parse_instruction(data.second);
    std::cout << "Part 1: " << my_walker.checksum() << "\n";

    walker cube_walker{map_builder(data.first, true)};
    cube_walker.parse_instruction(data.second);
    std::cout << "Part 2: " << cube_walker.checksum() << "\n";

    //std::cout << my_map << "\n";

    //for (const auto& line : data.first)
    //    std::cout << line << "\n";
    //std::cout << "\n";
    //std::cout << data.second << "\n";

    return 0;
}
