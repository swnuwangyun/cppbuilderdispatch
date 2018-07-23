//---------------------------------------------------------------------------

#pragma hdrstop

#include "ThreadPool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
static String TAG = L"Dispatcher";
static ThreadPool PoolForGeneralExecution(kMaxWorkersForGeneralExecutionPool, 0, L"async");
static ThreadPool PoolForSerialExecution(1, 0, L"serial");
static ThreadPool PoolForLowPriorityExecution(kMaxWorkersForGeneralExecutionPool, 0, L"background");
static ThreadPool PoolForDelayExecution(1, 1, L"after");

//---------------------------------------------------------------------------
ThreadPool::ThreadPool(int n, int mode, String threadPoolName) {
	// Init vars
	locker = shared_ptr<TCriticalSection>(new TCriticalSection());
	String name = String::Format(L"SemaphoreForThreadPool_%s_%d", ARRAYOFCONST((threadPoolName, GetTickCount())));
	semaphore = shared_ptr<TSemaphore>(new TSemaphore(NULL, 0, kMaxTasksInPool, name, false));
	this->name = threadPoolName;

	// Define work thread for pool
	class Runner : public TThread {
	private:
		ThreadPool *thiz;
		int mode;

	public:
		__fastcall Runner(ThreadPool *threadPool, int mode) {
			thiz = threadPool;
			this->mode = mode;
		}

		__fastcall void Execute() {
			while (true) {
				if (mode == 0) {
					// Wait until get a task, and execute it
					thiz->semaphore->WaitFor(INFINITE);
					shared_ptr<ThreadPool::Item>item;
					thiz->locker->Enter();
					{
						item = thiz->tasks.back();
						thiz->tasks.pop_back();
					}
					thiz->locker->Leave();
					if (item) {
						try {
							item->task();
						}
						catch (Exception &e) {
						}
						catch (...) {
						}
						__dispatch_main(item->completion, item->desc + L" completion");
					}
					else {
						break;
					}
				}
				else {
					// Counting down, and execute just one task
					Sleep(100);
					bool flag = false;
					thiz->locker->Enter();
					for (deque<shared_ptr<ThreadPool::Item> >::iterator i = thiz->tasks.begin(); i != thiz->tasks.end(); i++) {
						shared_ptr<ThreadPool::Item>item = *i;
						if (item) {
							if (item->delay > 0) {
								item->delay -= 100;
							}
							else {
								thiz->tasks.erase(i);
								thiz->semaphore->Acquire();
								__dispatch_async(item->task, item->desc);
								break;
							}
						}
						else {
							flag = true;
							break;
						}
					}
					thiz->locker->Leave();
					if (flag) {
						break;
					}
				}
			}
		}
	};

	// Create X worker thread and let them running
	for (int i = 0; i < n; i++) {
		shared_ptr<TThread>worker((TThread *)new Runner(this, mode));
		worker->FreeOnTerminate = false;
		workers.push_front(worker);
	}
}

//---------------------------------------------------------------------------
ThreadPool::~ThreadPool() {
	// Signal workers to terminate
	for (unsigned int i = 0; i < workers.size(); i++) {
		locker->Enter();
		shared_ptr<Item>item;
		tasks.push_front(item);
		locker->Leave();
		semaphore->Release();
	}
	// Wait until all works terminated
	for (deque<shared_ptr<TThread> >::iterator it = workers.begin(); it != workers.end(); it++) {
		(*it)->WaitFor();
	}
}

//---------------------------------------------------------------------------
void ThreadPool::Submit(function < void() > task, String desc, int delay, function < void() > completion) {
	// Push to task list
	locker->Enter();
	{
		shared_ptr<Item>item(new Item());
		item->task = task;
		item->desc = desc;
		item->delay = delay * 1000;
		item->completion = completion;
		tasks.push_front(item);
	}
	locker->Leave();
	// Notify number of task items
	semaphore->Release();
}

//---------------------------------------------------------------------------
void __dispatch_main(function < void() > handler, String desc, bool async) {
	if (handler) {
		struct notifier {
			function < void() > handler;
			String desc;

			notifier(function < void() > handler, String desc) {
				this->handler = handler;
				this->desc = desc;
			}

			__fastcall void notify() {
				handler();
			}
		};

		try {
			if (async) {
				TThread::Queue(TThread::CurrentThread, notifier(handler, desc).notify);
			}
			else {
				TThread::Synchronize(TThread::CurrentThread, notifier(handler, desc).notify);
			}
		}
		catch (...) {
		}
	}
}

//---------------------------------------------------------------------------
void __dispatch_main_after(function < void() > handler, String desc, int seconds, bool async) {
	struct task {
		static void execute(function < void() > handler, String desc, bool async) {
			__dispatch_main(handler, desc, async);
		}
	};

	__dispatch_after(boost::bind(&task::execute, handler, desc, async), desc, seconds);
}

//---------------------------------------------------------------------------
void __dispatch_async(function < void() > handler, String desc) {
	if (handler) {
		PoolForGeneralExecution.Submit(handler, desc, 0, NULL);
	}
}

//---------------------------------------------------------------------------
void __dispatch_async_serial(function < void() > handler, String desc) {
	if (handler) {
		PoolForSerialExecution.Submit(handler, desc, 0, NULL);
	}
}

//---------------------------------------------------------------------------
void __dispatch_async_background(function < void() > handler, String desc) {
	if (handler) {
		PoolForLowPriorityExecution.Submit(handler, desc, 0, NULL);
	}
}

//---------------------------------------------------------------------------
void __dispatch_after(function < void() > handler, String desc, int seconds) {
	if (handler) {
		PoolForDelayExecution.Submit(handler, desc, seconds, NULL);
	}
}
