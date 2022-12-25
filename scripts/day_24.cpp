
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

class Tornado;
class Space;

class Coord
{
public:
    int first, second;

    Coord(const int& xx, const int& yy): first{xx}, second{yy} {};
    Coord() = delete;
    ~Coord() = default;
    Coord(const Coord&) = default;
    bool operator<(const Coord& c) const { if (second != c.second) return second < c.second; return first < c.first; }
    bool operator>(const Coord& c) const { if (second != c.second) return second > c.second; return first > c.first; }
    bool operator>=(const Coord& c) const { return operator==(c) || operator>(c); }
    bool operator==(const Coord& c) const { return first == c.first && second == c.second; }
    bool operator!=(const Coord& c) const { return first != c.first || second != c.second; }
    Coord operator*(const int& num) const { return Coord(first*num, second*num); }
    Coord operator+(const Coord& c) const { return Coord(first + c.first, second + c.second); }
    Coord operator-(const Coord& c) const { return Coord(first - c.first, second - c.second); }
    Coord& operator+=(const Coord& c) { first += c.first; second += c.second; return *this; }
    Coord& operator-=(const Coord& c) { first -= c.first; second -= c.second; return *this; }
};
struct coord_hash
{
    std::size_t operator()(const Coord& c) const { return std::hash<int>()(c.first) ^ std::hash<int>()(c.second); }
};
int distance(const Coord& a, const Coord& b) { return abs(a.first-b.first) + abs(a.second-b.second); }
const Coord north = {0, 1};
const Coord south = {0, -1};
const Coord east = {1, 0};
const Coord west = {-1,0};

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

class Tornado
{
private:
    const Space& space;
    Coord position;
    Coord direction;

public:
    Tornado(const Space& space, const Coord& pos, const Coord& dir) : space{space}, position{pos}, direction{dir} {}
    ~Tornado() = default;
    Tornado(const Tornado&) = default;

    Coord get_position(const int& round) const;
};

class Space
{
    friend std::ostream& operator<<(std::ostream& os, const Space& space);
private:
    std::unordered_set<Coord, coord_hash> tornado_locs;
    std::vector<Tornado> tornadoes;
    std::unordered_set<Coord, coord_hash> walls;
    int xmin, xmax, ymin, ymax;
    Coord start{0, 0};
    Coord end{0 ,0};
    int round_state{0};

    void parse_input(const std::vector<std::string>& grid)
    {
        xmin = std::numeric_limits<int>::max();
        xmax = std::numeric_limits<int>::min();
        ymin = std::numeric_limits<int>::max();
        ymax = std::numeric_limits<int>::min();
        for (int y=0; y<grid.size(); y++)
        {
            for (int x = 0; x < grid[y].size(); x++)
            {
                char ch = grid[y][x];
                Coord c = {x-1, (int)grid.size()-y-2};
                if (ch == '#')
                {
                    walls.insert(c);
                    xmin = std::min(xmin, c.first);
                    xmax = std::max(xmax, c.first);
                    ymin = std::min(ymin, c.second);
                    ymax = std::max(ymax, c.second);
                }
                else if (ch == '>')
                {
                    tornado_locs.insert(c);
                    tornadoes.push_back(Tornado(*this, c, east));
                }
                else if (ch == '<')
                {
                    tornado_locs.insert(c);
                    tornadoes.push_back(Tornado(*this, c, west));
                }
                else if (ch == '^')
                {
                    tornado_locs.insert(c);
                    tornadoes.push_back(Tornado(*this, c, north));
                }
                else if (ch == 'v')
                {
                    tornado_locs.insert(c);
                    tornadoes.push_back(Tornado(*this, c, south));
                }
            }
        }

        for (int x=xmin; x<=xmax; x++)
            if (!is_in(Coord(x, ymax), walls))
            {
                start = Coord(x, ymax);
                break;
            }
        for (int x=xmin; x<=xmax; x++)
            if (!is_in(Coord(x, ymin), walls))
            {
                end = Coord(x, ymin);
                break;
            }
    }

public:
    Space(const std::vector<std::string>& grid) { parse_input(grid); }
    Space(const Space&) = default;
    Space(Space&&) = default;
    Space& operator=(const Space&) = default;
    Space& operator=(Space&&) = default;
    ~Space() = default;

