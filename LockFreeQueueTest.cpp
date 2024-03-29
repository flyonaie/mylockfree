#include "LockFreeQueue.hpp"
#include <thread>
 
//#define USE_LOCK
 
class Test
{
public:
   Test(int id = 0, int value = 0)
   {
        this->id = id;
        this->value = value;
        sprintf(data, "id = %d, value = %d\n", this->id, this->value);
   }
 
   void display()
   {
      printf("%s", data);
   }
private:
   int id;
   int value;
   char data[128];
};
 
double getdetlatimeofday(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec + end->tv_usec * 1.0 / 1000000) -
           (begin->tv_sec + begin->tv_usec * 1.0 / 1000000);
}
 
LockFreeQueue<Test> queue(1 << 12, "/shm");
 
// #define N ((1 << 20))
#define N (10 * (1 << 20))
 
void produce()
{
    struct timeval begin, end;
label_Lp:
    gettimeofday(&begin, NULL);
    unsigned int i = 0;
    while(i < N)
    {
        // if(queue.push(Test(i >> 10, i)))
        if(queue.push(Test(i % 1024, i)))
            i++;
        
        // printf("pthread_self()tid=%lu, i = %d\n", pthread_self(), i);
    }
    gettimeofday(&end, NULL);
    double tm = getdetlatimeofday(&begin, &end);
    printf("i = %d, producer tid=%lu %f MB/s %f msg/s elapsed= %f size= %u\n", i, pthread_self(), N * sizeof(Test) * 1.0 / (tm * 1024 * 1024), N * 1.0 / tm, tm, i);
    goto label_Lp;
}
 
void consume()
{
    Test test;
    struct timeval begin, endtime;
label_L1:    
    gettimeofday(&begin, NULL);
    unsigned int i = 0;
    while(i < N)
    {
        if(queue.pop(test))
        {
           //test.display();
           i++;
        }
    }
    gettimeofday(&endtime, NULL);
    double tm = getdetlatimeofday(&begin, &endtime);
    printf("consumer i = %d, tid=%lu %f MB/s %f msg/s elapsed= %f size= %u\n", i, pthread_self(), N * sizeof(Test) * 1.0 / (tm * 1024 * 1024), N * 1.0 / tm, tm, i);
    goto label_L1;  
}
 
int main(int argc, char const *argv[])
{
    std::thread producer1(produce);
    std::thread producer2(produce);
    std::thread consumer(consume);
    producer1.join();
    producer2.join();
    consumer.join();
 
    return 0;
}