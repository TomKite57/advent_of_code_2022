
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
using compressed_valve_graph = std::map<std::pair<std::string, std::string>, int>;

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

class graph_explorer
{
private:
    const valve_graph& graph;
    std::string pos;
    std::string destination;
    int len{0};

    void move_to(const std::string& new_pos)
    {
        if (!is_in(new_pos, graph.at(pos)))
            throw("No open paths to destination");
        ++len;
        pos = new_pos;
    }

public:
    graph_explorer(const valve_graph& gg, std::string start, std::string end):
        graph{gg}, pos{start}, destination{end} {}
    ~graph_explorer() = default;
    graph_explorer(const graph_explorer&) = default;
    graph_explorer& operator=(const graph_explorer& other)
    {
        pos = other.pos;
        destination = other.destination;
        len = other.len;

        return *this;
    };

    const std::string& get_pos() const { return pos; }
    const std::string& get_destination() const { return destination; }
    int get_len() const { return len; }
    bool is_finished() const { return pos==destination; }

    std::string get_hash_string() const { return get_pos(); }

    std::vector<graph_explorer> take_step()
    {
        if (is_finished())
            throw("Already at destination");

        std::vector<graph_explorer> out_clones;
        for (const auto& new_pos : graph.at(pos))
        {
            out_clones.push_back(graph_explorer(*this));
            out_clones.back().move_to(new_pos);
        }
        
        return out_clones;
    }
};

int min_path(const valve_graph& graph, const std::string& start, const std::string& end)
{
    if (!is_in(start, graph) || !is_in(end, graph))
        throw("Locations not in graph!");
    
    // Initial setup
    std::vector<graph_explorer> explorers{};
    explorers.push_back(graph_explorer{graph, start, end});
    int best_len = -1;
    std::unordered_map<std::string, int> history;
    history.insert({explorers.back().get_hash_string(), explorers.back().get_len()});

    // Allow all explorers to do their thing
    while (!explorers.empty())
    {
        graph_explorer current = std::move(explorers.back());
        explorers.pop_back();

        // Check if already finished
        if (current.is_finished())
        {
            best_len = std::max(best_len, current.get_len());
            continue;
        }

        // Find all possible steps
        auto new_explorers = current.take_step();
        for (const auto& exp : new_explorers)
        {
            auto hash = exp.get_hash_string();
            // If in new state always keep
            if (!is_in(hash, history))
            {
                history.insert({exp.get_hash_string(), exp.get_len()});
                explorers.push_back(exp);
            }
            else if (history.at(hash) > exp.get_len())
            {
                history[hash] = exp.get_len();
                explorers.push_back(exp);
            }
        }
    }

    return best_len;
}

compressed_valve_graph compress_graph(const valve_graph& graph, const rate_table& flows)
{
    std::vector<std::string> locations_to_map{"AA"};
    for (const auto& row : graph)
        if (flows.at(row.first) > 0)
            locations_to_map.push_back(row.first);

    std::map<std::pair<std::string, std::string>, int> compressed_graph;
    for (auto it_a=locations_to_map.begin(); it_a<locations_to_map.end(); ++it_a)
        for (auto it_b=it_a+1; it_b<locations_to_map.end(); ++it_b)
    {
        compressed_graph.insert( {{*it_a, *it_b}, min_path(graph, *it_a, *it_b)} );
        // Don't assume directed
        compressed_graph.insert( {{*it_b, *it_a}, min_path(graph, *it_b, *it_a)} );
    }

    return compressed_graph;
}

class valve_path_explorer
{
private:
    const compressed_valve_graph& path_lengths;
    const rate_table& flows;
    std::map<std::string, bool> open_valves;
    std::string pos{"AA"};
    int pressure{0};
    int time{0};
    int time_limit{30};

    void open_current_valve()
    {
        increment_counter(1);
        
        if (is_finished())
            return;

        if (open_valves.at(pos))
            throw("Valve already open.");
        open_valves.at(pos) = true;
    }
    void move_to(const std::string& new_pos)
    {
        if (!is_in({pos, new_pos}, path_lengths))
            throw("No open paths to destination");
        
        bool enough_time = (path_lengths.at({pos, new_pos}) <= get_time_remaining());
        increment_counter(path_lengths.at({pos, new_pos}));
        //pos = new_pos;
        if (enough_time)
            pos = new_pos;        
    }
    void increment_counter(int n)
    {
        if (is_finished())
            return;

        n = std::min(n, time_limit-time);
        time+=n;
        pressure += n*get_current_flow_rate();
    }

public:
    valve_path_explorer(const compressed_valve_graph& gg, const rate_table& ff):
        path_lengths{gg}, flows{ff}
        {
            for (const auto& row: flows)
                if (row.second > 0)
                    open_valves.insert({row.first, false});
        }
    ~valve_path_explorer() = default;
    valve_path_explorer(const valve_path_explorer&) = default;
    valve_path_explorer& operator=(const valve_path_explorer& other)
    {
        open_valves = other.open_valves;
        pos = other.pos;
        pressure = other.pressure;
        time = other.time;

        return *this;
    };

