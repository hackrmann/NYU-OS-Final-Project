#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

int num_threads = 8;
char *buffer;
unsigned int *buf;

double now()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

struct thread_data
{
    int thread_id;
    int size;
    unsigned int xor_result;
};

void *xorbuf(void *arg)
{

    struct thread_data *args;
    args = (struct thread_data *)arg;
    long tid = args->thread_id;
    long size = args->size;

    unsigned int result = 0;
    for (int i = tid; i < size; i += num_threads)
    {
        result ^= buf[i];
    }
    args->xor_result = result;
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    pthread_t *threads;
    struct thread_data td[num_threads];
    if(argc!=2)
    {
        cout<<"Check your arguments! Must include only filename";
    }
    else
    {
        int start, end, size=0;
        string file_name = argv[2];
        start = now();
        int no_of_elements = (int)(size / sizeof(int));
        buf = (unsigned int *)malloc(no_of_elements * sizeof(int));
        ifstream object;
        object.open(file_name, ios::binary);
        if (object.fail())
        {
            cout << "Can't read file " << file_name;
        }
        else
        {
            unsigned int final_xor;
            cout << "Read " << file_name << endl;
            object.read((char *)buf, size);

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
            final_xor = td[0].xor_result;
            for (int i = 1; i < num_threads; i++)
            {
                final_xor = final_xor ^ td[i].xor_result;
            }
            end = now();
            cout << "XOR value: " << final_xor;
        }
    }
    cout<<endl;
    return 0;
}