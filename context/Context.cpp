#include "Context.hpp"
#include <system_error>

Context::Context() noexcept
    : mIsWork(false), mIsStop(false)
{
}

Context::~Context() noexcept
{
	Stop();
}

void Context::Run() noexcept
{
	while (!mIsStop)
	{
		std::unique_lock<std::mutex> queueLock(mMutex);

		mAlarm.wait(queueLock, [this]() { return !mTasks.empty() || mIsStop || !mIsWork; });

		if (!mTasks.empty())
		{
			Task currentTask = std::move(mTasks.front());
			mTasks.pop();

			queueLock.unlock();

			currentTask();
		}
		else if (!mIsWork)
		{
			return;
		}
	}
}

void Context::Stop() noexcept
{
	//If the stop flag was false...
	if (!mIsStop.exchange(true))
	{
		//...threads must be woken up to check the new state of the stop flag
		mAlarm.notify_all();
	}
}

Context::Work::Work(Context& context) noexcept
    : mContext(context)
{
	mContext.mIsWork = true;
	//Threads do not sleep without true work flag,so no need to wake up threads
}

Context::Work::~Work() noexcept
{
	//If the work flag was true...
	if (mContext.mIsWork)
	{
		//...threads must be woken up to check the new state of the work flag
		mContext.mIsWork = false;
		mContext.mAlarm.notify_all();
	}
}
