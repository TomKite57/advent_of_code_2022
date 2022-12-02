
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <numeric>

using Round = std::pair<char,char>;

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

const std::unordered_map<char, int> score_map = {{'X', 1}, {'Y', 2}, {'Z', 3}};
const std::unordered_map<char, int> result_map = {{'X', 0}, {'Y', 3}, {'Z', 6}};
const std::unordered_map<Round, char, pair_hash> get_my_play = {
    {{'A', 'X'}, 'Z'}, {{'A', 'Y'}, 'X'}, {{'A', 'Z'}, 'Y'},
    {{'B', 'X'}, 'X'}, {{'B', 'Y'}, 'Y'}, {{'B', 'Z'}, 'Z'},
    {{'C', 'X'}, 'Y'}, {{'C', 'Y'}, 'Z'}, {{'C', 'Z'}, 'X'}
    };

std::vector<Round> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
        throw;

    std::vector<Round> rval;

    for (std::string line; std::getline(file, line);)
        rval.push_back({line[0], line.back()});

    file.close();

    return rval;
}

int play_round_v1(const Round& round)
{
    int score = score_map.at(round.second);

    const char &p1 = round.first;
    const char &p2 = round.second;

    if (p1 == 'A')
        score += (p2 == 'X') ? 3 : (p2 == 'Y') ? 6 : 0;

    else if (p1 == 'B')
        score += (p2 == 'X') ? 0 : (p2 == 'Y') ? 3 : 6;

    else if (p1 == 'C')
        score += (p2 == 'X') ? 6 : (p2 == 'Y') ? 0 : 3;

    return score;
}

int play_round_v2(const Round &round)
{
    auto my_play = get_my_play.at(round);

    return result_map.at(round.second) + score_map.at(my_play);
}

int main()
{
    auto data = read_file("data/day_02.dat");

    auto final_score_v1 = std::accumulate(data.begin(), data.end(), 0,
                                          [](int &score, Round &round)
                                          { return score + play_round_v1(round); });

    std::cout << "Final score: " << final_score_v1 << "\n";

    auto final_score_v2 = std::accumulate(data.begin(), data.end(), 0,
                                          [](int &score, Round &round)
                                          { return score + play_round_v2(round); });

    std::cout << "Final score: " << final_score_v2 << "\n";

    return 0;
}