    bool at(const Coord& c) const { return tornado_locs.count(c) > 0; }
    int get_round_state() const { return round_state; }
    int get_xmax() const { return xmax; }
    int get_xmin() const { return xmin; }
    int get_ymax() const { return ymax; }
    int get_ymin() const { return ymin; }
    Coord get_start() const { return start; }
    Coord get_end() const { return end; }

    bool valid_space(const Coord& pos) const
    {
        if (pos.first > xmax || pos.first < xmin)
            return false;
            
        if (pos.second > ymax || pos.second < ymin)
            return false;

        if (is_in(pos, walls))
            return false;
        
        if (is_in(pos, tornado_locs))
            return false;
        
        return true;
    }

    void set_state(const int& round)
    {
        if (round == round_state)
            return;
        round_state = round;
        tornado_locs.clear();
        for (const auto& t : tornadoes)
            tornado_locs.insert(t.get_position(round_state));
    }
};

std::ostream& operator<<(std::ostream& os, const Space& space)
{
    for (int y=space.ymax; y>=space.ymin; --y)
    {
        for (int x=space.xmin; x<=space.xmax; ++x)
        {
            if (is_in(Coord(x, y), space.walls))
                os << "#";
            else if (is_in(Coord(x, y), space.tornado_locs))
                os << "X";
            else
                os << "."; 
        }
        os << "\n";
    }

    return os;
}

Coord Tornado::get_position(const int& round) const
{
    Coord new_pos = position + direction*round;
    while (new_pos.second >= space.get_ymax())
        new_pos.second -= (space.get_ymax()-space.get_ymin()-1);
    while (new_pos.second <= space.get_ymin())
        new_pos.second += (space.get_ymax()-space.get_ymin()-1);
    while (new_pos.first >= space.get_xmax())
        new_pos.first -= (space.get_xmax()-space.get_xmin()-1);
    while (new_pos.first <= space.get_xmin())
        new_pos.first += (space.get_xmax()-space.get_xmin()-1);

    //new_pos.first = new_pos.first % (space.get_xmax()-1);
    //new_pos.second = new_pos.second % (space.get_ymax()-1);
    return new_pos;
}

class Explorer
{
public:
    Coord current_pos;
    Coord end_pos;
    int total_steps;

    Explorer(const Coord& current, const Coord& end, int steps = 0) : current_pos{current}, end_pos{end}, total_steps{steps} {}
    Explorer(const Explorer& other) : current_pos{other.current_pos}, end_pos{other.end_pos}, total_steps{other.total_steps} {}
    ~Explorer() = default;

    std::vector<Explorer> take_step() const
    {
        std::vector<Explorer> explorers;
        explorers.emplace_back(current_pos, end_pos, total_steps + 1);
        explorers.emplace_back(current_pos + north, end_pos, total_steps + 1);
        explorers.emplace_back(current_pos + west, end_pos, total_steps + 1);
        explorers.emplace_back(current_pos + east, end_pos, total_steps + 1);
        explorers.emplace_back(current_pos + south, end_pos, total_steps + 1);
        return explorers;
    }

    int get_total_steps() const { return total_steps; }
    Coord get_current_pos() const { return current_pos; }

    size_t hash() const
    {
        std::size_t h1 = std::hash<int>()(current_pos.first);
        std::size_t h2 = std::hash<int>()(current_pos.second);
        std::size_t h3 = std::hash<int>()(total_steps);
        return h1 ^ h2 ^ h3;
    }

    int score() const
    {
        return -distance(current_pos, end_pos) - 5*current_pos.second;
    }
};

class backwards_explorer
{
private:
    Coord current_pos;
    Coord end_pos;
    int current_time;

public:
    backwards_explorer(const Coord& current, const Coord& end, int time = 0) : current_pos{current}, end_pos{end}, current_time{time} {}
    backwards_explorer(const backwards_explorer& other) : current_pos{other.current_pos}, end_pos{other.end_pos}, current_time{other.current_time} {}
    ~backwards_explorer() = default;

    bool operator==(const backwards_explorer& other) const { return (current_pos==other.current_pos && current_time==other.current_time); }

