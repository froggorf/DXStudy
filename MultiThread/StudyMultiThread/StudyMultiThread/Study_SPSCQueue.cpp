#include <atomic>
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

LockFreeQueue<int> queue;
std::atomic<bool> end;

std::atomic<int> LastValue;

void ThreadFunc1()
{
    end = false;
	for(int i = 0; i < 1'000'000'000; ++i)
	{
		queue.enqueue(i);
	}
	bool expected = false;
	std::atomic_compare_exchange_strong(&end, &expected, true);
}

void ThreadFunc2()
{
	int value;
    LastValue = -1;

	while (true)
	{
        if(queue.dequeue(value))
        {
            int expected = value-1;
			if(std::atomic_compare_exchange_strong(&LastValue, &expected, value))
			{
				std::cout << value << std::endl;
			} else
			{
				std::cout << "Error: " << value << std::endl;
			}
            
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
}
int main() {

	std::thread t1(ThreadFunc1);
    //std::thread t3(ThreadFunc1);
	std::thread t2(ThreadFunc2);

	t1.join();
	//t3.join();
	t2.join();

    return 0;
}