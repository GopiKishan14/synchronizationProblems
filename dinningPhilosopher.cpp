#include <stdio.h>
#include <iostream>
#include <mutex>
#include <queue>
#include <atomic>
#include <unistd.h>
#include <pthread.h>
using namespace std;

class semaphore {
private:
	mutex MUTEX;
	atomic<int> s_value;
public:
	semaphore(int init = 1): s_value(init) {}
	void Wait() {
		while (1) {
			while (s_value <= 0);
			MUTEX.lock();
			if (s_value <= 0) {
				MUTEX.unlock();
				continue;
			}
			s_value--;
			MUTEX.unlock();
			break;
		} 
	}
	void Signal() {
		s_value++;
	}
};

std::vector<semaphore> SEMAPHORE_PHILOSOPHERS(5);
mutex Eat_Semaphore;

int PHILOSOPHERS_LIST[5] = { 0, 1, 2, 3, 4 }; 
  
void* Dinner(void* number) 
{ 
    int* num = (int *)number; 
    while (1) {
  		Eat_Semaphore.lock();
  		SEMAPHORE_PHILOSOPHERS[*num].Wait();
  		SEMAPHORE_PHILOSOPHERS[(*num + 1) % 5].Wait();
    	Eat_Semaphore.unlock();
    	cout << "Philosopher "<< *num <<" is dining" << endl ;
    	sleep(2);
  		SEMAPHORE_PHILOSOPHERS[*num].Signal();
  		SEMAPHORE_PHILOSOPHERS[(*num + 1) % 5].Signal();
        usleep((rand()) % 1000 * 1000);
    } 
}
 
int main() 
{ 
	srand(time(NULL));
    pthread_t thread_id[5]; 
    for (int i = 0; i < 5; i++) { 
        pthread_create(&thread_id[i], NULL, Dinner, &PHILOSOPHERS_LIST[i]); 
    }
    for (int i = 0; i < 5; i++) 
        pthread_join(thread_id[i], NULL); 
}