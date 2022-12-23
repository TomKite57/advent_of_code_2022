
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

bool is_int(const std::string& str)
{
    if (str.empty()) return false;

    for (char c : str)
    {
        if (!std::isdigit(c)) return false;
    }

    return true;
}

std::tuple<std::string, char, std::string> parse_line(const std::string& str)
{
    std::string s1, s2;
    char ch;
    std::stringstream ss{str};
    ss >> s1 >> ch >> s2;
    return {s1, ch, s2};
}

std::vector<std::pair<std::string, std::string>> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<std::pair<std::string, std::string>> data;
    while (!file.eof())
    {
        std::string str1, str2;
        char ch;
        std::getline(file, str1, ':');
        file >> std::ws;
        std::getline(file, str2);
        data.push_back({str1, str2});
    }

    return data;
}

long long int eval(const long long int& n1, char sym, const long long int& n2)
{
    if (sym == '+')
        return n1+n2;
    if (sym == '-')
        return n1-n2;
    if (sym == '*')
        return n1*n2;
    if (sym == '/')
        return n1/n2;
    
    throw("eval: Did not understand command.");
}

template<typename T, typename V>
bool is_in(const T& key, const std::map<T, V>& map)
{
    return map.find(key) != map.end();
}

class monkey_system
{
private:
    const std::vector<std::pair<std::string, std::string>>& instruc;
    std::map<std::string, long long int> registers;
    std::vector<int> indices_to_go;

    bool complete() const { return indices_to_go.empty(); }

    void build_indices_to_go()
    {
        for (int i=0; i<(int)instruc.size(); ++i)
            indices_to_go.push_back(i);
    }

    void initial_pass()
    {
        auto it=indices_to_go.begin();
        while (it!=indices_to_go.end())
        {
            const std::pair<std::string, std::string>& current_instruc = instruc.at(*it);
            if (is_int(current_instruc.second))
            {
                registers.insert({current_instruc.first, std::stoi(current_instruc.second)});
                it = indices_to_go.erase(it);
                continue;
            }
            ++it;
        }
    }

    void next_pass()
    {
        auto it=indices_to_go.begin();
        while (it!=indices_to_go.end())
        {
            const std::pair<std::string, std::string>& current_instruc = instruc.at(*it);
            std::string monke = current_instruc.first;
            auto parsed_instruc = parse_line(current_instruc.second);

            if (!is_in(std::get<0>(parsed_instruc), registers) || !is_in(std::get<2>(parsed_instruc), registers))
            {
                ++it;
                continue;
            }

            const long long int& num1 = registers.at(std::get<0>(parsed_instruc));
            const long long int& num2 = registers.at(std::get<2>(parsed_instruc));
            long long int new_num = eval(num1, std::get<1>(parsed_instruc), num2);
            registers.insert({monke, new_num});
            it = indices_to_go.erase(it);
        }
    }

public:
    monkey_system(const std::vector<std::pair<std::string, std::string>>& data)
    : instruc{data}
    {
        build_indices_to_go();
        initial_pass();
        while (!complete())
            next_pass();
    }
    ~monkey_system() = default;
    monkey_system(const monkey_system&) = default;

    long long int checksum() const { return registers.at("root"); }
    long long int operator[](const std::string& str) const { return registers.at(str); }
};

//std::string string_reduction(const std::vector<std::pair<std::string, std::string>>& data)
//{
//    
//}

int main()
{
    auto data = read_file("data/day_21.dat");

    monkey_system my_system(data);
    std::cout << "Part 1: " << my_system.checksum() << "\n";

    std::vector<std::pair<std::string, std::string>> data_copy;
    int humn_ind;
    std::string check_a, check_b;
    for (int i=0; i<(int)data.size(); ++i)
    {
        data_copy.push_back(data[i]);
        if (data[i].first=="humn")
            humn_ind = i;
        if (data[i].first=="root")
        {
            auto parsed = parse_line(data[i].second);
            check_a = std::get<0>(parsed);
            check_b = std::get<2>(parsed);
        }
    }

    long long int trial = (2000000000);
    long long int dx = trial/10;
    while (true)
    {
        data_copy[humn_ind] = {"humn", std::to_string(trial)};
        monkey_system sys_a(data_copy);
        data_copy[humn_ind] = {"humn", std::to_string(trial+dx)};
        monkey_system sys_b(data_copy);
        long long int f_a, f_b;
        f_a = sys_a[check_a] - sys_a[check_b];
        f_b = sys_b[check_a] - sys_b[check_b];
        long long int deriv = (f_a-f_b)/dx;

        std::cout << trial << " : " << sys_a[check_a] << " | " << sys_a[check_b] << "\n";

        trial -= f_a/deriv;
        std::cout << f_a/deriv << "\n";
    }
    std::cout << "Part 2: " << trial << "\n";

    return 0;
}
