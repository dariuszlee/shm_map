#include <unistd.h>

#include <sys/mman.h> 
#include <sys/stat.h> /* For mode constants */ 
#include <fcntl.h>

struct shared_data *global;

bool has_been_run = false;

struct shared_data {
    int i;
};

__attribute__ ((constructor)) void init_lib(void)
{
    has_been_run = true;
    /* std::cout << "Start Init" << std::endl; */
    /* int fd = shm_open("/tmp/", O_RDWR | O_CREAT | O_EXCL, 0660); */
    /* std::cout << "PRINTING RANDOM STUFF" << fd << std::endl; */
    /* std::cout << "PRINTING RANDOM STUFF" << fd << std::endl; */
    /* ftruncate(fd, sizeof(struct shared_data)); */
    /* global = static_cast<shared_data*>(mmap(NULL, sizeof(struct shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)); */
    /* global->i = 100; */
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

// A function always returning one
int SampleFunction2()
{
    // insert code here
    
    return 1;
}

int SharedMem()
{
    printf("PRINTING PRINTING");
    if(!has_been_run){
        init_lib();
    }
    return global->i;
}
