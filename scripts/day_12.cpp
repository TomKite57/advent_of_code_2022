
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <stack>
#include <algorithm>

using map = std::vector<std::vector<int>>;
using coord = std::pair<int, int>;

std::tuple<map, coord, coord> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    map rval;
    coord start, end;
    int row_count = 0;
    for (std::string str; std::getline(file, str);)
    {
        std::stringstream ss{str};
        char height;

        int col_count = 0;
        rval.push_back({});
        while (ss)
        {
            ss >> height;

            if (height == 'S')
            {
                start = coord({row_count, col_count});
                height = 'a';
            }
            else if (height == 'E')
            {
                end = coord({row_count, col_count});
                height = 'z';
            }

            rval.back().push_back((int)height - (int)'a');
            ++col_count;
        }
        ++row_count;
    }

    file.close();

    return {rval, start, end};
}

struct path
{
    coord _pos;
    int _len;

    path(coord c, int l): _pos{c}, _len{l} {};
    path(int x, int y, int l): _pos{x, y}, _len{l} {};
    ~path() = default;
    path(const path&) = default;
};

class maze_explorer
{
private:
    const map& _map;
    map _path_lengths;
    const coord& _start, _end;
    std::stack<path> _open_paths;

    bool in_range(coord pos)
    {
        if (pos.first < 0 || pos.first >= _map.size())
            return false;
        
        if (pos.second < 0 || pos.second >= _map[0].size())
            return false;
        
        return true;
    }

    void explore_path()
    {
        path current_path = _open_paths.top();
        coord pos = current_path._pos;
        int len = current_path._len;
        int current_height = _map[pos.first][pos.second];
        _open_paths.pop();

        coord north = {pos.first+1, pos.second};
        coord south = {pos.first-1, pos.second};
        coord east = {pos.first, pos.second+1};
        coord west = {pos.first, pos.second-1};

        std::vector<coord> to_consider = {north, south, east, west};
        auto it = to_consider.begin(); 
        while (it != to_consider.end())
        {
            if (!in_range(*it) || _path_lengths[it->first][it->second] <= len+1 || _map[it->first][it->second] > current_height+1)
                it = to_consider.erase(it);
            else
                ++it;
        }

        for (const auto& new_pos : to_consider)
        {
            _open_paths.push(path{new_pos, len+1});
            _path_lengths[new_pos.first][new_pos.second] = len+1;
        }
    }

    bool finished_explore()
    {
        return _open_paths.empty();
    }

public:
    maze_explorer(const map& map_in, const coord& start_in, const coord& end_in):
    _map{map_in}, _start{start_in}, _end{end_in}
    {
        _open_paths.push(path{_start, 0});
        _path_lengths = std::vector<std::vector<int>>(_map.size(), std::vector<int>(_map[0].size(), _map.size()*_map[0].size()));
        _path_lengths[_start.first][_start.second] = 0;
    };
    ~maze_explorer() = default;
    maze_explorer(const maze_explorer&) = delete;

    void run()
    {
        while (!finished_explore())
            explore_path();
    }

    int best_path(){ return _path_lengths[_end.first][_end.second]; }

};

int main()
{
    auto data = read_file("data/day_12.dat");
    map& _map = std::get<0>(data);
    coord& _start = std::get<1>(data);
    coord& _end = std::get<2>(data);
    maze_explorer explorer{_map, _start, _end};
    explorer.run();

    std::cout << "Part 1: " << explorer.best_path() << "\n";

    std::vector<coord> lowest_coords;
    std::vector<int> min_paths;
    for (int i=0; i<_map.size(); ++i)
        for (int j=0; j<_map[0].size(); ++j)
            if (_map[i][j] == 0)
                lowest_coords.push_back({i, j});
    
    for (const auto& c : lowest_coords)
    {
        maze_explorer test_explorer{_map, c, _end};
        test_explorer.run();
        min_paths.push_back(test_explorer.best_path());
    }
    std::cout << "Part 2: " << *std::min_element(min_paths.begin(), min_paths.end()) << "\n";

    return 0;
}