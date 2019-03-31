#include <iostream>
#include <pthread.h>
#include <queue>
#include <atomic>
#include <mutex>
#include <unistd.h>

using namespace std;

class semaphore {
private:
	mutex MUTEX;
	atomic<int> s_value;
public:
	semaphore(int init): s_value(init) {}
	void Wait() {
		while (true) {
			while (s_value <= 0);
			MUTEX.lock();
			if (s_value <= 0) { MUTEX.unlock(); continue; }
			s_value--;
			MUTEX.unlock();
			break;
		}
	}
	void Signal() { s_value++; }
};

mutex IO_MUTEX;
semaphore writer_semaphore(1);
std::queue<int> Buffer;

atomic<int> read_count{0};

void WriteBuffer() {
	int item = rand();
	cout << "Write data: " << item << " to Buffer" <<endl;
	usleep(35000);
	Buffer.push(item);
}

int ReadBuffer() {
	int item = Buffer.front();
	cout << "Read data: "<< Buffer.front() << endl;
	usleep(20000);
	Buffer.pop();
	return item;
}

void *Write(void *B)
{
    while (true) 
    {
		writer_semaphore.Wait();
		WriteBuffer();
		writer_semaphore.Signal();
    }
}

void *Read(void *B) 
{
    while (true) 
    {
    	IO_MUTEX.lock();
		read_count++;
		if (read_count == 1) writer_semaphore.Wait();
		IO_MUTEX.unlock();

		ReadBuffer();

		IO_MUTEX.lock();
		read_count--;
		if (read_count == 0) writer_semaphore.Signal();
		IO_MUTEX.unlock();
    }
}

int32_t main()
{
	srand(time(NULL));
	pthread_t Reader;
	pthread_t Writer;
	pthread_create(&Writer, NULL, Write, NULL);
	pthread_create(&Reader, NULL, Read, NULL);
	pthread_exit(NULL);
}
