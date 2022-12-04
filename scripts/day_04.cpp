
#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

class section
{
public:
    int start, finish;

    section(int s=0, int e=0): start{s}, finish{e} {};
    ~section() = default;
    section(const section&) = default;
};

std::istream& operator >> (std::istream &in,  section &s)
{
    int a, b;
    char c;
    in >> a >> c >> b;
    s = section(a, b);
    return in;
}

class elf_pair
{
public:
    section elf_1, elf_2;

    elf_pair(): elf_1{}, elf_2{} {};
    elf_pair(section a, section b): elf_1{a}, elf_2{b} {};
    ~elf_pair() = default;
    elf_pair(const elf_pair&) = default;
};

std::istream& operator >> (std::istream &in,  elf_pair &p)
{
    section a, b;
    char c;
    in >> a >> c >> b;
    p = elf_pair(a, b);
    return in;
}

std::vector<elf_pair> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::vector<elf_pair> rval;
    while (!file.eof())
    {
        rval.push_back(elf_pair());
        file >> rval.back();
    }

    file.close();

    return rval;
}


int fully_contains(const elf_pair& p)
{
    section s1 = p.elf_1;
    section s2 = p.elf_2;

    if (s1.start <= s2.start && s1.finish >= s2.finish)
        return 1;
    
    if (s2.start <= s1.start && s2.finish >= s1.finish)
        return 1;
    
    return 0;
}

int any_overlap(const elf_pair& p)
{
    section s1 = p.elf_1;
    section s2 = p.elf_2;

    // Ensure ordering
    if (s1.start == s2.start)
        return 1;
    if (s1.start > s2.start)
        return any_overlap(elf_pair(s2, s1));

    if (s1.finish >= s2.start)
        return 1;
    
    return 0;
}

int part_1_checksum(std::vector<elf_pair> pairs)
{
    return std::accumulate(pairs.begin(), pairs.end(), 0,
                           [](int& score, const elf_pair& e){ return score + fully_contains(e); });
}

int part_2_checksum(std::vector<elf_pair> pairs)
{
    return std::accumulate(pairs.begin(), pairs.end(), 0,
                           [](int& score, const elf_pair& e){ return score + any_overlap(e); });
}

int main()
{
    auto data = read_file("data/day_04.dat");

    std::cout << "Fully overlapping pairs: " << part_1_checksum(data) << "\n";
    std::cout << "All overlapping pairs: " << part_2_checksum(data) << "\n";

    return 0;
}