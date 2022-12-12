
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <utility>

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

class maze_explorer
{
private:
    map _map;
    map _path_lengths;
    coord _start, _end;
    std::vector<coord> _open_paths;

public:
    maze_explorer(map map_in, coord start_in, coord end_in):
    _map{map_in}, _start{start_in}, _end{end_in}
    {
        _open_paths.push_back(_start);
        _path_lengths = std::vector<std::vector<int>>(_map.size(), std::vector<int>(_map[0].size(), -1));
        _path_lengths[_start.first][_start.second] = 0;
    };
    ~maze_explorer() = default;
    maze_explorer(const maze_explorer&) = delete;
};

int main()
{
    auto data = read_file("data/day_12.dat");
    maze_explorer explorer{std::get<0>(data), std::get<1>(data), std::get<2>(data)};

    for (const auto& row : std::get<0>(data))
    {
        for (const auto& val : row)
            std::cout << val << ", ";
        std::cout << "\n";
    }

    return 0;
}