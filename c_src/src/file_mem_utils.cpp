#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include <map>

#include <sstream>

#include "phf.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
using namespace boost::interprocess;

enum Data_Type {  
    FLOAT,
    INT32,
    INT64,
    ARRAY_INT64
};

struct HashTarget {
    /* HashTarget(size_t key_hash, V* value_offset, size_t num_of_items) : */ 
    /*     key_hash(key_hash), value(value_address), num_of_items(num_of_items){} */
    HashTarget(){}
    HashTarget(size_t key_hash, size_t value_address) : 
        key_hash(key_hash), value(value_address), num_of_items(1){}

    size_t key_hash;
    size_t value; 
    size_t num_of_items;
};

template <class V>
struct ShareMemDict {
    bool is_raw_type;
    size_t offset_to_data;
    phf* alg;
    mapped_region* region;
    std::hash<std::string> hasher;

    bool get(std::string to_hash, V& r_val){
        void* offset = static_cast<char*>(region->get_address()) + offset_to_data;
        size_t actual_hash = hasher(to_hash);
        size_t hash_offset = PHF::hash(alg, to_hash);

        size_t* hash_targ = (static_cast<size_t*>(offset) + 3 * hash_offset);
        if(*hash_targ != actual_hash){
            std::cout << "Hash not found: " << *hash_targ << " " << actual_hash << std::endl;
            r_val = 0;
            return false;
        }
        size_t* hash_val = (static_cast<size_t*>(offset) + 3 *  alg->r);
        float* data = reinterpret_cast<float*>(hash_val) + PHF::hash(alg, to_hash);

        r_val = *data;
        return true;
    }

    size_t hash(std::string to_hash){
        return PHF::hash(alg, to_hash) ;
    }
};

mapped_region* mem_exists(std::string name){
    try
    {
        shared_memory_object shm (open_only, name.c_str(), read_only);
        mapped_region* region = new mapped_region(shm, read_only);
        return region;
    }
    catch (const std::exception &ex) {
        std::cout << "SharedMem Doesn't exist... creating." << std::endl;
    }
    return 0;
};

template <class T>
ShareMemDict<T> load_alg_from_mem(mapped_region* region){
    ShareMemDict<T> data; 
    void* address = region->get_address();
    phf* new_alg = new phf;

    // No div get
    bool* no_div_addr = static_cast<bool*>(address);
    new_alg->nodiv = *no_div_addr++;

    phf_seed_t* seed_adr = reinterpret_cast<phf_seed_t*>(no_div_addr);
    new_alg->seed = *seed_adr++;

    size_t* size_t_adr = reinterpret_cast<size_t*>(seed_adr);
    new_alg->r = *size_t_adr++;
    new_alg->m = *size_t_adr++;
    new_alg->d_max = *size_t_adr++;

    decltype(new_alg->g_op)* g_op_adr = reinterpret_cast<decltype(new_alg->g_op)*>(size_t_adr);
    new_alg->g_op = *g_op_adr++;

    new_alg->g_jmp = 0;

    new_alg->g = new uint32_t[new_alg->r];
    memcpy(new_alg->g, g_op_adr, sizeof(uint32_t) * new_alg->r);


    uint32_t* alg_offset = reinterpret_cast<uint32_t*>(g_op_adr);
    alg_offset += new_alg->r;
    size_t offset = (size_t) alg_offset - (size_t) region->get_address();
    std::cout << (int) offset << std::endl;

    data.alg = new_alg;
    data.is_raw_type = true;
    data.offset_to_data = offset;
    data.region = region;

    return data;
}

template <class T>
ShareMemDict<T> load_shared_mem(std::string name){
    mapped_region* region = mem_exists(name);
    if(!region){
        std::ifstream in(name, std::ifstream::ate | std::ifstream::binary);
        shared_memory_object shdmem_obj{create_only, name.c_str(), read_write};
        shdmem_obj.truncate(in.tellg());
        std::cout << "File size: " << in.tellg() << std::endl;

        mapped_region write_region{shdmem_obj, read_write};
        std::ifstream in_writer(name, std::ifstream::binary);
        in_writer.read(static_cast<char*>(write_region.get_address()), write_region.get_size());

        region = new mapped_region(shdmem_obj, read_only);
    }
    return load_alg_from_mem<T>(region);
}

