
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <tuple>
#include <unordered_set>

template<typename T>
class ore_tuple
{
    template <typename V>
    friend std::ostream& operator<<(std::ostream& os, const ore_tuple<V>& ot);

    template <typename V, typename M>
    friend ore_tuple<V> operator*(const M& num, const ore_tuple<V>& ot);


private:
    T ore, clay, obsidian, geode;

public:
    ore_tuple() = delete;
    ore_tuple(const T& ore_in, const T& clay_in, const T& obsidian_in, const T& geode_in)
    : ore{ore_in}, clay{clay_in}, obsidian{obsidian_in}, geode{geode_in} {};
    ~ore_tuple() = default;
    ore_tuple(const ore_tuple&) = default;

    T get_ore() const { return ore; }
    T get_clay() const { return clay; }
    T get_obsidian() const { return obsidian; }
    T get_geode() const { return geode; }

    T operator[](const int& index) const
    {
        if (index==0)
            return get_ore();
        if (index==1)
            return get_clay();
        if (index==2)
            return get_obsidian();
        if (index==3)
            return get_geode();

        throw("");
    }

    void set_ore(const T& val_in) { ore=val_in; }
    void set_clay(const T& val_in) { clay=val_in; }
    void set_obsidian(const T& val_in) { obsidian=val_in; }
    void set_geode(const T& val_in) { geode=val_in; }

    ore_tuple<T> operator+(const ore_tuple& other) const { return ore_tuple(ore+other.ore, clay+other.clay, obsidian+other.obsidian, geode+other.geode); }
    ore_tuple<T> operator-(const ore_tuple& other) const { return ore_tuple(ore-other.ore, clay-other.clay, obsidian-other.obsidian, geode-other.geode); }
    ore_tuple<T> operator*(int num) const { return ore_tuple(num*ore, num*clay, num*obsidian, num*geode); }
    ore_tuple<T>& operator+=(const ore_tuple& other) { ore+=other.ore; clay+=other.clay; obsidian+=other.obsidian; geode+=other.geode; return *this; }
    ore_tuple<T>& operator-=(const ore_tuple& other) { ore-=other.ore; clay-=other.clay; obsidian-=other.obsidian; geode-=other.geode; return *this; }
    ore_tuple<T>& operator*=(int num) { ore*=num; clay*=num; obsidian*=num; geode*=num; return *this; }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const ore_tuple<T>& ot)
{
    os << "(" << ot.ore << ", " << ot.clay << ", " << ot.obsidian << ", " << ot.geode << ")";
    return os;
}

template <typename V, typename M>
    ore_tuple<V> operator*(const M& num, const ore_tuple<V>& ot) { return ore_tuple<V>(num*ot.ore, num*ot.clay, num*ot.obsidian, num*ot.geode); }

ore_tuple<ore_tuple<int>> parse_line(const std::string& line)
{
    std::regex pattern("^Blueprint (\\d+): Each ore robot costs (\\d+) ore. Each clay robot costs (\\d+) ore. Each obsidian robot costs (\\d+) ore and (\\d+) clay. Each geode robot costs (\\d+) ore and (\\d+) obsidian.$");
    std::smatch matches;
    if (!std::regex_match(line, matches, pattern))
    {
        throw("Could not match tuple");
    }

    int index = 2;
    ore_tuple<int> ore_robot_cost{std::stoi(matches[index++]), 0, 0, 0};
    ore_tuple<int> clay_robot_cost{std::stoi(matches[index++]), 0, 0, 0};
    ore_tuple<int> obsidian_robot_cost{std::stoi(matches[index++]), std::stoi(matches[index++]), 0, 0};
    ore_tuple<int> geode_robot_cost{std::stoi(matches[index++]), 0, std::stoi(matches[index++]), 0};

    return ore_tuple<ore_tuple<int>>(ore_robot_cost, clay_robot_cost, obsidian_robot_cost, geode_robot_cost);
}

std::vector<ore_tuple<ore_tuple<int>>> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<ore_tuple<ore_tuple<int>>> data;
    for (std::string line; std::getline(file, line);)
        data.push_back(parse_line(line));

    return data;
}

class robot_system
{
    friend std::ostream& operator<<(std::ostream& os, const robot_system& rs);
private:
    const ore_tuple<ore_tuple<int>>& blueprint;
    const ore_tuple<ore_tuple<int>> robot_options = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    ore_tuple<int> materials{0, 0, 0, 0};
    ore_tuple<int> robots{1, 0, 0, 0};
    ore_tuple<int> max_costs{0, 0, 0, 0};
    int next_robot{-1};
    int time{0};
    int time_limit;

    void yield_resource() { materials += robots; }
    void time_increment() { ++time; }
    void pay_construction(const ore_tuple<int>& robot)
    {
        for (const int& i : {0, 1, 2, 3})
            materials -= robot[i] * blueprint[i];
    }
    void pay_construction(){ pay_construction(robot_options[next_robot]); }
    void add_robot(){ robots += robot_options[next_robot]; }
    void set_next_robot(int next) { next_robot = next; }
    bool can_afford_next()
    {
        ore_tuple<int> cost = blueprint[next_robot];
        for (const int& i : {0, 1, 2, 3})
            if (materials[i] < cost[i])
                return false;
        return true;
    }

