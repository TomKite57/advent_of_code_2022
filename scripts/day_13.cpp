
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


std::vector<std::pair<std::string, std::string>> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::vector<std::pair<std::string, std::string>> rval;
    for (std::string str1, str2; std::getline(file, str1);)
    {
        std::getline(file, str2);
        rval.push_back({str1, str2});
        // Waste empty line
        std::getline(file, str1);
    }

    file.close();

    return rval;
}

class packet_base;
class packet_bit;
class packet_composite;

class packet_base
{
public:
    packet_base() = default;
    ~packet_base() = default;
    packet_base(const packet_base&) = default;
    
    virtual void add_packet(packet_base*) = 0;
    virtual void show() = 0;
};

class packet_bit: public packet_base
{
private:
    int _bit;

public:
    packet_bit(int bit): _bit{bit} {}
    ~packet_bit() = default;
    packet_bit(const packet_bit&) = default;

    void add_packet(packet_base*) { throw("Can't cadd packet to single bit"); }
    void show() { std::cout << _bit; }
};

class packet_composite: public packet_base
{
private:
    std::vector<packet_base*> _bits;

public:
    packet_composite() = default;
    ~packet_composite(){ for (auto& bit : _bits) delete bit; };
    packet_composite(const packet_composite&) = default;

    void add_packet(packet_base* pack){ _bits.push_back(pack); }
    void show()
    {
        std::cout << "[";
        for (auto& bit : _bits)
        {
            bit->show();
            if (&bit != &_bits.back()) std::cout << ",";
        }
        std::cout << "]";
    }
};

packet_base* packet_builder(std::stringstream& ss)
{
    packet_base* rval;
    char ch = (char)ss.peek();

    if (ch == '[')
    {
        rval = new packet_composite();
        ss >> ch;
        while (ss.peek() != (int)']')
        {
            rval->add_packet(packet_builder(ss));
        }
        // Waste the ']'
        ss >> ch;
    }
    else
    {
        std::string substr;
        int num;
        while (ss.peek() != (int)',' && ss.peek() != (int)']')
            substr.push_back((char)ss.get());
        num = std::stoi(substr);
        rval = new packet_composite();
        rval->add_packet(new packet_bit(num));
    }

    if (ss.peek() == (int)',')
        ss >> ch;
    return rval;
}

packet_base* packet_builder(std::string& str){ std::stringstream ss{str}; return packet_builder(ss); }

int main()
{
    auto data = read_file("data/day_13.dat");
    std::vector<std::pair<packet_base*, packet_base*>> packets;
    for (auto& row : data)
    {
        packets.push_back( { packet_builder(row.first), packet_builder(row.second) } );
    }

    for (const auto& row : packets)
    {
        std::cout << "\n";
        row.first->show();
        std::cout << "\n";
        row.second->show();
        std::cout << "\n\n";
    }

    for (const auto& row : packets)
    {
        delete row.first;
        delete row.second;
    }

    return 0;
}