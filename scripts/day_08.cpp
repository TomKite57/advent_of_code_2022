
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <set>

using grid = std::vector<std::vector<int>>;
using coord = std::pair<int, int>;

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

grid read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    grid rval;
    for (std::string line; std::getline(file, line);)
    {
        rval.push_back({});
        for (const char& c : line)
            rval.back().push_back(c - '0'); 
    }

    file.close();

    return rval;
}

void view_row_left(int row, const grid& trees, std::set<coord>& seen)
{
    int running_max = -1;
    for (size_t col=0; col<trees[row].size(); ++col)
    {
        if (trees[row][col] > running_max)
        {
            running_max = trees[row][col];
            seen.insert({row, col});
        }
    }
}

void view_row_right(int row, const grid& trees, std::set<coord>& seen)
{
    int running_max = -1;
    for (int col=(int)trees[row].size()-1; col>=0; --col)
    {
        if (trees[row][col] > running_max)
        {
            running_max = trees[row][col];
            seen.insert({row, col});
        }
    }
}

void view_col_top(int col, const grid& trees, std::set<coord>& seen)
{
    int running_max = -1;
    for (size_t row=0; row<trees.size(); ++row)
    {
        if (trees[row][col] > running_max)
        {
            running_max = trees[row][col];
            seen.insert({row, col});
        }
    }
}

void view_col_bot(int col, const grid& trees, std::set<coord>& seen)
{
    int running_max = -1;
    for (int row=(int)trees.size()-1; row>=0; --row)
    {
        if (trees[row][col] > running_max)
        {
            running_max = trees[row][col];
            seen.insert({row, col});
        }
    }
}

int count_visible(const grid& trees)
{
    std::set<coord> seen;
    for (size_t i=0; i<trees.size(); ++i)
    {
        view_row_left(i, trees, seen);
        view_row_right(i, trees, seen);
    }
    for (size_t i=0; i<trees[0].size(); ++i)
    {
        view_col_top(i, trees, seen);
        view_col_bot(i, trees, seen);
    }

    return seen.size();
}

int scenic_score(int row, int col, const grid& trees)
{
    int height = trees[row][col];
    int u, d, l, r;
    u = d = l = r = 0;
    // Look right
    for (int i=row+1; i<(int)trees[0].size(); ++i)
    {   
        r++;
        if (trees[i][col] >= height)
            break;
    }
    // Look left
    for (int i=row-1; i>=0; --i)
    {   
        l++;
        if (trees[i][col] >= height)
            break;
    }
    // Look up
    for (int i=col-1; i>=0; --i)
    {   
        u++;
        if (trees[row][i] >= height)
            break;
    }
    // Look down
    for (int i=col+1; i<(int)trees.size(); ++i)
    {   
        d++;
        if (trees[row][i] >= height)
            break;
    }

    return u*d*l*r;
}

int find_best_scenic_score(const grid& trees)
{
    int running_max = -1;
    for (int i=0; i<(int)trees.size(); ++i)
        for (int j=0; j<(int)trees[0].size(); ++j)
            running_max = std::max(running_max, scenic_score(i, j, trees));
    
    return running_max;
}

int main()
{
    auto data = read_file("data/day_08.dat");

    std::cout << "Visible trees: " << count_visible(data) << "\n";
    std::cout << "Best scenic score: " << find_best_scenic_score(data) << "\n";

    return 0;
}