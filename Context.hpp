#ifndef TASK_QUEUE_TASKQUEUE_HPP
#define TASK_QUEUE_TASKQUEUE_HPP

#include <thread>
#include <future>
#include <queue>

class Context
{
public:
	using Task = std::packaged_task<bool()>;

	class Work;
	class Strand;

	Context();
	~Context();

	std::future<bool> Post(std::function<bool()>&& func);
	void Run();

	void Stop();
	void Restart();
private:
	std::queue<Task> mTasks;
	std::mutex mMutex;
	std::atomic_bool mStop;
	std::atomic_bool mIsWork;
	std::condition_variable myCondition;
};

class Context::Work
{
public:
	explicit Work(Context& context);
	~Work();
private:
	Context& m_context;
};

class Context::Strand
{
 public:
	explicit Strand(Context& context);
	~Strand();

 private:

};


#endif //TASK_QUEUE_TASKQUEUE_HPP

