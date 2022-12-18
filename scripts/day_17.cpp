
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <utility>
#include <unordered_set>

class coord
{
public:
    long long int first, second;

    coord(const long long int& xx, const long long int& yy): first{xx}, second{yy} {};
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
    std::size_t operator()(const coord& c) const { return c.first ^ c.second; }
};

using coord_container = std::unordered_set<coord, coord_hash>;

std::ostream& operator<<(std::ostream& os, const coord& c){ os << "(" << c.first << "," << c.second << ")"; return os; }
const coord north = {0, +1};
const coord south = {0, -1};
const coord east = {1, 0};
const coord west = {-1, 0};

const coord_container bar_shape = { {0, 0}, {1, 0}, {2, 0}, {3, 0} };
const coord_container plus_shape = { {1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2} };
const coord_container L_shape = { {0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2} };
const coord_container tall_shape = { {0, 0}, {0, 1}, {0, 2}, {0, 3} };
const coord_container square_shape = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };

bool is_in(const coord& key, const coord_container& set) { return set.find(key) != set.end(); }

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
    friend std::ostream& operator<<(std::ostream&, const rock_formation&);

private:
    coord_container formation;
    long long int xmin, xmax, ymin, ymax;

public:
    rock_formation(const coord_container& form_in, const coord& offset={0,0})
    : formation{form_in}
    {
        if (empty())
        {
            xmin = xmax = 0;
            ymin = ymax = 0;
        }
        else
        {
            xmin = xmax = form_in.begin()->first;
            ymin = ymax = form_in.begin()->second;
        }

        for (const auto& c : formation)
        {
            xmin = std::min(xmin, c.first);
            xmax = std::max(xmax, c.first);
            ymin = std::min(ymin, c.second);
            ymax = std::max(ymax, c.second);
        }

        if (offset==coord({0,0}))
            return;
        move(offset);
    }
    ~rock_formation() = default;
    rock_formation(const rock_formation&) = default;

    const coord_container& get_formation() const { return formation; }

    void move(const coord& direc)
    {
        coord_container new_formation{}; 
        for (auto& c : formation)
            new_formation.insert(c + direc);
        formation = new_formation;
        xmin+=direc.first;
        xmax+=direc.first;
        ymin+=direc.second;
        ymax+=direc.second;
    }

    bool detect_collision(const rock_formation& other) const
    {
        // Make this instance is is smaller
        if (formation.size() > other.formation.size())
            return other.detect_collision(*this);

        for (const auto& c1 : formation)
        {
            if (is_in(c1, other.formation))
                return true;
        }
        return false;
    }

    bool empty() const { return formation.empty(); }

    long long int get_xmin() const { return xmin; }
    long long int get_xmax() const { return xmax; }
    long long int get_ymin() const { return ymin; }
    long long int get_ymax() const { return ymax; }

    void add_rocks(const coord& r)
    {
        if (empty())
        {
            xmin=r.first;
            xmax=r.first;
            ymin=r.second;
            ymax=r.second;
        }
        else
        {            
            xmin = std::min(xmin, r.first);
            xmax = std::max(xmax, r.first);
            ymin = std::min(ymin, r.second);
            ymax = std::max(ymax, r.second);
        }
        formation.insert(r);
    }
    void add_rocks(const rock_formation& other){ for (const auto& c : other.formation) add_rocks(c); }
};

rock_formation rock_builder(int index, const coord& offset={0,0})
{
    // bar shape
    if (index==0)
        return rock_formation(bar_shape, offset);
    // plus shape
    if (index==1)
        return rock_formation(plus_shape, offset);
    // L shape
    if (index==2)
        return rock_formation(L_shape, offset);
    // tall shape
    if (index==3)
        return rock_formation(tall_shape, offset);
    // square shape
    if (index==4)
        return rock_formation(square_shape, offset);
    
    throw("Index out of range!");
}

