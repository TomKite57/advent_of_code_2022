
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <set>

enum direction { U, D, L, R };
const std::map<char, direction> direction_map = {{'U', U}, {'D', D}, {'L', L}, {'R', R}};
using coord = std::pair<int, int>;

std::vector<std::pair<direction, int>> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::vector<std::pair<direction, int>> rval;
    for (std::string line; std::getline(file, line);)
    {
        std::stringstream ss{line};
        char ch; int len;
        ss >> ch >> len;
        rval.push_back({direction_map.at(ch), len});
    }

    file.close();

    return rval;
}

class knot
{
public:
    knot() = default;
    ~knot() = default;
    knot(const knot&) = delete;

    void set_following(knot& to_follow){ following = &to_follow; following->set_follower(*this); }
    int get_num_visited() const { return visited.size(); }
    void explicit_move(direction dir, int len)
    {
        coord dx_dy = get_dx_dy(dir);
        int& dx = dx_dy.first;
        int& dy = dx_dy.second;
        
        for (size_t i=0; i<len; ++i)
        {
            pos.first += dx;
            pos.second += dy;

            add_visited();
            update_follower();
        }
    }

private:
    coord pos = {0, 0};
    std::set<coord> visited = {{0,0}};
    knot* following = nullptr;
    knot* follower = nullptr;

    void set_follower(knot& new_follower){ follower = &new_follower; }

    void follow_move()
    {
        if (!following)
            throw("This knot is not following another knot!");
        
        if (abs(pos.first - following->pos.first) <= 1 && abs(pos.second - following->pos.second) <= 1)
            return;
                
        if (pos.first == following->pos.first)
            pos.second += (pos.second < following->pos.second) ? 1 : -1;
        
        else if (pos.second == following->pos.second)
            pos.first += (pos.first < following->pos.first) ? 1 : -1;

        else
        {
            int dx, dy;
            dx = (pos.first < following->pos.first) ? 1 : -1;
            dy = (pos.second < following->pos.second) ? 1 : -1;
            pos.first += dx;
            pos.second += dy;
        }

        add_visited();
        update_follower();
    }

    void add_visited()
    {
        visited.insert(pos);
    }

    void update_follower()
    {
        if (!follower)
            return;
        
        follower->follow_move();
    }

    static coord get_dx_dy(direction dir)
    {
        if (dir == U)
            return {0, 1};
        if (dir == D)
            return {0, -1};
        if (dir == L)
            return {-1, 0};
        if (dir == R)
            return {1, 0};
    }
};

int evolve_rope(const std::vector<std::pair<direction, int>>& movements, int rope_len)
{
    if (rope_len < 1)
        throw("Rope must be at least one long!");

    std::vector<knot> rope(rope_len);
    knot& head = rope[0];
    for (auto it=rope.begin()+1; it<rope.end(); ++it)
        it->set_following(*(it-1));

    for (const auto& move : movements)
        head.explicit_move(move.first, move.second);
    
    return rope.back().get_num_visited();
}

int main()
{
    auto data = read_file("data/day_09.dat");

    std::cout << "Tail visited " << evolve_rope(data, 2) << " spaces\n";
    std::cout << "Tail visited " << evolve_rope(data, 10) << " spaces\n";

    return 0;
}