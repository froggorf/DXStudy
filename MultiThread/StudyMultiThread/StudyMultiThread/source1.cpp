#include <iostream>
#include <mutex>
#include <thread>
#include <Windows.h>

std::atomic<int> a;
//int a;
std::mutex mylock;
//CRITICAL_SECTION cs_lock;

void func()
{
	for(auto i = 0; i < 50000000; ++i)
	{
		//mylock.lock();
		//EnterCriticalSection(&cs_lock);
		//a= a+2;
		a.fetch_add(2);
		//a +=2;
		//LeaveCriticalSection(&cs_lock);
		//mylock.unlock();
	}
}

int finalSum = 0;
void FastFunc()
{
	volatile int local_sum = 0;
	for(auto i = 0 ; i < 50000000; ++i)
	{
		local_sum += 2;
	}
	mylock.lock();
	finalSum += local_sum;
	mylock.unlock();
}

int main()
{
	//InitializeCriticalSection(&cs_lock);
	auto t = std::chrono::high_resolution_clock::now();
	std::thread t1(FastFunc);
	std::thread t2(FastFunc);
	std::thread t3(FastFunc);
	std::thread t4(FastFunc);

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	auto d = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(d - t).count() << std::endl;
	std::cout << finalSum << std::endl;

	// Atomic - 2078 ms / 결과 정확
	//		여기서 Atomic을 a += 2 에서 a.fetch_add(2)로 진행시 1128ms 
	// Lock - 1818 ms / 결과 정확
	// No Lock - 20 ms / 결과 부정확

	// FastFunc - 4쓰레드 27ms / 결과 정확


}