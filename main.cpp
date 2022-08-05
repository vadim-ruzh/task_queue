#include "Context.hpp"
#include <iostream>
#include <chrono>

int main()
{
	Context context;

	Context::Work mWork(context);

	uint8_t workersCount = std::thread::hardware_concurrency();

	std::vector<std::thread> workers(workersCount);

	for (uint8_t i = 0 ; i < workersCount ; ++i)
	{
		workers.emplace_back(std::thread([&context](){	context.Run();}));
	}

	for(int i = 0;i < 100 ; ++i)
	{
		context.Post([](){std::cout << "done" << std::endl;return 1;});
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));

	context.Stop();

	for(int i = 0;i < 100 ; ++i)
	{
		context.Post([](){std::cout << "done with work" << std::endl;return 1;});
	}

	for (std::thread &t: workers) {
		if (t.joinable()) {
			t.join();
		}
	}

	return 0;
}