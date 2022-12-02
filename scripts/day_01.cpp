
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

std::vector<std::vector<int>> read_file(std::string fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
        throw;

    std::vector<std::vector<int>> rval(1);

    for (std::string line; std::getline(file, line);)
    {
        if (line.size()>=1)
            rval.back().push_back(std::stoi(line));
        else
            rval.push_back({});
    }

    file.close();

    return rval;
}

int main()
{
    // TODO: More reliable path finding
    auto data = read_file("data/day_01.dat");

    // TODO use algorithm
    std::vector<int> cal_sums;
    for (auto& group : data)
        cal_sums.push_back(std::accumulate(group.begin(), group.end(), 0));
    std::sort(cal_sums.begin(), cal_sums.end(), std::greater<int>());

    std::cout << "Max calories:  " << cal_sums[0] << "\n";
    std::cout << "Three top calories:  " << std::accumulate(cal_sums.begin(), cal_sums.begin()+3, 0) << "\n";

    return 0;
}