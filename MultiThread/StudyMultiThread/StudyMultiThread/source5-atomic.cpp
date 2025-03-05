//#include <atomic>
//
//int main()
//{
//	std::atomic<int> a,b;
//	a.store(1, std::memory_order::memory_order_release);
//	b.store(2, std::memory_order::memory_order_release);
//	int c = a.load(std::memory_order_acquire);
//	c = a.load(std::memory_order_seq_cst);
//	b.store(3, std::memory_order_seq_cst);
//
//}

#include <atomic>
#include <iostream>
#include <thread>

volatile int victim = 0;
std::atomic <unsigned char> flag[2] = {0, 0};
void Lock(int myId)
{
	int other = 1 - myId;
	flag[myId] = 1;
	victim = myId;
	while ((flag[other].fetch_add(0) == 1) && victim == myId);
}
void Unlock (int myId)
{
	flag[myId] = 0;
}

int sum = 0;
void ThreadFunc(int threadID)
{
	for(int i = 0; i< 5'000'000; ++i)
	{
		Lock(threadID);
		sum += 1;
		Unlock(threadID);
	}
}

int main()
{
	std::thread t1(ThreadFunc, 0);
	std::thread t2(ThreadFunc, 1);
	t1.join();
	t2.join();
	std::cout << sum << std::endl;
	return 0;
}