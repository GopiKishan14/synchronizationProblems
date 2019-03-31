#include <queue>
#include <atomic>
#include <unistd.h>
#include <pthread.h>
#include <mutex>
#include <stdio.h>
#include <iostream>

using namespace std;

class semaphore
{
  private:
	mutex MUTEX;
	atomic<int> s_value;

  public:
	semaphore(int init) : s_value(init) {}

	void Wait()
	{
		while (1)
		{
			while (s_value <= 0)
				;
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


mutex buffer_mutex;
semaphore fillCount(0);
semaphore emptyCount(15);
std::queue<int> buffer;


void produceItem()
{
	int random_num = rand();
	cout << "Random Item pushed into buffer : " << random_num << endl;
	buffer.push(random_num);
}

void consumeItem()
{
	cout << "Consumed Item: " << buffer.front() << " buffer size: " << buffer.size() << endl;
	buffer.pop();
}


void *produce(void *ptr)
{
	while (true)
	{
		emptyCount.Wait();
		buffer_mutex.lock();

		produceItem();
		usleep(150000);
		buffer_mutex.unlock();

		fillCount.Signal();
	}
}

void *consume(void *ptr)
{
	while (true)
	{
		fillCount.Wait();
		buffer_mutex.lock();
		consumeItem();
		usleep(300000);
	
		buffer_mutex.unlock();
		emptyCount.Signal();
		
	}
}

int32_t main()
{
	int prod_threads = 4;
	int cons_threads = 2;
	srand(time(NULL));
	pthread_t Producers[prod_threads];
	pthread_t Consumers[cons_threads];
	
	for (int i = 0; i < prod_threads; i++) 
        pthread_create(&Producers[i], NULL, produce, NULL);

	for (int i = 0; i < cons_threads; i++) 
        pthread_create(&Consumers[1], NULL, consume, NULL);


	for (int i = 0; i < prod_threads; i++) 
        pthread_join(Producers[i], NULL); 

	for (int i = 0; i < cons_threads; i++) 
        pthread_join(Consumers[i], NULL); 
	

	pthread_exit(NULL);
}