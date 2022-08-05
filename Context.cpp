#include "Context.hpp"

Context::Context() : mIsWork(false),mStop(false)
{

}

Context::~Context()
{
	Stop();
}

std::future<bool> Context::Post(std::function<bool()>&& func)
{
	Task newTask (std::forward<std::function<bool()>>(func));
	auto future = newTask.get_future();

	{
		std::lock_guard<std::mutex> lock(mMutex);
		mTasks.push(std::move(newTask));
		myCondition.notify_one();
	}

	return future;
}

void Context::Run()
{
	Task currentTask;
	while(!mStop)
	{
		std::unique_lock<std::mutex> lock(mMutex);
		myCondition.wait(lock, [=]() { return !mTasks.empty() || mStop || !mIsWork;});

		if(!mTasks.empty())
		{
			currentTask = std::move(mTasks.front());
			mTasks.pop();

			lock.unlock();

			currentTask();
		}
		else if(!mIsWork)
		{
			return;
		}
	}
}

void Context::Stop()
{
	if (mStop.exchange(true))
		return;

	myCondition.notify_all();
}

void Context::Restart()
{
	mStop.exchange(false);
}

Context::Work::Work(Context& context): m_context(context)
{
	m_context.mIsWork.exchange(true);
}

Context::Work::~Work()
{
	m_context.mIsWork.exchange(false);
	m_context.myCondition.notify_all();
}
