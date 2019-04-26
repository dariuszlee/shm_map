#include <iostream>
#include <cereal/archives/binary.hpp>

#include <map>
#include <vector>

class SomeData
{
public:

    void load_data(std::string k, std::vector<uint32_t> v)
    {
        data.insert(std::pair<std::string, std::vector<uint32_t>>(k, v));
    }

    template <class Archive>
    void save( Archive & ar ) const
    {
        ar( data );
    }

    template <class Archive>
    void load( Archive & ar )
    {
        ar( data );
    }
private:
    std::map<std::string, std::vector<uint32_t>> data;
};


int main(int argc, char *argv[])
{
    SomeData data_pre;
    for (int i = 0; i < 20; ++i) {
        std::vector<uint32_t> v;
        for (int j = 1; j < 5; ++j) {
            v.push_back(std::rand() * j);
        }
        data_pre.load_data(std::to_string(i), v);
    }

    

    
    return 0;
}
