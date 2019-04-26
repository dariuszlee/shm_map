#include <map>
#include <vector>

std::map<std::string, std::vector<uint32_t>> load_node_cilacs()
{
    std::map<std::string, std::vector<uint32_t>> data;

    for (int i = 0; i < 3; ++i) {
        std::cout << "ID : " << i << std::endl;
        std::vector<uint32_t> v;
        for (int j = 1; j < 5; ++j) {
            uint32_t d = std::rand() * j;
            std::cout << d << std::endl;
            v.push_back(d);
        }
        data.insert(std::pair<std::string, std::vector<uint32_t>>(std::to_string(i), v));
    }
    return data;
}
