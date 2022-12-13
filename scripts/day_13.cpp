
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

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

class packet_composite
{
private:
    std::vector<packet_composite> _bits;
    int _val = -1;

    bool is_composite() const { return _val==-1; }
    int get_val() const { if (is_composite()){ throw("Called get_val of composite");} return _val; }
    size_t get_range() const { if (!is_composite()){ throw("Called get_range of non-composite");} return _bits.size(); }
    packet_composite get_packet(size_t i) const { if (!is_composite()){ throw("Called get_packet of non-composite");} return _bits.at(i); }

public:
    packet_composite() = default;
    packet_composite(int val): _val{val} {}
    ~packet_composite() = default;
    packet_composite(const packet_composite&) = default;

    void add_packet(packet_composite pack){ _bits.push_back(pack); }
    void show() const
    {
        if (!is_composite())
        {
            std::cout << get_val();
            return;
        }
        
        std::cout << "[";
        for (auto& bit : _bits)
        {
            bit.show();
            if (&bit != &_bits.back()) std::cout << ",";
        }
        std::cout << "]";
    }

    bool operator!=(const packet_composite& other) const
    {
        return !operator==(other);
    }

    bool operator==(const packet_composite& other) const
    {
        if (!is_composite() && !other.is_composite())
            return get_val() == other.get_val();
        if (is_composite() && !other.is_composite())
            return false;
        if (!is_composite() && other.is_composite())
            return false;
        // Both composite
        if (get_range() != other.get_range())
            return false;

        for (size_t i=0; i<get_range(); ++i)
            if (get_packet(i) != other.get_packet(i))
                return false;
        return true;
    }

    bool operator<(const packet_composite& other) const
    {
        if (!is_composite() && !other.is_composite())
            return get_val() < other.get_val();

        if (is_composite() && !other.is_composite())
        {
            packet_composite comp_other{};
            comp_other.add_packet(packet_composite{other.get_val()});
            return operator<(comp_other);
        }
        if (!is_composite() && other.is_composite())
        {
            packet_composite comp_self{};
            comp_self.add_packet(packet_composite{get_val()});
            return comp_self<other;
        }
        // Both composite
        for (size_t i=0; i<std::min(get_range(), other.get_range()); ++i)
        {
            if (get_packet(i) != other.get_packet(i))
                return get_packet(i) < other.get_packet(i);
        }
        // Must be the same
        return get_range() < other.get_range();
    }

    bool operator>(const packet_composite& other) const
    {
        if (operator==(other))
            return false;
        
        return !operator<(other);
    }

    bool operator>=(const packet_composite& other) const { return operator==(other) || operator>(other); }
    bool operator<=(const packet_composite& other) const { return operator==(other) || operator<(other); }
};

packet_composite packet_builder(std::stringstream& ss)
{
    packet_composite rval;
    char ch = (char)ss.peek();

    if (ch == '[')
    {
        ss >> ch;
        while (ss.peek() != (int)']')
            rval.add_packet(packet_builder(ss));
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
        rval = packet_composite(num);
    }

    if (ss.peek() == (int)',')
        ss >> ch;
    return rval;
}

packet_composite packet_builder(std::string& str){ std::stringstream ss{str}; return packet_builder(ss); }

int part_1(const std::vector<packet_composite>& packets)
{
    int count = 0;
    for (size_t i=0; i<packets.size(); i+=2)
    {
        if (packets[i]<packets[i+1])
            count += (i+2)/2;
    }
    return count;
}

int part_2(const std::vector<packet_composite>& packets)
{
    std::string str1 = "[[2]]";
    std::string str2 = "[[6]]";
    packet_composite pck1 = packet_builder(str1);
    packet_composite pck2 = packet_builder(str2);

    int count1 = 1;
    int count2 = 2;
    for (const auto& pack : packets)
    {
        if (pack<pck1)
            count1++;
        if (pack<pck2)
            count2++;
    }

    return count1*count2;
}

int main()
{
    auto data = read_file("data/day_13.dat");
    std::vector<packet_composite> packets;
    for (auto& row : data)
    {
        packets.push_back(packet_builder(row.first));
        packets.push_back(packet_builder(row.second));
    }

    std::cout << "Part 1: " << part_1(packets) << "\n";
    std::cout << "Part 2: " << part_2(packets) << "\n";

    return 0;
}