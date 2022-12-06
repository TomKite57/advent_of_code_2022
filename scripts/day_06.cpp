
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>


std::string read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::string rval;
    std::getline(file, rval);

    file.close();

    return rval;
}

std::string slice(const std::string& str, int start, int width)
{
    if (start+width > str.size())
        throw("Slice would exceed string length!");
    
    std::string rval;
    std::for_each(str.begin()+start, str.begin()+start+width, [&rval](const char& c){rval.push_back(c);});
    return rval;
}

int find_repeat(std::string str)
{
    std::set<char> letters(str.begin(), str.end());
    return letters.size() != str.size();
}

int find_start(std::string str, size_t width)
{
    for (size_t i=0; i<str.size()-width; ++i)
        if (!find_repeat(slice(str, i, width)))
            return i+width;
    
    throw("Didn't find a repeat!");
    return -1;
}

int main()
{
    auto data = read_file("data/day_06.dat");

    std::cout << "Start of packet: " << find_start(data, 4) << "\n";
    
    std::cout << "Start of message: " << find_start(data, 14) << "\n";

    return 0;
}