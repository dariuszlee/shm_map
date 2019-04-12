#include <unistd.h>

#include <sys/mman.h> 
#include <sys/stat.h> /* For mode constants */ 
#include <fcntl.h>

#include <stdio.h>

struct shared_data *global;

int has_been_run = 0;

struct shared_data {
    int i;
};

void load_dumps()
{
    FILE* fd = fopen("/home/dariuslee/motionlogic/dumps/cilac-nodes.csv", "r");
    char line [ 128 ];
    while (fgets(line, sizeof(line), fd) != NULL ) /* read a line */
    {
        fputs(line, stdout); /* write the line */
    }
    fclose (fd);
}

__attribute__ ((constructor)) void init_lib(void)
{
    has_been_run = 1;
    printf("Start Init\n");
    int fd = -1;
    if( access( "/dev/shm/libmem", F_OK ) != -1 ) {
        fd = shm_open("/libmem", O_RDWR, 0); 
        global = mmap(NULL, sizeof(struct shared_data), PROT_READ, MAP_SHARED, fd, 0);
    } 
    else {
        fd = shm_open("/libmem", O_RDWR, 0); 
        printf("FD is %i.\n", fd);
        int ret_val = ftruncate(fd, sizeof(struct shared_data));
        printf("TRUCNATE RESULT: %i", ret_val);
        global = mmap(NULL, sizeof(struct shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        global->i = 100;
        load_dumps();
    }
}

// A function adding two integers and returning the result
int SampleAddInt(int i1, int i2)
{
    return i1 + i2;
}

// A function doing nothing ;)
void SampleFunction1()
{
    // insert code here
}

int SampleFunction2()
{
    // insert code here
    
    return 1;
}

void AddToMemory()
{
    global->i += 1;
}

int SharedMem()
{
    if(!has_been_run){
        init_lib();
    }
    return global->i;
}
