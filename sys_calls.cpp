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

void print_performance(unsigned int size, double start, double end, unsigned int block_count, int flag)
{
    cout << "Number of blocks read: " << block_count << " blocks";
    if (flag == 1)
    {
        cout << " (file not fully read, program exceeded time limit of 15s and was terminated)";
    }
    cout << endl;
    cout << fixed << "Number of system calls: " << size << " B" << endl;
    cout << fixed << "Number of system calls per second: " << (size / (end - start)) << " B/sec" << endl;
    cout << "Time taken: " << (end - start) << " seconds" << endl;
}

void print_performance_seek(unsigned int size, double start, double end, int flag)
{
    if (flag == 1)
    {
        cout << " (file not fully read, program exceeded time limit of 15s and was terminated)" << endl;
    }
    cout << fixed << "Number of system calls: " << size << " B" << endl;
    cout << fixed << "Number of system calls per second: " << (size / (end - start)) << " B/sec" << endl;
    cout << "Time taken: " << (end - start) << " seconds" << endl;
}

int main(int argc, char *argv[])
{
    unsigned int block_size = 1, block_count = 0, read_sys_calls = 0, seek_sys_calls = 0;
    double start, end;
    string file_name = "";
    struct thread_data td[num_threads];

    if (argc != 2)
        print_error("Check arguments!");

    else
    {
        file_name = argv[1];
    }

    srandom(time(NULL));

    unsigned int no_of_elements = (unsigned int)((block_size + sizeof(int) - 1) / sizeof(int));
    unsigned int size_of_buf = no_of_elements * sizeof(int);
    int flag = 0;
    buf = (unsigned int *)malloc(size_of_buf);
    // memset(buf,0,no_of_elements*sizeof(int));
    // cout<<no_of_elements<<" ----- "<<size<<" ----- "<<sizeof(buf)<<"-----"<<(no_of_elements * sizeof( unsigned int))<<endl;

    ifstream object;
    object.open(file_name, ios::binary);
    if (object.fail())
        print_error("Cannot read file!");
    else
    {
        cout << "Reading " << file_name << " in chunks of " << block_size << " ..... " << endl;
        cout << "Checking overhead of read system call...." << endl;
        start = now();
        while (object.read((char *)buf, size_of_buf))
        {
            block_count++;
            read_sys_calls += 1;
        }
        end = now();
        print_performance(read_sys_calls, start, end, block_count, flag);

        cout << "\nChecking overhead of seek system call...." << endl;
        start = now();
        for (int i = 0; i < block_count; i++)
        {
            object.seekg((i >> 2));
            seek_sys_calls++;
        }
        end = now();
        print_performance_seek(seek_sys_calls, start, end, flag);
    }

    cout << "\n";
    return 0;
}