
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <algorithm>

class primate
{
    friend std::ostream& operator<<(std::ostream& os, const primate& monke);

private:
    std::vector<int> items;
    std::function<int(int)> worry_function;
    bool worry_reduction = true;
    std::function<bool(int)> throw_check;
    primate* true_monke;
    primate* false_monke;
    int inspections = 0;

    void apply_worry(){
        for (auto& it : items)
        {
            it = worry_function(it);
            if (worry_reduction)
                it = it/3;
            ++inspections;
        }
    }

    void consider_throws(){
        auto it = items.begin();
        while (it != items.end())
        {
            if (throw_check(*it))
                throw_to(it, *true_monke);
            else
                throw_to(it, *false_monke);
        }
    }

public:
    primate() = default;
    ~primate() = default;
    primate(const primate&) = delete;

    void set_worry_reduction(bool wor_red){ worry_reduction = wor_red; }

    void add_items(const std::vector<int>& its) { for (const auto& it: its) items.push_back(it); }
    void set_worry_func(std::function<int(int)> func) { worry_function = func; }
    void set_throw_check(std::function<bool(int)> func) { throw_check = func; }
    void set_true_monke(primate& monke) { true_monke = &monke; }
    void set_false_monke(primate& monke) { false_monke = &monke; }

    void recieve(int it){ items.push_back(it); }
    void throw_to(std::vector<int>::iterator& it, primate& monke){ monke.recieve(*it); it = items.erase(it); }

    void take_turn(){ apply_worry(); consider_throws(); }

    int get_inspections(){ return inspections; }
};

std::ostream& operator<<(std::ostream& os, const primate& monke)
{
    for (const auto& it : monke.items)
        os << it << ", ";
    return os;
}

std::vector<primate> build_monkes_test(bool worry_reduction)
{
    std::vector<primate> monkes(4);
    for (auto& monke : monkes)
        monke.set_worry_reduction(worry_reduction);

    // Monke 0
    monkes[0].add_items({79, 98});
    monkes[0].set_worry_func([](int old){ return old * 19; });
    monkes[0].set_throw_check([](int num){ return num%23 == 0; });
    monkes[0].set_true_monke(monkes[2]);
    monkes[0].set_false_monke(monkes[3]);
    // Monke 1
    monkes[1].add_items({54, 65, 75, 74});
    monkes[1].set_worry_func([](int old){ return old + 6; });
    monkes[1].set_throw_check([](int num){ return num%19 == 0; });
    monkes[1].set_true_monke(monkes[2]);
    monkes[1].set_false_monke(monkes[0]);
    // Monke 2
    monkes[2].add_items({79, 60, 97});
    monkes[2].set_worry_func([](int old){ return old * old; });
    monkes[2].set_throw_check([](int num){ return num%13 == 0; });
    monkes[2].set_true_monke(monkes[1]);
    monkes[2].set_false_monke(monkes[3]);
    // Monke 3
    monkes[3].add_items({74});
    monkes[3].set_worry_func([](int old){ return old + 3; });
    monkes[3].set_throw_check([](int num){ return num%17 == 0; });
    monkes[3].set_true_monke(monkes[0]);
    monkes[3].set_false_monke(monkes[1]);

    return monkes;
}

std::vector<primate> build_monkes(bool worry_reduction)
{
    std::vector<primate> monkes(8);
    for (auto& monke : monkes)
        monke.set_worry_reduction(worry_reduction);
 
    // Monke 0
    monkes[0].add_items({85, 77, 77});
    monkes[0].set_worry_func([](int old){ return old * 7; });
    monkes[0].set_throw_check([](int num){ return num%19 == 0; });
    monkes[0].set_true_monke(monkes[6]);
    monkes[0].set_false_monke(monkes[7]);

    // Monke 1
    monkes[1].add_items({80, 99});
    monkes[1].set_worry_func([](int old){ return old * 11; });
    monkes[1].set_throw_check([](int num){ return num%3 == 0; });
    monkes[1].set_true_monke(monkes[3]);
    monkes[1].set_false_monke(monkes[5]);

    // Monke 2
    monkes[2].add_items({74, 60, 74, 63, 86, 92, 80});
    monkes[2].set_worry_func([](int old){ return old + 8; });
    monkes[2].set_throw_check([](int num){ return num%13 == 0; });
    monkes[2].set_true_monke(monkes[0]);
    monkes[2].set_false_monke(monkes[6]);

    // Monke 3
    monkes[3].add_items({71, 58, 93, 65, 80, 68, 54, 71});
    monkes[3].set_worry_func([](int old){ return old + 7; });
    monkes[3].set_throw_check([](int num){ return num%7 == 0; });
    monkes[3].set_true_monke(monkes[2]);
    monkes[3].set_false_monke(monkes[4]);

    // Monke 4
    monkes[4].add_items({97, 56, 79, 65, 58});
    monkes[4].set_worry_func([](int old){ return old + 5; });
    monkes[4].set_throw_check([](int num){ return num%5 == 0; });
    monkes[4].set_true_monke(monkes[2]);
    monkes[4].set_false_monke(monkes[0]);

    // Monke 5
    monkes[5].add_items({77});
    monkes[5].set_worry_func([](int old){ return old + 4; });
    monkes[5].set_throw_check([](int num){ return num%11 == 0; });
    monkes[5].set_true_monke(monkes[4]);
    monkes[5].set_false_monke(monkes[3]);

    // Monke 6
    monkes[6].add_items({99, 90, 84, 50});
    monkes[6].set_worry_func([](int old){ return old * old; });
    monkes[6].set_throw_check([](int num){ return num%17 == 0; });
    monkes[6].set_true_monke(monkes[7]);
    monkes[6].set_false_monke(monkes[1]);

    // Monke 7
    monkes[7].add_items({50, 66, 61, 92, 64, 78});
    monkes[7].set_worry_func([](int old){ return old + 3; });
    monkes[7].set_throw_check([](int num){ return num%2 == 0; });
    monkes[7].set_true_monke(monkes[5]);
    monkes[7].set_false_monke(monkes[1]);


    return monkes;
}

int play_game(std::vector<primate>& monkes, int rounds)
{
    // Play game
    for (int i=0; i<rounds; ++i)
    {
        for (auto& monke : monkes)
            monke.take_turn();
        
        std::cout << "Round " << i << ":\n";
        for (auto& monke : monkes)
            std::cout << monke << "\n";
        std::cout << "\n";
    }
    
    std::vector<int> inspections;
    for (auto& monke : monkes)
        inspections.push_back(monke.get_inspections());

    std::sort(inspections.begin(), inspections.end(), std::greater<int>());

    return inspections[0]*inspections[1];
}

int main()
{
    //auto monkes = build_monkes_test(true);
    //std::cout << "Part 1: " << play_game(monkes, 20) << "\n";
    //monkes = build_monkes_test(false);
    //std::cout << "Part 2: " << play_game(monkes, 10000) << "\n";
    //std::cout << "Part 2: " << 2713310158 << "\n";

    auto monkes = build_monkes_test(false);
    play_game(monkes, 20);

    return 0;
}