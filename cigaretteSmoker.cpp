#include <iostream>
#include <stdio.h>
#include <queue>
#include <atomic>
#include <mutex>
#include <unistd.h>
#include <pthread.h>
using namespace std;

class semaphore
{
  private:
    mutex MUTEX;
    atomic<int> s_value;

  public:
    semaphore(int init = 1) : s_value(init) {}
    void Wait()
    {
        while (1)
        {
            while (s_value <= 0);
            MUTEX.lock();
            if (s_value <= 0)
            {
                MUTEX.unlock();
                continue;
            }
            s_value--;
            MUTEX.unlock();
            break;
        }
    }
    void Signal()
    {
        s_value++;
    }
};

semaphore paper_tobacco(0);
semaphore paper_matches(0);
semaphore tobacco_matches(0);
semaphore Smoking_completed(1);


void smoke(string name)
{
    cout<< name <<" is currently Smoking"<< endl;
}

void *Dealer(void *ptr)
{
    while (true)
    {

        Smoking_completed.Wait();
        int rand_choice = rand() % 3;
        switch (rand_choice)
        {
        case 0:
            paper_tobacco.Signal();
            break;
        case 1:
            paper_matches.Signal();
            break;
        case 2:
            tobacco_matches.Signal();
            break;
        }
    }
}

void *Smoker_Alice(void *ptr)
{
    while (true)
    {
        paper_tobacco.Wait();
        smoke("Alice");
        usleep(500000);

        Smoking_completed.Signal();
    }
}

void *Smoker_Bob(void *ptr)
{
    while (true)
    {
        paper_matches.Wait();
        smoke("Bob");
        usleep(600000);
        Smoking_completed.Signal();
    }
}

void *Smoker_Charlie(void *ptr)
{
    while (true)
    {
        tobacco_matches.Wait();
        smoke("Charlie");
        usleep(500000);
        Smoking_completed.Signal();
    }
}

int main()
{
    int num_thread = 4;
    pthread_t thread_id[num_thread];
    
    pthread_create(&thread_id[0], NULL, Smoker_Alice, NULL);
    pthread_create(&thread_id[1], NULL, Smoker_Bob, NULL);
    pthread_create(&thread_id[2], NULL, Smoker_Charlie, NULL);
    pthread_create(&thread_id[3], NULL, Dealer, NULL);


    for(int i=0; i<num_thread; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
}