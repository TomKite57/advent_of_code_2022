
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <sstream>
#include <memory>

std::vector<std::string> read_file(const std::string &fname)
{
    std::ifstream file;
    file.open(fname);

    if (!file)
    {
        std::cout << "Could not open file!" << "\n";
        throw("Could not open file!");
    }

    std::vector<std::string> rval;
    for (std::string line; std::getline(file, line); rval.push_back(line)){}

    file.close();

    return rval;
}

class file_system
{
public:
    std::shared_ptr<file_system> _parent;
    std::string _name;
    std::vector<std::shared_ptr<file_system>> _children;

    file_system(): _name{"root"}, _parent{nullptr} {};
    file_system(std::string name, std::shared_ptr<file_system> parent): _name{name}, _parent{parent} {};
    virtual ~file_system() = default;
    file_system(const file_system&) = delete;

    virtual void add_child(std::shared_ptr<file_system>) = 0;
    virtual int get_size() = 0;
    virtual int part_1_checksum() = 0;
    virtual void part_2_checksum(int& target, int& current_best) = 0;

    void smart_print(int ind=0)
    {
        auto apply_indent = [&](){ for (int i=0; i<ind; ++i) std::cout << "  "; std::cout << "|";};

        apply_indent();
        std::cout << "--" << _name;
        if (_children.size() == 0)
            return;

        for (const auto& ch : _children)
        {
            std::cout << "\n";
            ch->smart_print(ind+1);
        }
    }
};

class directory: public file_system
{
public:
    directory(): file_system{} {};
    directory(std::string name, std::shared_ptr<file_system> parent): file_system{name, parent} {};
    virtual ~directory() = default;
    directory(const directory&) = delete;

    void add_child(std::shared_ptr<file_system> child){ _children.push_back(child); }
    virtual int get_size(){
        auto func = [](const int& s, std::shared_ptr<file_system> child){ return s + child->get_size(); };
        return std::accumulate(_children.begin(), _children.end(), 0.0, func);
    }
    
    virtual int part_1_checksum(){
        auto func = [this](const int& s, std::shared_ptr<file_system> child){
            return s + child->part_1_checksum();
        };

        int size = get_size();
        size = (size > 100000) ? 0 : size;
        return size + std::accumulate(_children.begin(), _children.end(), 0.0, func);
    }

    virtual void part_2_checksum(int& target, int& current_best){
        int size = get_size();
        if (size > target)
            current_best = std::min(current_best, size);
        for (auto& ch : _children)
            ch->part_2_checksum(target, current_best);
    }
};

class file: public file_system
{
public:
    int _size;
    
    file(std::string name, std::shared_ptr<file_system> parent, int size): file_system{name, parent}, _size{size} {}
    virtual ~file() = default;
    file(const file&) = delete;

    void add_child(std::shared_ptr<file_system>){ throw("Can't add child to file"); }
    virtual int get_size(){ return _size; }
    virtual int part_1_checksum(){ return 0; }
    virtual void part_2_checksum(int& target, int& current_best){ return; }
};

bool strcomp(std::string s1, std::string s2)
{
    if (s1.size() != s2.size())
        return false;

    for (size_t i=0; i<s1.size(); ++i)
        if (s1[i] != s2[i])
            return false;
    return true;
}

bool strcomp(char c1, char c2)
{
    return c1==c2;
}

std::shared_ptr<file_system> consturct_root(const std::vector<std::string>& commands)
{
    std::shared_ptr<file_system> root = std::make_shared<directory>();
    std::shared_ptr<file_system> current_pos = root;

    for (auto com=commands.begin(); com<commands.end(); ++com)
    {
        std::stringstream ss{*com};
        std::string substr;
        ss >> substr;

        if (!strcomp(substr[0], '$'))
            throw("Didn't understand command!");

        ss >> substr;
        if (strcomp(substr, "cd"))
        {
            ss >> substr;
            if (strcomp(substr, "/"))
                current_pos = root;
            else if (strcomp(substr, ".."))
                current_pos = current_pos->_parent;
            else
            {
                bool found = false;
                for (const auto& child : current_pos->_children)
                {
                    if (strcomp(substr, child->_name))
                    {
                        current_pos = child;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    throw("Couldn't find destination!");
            }
        }
        else if (strcomp(substr, "ls"))
        {
            while (++com < commands.end())
            {
                std::stringstream ss{*com};
                std::string substr;
                ss >> substr;

                if (strcomp(substr[0], '$'))     
                {
                    --com;
                    break;
                }

                if (strcomp(substr, "dir"))
                {
                    ss >> substr;
                    std::shared_ptr<directory> new_child = std::make_shared<directory>(substr, current_pos);
                    current_pos->add_child(new_child);
                }
                else
                {
                    int size = std::stoi(substr);
                    ss >> substr;
                    std::shared_ptr<file> new_child = std::make_shared<file>(substr, current_pos, size);
                    current_pos->add_child(new_child);
                }
            }
        }
        else
            throw("Didn't understand command!");
    }

    return root;
}

int part_1(const std::vector<std::string>& commands)
{
    auto root = consturct_root(commands);

    return root->part_1_checksum();
}

int part_2(const std::vector<std::string>& commands)
{
    auto root = consturct_root(commands);

    int must_free = 30000000 - (70000000 - root->get_size());
    int best = 70000000;

    root->part_2_checksum(must_free, best);
    return best;
}

int main()
{
    auto data = read_file("data/day_07.dat");

    std::cout << "Part 1: " << part_1(data) << "\n";
    std::cout << "Part 2: " << part_2(data) << "\n";

    return 0;
}