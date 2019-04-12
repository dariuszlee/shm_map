typedef struct {
    int key;
    int val;
} KeyToInt ;

typedef struct {
    int key;
    int size;
    int * val;
} key_to_array;

KeyToInt * create_nodes();