template <class T>
ShareMemDict<T> reset_shared_mem(std::string name){
    shared_memory_object::remove(name.c_str());
    return load_shared_mem<T>(name);
}

float* write_binary(phf * p_hash, const std::map<std::string,float>& data){
    /* size_t num = p_hash-> */
    std::cout << "TOTAL: " << p_hash->r << std::endl;
    std::cout << "In hash : " << p_hash->m << std::endl;
    size_t bytes = sizeof(bool) + sizeof(phf_seed_t) + (3 * sizeof(size_t)) + sizeof(p_hash->g_op) +
        (sizeof(uint32_t) * p_hash->r);
    std::cout << "Size: " << bytes << std::endl;

    FILE* pFile;
    pFile = fopen("file.binary", "wb");
    fwrite(&(p_hash->nodiv), sizeof(bool), 1, pFile);
    fwrite(&(p_hash->seed), sizeof(phf_seed_t), 1, pFile);
    fwrite(&(p_hash->r), sizeof(size_t), 1, pFile);
    fwrite(&(p_hash->m), sizeof(size_t), 1, pFile);
    fwrite(&(p_hash->d_max), sizeof(size_t), 1, pFile);
    fwrite(&(p_hash->g_op), sizeof(p_hash->g_op), 1, pFile);
    fwrite(p_hash->g, sizeof(uint32_t), p_hash->r, pFile);

    float* as_array = new float[p_hash->r];
    HashTarget* hash_targets = new HashTarget[p_hash->r];
    std::hash<std::string> hasher;
    for (std::map<std::string,float>::const_iterator i = data.begin(); i != data.end(); ++i) {
        size_t hash = PHF::hash(p_hash, i->first);
        size_t key_hash = hasher(i->first);
        as_array[hash] = i->second;
        hash_targets[hash] = HashTarget(key_hash, hash);
    }
    for (size_t i = 0; i < p_hash->r; ++i){
        HashTarget* data = hash_targets++;
        fwrite(&((data)->key_hash), sizeof(size_t), 1, pFile);
        fwrite(&(data)->value, sizeof(size_t), 1, pFile);
        fwrite(&(data)->num_of_items, sizeof(size_t), 1, pFile);
    }
    float* iterator = as_array;
    for(size_t i = 0; i < p_hash->r; ++i){
        float * data = iterator++;
        fwrite(data, sizeof(*data), 1, pFile);
    }
    fclose(pFile);

    return as_array;
};

phf* read_binary_hash(){
    FILE* pFile = std::fopen("file.binary", "rb");

    phf * p_hash = new phf;
    fread(&(p_hash->nodiv), sizeof(bool), 1, pFile);
    fread(&(p_hash->seed), sizeof(phf_seed_t), 1, pFile);
    fread(&(p_hash->r), sizeof(size_t), 1, pFile);
    fread(&(p_hash->m), sizeof(size_t), 1, pFile);
    fread(&(p_hash->d_max), sizeof(size_t), 1, pFile);
    fread(&(p_hash->g_op), sizeof(p_hash->g_op), 1, pFile);
    p_hash->g_jmp = 0;
    p_hash->g = new uint32_t[p_hash->r];
    fread(p_hash->g, sizeof(uint32_t), p_hash->r, pFile);
    fclose(pFile);

    return p_hash;
}

bool check_phfs(phf* first, phf* second){
    if(first->nodiv != second->nodiv){
        std::cout << "nodiv error" << std::endl;
        return false;
    }
    if(first->seed != second->seed){
        std::cout << "seed error" << std::endl;
        return false;
    }
    if(first->r != second->r){
        std::cout << "r error" << std::endl;
        return false;
    }
    if(first->m != second->m){
        std::cout << "m error" << std::endl;
        return false;
    }
    if(first->d_max != second->d_max){
        std::cout << "d_max error" << std::endl;
        return false;
    }
    if(first->g_op != second->g_op){
        std::cout << "g_op error" << std::endl;
        return false;
    }

    if((first->g_jmp) != (second->g_jmp)){
        std::cout << "g_jmp error" << std::endl;
        return false;
    }
    for(size_t i = 0; i != first->r; ++i)
    {
        if(first->g[i] != second->g[i] ){
            std::cout << "g error at item " << i << " " << first->g[i] << " " << second->g[i] << std::endl;
            return false;
        }
    }
    
    return true;
}
