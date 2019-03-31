#include <bits/stdc++.h>
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


semaphore barber(0);
semaphore aquireSeat(1); 
semaphore seekCustomer(0) ;  
int availableSeats = 20;    

void *Barber(void *ptr) {
  while(true) {
    seekCustomer.Wait();            
    aquireSeat.Wait()   ; 
    cout << "Barber is cutting hair" << endl;
    availableSeats += 1;   
    cout << "Free chair in waiting room " << availableSeats << endl;
    usleep(500000);
    barber.Signal();       
    aquireSeat.Signal();      
  } 

}
void *Customer(void *ptr) {
  while(true) {
  	aquireSeat.Wait();
      cout << "New customer entered the waiting room" << endl;
  	if (availableSeats > 0) {
  		availableSeats--;
        cout << "Free seats in waiting room: " <<availableSeats << endl;
        usleep(500000);
  		seekCustomer.Signal();
  		aquireSeat.Signal();
  		barber.Wait();
  	} else {
        cout << "No free seats in the waiting room."<<endl;
  		aquireSeat.Signal();
  	}   
  } 

}

int main() 
{ 
    int num_threads = 15; 
    pthread_t thread_id[num_threads]; 
    pthread_t barber_thread;
    for(int i=0; i<num_threads; i++)
    {
    pthread_create(&thread_id[i], NULL, Customer, NULL); 
    }
    pthread_create(&barber_thread, NULL, Barber, NULL); 

    for(int i=0; i<num_threads; i++){
        pthread_join(thread_id[i], NULL);
    }

    pthread_join(barber_thread, NULL);
}