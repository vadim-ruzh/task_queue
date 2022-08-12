#include "Context.hpp"
#include <gtest/gtest.h>

TEST(task_queue, run_without_work)
{
	std::mutex coutMutex;

	Context context;

	unsigned int workersCount = std::thread::hardware_concurrency();

	std::vector<std::thread> workers(workersCount);
	for (unsigned int i = 0; i < workersCount; ++i)
	{
		workers.emplace_back([&context]() { context.Run(); });
	}

	auto messager = [&coutMutex]()
	{
		coutMutex.lock();
		std::cout << "sending messages failed" << std::endl;
		coutMutex.unlock();

		return std::make_error_code(std::errc::bad_message);
	};


	for (int i = 0; i < 100; ++i)
	{
		context.Post(messager);
	}

	for (std::thread& t : workers)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
}
