#include "Context.hpp"
#include <iostream>
#include <chrono>

int main()
{
	Context context;

	// TODO: лучше фигурные скобки
	// NAMING: префикс m для полей-данных класса
	Context::Work mWork(context);

	// TODO: теоретическое переполнение при касте undigned int -> uint8_t
	uint8_t workersCount = std::thread::hardware_concurrency();

	// TODO: boost thread_pool
	std::vector<std::thread> workers(workersCount);

	for (uint8_t i = 0 ; i < workersCount ; ++i)
	{
		// TODO: emplace позволяет не создавать временного объекта здесь, в этом и есть его отличие от push
		// тут можно написать workers.emplace_back([&context](){ context.Run(); });

		// TODO: Run не помечен как noexcept, следовательно он может бросать исключения. 
		// Если в другом потоке вылетает исключение (в данном случае оно вылетит из Run), 
		// и мы его не перехватим, то программа упадёт.  
		workers.emplace_back(std::thread([&context](){ context.Run(); }));
	}

	// TODO: Выше для счётчика используется uint8_t, а тут int. Нужен единый код стайл, потому что такие вот нюансы выдают новичков.
	// Лучше придерживаться единого стиля, и экономить на байтах тут нет особого смысла: 
	// 			этот код вызывается единожды и эти 'лишние' байты в общей сложности висят в памяти не долго.
	
	// TODO: Систематически едет форматирование, от этого смазываются впечатления от кода. 
	// Лучше настрой решарпер на автоформатирование.
	// Ну и чтобы оно соблюдалось всеми программистами настрой и создай файлик для clangformat.
	for(int i = 0;i < 100 ; ++i)
	{
		// TODO: а синхронизировать доступ к cout через мьютекс не нужно?
		context.Post([](){std::cout << "done" << std::endl;return 1;});
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));

	context.Stop();

	for(int i = 0;i < 100 ; ++i)
	{
		// TODO: Мы остановили контекст, и постим новые задачи? В таком случае они не должны исполниться, зачем мы их постим?
		// Если так и задумано, то тут нужен поясняющий коммент.
		context.Post([](){std::cout << "done with work" << std::endl;return 1;});
	}

	for (std::thread &t: workers) {
		if (t.joinable()) {
			t.join();
		}
	}

	return 0;
}