
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <map>
#include <unordered_map>
#include <functional>

using valve_graph = std::map<std::string, std::set<std::string>>;
using rate_table = std::map<std::string, int>;

std::tuple<std::string, int, std::set<std::string>> parse_line(const std::string& line) {
    std::regex pattern("^Valve\\s+([A-Z]{2})(?:\\s+.*){0,2}\\s+has flow rate=(\\d+);(?: tunnel(?:s)? lead(?:s)? to valve(?:s)? (.*))?$");
    std::smatch matches;
    if (!std::regex_match(line, matches, pattern)) {
        // If the line doesn't match the pattern, return an empty tuple.
        return {};
    }

    // Extract the valve name, flow rate, and connected valves from the matches.
    std::string valve_name = matches[1];
    int flow_rate = std::stoi(matches[2]);
    std::set<std::string> connected_valves;
    if (matches[3].matched) {
        // Split the connected valves string on any sequence of non-letter characters.
        std::regex connected_valves_pattern("[^A-Z]+");
        std::copy(std::sregex_token_iterator(matches[3].first, matches[3].second, connected_valves_pattern, -1),
                  std::sregex_token_iterator(),
                  std::inserter(connected_valves, connected_valves.begin()));
    }

    return {valve_name, flow_rate, connected_valves};
}

std::pair<valve_graph, rate_table> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<std::tuple<std::string, int, std::set<std::string>>> data;
    for (std::string line; std::getline(file, line);)
        data.push_back(parse_line(line));

    valve_graph graph;
    rate_table flows;
    
    for (const auto& row : data)
    {
        graph.insert({std::get<0>(row), std::get<2>(row)});
        flows.insert({std::get<0>(row), std::get<1>(row)});
    }

    return {graph, flows};
}

template<typename T, typename V>
bool is_in(const T& key, const std::map<T, V>& map)
{
    return map.find(key) != map.end();
}

template<typename T>
bool is_in(const T& key, const std::set<T>& set)
{
    return set.find(key) != set.end();
}

template<typename T, typename V>
bool is_in(const T& key, const std::unordered_map<T, V>& map)
{
    return map.find(key) != map.end();
}

class path_explorer
{
private:
    const valve_graph& graph;
    const rate_table& flows;
    std::map<std::string, bool> open_valves;
    std::string pos{"AA"};
    int pressure{0};
    int time{30};

    void open_current_valve(){ if (open_valves.at(pos)) throw("Valve already open."); open_valves.at(pos) = true; }
    void move_to(const std::string& new_pos){ if (!is_in(new_pos, graph.at(pos))) throw("No open paths to destination"); pos = new_pos; }
    int delta_pressure() const { int dp=0; for (const auto& v_o : open_valves) if (v_o.second) dp+=flows.at(v_o.first); return dp; }
    void increment_counter(){ --time; pressure += delta_pressure(); }

public:
    path_explorer(const valve_graph& gg, const rate_table& ff):
        graph{gg}, flows{ff}
        {
            for (const auto& row: flows) open_valves.insert({row.first, false});
        }
    ~path_explorer() = default;
    path_explorer(const path_explorer&) = default;

    const std::string& get_pos() const { return pos; }
    const std::map<std::string, bool>& get_valve_states() const { return open_valves; }
    int get_pressure() const { return pressure; }
    int get_time_remaining() const { return time; }
    std::pair<int,int> get_time_pressure() const { return {time, pressure}; }
    bool is_finished() const { return time==0; }

    std::string get_hash_string() const
    {
        std::string rval = pos;
        for (const auto& v_o : open_valves)
            rval = rval + v_o.first + std::to_string((int)v_o.second);
        return rval;
    }

    std::vector<path_explorer> take_step()
    {
        if (time == 0)
            throw("No time left to take step");
        
        std::vector<path_explorer> out_clones;
        for (const auto& new_pos : graph.at(pos))
        {
            out_clones.push_back(path_explorer(*this));
            out_clones.back().move_to(new_pos);
        }

        if (!open_valves.at(pos) && flows.at(pos)>0)
        {
            out_clones.push_back(path_explorer(*this));
            out_clones.back().open_current_valve();
        }

        for (auto& pe : out_clones)
            pe.increment_counter();
        
        return out_clones;
    }
};

int part_1(const valve_graph& graph, const rate_table& flows)
{
    std::vector<path_explorer> explorers{};
    explorers.push_back(path_explorer{graph, flows});
    int best_pressure = 0;
    std::unordered_map<std::string, std::pair<int,int>> history;
    history.insert({explorers.back().get_hash_string(), explorers.back().get_time_pressure()});

    int max_flow_rate = 0;
    for (const auto& v_f : flows)
        max_flow_rate += v_f.second;

    while (!explorers.empty())
    {
        path_explorer current = explorers.back();
        explorers.pop_back();
        std::cout << current.get_pressure() << " | "; //DEBUG
        std::cout << current.get_time_remaining() << " | "; //DEBUG
        std::cout << current.get_hash_string() << "\n"; //DEBUG

        if (current.is_finished())
        {
            best_pressure = std::max(best_pressure, current.get_pressure());
            continue;
        }

        auto new_explorers = current.take_step();
        for (const auto& exp : new_explorers)
        {
            bool to_keep = true;
            if (!is_in(exp.get_hash_string(), history))
                history.insert({exp.get_hash_string(), exp.get_time_pressure()});
            else
            {
                auto old_time_pressure = history.at(exp.get_hash_string());
                auto current_time_pressure = current.get_time_pressure();
                int max_improvement = max_flow_rate*current_time_pressure.first;

                if (current_time_pressure.first <= old_time_pressure.first && current_time_pressure.second <= old_time_pressure.second)
                    to_keep = false;
                if (current_time_pressure.second + max_improvement <= old_time_pressure.first)
                    to_keep = false;
            }
            
            if (to_keep)
            {
                history[exp.get_hash_string()] = exp.get_time_pressure();
                explorers.push_back(exp);
            }
        }
    }

    return best_pressure;
}

int main()
{
    auto data = read_file("data/day_16.dat");
    valve_graph graph = data.first;
    rate_table flows = data.second;

    //for (const auto& row : graph)
    //{
    //    std::cout << row.first << ": ";
    //    for (const auto& elem : row.second)
    //        std::cout << elem << ", ";
    //    std::cout << "\n";
    //}
    //std::cout << "\n";
    //for (const auto& row : flows)
    //{
    //    std::cout << row.first << ": " << row.second << "\n";
    //}
    //std::cout << "\n";

    std::cout << "Part 1: " << part_1(graph, flows) << "\n";
    
    return 0;
}