    std::vector<backwards_explorer> take_step() const
    {
        std::vector<backwards_explorer> explorers;
        explorers.emplace_back(current_pos, end_pos, current_time - 1);
        explorers.emplace_back(current_pos + south, end_pos, current_time - 1);
        explorers.emplace_back(current_pos + east, end_pos, current_time - 1);
        explorers.emplace_back(current_pos + west, end_pos, current_time - 1);
        explorers.emplace_back(current_pos + north, end_pos, current_time - 1);
        return explorers;
    }

    int get_current_time() const { return current_time; }
    Coord get_current_pos() const { return current_pos; }

    int score() const { return -distance(current_pos, end_pos); }

    size_t hash() const
    {
        std::size_t h1 = std::hash<int>()(current_pos.first);
        std::size_t h2 = std::hash<int>()(current_pos.second);
        std::size_t h3 = std::hash<int>()(current_time);
        return h1 ^ h2 ^ h3;
    }
};

struct backwards_explorer_hash
{
    std::size_t operator()(const backwards_explorer& e) const { return e.hash(); }
};
//bool operator==(const backwards_explorer& e1, const backwards_explorer& e2) { return e1==e2; }

int part_1(Space& space)
{
    const Coord end = space.get_end();

    auto heuristic = [&](const Explorer& exp_a, const Explorer& exp_b)
    {
        return (exp_a.score() < exp_b.score());
    };

    std::vector<Explorer> explorers;
    explorers.emplace_back(space.get_start(), end);

    int current_best = std::numeric_limits<int>::max();

    while (!explorers.empty())
    {
        auto it = std::max_element(explorers.begin(), explorers.end(), heuristic);
        Explorer explorer = *it;
        explorers.erase(it);

        //std::cout << current_best << " | " << explorer.total_steps << " | " << explorer.current_pos.first << ", " << explorer.current_pos.second << " | " << explorers.size() << "\n";

        if (explorer.current_pos == end)
        {
            if (explorer.get_total_steps() < current_best)
                std::cout << explorer.get_total_steps() << "\n";
            current_best = std::min(current_best, explorer.get_total_steps());
            continue;
        }

        if (explorer.total_steps + distance(explorer.current_pos, end) >= current_best)
            continue;

        std::vector<Explorer> next_explorers = explorer.take_step();
        for (const Explorer& next_explorer : next_explorers)
        {
            space.set_state(next_explorer.get_total_steps());

            if (!space.valid_space(next_explorer.get_current_pos()))
                continue;

            explorers.push_back(std::move(next_explorer));
        }
    }

    return current_best;
}


int part_1_backwards(Space& space, const Coord& start, const Coord& end, const int& min_time=0)
{
    int global_search_time = min_time+distance(start, end);
    std::unordered_set<backwards_explorer, backwards_explorer_hash> visited;
    
    while (true)
    {
        //std::cout << global_search_time << "\n";
        space.set_state(global_search_time);

        std::vector<backwards_explorer> explorers;
        explorers.emplace_back(end, start, global_search_time);

        int current_best = std::numeric_limits<int>::max();

        while (!explorers.empty())
        {
            auto explorer = explorers.back();
            explorers.pop_back();
            //std::cout << explorers.size() << " | " << explorer.get_current_time() << " | " << explorer.get_current_pos().first << " , " << explorer.get_current_pos().second << "\n";

            if (explorer.get_current_pos() == start)
                return global_search_time;
            
            if (explorer.get_current_time()-min_time < distance(explorer.get_current_pos(), start))
                continue;

            if (is_in(explorer, visited))
                continue;
            else
                visited.insert(explorer);

            auto next_explorers = explorer.take_step();
            for (const auto& next_explorer : next_explorers)
            {
                space.set_state(next_explorer.get_current_time());

                if (!space.valid_space(next_explorer.get_current_pos()))
                    continue;

                explorers.push_back(next_explorer);
            }
        }
        global_search_time++;
    }
}

void solve_AoC(Space& space)
{
    int time_a, time_b, time_c;
    time_a = part_1_backwards(space, space.get_start(), space.get_end(), 0);
    std::cout << "Part 1: " << time_a << "\n";
    time_b = part_1_backwards(space, space.get_end(), space.get_start(), time_a);
    time_c = part_1_backwards(space, space.get_start(), space.get_end(), time_b);
    std::cout << "Part 2: " << time_c << "\n";
}


int main()
{
    auto data = read_file("data/day_24.dat");
    Space space{data};
    solve_AoC(space);

    return 0;
}
