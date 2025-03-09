#include <iostream>
#include <thread>

//volatile bool done = false;
//volatile int *bound;
//int error ;
//
//void ThreadFunc1()
//{
//	for(int j = 0 ;  j<=250000000; ++j)
//	{
//		*bound = -(1+*bound);
//		done = true;
//	}
//}
//void ThreadFunc2()
//{
//	while(!done)
//	{
//		int v = *bound;
//		if((v!=0)&& (v!=-1))
//		{
//			std::cout<<"??"<<std::endl;
//			error++;
//		}
//	}
//}
//
//int main()
//{
//	int a = 0;
//	bound = &a;
//	std::thread t1(ThreadFunc1);
//	std::thread t2(ThreadFunc2);
//	t1.join();
//	t2.join();
//	std::cout << error << std::endl;
//	
//}



volatile bool done = false;
volatile int *bound;
int error ;

void ThreadFunc1()
{
	for(int j = 0 ;  j<=250000000; ++j)
	{
		*bound = -(1+*bound);
		done = true;
	}
}
void ThreadFunc2()
{
	while(!done)
	{
		int v = *bound;
		if((v!=0)&& (v!=-1))
		{
			std::cout<<"??"<<std::endl;
			error++;
		}
	}
}
int main()
{
	int ARR[128];
	long long temp = (long long) & ARR[127];
	temp = temp - (temp % 64);
	temp-=2;
	bound = (int*)temp;
	*bound = 0;

	std::thread t1(ThreadFunc1);
	std::thread t2(ThreadFunc2);
	t1.join();
	t2.join();
	std::cout << error << std::endl;

}