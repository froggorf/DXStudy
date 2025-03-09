#include <array>
#include <atomic>
#include <functional>
#include <iostream>
#include <thread>

template <typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::atomic<Node*> head; // 소비자가 읽는 포인터
    std::atomic<Node*> tail; // 생산자가 쓰는 포인터

public:
    LockFreeQueue() {
        Node* dummy = new Node(T{}); // 더미 노드 생성
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        while (Node* node = head.load()) {
            head.store(node->next);
            delete node;
        }
    }

    // 생산자: 데이터 추가
    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        Node* oldTail = tail.exchange(newNode); // tail을 원자적으로 교체
        oldTail->next = newNode; // 이전 tail의 next를 새로운 노드로 설정
    }

    // 소비자: 데이터 처리
    bool dequeue(T& result) {
        Node* oldHead = head.load();
        Node* next = oldHead->next;

        if (next == nullptr) {
            // 큐가 비어 있음
            return false;
        }

        result = next->data; // 다음 노드의 데이터를 읽음
        head.store(next);    // head를 다음 노드로 이동
        delete oldHead;      // 이전 head 삭제
        return true;
    }
};




LockFreeQueue<std::function<int()>> TestQueue;
std::atomic<bool> end;

std::array<bool, 100'000'000> check{ false, };

void ThreadFunc1(int ThreadID)
{
    end = false;
    int value = ThreadID*12'500'000;
    for(int i = 0; i < 12'500'000; ++i)
    {
        int input = value+i;
		TestQueue.enqueue([input]() {
			check[input] = true;
            return input;
			});
    }
    bool expected = false;
    std::atomic_compare_exchange_strong(&end, &expected, true);
}

void ThreadFunc2()
{
    std::function<int()> value;

    while (true)
    {
        if(TestQueue.dequeue(value))
        {
            value();
			//std::cout << val << std::endl;
        }
        else
        {
            if(end.load())
            {
                break;
            }
            else
            {
                std::this_thread::yield();
            }
        }

    }

	std::cout << "End" << std::endl;
	for (int i = 0; i < 100'000'000; ++i)
	{
		if (!check[i])
		{
			std::cout << "Error" << std::endl;
		}
	}
}
int main() {
    auto t = std::chrono::high_resolution_clock::now();
    std::thread t1(ThreadFunc1,0);
    std::thread t3(ThreadFunc1,1);

    std::thread t4(ThreadFunc1,2);

	std::thread t5(ThreadFunc1,3);
    std::thread t6(ThreadFunc1,4);
    std::thread t7(ThreadFunc1,5);
    std::thread t8(ThreadFunc1,6);
    std::thread t9(ThreadFunc1,7);
    std::thread t2(ThreadFunc2);

    t1.join();
    t3.join();
	t4.join();
	t5.join();
    t2.join();
	t6.join();
	t7.join();
	t8.join();
	t9.join();
    auto d = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(d - t).count() << std::endl;
    // 쓰레드 1개 - 6835ms
	// 쓰레드 2개 - 7524ms
    // 쓰레드 4개 - 7995ms
    // 쓰레드 8개 - 9085ms
    return 0;
}