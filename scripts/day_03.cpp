
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include <numeric>
#include <algorithm>

using Bag = std::pair<std::set<char>, std::set<char>>;

std::vector<std::string> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::vector<std::string> contents;

    for (std::string line; std::getline(file, line);)
        contents.push_back(line);

    file.close();

    return contents;
}

int char_score(const char &character)
{
    if (isupper(character))
        return (int)character - 38;

    return (int)character - 2 * 48;
}

Bag split_compartments(const std::string& line)
{
    Bag bag;
    for (size_t i=0; i<line.size()/2; ++i)
    {
        bag.first.insert(line[i]);
    }

    for (size_t i=line.size()/2; i<line.size(); ++i)
        bag.second.insert(line[i]);
    
    return bag;
}


std::vector<Bag> split_compartments(const std::vector<std::string>& all_bags)
{
    // TODO use algorithm?
    std::vector<Bag> rval;
    for (const auto& bag : all_bags)
    {
        rval.push_back(split_compartments(bag));
    }
    
    return rval;
}

std::set<char> find_intersection(const std::set<char>& s1, const std::set<char>& s2)
{
    std::set<char> intersec;
    std::set_intersection(s1.begin(), s1.end(),
                          s2.begin(), s2.end(),
                          std::inserter(intersec, intersec.begin()));
    return intersec;
}

std::set<char> find_union(const std::set<char>& s1, const std::set<char>& s2)
{
    std::set<char> united;
    std::set_union(s1.begin(), s1.end(),
                   s2.begin(), s2.end(),
                   std::inserter(united, united.begin()));
    return united;
}

std::set<char> find_intersection(const Bag& bag)
{
    return find_intersection(bag.first, bag.second);
}

template<typename T>
T get_elem(const std::set<T>& set)
{
    if (set.size()==0)
        throw("Set is empty!");
    
    for (auto elem : set)
        return elem;
}

int part_1_checksum(const std::vector<Bag>& bags)
{
    return std::accumulate(bags.begin(), bags.end(), 0, [](const int& s, const Bag& b){ return s + char_score(get_elem(find_intersection(b))); });
}

int part_2_checksum(const std::vector<Bag>& bags, size_t group_size=3)
{
    int score = 0;
    for (size_t i=0; i<bags.size(); i+=group_size)
    {
        std::set<char> potential_chars = find_union(bags[i].first, bags[i].second);
        for (size_t j=1; j<group_size; ++j)
        {
            potential_chars = find_intersection(potential_chars, find_union(bags[i+j].first, bags[i+j].second));
        }
        score += char_score(get_elem(potential_chars));
    }
    return score;
}


int main()
{
    auto data = read_file("data/day_03.dat");
    auto bags = split_compartments(data);

    std::cout << "Part 1 checksum: " << part_1_checksum(bags) << "\n";
    std::cout << "Part 2 checksum: " << part_2_checksum(bags) << "\n";

    return 0;
}