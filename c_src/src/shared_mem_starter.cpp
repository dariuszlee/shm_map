#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <map>
#include <vector>
#include <set>

#include "phf.h"

#include "file_mem_utils.cpp"

using namespace boost::interprocess;

int get_mem(const mapped_region& p_region)
{
    int *il = static_cast<int*>(p_region.get_address());
    return *il;
}

void set_mem(const mapped_region& p_region, int p_node_id)
{
    int *il = static_cast<int*>(p_region.get_address());
    *il = p_node_id;
}

std::map<std::string, float> load_file()
{
    std::map<std::string,float> node_weights;
    /* std::ifstream infile("/home/dariuslee/motionlogic/dumps//weighted_graph.csv"); */
    std::ifstream infile("./weight_graph_sample.csv");

    std::string line;
    while(std::getline(infile, line))
    {
        size_t src_pos = line.find(',', 0);
        std::string src = line.substr(0, src_pos);
        line = line.substr(src_pos + 1);

        src_pos = line.find(',', 0);
        std::string trg = line.substr(0, src_pos);
        std::string weight = line.substr(src_pos + 1); 
        float weight_f = std::stof(weight);

        std::stringstream key_ss; 
        key_ss << src << "_" << trg;
        std::string key = key_ss.str();

        node_weights.insert(std::pair<std::string, float>(key, weight_f));
    }

    return node_weights;
}

uint64_t create_key_int(std::string key)
{
    size_t underscore_pos = key.find('_', 0);
    uint32_t src = std::stoul(key.substr(0, underscore_pos));
    uint32_t trg = std::stoul(key.substr(underscore_pos + 1));

    return src + trg;
}

size_t hash_key(std::string key)
{
    return std::hash<std::string>()(key);
}

std::vector<size_t> hash_all(std::map<std::string,float> to_hash)
{
    std::vector<size_t> collection_of_hashes;
    for(std::map<std::string,float>::iterator it = to_hash.begin();
        it != to_hash.end(); ++it)
    {
        collection_of_hashes.push_back(hash_key(it->first));
    }
    return collection_of_hashes;
}

bool check_uniqueness(const std::vector<size_t>& hashes, size_t shift){
    std::set<size_t> uniqueness;
    bool current_run = true;
    for(std::vector<size_t>::const_iterator it = hashes.begin(); it!= hashes.end(); ++it){
        size_t new_hash = *it >> shift;
        /* std::cout << "Old hash " << *it << std::endl; */
        /* std::cout << "NEW HASH " << new_hash << std::endl; */
        if(uniqueness.find(new_hash) != uniqueness.end()){
            current_run = false;
            break;
        }
        else{
            uniqueness.insert(new_hash);
        }
    }
    return current_run;
}

template<class T>
std::string* generate_array(std::map<std::string,T> to_array){
    std::string* ret_data = new std::string[to_array.size()];
    size_t j = 0;
    for (auto i = to_array.begin(); i != to_array.end(); ++i) {
        ret_data[j] = i->first;
        ++j;
    }       
    return ret_data;
}

void create_file(phf* new_alg){
    std::map<std::string,float> file_as_map = load_file();
    std::string * data = generate_array(file_as_map);
    std::cout << "FINISHED_LOADING_FILE" << std::endl;

    
    phf * alg_def = new phf;
    PHF::init<std::string, false>(alg_def, data, file_as_map.size(), 1, 
            100, (phf_seed_t)0);
    std::cout << "Finished_building hash" << std::endl;

    write_binary(alg_def, file_as_map);
    std::cout << "Finished writing hash" << std::endl;

    /* phf* new_alg = read_binary_hash(); */
    /* std::cout << "Finished reloading hash" << std::endl; */

    /* check_phfs(alg_def, new_alg); */
    /* std::cout << "Finished checking phf" << std::endl; */

    /* std::set<size_t> res; */
    /* size_t num_of_matches = 0; */
    /* for(size_t i = 0; i != file_as_map.size(); i++){ */
    /*     size_t hash = PHF::hash<std::string>(alg_def, data[i]); */
    /*     size_t new_hash = PHF::hash<std::string>(new_alg, data[i]); */
    /*     if(hash != new_hash){ */
    /*         num_of_matches++; */
    /*     } */
    /*     res.insert(hash); */
    /* } */
    /* std::cout << "Number of misses: " << num_of_matches << std::endl; */
    /* std::cout << "Finished hashing. Size of hash:" << res.size() << std::endl; */

}

void test()
{
    std::map<std::string,float> file = load_file();
    std::string * data = generate_array(file);

    phf * alg_def = new phf;
    PHF::init<std::string, false>(alg_def, data, file.size(), 1, 
            100, (phf_seed_t)0);
    std::cout << "Finished_building hash" << std::endl;

    float* float_data = write_binary(alg_def, file);
    std::cout << "Finished writing hash" << std::endl;

    ShareMemDict<float> new_alg = reset_shared_mem<float>("file.binary");
    for (auto i = file.begin(); i != file.end(); ++i) {
        float* query = 0;
        size_t num_eles = 0;
        bool res = new_alg.get(i->first, query, num_eles);

        size_t hash = new_alg.hash(i->first);
        if(*query != i->second){
            std::cout << "Failed: " << i->first << " " <<i->second << " " << query << std::endl;
            std::cout << "Failed hash: " << hash << std::endl;
            std::cout << "Data: " << (float_data[hash]) << std::endl;
        }
        else {
            /* std::cout << "Pass: " << i->first << " " <<i->second << " " << query << std::endl; */
        }
    }
    float* query = 0;
    size_t num_eles = 0;
    std::cout << "illegal hash " << new_alg.get("asdfasdfasdf", query, num_eles) << query << std::endl;
};

void test_vector()
{
    std::map<std::string, std::vector<uint32_t>> file = load_node_cilacs();
    std::string * data = generate_array(file);

    phf * alg_def = new phf;
    PHF::init<std::string, false>(alg_def, data, file.size(), 1, 
            100, (phf_seed_t)0);
    std::cout << "Finished_building hash" << std::endl;

    int** int_data = write_binary(alg_def, file);
    std::cout << "Finished writing hash " << std::endl;
    std::cout << "FIRST ELE: " << int_data[0][0] << std::endl;

    ShareMemDict<uint32_t> new_alg = reset_shared_mem<uint32_t>("file.binary");
    for (auto i = file.begin(); i != file.end(); ++i) {
        uint32_t* query = 0;
        size_t eles;
        new_alg.get(i->first, query, eles);
        size_t hash = new_alg.hash(i->first);
        for (int j = 0; j < eles; j++) {
            std::cout << "HASH " << hash << " RETURNED " << (query[j])  << " data " << int_data[hash][j] << std::endl;
        }

        /* if(query[0] != int_data[hash]){ */
        /*     std::cout << "Failed: " << i->first << " " <<i->second << " " << query << std::endl; */
        /*     std::cout << "Failed hash: " << hash << std::endl; */
        /*     std::cout << "Data: " << (int_data[hash]) << std::endl; */
        /* } */
    }
}

int main(int argc, char *argv[])
{
    test_vector();
}