    bool keep_option() const
    {
        if (next_robot==3)
            return true;

        if (robots[next_robot] >= max_costs[next_robot])
            return false;

        if ((max_costs[next_robot]-robots[next_robot])*get_time_remaining() <= materials[next_robot])
            return false;

        return true;
    }

public:
    robot_system() = delete;
    robot_system(const ore_tuple<ore_tuple<int>>& blue_in, int next, int limit)
    : blueprint{blue_in}, next_robot{next}, time_limit{limit}
    {
        for (const auto& rob_cost : {blueprint.get_ore(), blueprint.get_clay(), blueprint.get_obsidian(), blueprint.get_geode()})
        {
            max_costs.set_ore( std::max(max_costs.get_ore(), rob_cost.get_ore()) );
            max_costs.set_clay( std::max(max_costs.get_clay(), rob_cost.get_clay()) );
            max_costs.set_obsidian( std::max(max_costs.get_obsidian(), rob_cost.get_obsidian()) );
            max_costs.set_geode( std::max(max_costs.get_geode(), rob_cost.get_geode()) );
        }
    }
    ~robot_system() = default;
    robot_system(const robot_system&) = default;

    bool is_finished() const { return time==time_limit; }
    int geode_yield() const { return materials.get_geode(); }
    int get_time_remaining() const { return time_limit-time; }
    int optimistic_projection() const { int tr = get_time_remaining(); return materials[3] + robots[3]*tr + tr*(tr-1);}

    std::vector<robot_system> evolve_v2()
    {
        bool can_afford = (can_afford_next() && time!=time_limit-1);

        if (can_afford)
            pay_construction();
        yield_resource();
        time_increment();
        
        if (!can_afford)
            return {*this};
        
        add_robot();
        std::vector<robot_system> out_clones;
        for (const int& i : {3, 2, 1, 0})
        {
            robot_system new_clone{*this};
            new_clone.set_next_robot(i);
            if (!new_clone.keep_option())
                continue;
            out_clones.push_back(new_clone);
        }

        return out_clones;
    }
};

std::ostream& operator<<(std::ostream& os, const robot_system& rs)
{
    os << "Robots:    (" << rs.robots.get_ore() << ", " << rs.robots.get_clay() << ", " << rs.robots.get_obsidian() << ", " << rs.robots.get_geode() << ")\n";
    os << "Materials: (" << rs.materials.get_ore() << ", " << rs.materials.get_clay() << ", " << rs.materials.get_obsidian() << ", " << rs.materials.get_geode() << ")\n";
    os << "Max costs: (" << rs.max_costs.get_ore() << ", " << rs.max_costs.get_clay() << ", " << rs.max_costs.get_obsidian() << ", " << rs.max_costs.get_geode() << ")\n";
    os << "Time passed: " << rs.time << "/" << rs.time_limit;
    return os;
}

int best_yield(const ore_tuple<ore_tuple<int>>& blueprint, int time_limit=24)
{
    std::vector<robot_system> all_systems;
    all_systems.push_back(robot_system(blueprint, 0, time_limit));
    all_systems.push_back(robot_system(blueprint, 1, time_limit));
    int max_yield = -1;

    while (!all_systems.empty())
    {
        robot_system& current = all_systems.back();
        //std::cout << all_systems.size() << " | " << max_yield << "\n";
        //std::cout << current << "\n\n";

        if (current.is_finished())
        {
            //if (current.geode_yield() > max_yield)
            //    std::cout << current << "\n\n";
            max_yield = std::max(max_yield, current.geode_yield());
            all_systems.pop_back();
            continue;
        }

        if (current.optimistic_projection() < max_yield)
        {
            all_systems.pop_back();
            continue;
        }

        auto new_children = current.evolve_v2();
        all_systems.pop_back();

        for (const auto& child : new_children)
            all_systems.push_back(child);
    }

    return max_yield;
}

int part_1(const std::vector<ore_tuple<ore_tuple<int>>>& blueprints)
{
    int rval{0};
    for (auto it=blueprints.begin(); it<blueprints.end(); ++it)
        rval += (1+(it-blueprints.begin()))*best_yield(*it);
    return rval;
}

int part_2(const std::vector<ore_tuple<ore_tuple<int>>>& blueprints)
{
    int rval{1};
    for (auto it=blueprints.begin(); it<blueprints.begin()+3; ++it)
        rval *= best_yield(*it, 32);
    return rval;
}

int main()
{
    auto data = read_file("data/day_19.dat");

    std::cout << "Part 1: " << part_1(data) << "\n";
    std::cout << "Part 2: " << part_2(data) << "\n";

    return 0;
}