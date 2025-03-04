#include <iostream>
#include <chrono>
#include <mutex>

// 피터슨 알고리즘

volatile int victim = 0;
volatile bool flag[2] = { false,false };

volatile int sum = 0;
std::mutex m;

void Lock(int myID) 
{
	int other = 1 - myID;
	flag[myID] = true;
	victim = myID;
	while (flag[other] && victim == myID) {}	// 대기루프
}


void UnLock(int myID) {
	flag[myID] = false;
}

void thread_func(int thisId) {
	for (int i = 0; i < 50'000'000; ++i) {
		// 이부분이 임계영역
		{
			//m.lock();
			Lock(thisId);
			sum += 1;
			UnLock(thisId);
			//m.unlock();
		}
		
	}
	
}

int main() {
	//InitializeCriticalSection(&cs_lock);
	auto t = std::chrono::high_resolution_clock::now();
	std::thread t1(thread_func,0);
	std::thread t2(thread_func,1);

	t1.join();
	t2.join();

	auto d = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(d - t).count() << std::endl;
	std::cout << sum << std::endl;

	// 락의 경우 11286 ms / 결과는 정상
	// 피터슨 알고리즘의 경우 
	// 11843ms 결과는 DataRace 때 만큼은 아니지만 비정상

}