    const rate_table& get_rate_table() const { return flows; }
    const std::string& get_pos() const { return pos; }
    const std::map<std::string, bool>& get_valve_states() const { return open_valves; }
    int get_pressure() const { return pressure; }
    int get_time_remaining() const { return time_limit-time; }
    int get_time() const { return time; }
    std::pair<int,int> get_time_and_pressure() const { return {get_time_remaining(), pressure}; }
    bool is_finished() const { return time==time_limit; }

    int get_current_flow_rate() const
    {
        int fr=0;
        for (const auto& v_o : open_valves)
            if (v_o.second)
                fr += flows.at(v_o.first);
        
        return fr;
    }

    std::string get_hash_string() const
    {
        std::string rval = pos;
        for (const auto& v_o : open_valves)
            rval = rval + v_o.first + std::to_string((int)v_o.second);
        return rval;
    }

    int heuristic_score() const
    {
        int score = pressure;
        for (const auto& v_o : open_valves)
            score += (v_o.second) ? flows.at(v_o.first)*(time-1) : 0;
        return score;
    }

    std::vector<valve_path_explorer> take_step()
    {
        if (is_finished())
            throw("No time left to take step");
        
        std::vector<std::string> locs;
        for (const auto& row : path_lengths)
            if (row.first.first == pos && path_lengths.at({pos, row.first.second})<=get_time_remaining() && is_in(row.first.second, open_valves) && !open_valves.at(row.first.second))
                locs.push_back(row.first.second);

        if (locs.empty())
        {
            increment_counter(time_limit);
            return {*this};
        }

        std::vector<valve_path_explorer> out_clones;
        for (const auto& new_pos : locs)
        {
            out_clones.push_back(valve_path_explorer(*this));
            out_clones.back().move_to(new_pos);
            out_clones.back().open_current_valve();
        }

        return out_clones;
    }
};

int part_1(const compressed_valve_graph& graph, const rate_table& flows)
{
    // Initial setup
    std::vector<valve_path_explorer> explorers{};
    explorers.push_back(valve_path_explorer{graph, flows});
    int best_pressure = -1;
    std::unordered_map<std::string, std::pair<int,int>> history;
    //history.insert({explorers.back().get_hash_string(), explorers.back().get_time_and_pressure()});

    // Find max flow rate
    int max_flow_rate = 0;
    for (const auto& v_f : flows)
        max_flow_rate += v_f.second;

    // Allow all explorers to do their thing
    while (!explorers.empty())
    {
        // Just most likely to succeed
        auto current_it = std::max_element(explorers.begin(), explorers.end(),
                                           [](const valve_path_explorer& pe_a, const valve_path_explorer& pe_b){return pe_a.heuristic_score()<pe_b.heuristic_score();});
        valve_path_explorer current = std::move(*current_it);
        explorers.erase(current_it);

        std::cout << best_pressure << " | " << current.get_pressure() << " | " << current.get_time() << " | " << current.get_hash_string() << "\n";

        // Check if already finished
        if (current.is_finished())
        {
            best_pressure = std::max(best_pressure, current.get_pressure());
            continue;
        }

        // Check if definitely worse than best pressure
        //if (best_pressure != -1 && current.get_pressure() + max_flow_rate*current.get_time_remaining() <= best_pressure)
        //    continue;

        // Find all possible steps
        auto new_explorers = current.take_step();
        for (const auto& exp : new_explorers)
        {
            bool to_keep = true;
            // If in new state always keep
//            if (!is_in(exp.get_hash_string(), history))
//                history.insert({exp.get_hash_string(), exp.get_time_and_pressure()});
//            else
//            {
//                // Previous time in state
//                auto old_time_pressure = history.at(exp.get_hash_string());
//                int prev_time_remain = old_time_pressure.first;
//                int prev_pressure = old_time_pressure.second;
//
//                // This state
//                auto current_time_pressure = current.get_time_and_pressure();
//                int current_time_remain = current_time_pressure.first;
//                int current_pressure = current_time_pressure.second;
//
//                // Max improvement and default improvement compared to over state
//                int max_improvement = max_flow_rate*(current_time_remain - prev_time_remain);
//                int default_improvement = current.get_current_flow_rate()*(current_time_remain - prev_time_remain);
//
//                // Max improvement and default improvement to final moment
//                int optimistic_projection = max_flow_rate*current_time_remain;
//                int default_projection = current.get_current_flow_rate()*current_time_remain;
//
//                // Simply worse or equal in both metrics
//                if (current_time_remain <= prev_time_remain && current_pressure <= prev_pressure)
//                    to_keep = false;
//                // Even optimistic progress would be worse
//                else if (current_time_remain > prev_time_remain && current_pressure + max_improvement < prev_pressure)
//                    to_keep = false;
//                // Even optimistic projection would fail
//                else if (best_pressure != -1 && current_pressure + optimistic_projection <= best_pressure)
//                    to_keep = false;
//            }
            
            if (to_keep)
            {
                //history[exp.get_hash_string()] = exp.get_time_and_pressure();
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

    auto compressed = compress_graph(graph, flows);

    //for (const auto& row : compressed)
    //    std::cout << row.first.first << " ==> " << row.first.second << ": " << row.second << "\n";

    std::cout << "Part 1: " << part_1(compressed, flows) << "\n";
    
    return 0;
}