#include <stdio.h>
enum Data_Type {  
    FLOAT,
    INT32,
    INT64,
    ARRAY_INT64
};

void write_binary(phf * p_hash, float* data){
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
    fclose(pFile);
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
    for(size_t i = 0; i != first->r; ++i)
    {
        if(first->g[i] != second->g[i] ){
            std::cout << "g error at item " << i << " " << first->g[i] << " " << second->g[i] << std::endl;
            return false;
        }
    }
    
    return true;
}
