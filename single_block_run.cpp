#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

int num_threads = 8;
char *buffer;
unsigned int *buf;

struct thread_data
{
    int thread_id;
    int size;
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

void perror(string s)
{
    cout << "Error! " << s << endl;
    exit(0);
}

void print_performance(double size, double start, double end)
{
    cout<<"Read "<<(size/(1024*1024))<<" Megabytes in "<< (end - start) << " seconds" << endl;
    cout << "Rate in MiB/sec is " << get_rate(size, start, end) << endl;
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

int main(int argc, char *argv[])
{
    int block_size = -1, block_count = -1;
    bool read_mode = false, write_mode = false;
    double start, end;
    string file_name = "";
    pthread_t *threads;
    struct thread_data td[num_threads];

    // for (int i = 0; i < argc; i++)
    // {
    //     cout<<"arg--'"<<argv[i]<<"'"<<endl;
    // }

    if (argc != 5)
    {
        perror("Too few arguments!");
    }

    else
    {
        string s = argv[2];
        file_name = argv[1];
        read_mode = ("-r" == s || "-R" == s);
        write_mode = ("-w" == s || "-W" == s);
        block_size = stoi(argv[3]);
        block_count = stoi(argv[4]);
    }
    // cout<<"flag--"<<read_mode<<" "<<write_mode<<endl;

    srandom(time(NULL));
    unsigned int size = block_count * block_size;

    if (read_mode)
    {
        start = now();
        unsigned int no_of_elements = (unsigned int)(size / sizeof( unsigned int));
        buf = (unsigned int *)malloc(no_of_elements * sizeof( unsigned int));
        // cout<<no_of_elements<<" ----- "<<size<<" ----- "<<sizeof(buf)<<"-----"<<(no_of_elements * sizeof(int))<<endl;
        ifstream object;
        object.open(file_name, ios::binary);
        if (object.fail())
        {
            cout << "Can't read file " << file_name;
        }
        else
        {
            unsigned int final_xor = 0;
            cout << "Read " << file_name << endl;
            object.read((char *)buf, size);
            // cout<<"buf4-----"<<buf[4]<<endl;
            threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
            if (!threads)
            {
                perror("out of memory for threads!");
            }

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
            end = now();
            print_performance(size, start, end);
            printf("Xor value is %08x",final_xor);
        }
    }
    else if (write_mode)
    {
        start = now();
        ofstream object(file_name);
        buffer = new char[size];

        object.write(buffer, size);
        end = now();

        print_performance(size, start, end);
    }
    else
    {
        perror("Please check your arguments and specify if its -r/-w");
    }
    cout << "\n";
    return 0;
}