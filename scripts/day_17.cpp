
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <set>

using coord = std::pair<int, int>;
coord operator+(const coord& c1, const coord& c2){ return {c1.first + c2.first, c1.second + c2.second}; }
coord operator-(const coord& c1, const coord& c2){ return {c1.first - c2.first, c1.second - c2.second}; }
bool operator<(const coord& c1, const coord& c2){ return c1.second < c2.second; }
const coord north = {0, -1};
const coord south = {0, +1};
const coord east = {1, 0};
const coord west = {-1, 0};

const std::set<coord> bar_shape = { {0, 0}, {1, 0}, {2, 0}, {3, 0} };
const std::set<coord> plus_shape = { {0, 1}, {1, 1}, {2, 1}, {1, 0}, {1, 2} };
const std::set<coord> L_shape = { {0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2} };
const std::set<coord> tall_shape = { {0, 0}, {0, 1}, {0, 2}, {0, 3} };
const std::set<coord> square_shape = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };

template<typename T>
bool is_in(const T& key, const std::set<T>& set) { return set.find(key) != set.end(); }

std::string read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::string rval;
    std::getline(file, rval);

    file.close();

    return rval;
}

class rock_formation
{
    friend rock_formation rock_builder(int, const coord&);
    friend std::ostream& operator<<(std::ostream&, const rock_formation&);

private:
    std::set<coord> formation;
    coord origin;

public:
    rock_formation(const std::set<coord>& form_in, const coord& origin_in)
    : formation{form_in}, origin{origin_in} {}
    ~rock_formation() = default;
    rock_formation(const rock_formation&) = default;

    std::set<coord> get_formation(const coord& custom_origin) const
    {
        std::set<coord> rval;
        for (const auto& c : formation)
            rval.insert(c+custom_origin);
        return rval;
    }
    std::set<coord> get_formation() const { return get_formation({0,0}); }
    std::set<coord> get_coords() const { return get_formation(origin); }

    void move(const coord& direc) { origin = origin + direc; }

    bool will_collide(const rock_formation& other, const coord& direc) const
    {
        for (const auto& c1 : formation)
            for (const auto& c2 : other.get_formation())
                if (c1 + origin == c2 + other.get_origin())
                    return true;
        return false;
    }

    bool empty() const { return formation.empty(); }
    coord get_origin() const { return origin; }
    int get_ymin() const { if (empty()) return 0; return (formation.begin())->second + origin.second; }
    int get_ymax() const { if (empty()) return 0; return (--formation.end())->second + origin.second; }
    int get_xmin() const { if (empty()) return 0; int xmin=formation.begin()->first+origin.first; for (const auto& r : formation) xmin=std::min(xmin, r.first+origin.first); return xmin; }
    int get_xmax() const { if (empty()) return 0; int xmax=formation.begin()->first+origin.first; for (const auto& r : formation) xmax=std::max(xmax, r.first+origin.first); return xmax; }

    void add_rocks(const coord& rock){ formation.insert(rock-origin); }
    void add_rocks(const rock_formation& rocks){ for (const auto& c : rocks.get_coords()) add_rocks(c); }
};

rock_formation rock_builder(int index, const coord& origin)
{
    // bar shape
    if (index==0)
        return rock_formation(bar_shape, origin);
    // plus shape
    if (index==1)
        return rock_formation(plus_shape, origin);
    // L shape
    if (index==2)
        return rock_formation(L_shape, origin);
    // tall shape
    if (index==3)
        return rock_formation(tall_shape, origin);
    // square shape
    if (index==4)
        return rock_formation(square_shape, origin);
    
    throw("Index out of range!");
}

std::ostream& operator<<(std::ostream& os, const rock_formation& rock)
{
    int ymin = rock.get_ymin();
    int ymax = rock.get_ymax();

    for (int y=ymax; y>=ymin; --y)
    {
        for (int x=0; x<=3; ++x)
        {
            if (is_in( coord{x, y}, rock.get_formation({0, 0})))
                os << '#';
            else
                os << '.';
        }
        os << "\n";
    }
    return os;
}

class cave_map
{
    friend std::ostream& operator<<(std::ostream&, const cave_map&);

private:
    //std::set<coord> rocks{};
    rock_formation rocks{{}, {0,0}};
    std::string jets;
    int jet_ind{0};
    int rocks_added{0};
    // Floor is y=0... All coords will be negative... *Sigh*
    int ymin{0};
    // Not inclusive. e.g. walls at -1 and 8
    int xmin{0};
    int xmax{7};

    coord get_next_origin() const {return {3, rocks.get_ymin()-4};}

    rock_formation get_next_rock() const { return rock_builder(rocks_added%5, get_next_origin()); }
    coord get_LR_direction()
    {
        jet_ind = jet_ind%jets.size();
        
        if (jets[jet_ind++] == '<')
            return west;
        return east;
    }

public:
    cave_map(const std::string& jets_in)
    : jets{jets_in} {}
    ~cave_map() = default;
    cave_map(const cave_map&) = delete;

    void add_rock()
    {
        std::cout << get_next_origin().first << ", " << get_next_origin().second << "\n";
        rock_formation current_rock = get_next_rock();
        std::cout << current_rock.get_origin().first << ", " << current_rock.get_origin().first << "\n\n";
        
        while (true)
        {
            // LR movement
            coord direc = get_LR_direction();

            if (current_rock.will_collide(rocks, direc))
                break;
            
            std::cout << current_rock.get_origin().first << ", " << current_rock.get_origin().first << "\n";
            current_rock.move(direc);
            std::cout << current_rock.get_origin().first << ", " << current_rock.get_origin().first << "\n\n";
            if (current_rock.get_xmax() > xmax)
                current_rock.move(west);
            if (current_rock.get_xmin() < xmin)
                current_rock.move(east);
            
            // Down movement
            if (current_rock.will_collide(rocks, south))
                break;
            current_rock.move(south);
            if (current_rock.get_ymin() > ymin)
            {
                current_rock.move(north);
                break;
            }
        }
        
        std::cout << rocks.get_ymax() << "\n";
        std::cout << rocks.get_ymin() << "\n";
        rocks.add_rocks(current_rock);
        ++rocks_added;
    }

};

std::ostream& operator<<(std::ostream& os, const cave_map& cave)
{
    for (int y=cave.rocks.get_ymin(); y>=0; --y)
    {
        os << "|";
        for (int x=1; x<=7; ++x)
        {
            if (is_in( coord{x, y}, cave.rocks.get_coords()))
                os << '#';
            else
                os << '.';
        }
        os << "|";
        os << "\n";
    }
    os << "+-------+";

    return os;
}


int main()
{
    auto data = read_file("data/day_17.dat");
    cave_map cave{data};
    std::cout << cave << "\n\n";
    cave.add_rock();
    std::cout << cave << "\n\n";
    cave.add_rock();
    std::cout << cave << "\n\n";
    cave.add_rock();
    std::cout << cave << "\n\n";
    cave.add_rock();
    std::cout << cave << "\n\n";

    //std::cout << data << "\n";

    //std::cout << rock_builder(0, {0,0}) << "\n";
    //std::cout << rock_builder(1, {0,0}) << "\n";
    //std::cout << rock_builder(2, {0,0}) << "\n";
    //std::cout << rock_builder(3, {0,0}) << "\n";
    //std::cout << rock_builder(4, {0,0}) << "\n";
    
    return 0;
}