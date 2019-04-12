#include "hash_map.h"
#include <stdlib.h>

KeyToInt * create_nodes()
{
    KeyToInt* key_storage = malloc(sizeof(KeyToInt));
    key_storage->key = 0;
    key_storage->val = 1;
    return key_storage;
};

