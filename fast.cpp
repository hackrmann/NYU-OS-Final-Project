#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

int num_threads = 4;
unsigned int *buf;
pthread_t *threads;

struct thread_data
{
    unsigned int thread_id; 
    unsigned int size;
    unsigned int xor_result;
};

double now()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

double get_rate(double size, double start, double end)
{
    return size / ((end - start) * 1024 * 1024);
}

void print_error(string s)
{
    cout << "Error! " << s << endl;
    exit(EXIT_FAILURE);
}
void *xorbuf(void *arg)
{
    struct thread_data *args;
    args = (struct thread_data *)arg;
    long tid = args->thread_id;
    long size = args->size;

    unsigned int result = args->xor_result;
    for (int i = tid; i < size; i += num_threads)
    {
        // if(buf[i]!=0) cout<<buf[i]<<" thread "<<tid<<" "<<i<<endl;
        result ^= buf[i];
    }
    args->xor_result = result;
    pthread_exit(NULL);
    return NULL;
}

void multithreaded_xor(unsigned int no_of_elements, struct thread_data td[])
{
    unsigned int final_xor = 0;
    for (int i = 0; i < num_threads; i++)
    {
        td[i].size = no_of_elements;
        td[i].thread_id = i;
        pthread_create(&threads[i], NULL, xorbuf, (void *)&td[i]);
    }
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char *argv[])
{
    unsigned int block_size = 16777216, final_xor = 0;  //16MB of buffer
    double start, end;
    string file_name = "";
    struct thread_data td[num_threads];

    for (int i = 0; i < num_threads; i++)
        td[i].xor_result = 0;

    if (argc != 2)
        print_error("Check arguments!");

    else
    {
        file_name = argv[1];
    }

    srand(time(NULL));

    unsigned int no_of_elements = (unsigned int)(block_size / sizeof(int));
    unsigned int size_of_buf = no_of_elements * sizeof(int);
    buf = (unsigned int *)malloc(size_of_buf);
    // memset(buf,0,no_of_elements*sizeof(int));
    start = now();
    ifstream object;
    object.open(file_name, ios::binary);
    if (object.fail())
        print_error("Cannot read file!");
    else
    {
        threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
        if (!threads)
            perror("out of memory for threads!");

        while (object.read((char *)buf, size_of_buf))
            multithreaded_xor(no_of_elements, td);

        if (object.gcount() < block_size && object.gcount() > 0)
            multithreaded_xor(object.gcount() / sizeof(unsigned int), td);

        for (int i = 0; i < num_threads; i++)
            final_xor = final_xor ^ td[i].xor_result;
            
        end = now();
        cout << "Time taken: " << (end - start) << " seconds" << endl << 
                "Block size: " << block_size << " bytes" << endl;
        printf("Xor value is %08x", final_xor);
    }

    cout << "\n";
    return 0;
}