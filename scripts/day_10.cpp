
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>

enum event_tag {addx, check_signal};

std::vector<std::vector<std::string>> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::vector<std::vector<std::string>> rval;
    for (std::string str, substr; std::getline(file, str);)
    {
        std::stringstream ss{str};
        rval.push_back({});
        while (ss)
        {
            ss >> substr;
            rval.back().push_back(substr);
        }
    }

    file.close();

    return rval;
}

class computer
{
    using event = std::tuple<event_tag, int, int>;

public:
    computer() = default;
    ~computer() = default;
    computer(const computer&) = delete;

    void register_event(event ev)
    {
        event_list.push_back(ev);
    }

    void increment_cycle(int inc=1)
    {
        for (int i=0; i<inc; ++i)
        {
            ++cycle;
            for (auto& ev : event_list)
                --std::get<2>(ev);
            event_check();
        }
    }

    int get_signal_strength() const { return signal_strength; }
    int get_reg() const { return reg_x; }

private:
    int reg_x = 1;
    int cycle = 0;
    int signal_strength = 0;
    std::vector<event> event_list;

    void event_check()
    {
        auto it = event_list.begin();
        while (it!=event_list.end())
        {
            if (std::get<2>(*it)!=0)
            {
                ++it;
                continue;
            }

            if (std::get<0>(*it) == addx)
                reg_x += std::get<1>(*it);
            
            else if (std::get<0>(*it) == check_signal)
                signal_strength += cycle*reg_x;
            
            it = event_list.erase(it);
        }
    }
};

class screen
{
public:
    screen() = default;
    ~screen() = default;
    screen(const screen&) = delete;

    void draw(int cycle, int pos)
    {
        auto [i, j] = std::div(cycle, 40);
        
        if (abs(pos-j) <= 1)
            panel[i][j] = '#';
    }

    void show()
    {
        for (const auto& row : panel)
        {
            for (const auto& px : row)
                std::cout << px;
            std::cout << "\n";
        }
    }

private:
    std::vector<std::vector<char>> panel{6, std::vector<char>(40, '.')};
};

int part_1(std::vector<std::vector<std::string>> data)
{
    computer device{};
    for (int x : {20, 60, 100, 140, 180, 220})
        device.register_event({check_signal, -1, x});

    int counter = 0; 
    for (const auto& instruc : data)
    {
        if (instruc[0] == "addx")
        {
            device.register_event({addx, std::stoi(instruc[1]), counter+2});
            counter += 2;
            continue;
        }

        if (instruc[0] == "noop")
        {
            counter += 1;
            continue;
        }
    }

    for (int i=0; i<240; ++i)
        device.increment_cycle();

    return device.get_signal_strength();
}

void part_2(std::vector<std::vector<std::string>> data)
{
    computer device{};
    screen display{};
    int counter = 0; 
    for (const auto& instruc : data)
    {
        if (instruc[0] == "addx")
        {
            device.register_event({addx, std::stoi(instruc[1]), counter+2});
            counter += 2;
            continue;
        }

        if (instruc[0] == "noop")
        {
            counter += 1;
            continue;
        }
    }

    for (int i=0; i<240; ++i)
    {
        display.draw(i, device.get_reg());
        device.increment_cycle();
    }

    display.show();
}

int main()
{
    auto data = read_file("data/day_10.dat");

    std::cout << "Signal strength: " << part_1(data) << "\n";
    part_2(data);

    return 0;
}