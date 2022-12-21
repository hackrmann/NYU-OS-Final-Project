#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

int num_threads = 4;
char *buffer;
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

void print_performance(double size, double start, double end, unsigned int block_count, unsigned int final_xor, int flag)
{
    cout << "Number of blocks read: " << block_count << " blocks";
    if (flag == 1)
    {
        cout << " (file not fully read, program exceeded time limit of 15s and was terminated)";
    }
    cout << endl;
    cout << "Size read: " << (size / (1024 * 1024)) << " MB" << endl;
    // cout << "Size of the file read in bytes (Number of system calls): " << (size) << " B" << endl;
    // cout << "Number of system calls per second: " << (size/(end-start)) << " B/sec" << endl;
    cout << "Time taken: " << (end - start) << " seconds" << endl;
    cout << "Rate at which file was read: " << get_rate(size, start, end) << "MiB/sec" << endl;
    printf("Xor value is %08x", final_xor);
}

void *xorbuf(void *arg)
{

    struct thread_data *args;
    args = (struct thread_data *)arg;
    long tid = args->thread_id;
    long size = args->size;

    unsigned int result = 0;
    for (int i = tid; i < size; i += num_threads)
    {
        // if(buf[i]!=0) cout<<buf[i]<<" thread "<<tid<<" "<<i<<endl;
        result ^= buf[i];
    }
    args->xor_result = result;
    pthread_exit(NULL);
}

unsigned int multithreaded_xor(unsigned int no_of_elements, struct thread_data td[])
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
    for (int i = 0; i < num_threads; i++)
    {
        final_xor = final_xor ^ td[i].xor_result;
    }
    return final_xor;
}

int main(int argc, char *argv[])
{
    unsigned int block_size = 0, block_count = 0, final_xor = 0, size = 0;
    double start, end;
    string file_name = "";
    struct thread_data td[num_threads];

    if (argc != 3)
        print_error("Check arguments!");

    else
    {
        file_name = argv[1];
        block_size = (unsigned int)stoi(argv[2]);
    }

    srandom(time(NULL));

    unsigned int no_of_elements = (unsigned int)((block_size + sizeof(int) - 1) / sizeof(int));
    unsigned int size_of_buf = no_of_elements * sizeof(int);
    int flag = 0;
    buf = (unsigned int *)malloc(size_of_buf);
    // memset(buf,0,no_of_elements*sizeof(int));
    // cout<<no_of_elements<<" ----- "<<size<<" ----- "<<sizeof(buf)<<"-----"<<(no_of_elements * sizeof( unsigned int))<<endl;

    start = now();
    ifstream object;
    object.open(file_name, ios::binary);
    if (object.fail())
        print_error("Cannot read file!");
    else
    {
        cout << "Reading " << file_name << " in chunks of " << block_size << " ..... " << endl;
        // cout<<"buf4-----"<<buf[4]<<endl;
        threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
        if (!threads)
            perror("out of memory for threads!");

        while (object.read((char *)buf, size_of_buf))
        {
            final_xor ^= multithreaded_xor(no_of_elements, td);
            block_count++;
            size += object.gcount();
            // if ((end = now()) - start > 15)
            // {
            //     flag = 1;
            //     break;
            // }
        }
        size = block_count*block_size;
        if (object.gcount() < block_size && object.gcount() > 0 && flag == 0)
        {
            final_xor ^= multithreaded_xor(object.gcount() / sizeof(unsigned int), td);
            block_count++;
            size += object.gcount();
        }
        // cout<<"----"<<object.gcount()<<endl;
        end = now();
        print_performance(size, start, end, block_count, final_xor, flag);
    }

    cout << "\n";
    return 0;
}