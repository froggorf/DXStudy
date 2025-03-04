#include <iostream>
#include <chrono>
#include <vector>
#include <mutex>
#include <algorithm>

volatile int sum = 0;
int currentThreadCount = 1;

int threadCounts[] = { 1, 2, 4 , 8 };
//std::mutex m;



class Bakery {
	std::vector<bool> flags;
	std::vector<int> Labels;
public:
	Bakery() = default;
	Bakery(int n) {
		flags.resize(n);
		Labels.resize(n,0);
		
	}
	~Bakery() {}

	void Lock(int threadID)
	{
		flags[threadID] = true;
		Labels[threadID] = *std::max_element(Labels.begin(), Labels.end()) + 1;;
		flags[threadID] = false;

		for (int i = 0; i < flags.size(); ++i) {
			if (i == threadID) continue;

			while (flags[i]) {}
			while (Labels[i] != 0 &&
				(Labels[i] < Labels[threadID] ||
					(Labels[i] == Labels[threadID] && i < threadID))) {
			}
		}
	}

	void Unlock(int threadID) 
	{
		flags[threadID] = 0;
		Labels[threadID] = 0;
	}
};
std::unique_ptr<Bakery> bakery;

void thread_func(int thisID) 
{
	for (int i = 0; i < 50'000 / currentThreadCount; ++i) {
		bakery->Lock(thisID);
		sum += 1;
		//std::cout << thisID<<std::endl;
		bakery->Unlock(thisID);
	}
}

int main() {
	
	std::vector<std::thread> threads;
	for (int i = 0; i < 4; ++i) {

		auto t = std::chrono::high_resolution_clock::now();
		
		currentThreadCount = threadCounts[i];
		bakery = std::make_unique<Bakery>(currentThreadCount);
		sum = 0;
		threads.clear();
		for (int threadCountIndex = 0; threadCountIndex < currentThreadCount; ++threadCountIndex) {
			threads.push_back(std::thread{ thread_func, threadCountIndex });
		}
		for (auto& thread : threads) {
			thread.join();
		}
		auto d = std::chrono::high_resolution_clock::now();
		std::cout << "쓰레드 " << currentThreadCount << "개\n";
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(d - t).count() << std::endl;
		std::cout << sum << std::endl << std::endl; 

 	}

	

}

/*
* 결과

============= 1. Lock이 없을 때 ============= 
쓰레드 1개
93
10000000

쓰레드 2개
196
5204175

쓰레드 4개
128
2583212

쓰레드 8개
164
1487471
=============================================

============= 2. Lock이 있을 때 =============
쓰레드 1개
986
10000000

쓰레드 2개
1509
10000000

쓰레드 4개
1089
10000000

쓰레드 8개
1118
10000000
=============================================

============= 3. Bakery Algorithm =============
쓰레드 1개
19
1000

쓰레드 2개
13
1000

쓰레드 4개
49
1000

쓰레드 8개
48
1000
=============================================

*/