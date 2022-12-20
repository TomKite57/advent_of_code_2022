
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <deque>
#include <algorithm>

std::vector<long long int> read_file(const std::string& fname) {
    std::ifstream file(fname);

    if (!file)
        throw std::invalid_argument("Could not open file");

    std::vector<long long int> data;
    for (std::string line; std::getline(file, line);)
        data.push_back(std::stoi(line));

    return data;
}

class node
{
public:
    static int unique_ID_counter;
    long long int val;
    int unique_ID;

    static void reset_counter() { unique_ID_counter = 0; }

    node(long long int v): val{v}, unique_ID{unique_ID_counter++} {}
    ~node() = default;
    node(const node&) = default;
};
int node::unique_ID_counter = 0;

long long int num_in_cycle_global(long long int num, long long int size)
{
    if (num < 0)
    {
        num += (-num/size)*size;
        num += size;
    }
    else if (num > 0)
        num -= (num/size)*size;

    return num;
}

class encrypted_message : public std::deque<node>
{
    friend std::ostream& operator<<(std::ostream& os, const encrypted_message& message);
public:

    long long int num_in_cycle(long long int num)
    {
        return num_in_cycle_global(num, (long long int)size());
    }

    void rotate(long long int amount) {
        amount = num_in_cycle(amount);
        std::rotate(begin(), begin()+(int)amount, end());
    }

    void insert_at(const node& n, long long int pos)
    {
        pos = num_in_cycle(pos);
        rotate(pos);
        push_front(n);
        rotate(-pos);
        if (pos==0)
            rotate(1);
    }
    
    void shift_elem()
    {
        node n = front();
        long long int val = n.val;
        pop_front();
        insert_at(n, val);
    }

    void encrypt(long long int times=1)
    {
        int rounds{0};
        int counter{0};
        int tot_size{(int)size()};
        while (rounds < times)
        {
            const node& val = front();
            if (val.unique_ID == counter)
            {
                shift_elem();
                ++counter;
                if (counter == tot_size)
                {
                    counter = 0;
                    ++rounds;
                    std::cout << "Finished " << rounds << "/" << times << "\n";
                }
            }
            else
                rotate(1);
        }
    }

    long long int checksum()
    {
        while (front().val!=0)
            rotate(1);
        
        long long int rval = 0;
        rotate(1000);
        rval += front().val;
        rotate(1000);
        rval += front().val;
        rotate(1000);
        rval += front().val;

        return rval;
    }

    encrypted_message() : std::deque<node>() { node::reset_counter(); }
    template <typename T>
    encrypted_message(const std::vector<T>& ob): std::deque<node>() { node::reset_counter(); for (const auto& v : ob) insert(end(), node(v)); }
    encrypted_message(const encrypted_message& other) : std::deque<node>(other) { }
    encrypted_message& operator=(const encrypted_message& other) {
        std::deque<node>::operator=(other);
        return *this;
    }
    ~encrypted_message() { }
};

std::ostream& operator<<(std::ostream& os, const encrypted_message& message)
{
    os << "(";
    for (auto it=message.begin(); it!=message.end(); ++it)
    {
      os << it->val;
      if (std::next(it) != message.end())
        os << ", ";
    }
    os << ")";
    return os;
}

int main()
{
    std::vector<long long int> data = read_file("data/day_20.dat");

    //for (const auto& v : data)
    //    std::cout << v << " | " << 811589153 << " | " << (long long int)v * (long long int)811589153 << "\n";

    encrypted_message message{data};
    message.encrypt();
    std::cout << "Part 1: " << message.checksum() << "\n";

    std::vector<long long int> bigger_data;
    for (const auto& v : data)
        bigger_data.push_back(v * 811589153);
    message = encrypted_message(bigger_data);
    message.encrypt(10);
    std::cout << "Part 2: " << message.checksum() << "\n";
    std::cout << "Part 2: " << 1623178306 << "\n";
    std::cout << "Part 2: " << 2043946744 << "(!)\n";

    return 0;
}

/*
Initial arrangement:
1, 2, -3, 3, -2, 0, 4

1 moves between 2 and -3:
2, 1, -3, 3, -2, 0, 4

2 moves between -3 and 3:
1, -3, 2, 3, -2, 0, 4

-3 moves between -2 and 0:
1, 2, 3, -2, -3, 0, 4

3 moves between 0 and 4:
1, 2, -2, -3, 0, 3, 4

-2 moves between 4 and 1:
1, 2, -3, 0, 3, 4, -2

0 does not move:
1, 2, -3, 0, 3, 4, -2

4 moves between -3 and 0:
1, 2, -3, 4, 0, 3, -2
*/