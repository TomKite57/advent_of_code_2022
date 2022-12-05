
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stack>
#include <tuple>
#include <string>
#include <utility>

using Crates = std::vector<std::stack<char>>;
using instruction = std::tuple<int, int, int>;

template<class T>
void flip_stack(std::stack<T>& s)
{
    std::stack<T> rval;
    while (s.size())
    {
        rval.push(s.top());
        s.pop();
    }
    
    s.swap(rval);
}

std::pair<Crates, std::stack<instruction>> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    Crates crates;
    for (std::string line; std::getline(file, line);)
    {
        // Count number of cranes
        if (crates.size()==0)
            crates = Crates((line.size()+1)/4);
        
        if (line[1] == '1')
        {
            // Waste blank line and break
            std::getline(file, line);    
            break;
        }

        std::stringstream ss{line};
        for (size_t i=0; i<crates.size(); ++i)
        {
            char letter = line[4*i + 1];
            if (letter==' ')
                continue;
            
            crates[i].push(letter);
        }
    }

    std::stack<instruction> instructions; 
    for (std::string line; std::getline(file, line);)
    {
        std::stringstream ss{line};
        std::string waste;
        int a, b, c;
        ss >> waste >> a >> waste >> b >> waste >> c;
        instructions.push({a, b, c});
    }

    file.close();

    for (auto& stack : crates)
        flip_stack(stack);
    flip_stack(instructions);

    return {crates, instructions};
}

void process_instruction_p1(Crates& cr, std::stack<instruction>& ins)
{
    if (ins.size() == 0)
        return;
    
    auto current_ins = ins.top();
    ins.pop();

    int a, b, c;
    a = std::get<0>(current_ins);
    b = std::get<1>(current_ins) - 1;
    c = std::get<2>(current_ins) - 1;

    for (size_t i=0; i<a; ++i)
    {
        cr[c].push(cr[b].top());
        cr[b].pop();
    }
}

void process_instruction_p2(Crates& cr, std::stack<instruction>& ins)
{
    if (ins.size() == 0)
        return;
    
    auto current_ins = ins.top();
    ins.pop();

    int a, b, c;
    a = std::get<0>(current_ins);
    b = std::get<1>(current_ins) - 1;
    c = std::get<2>(current_ins) - 1;

    std::stack<char> temp_stack;
    for (size_t i=0; i<a; ++i)
    {
        temp_stack.push(cr[b].top());
        cr[b].pop();
    }
    //flip_stack(temp_stack);
    for (size_t i=0; i<a; ++i)
    {
        cr[c].push(temp_stack.top());
        temp_stack.pop();
    }
}

int main()
{
    // Part 2
    auto data = read_file("data/day_05.dat");
    Crates& crates = data.first;
    std::stack<instruction>& instructions = data.second;

    while (!instructions.empty())
        process_instruction_p1(crates, instructions);

    std::cout << "Part 1: ";
    for (auto& stack : crates)
        std::cout << stack.top();
    std::cout << "\n";

    // Part 2
    data = read_file("data/day_05.dat");

    while (!instructions.empty())
        process_instruction_p2(crates, instructions);

    std::cout << "Part 2: ";
    for (auto& stack : crates)
        std::cout << stack.top();
    std::cout << "\n";

    return 0;
}