std::ostream& operator<<(std::ostream& os, const rock_formation& rock)
{
    long long int xmin = rock.get_xmin();
    long long int xmax = rock.get_xmax();
    long long int ymin = rock.get_ymin();
    long long int ymax = rock.get_ymax();

    for (long long int y=ymax; y>=ymin; --y)
    {
        for (long long int x=xmin; x<=xmax; ++x)
        {
            if (is_in( coord{x, y}, rock.get_formation()))
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
    //coord_container rocks{};
    rock_formation rocks{{}};
    std::string jets;
    int jet_ind{-1};
    long long int rocks_added{0};

    int floor{-1};
    int lwall{0};
    int rwall{8};

    coord get_next_origin() const {if (rocks.empty()) return {3, floor+4}; return {3, rocks.get_ymax()+4};}

    rock_formation get_next_rock() const { return rock_builder(rocks_added%5, get_next_origin()); }
    coord get_LR_direction()
    {
        jet_ind++;
        jet_ind = jet_ind%jets.size();
        //std::cout << jet_ind << " / " << jets.size() << "\n";
        
        if (jets[jet_ind] == '<')
        {
            return west;
        }
        return east;
    }

public:
    cave_map(const std::string& jets_in)
    : jets{jets_in} {}
    ~cave_map() = default;
    cave_map(const cave_map&) = delete;

    const rock_formation& get_rock_formation() const { return rocks; }

    bool new_floor() const
    {
        for (int x=lwall+1; x<rwall; ++x)
            if (!is_in(coord({x, rocks.get_ymax()}), rocks.get_formation()))
                return false;
        return true;
    }

    bool new_state_found() const
    {
        return (rocks_added%5==0 && jet_ind==jets.size() && new_floor());
    }

    int get_next_jet_ind() const { return jet_ind+1; }

    long long int total_rocks() const { return rocks_added; }

    void add_rock()
    {
        rock_formation current_rock = get_next_rock();
        
        while (true)
        {    
            // LR movement
            coord direc = get_LR_direction();
            current_rock.move(direc);

            if (current_rock.detect_collision(rocks))
            {   
                current_rock.move(coord({0,0})-direc);
                //break;
            }

            if (current_rock.get_xmax() == rwall)
                current_rock.move(west);
            if (current_rock.get_xmin() == lwall)
                current_rock.move(east);
            
            // Down movement
            current_rock.move(south);
            if (current_rock.detect_collision(rocks) || current_rock.get_ymin() == floor)
            {
                current_rock.move(north);
                break;
            }
        }
        
        rocks.add_rocks(current_rock);
        ++rocks_added;
    }

};

std::ostream& operator<<(std::ostream& os, const cave_map& cave)
{
    long long int ymax = (cave.rocks.empty()) ? cave.floor+3 : cave.rocks.get_ymax()+3;
    for (long long int y=ymax; y>ymax-8; --y)
    {
        os << "|";
        for (long long int x=cave.lwall+1; x<=cave.rwall-1; ++x)
        {
            if (is_in( coord{x, y}, cave.rocks.get_formation()))
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

std::vector<long long int> find_pattern(const std::vector<long long int> answers)
{
    for (long long int period=1; period<answers.size()/2; ++period)
    {
        bool correct = true;
        for (long long int j=0; j<period; ++j)
        {
            if (answers[answers.size()-1-j-period] !=  answers[answers.size()-1-j])
            {
                correct = false;
                break;
            }
        }
        if (correct)
        {
            std::vector<long long int> seq;
            for (long long int i=0; i<period; ++i)
                seq.push_back(answers[answers.size()-1-i]);
            return seq;
        }
    }

    return {-1};
}

int main()
{
    auto data = read_file("data/day_17.dat");
    cave_map cave{data};

    for (int i=0; i<2022; ++i)
        cave.add_rock();
    std::cout << "Part 1: " << cave.get_rock_formation().get_ymax()+1 << "\n";

    int memory = -1;
    for (int i=2022; i<1'000'000; ++i)
    {
        cave.add_rock();
        if (cave.new_floor())
        {
            std::cout << cave.get_rock_formation().get_ymax()+1 - memory << "\n";
            memory = cave.get_rock_formation().get_ymax()+1;
            std::cout << i << " | " << cave.total_rocks()%5 << " | " << cave.get_next_jet_ind()%data.size() << "\n";
        }
    }

    return 0;

    std::vector<long long int> height_record;
    std::vector<long long int> increments;
    long long int min_period = 5*data.size();

    std::vector<long long int> seq;
    while (true)
    {
        cave.add_rock();
        if (cave.total_rocks()%min_period == 0)
        {
            height_record.push_back(cave.get_rock_formation().get_ymax()+1);
            if (height_record.size()>1)
                increments.push_back(height_record[height_record.size()-1] - height_record[height_record.size()-2]);
        }
        
        if (cave.total_rocks()%(10*min_period) == 0)
        {
            seq = find_pattern(increments);
            if (seq.size() != 1)
                break;
            seq.clear();
        }
    }

    long long int total_cycles = (1'000'000'000'000 - cave.total_rocks())/(min_period*seq.size());
    long long int remainder = (1'000'000'000'000 - cave.total_rocks())%(min_period*seq.size());
    long long int total_inc = std::accumulate(seq.begin(), seq.end(), 0);
    for (long long int i=0; i<remainder; ++i)
        cave.add_rock();
    
    std::cout << "Part 2: " << "eval(\"" << cave.get_rock_formation().get_ymax()+1 << "+" << total_inc << "*" << total_cycles << "\")" << "\n";
    std::cout << "Part 2: " << cave.get_rock_formation().get_ymax()+1 + total_inc*total_cycles << "\n";
    std::cout << "Part 2: " << "1514285714288" << "\n";

    //for (int i=2022; i<1000000000000; ++i)
    //{
    //    cave.add_rock();
    //    //std::cout << cave << "\n\n";
    //}

}