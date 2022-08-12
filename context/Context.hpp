#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <future>
#include <queue>
#include <thread>

class Context
{
public:
	using Task = std::packaged_task<std::error_code()>;

	class Work;

	//Todo:
	//class Strand;

	Context() noexcept;
	~Context() noexcept;

	template<typename... Args>
	std::future<std::error_code> Post(Args&&... args) //noexcept(false)
	{
		std::lock_guard<std::mutex> queueLock(mMutex);

		mTasks.emplace(std::forward<Args>(args)...);

		mAlarm.notify_one();

		return mTasks.back().get_future();
	}

	void Run() noexcept;
	void Stop() noexcept;

private:
	std::mutex mMutex;
	std::condition_variable mAlarm;
	std::queue<Task> mTasks;
	std::atomic_bool mIsStop;
	bool mIsWork;
};

class Context::Work
{
public:
	explicit Work(Context& context) noexcept;
	~Work() noexcept;

private:
	Context& mContext;
};

#endif//CONTEXT_HPP
