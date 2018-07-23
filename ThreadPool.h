//---------------------------------------------------------------------------

#ifndef ThreadPoolH
#define ThreadPoolH

#include <vcl.h>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <deque>
using namespace std;
using namespace boost;

//---------------------------------------------------------------------------
#define kMaxWorkersForGeneralExecutionPool			4
#define kMaxWorkersForLowPriorityExecutionPool		2
#define kMaxWorkersForDelayExecutionPool			1
#define kMaxTasksInPool								65535

//---------------------------------------------------------------------------
class ThreadPool
{
public:
	typedef struct {
		int delay;
		String desc;
		function < void() > task;
		function < void() > completion;
	}Item;

	deque<shared_ptr<Item> >tasks;
	deque<shared_ptr<TThread> >workers;
	shared_ptr<TCriticalSection>locker;
	shared_ptr<TSemaphore>semaphore;
	String name;

public:
	ThreadPool(int n, int mode, String name);
	~ThreadPool();
	void Submit(function < void() > task, String desc, int delay, function < void() > completion);
};

//---------------------------------------------------------------------------
void __dispatch_main(function < void() > handler, String desc, bool async = false);
void __dispatch_main_after(function < void() > handler, String desc, int seconds, bool async = false);
void __dispatch_async(function < void() > handler, String desc);
void __dispatch_async_serial(function < void() > handler, String desc);
void __dispatch_async_background(function < void() > handler, String desc);
void __dispatch_after(function < void() > handler, String desc, int seconds);

//---------------------------------------------------------------------------
#define __desc__ 										String::Format(L"%s Line %d %s", ARRAYOFCONST((__FILE__, __LINE__, __FUNCTION__))) 
#define dispatch_main(handler) 							__dispatch_main(handler, __desc__, false)
#define dispatch_main_async(handler)					__dispatch_main(handler, __desc__, true)
#define dispatch_main_after(handler, seconds) 			__dispatch_main_after(handler, __desc__, seconds, false)
#define dispatch_main_after_async(handler, seconds) 	__dispatch_main_after(handler, __desc__, seconds, true)
#define dispatch_async(handler) 						__dispatch_async(handler, __desc__)
#define dispatch_async_serial(handler) 					__dispatch_async_serial(handler, __desc__)
#define dispatch_async_background(handler) 				__dispatch_async_background(handler, __desc__)
#define dispatch_after(handler, seconds) 				__dispatch_after(handler, __desc__, seconds)
//---------------------------------------------------------------------------
#endif
