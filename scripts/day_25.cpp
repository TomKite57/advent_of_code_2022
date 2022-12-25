
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

const std::unordered_map<char, long long int> snafu_char_map = {{'=', -2}, {'-', -1}, {'0', 0}, {'1', 1}, {'2', 2}};
const std::unordered_map<long long int, char> inverse_snafu_char_map = {{-2, '='}, {-1, '-'}, {0, '0'}, {1, '1'}, {2, '2'}};

std::vector<std::string> read_file(const std::string& fname)
{
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<std::string> rval;
    for (std::string str; std::getline(file, str);)
        rval.push_back(str);

    return rval;
}

long long int snafu_to_decimal(const std::string& snafu)
{
    long long int rval=0;

    for (const auto& digit : snafu)
    {
        rval *= 5;
        rval += snafu_char_map.at(digit);
    }
    return rval;
}

std::string int_to_base_n(long long int x, int n)
{
    std::string result;

    while (x)
    {
        result.push_back('0' + x % n);
        x /= n;
    }

    if (result.empty())
        return "0";

    std::reverse(result.begin(), result.end());
    return result;
}

std::string decimal_to_snafu(const long long int& decimal)
{
    std::string first_pass_str = int_to_base_n(decimal, 5);

    std::vector<int> first_pass;
    for (auto it=first_pass_str.begin(); it!=first_pass_str.end(); ++it)
        first_pass.push_back((int)*it - '0');

    std::reverse(first_pass.begin(), first_pass.end());
    int i = 0;
    while (i+1 < first_pass.size() || first_pass.back()!=0)
    {
        if (i+1 == first_pass.size())
            first_pass.push_back(0);

        int& num = first_pass[i];
        int& next_num = first_pass[i+1];

        while (num >= 3)
        {
            num -= 5;
            next_num++;
        }

        ++i;
    }

    first_pass.pop_back();
    std::reverse(first_pass.begin(), first_pass.end());

    std::string rval="";
    for (const auto& v : first_pass)
        rval += inverse_snafu_char_map.at(v);

    return rval;
}

long long int snafu_sum(const std::vector<std::string>& lines)
{
    long long int rval=0;
    for (const auto& line : lines)
        rval += snafu_to_decimal(line);
    return rval;
}

int main()
{
    auto data = read_file("data/day_25.dat");
    auto sum = snafu_sum(data);

    std::cout << sum << "\n";
    std::cout << "Part 1: " << decimal_to_snafu(sum) << "\n";

    return 0;
}
