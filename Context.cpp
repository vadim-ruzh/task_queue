#include "Context.hpp"

Context::Context() : mIsWork(false),mStop(false)
{
}

Context::~Context()
{
	// TODO: Бросать исключения из деструктора нельзя, потому что очень легко схватить падение:
	//			если в try блоке вылетело исключение, то вызываются деструкторы всех объектов в try блоке, и если какой-нибудь деструктор кинет исключение, то программа упадёт так и не дойдя до catch.
	// Лучше перехватывать теоретические исключения в деструкторе, и логировать их.
	// И деструктор тогда пометить как noexcept.
	Stop();
}

// TODO: Не вижу смысла здесь в &&. Либо делай perfect forwarding, но лучше уж просто принимать сразу Task. 
// Смотри коммент внутри функции, я там описал подробнее как по-моему лучше сделать. 
std::future<bool> Context::Post(std::function<bool()>&& func)
{
	// TODO: forward имеет смысл только при идеальной передаче в шаблонном коде с универсальными ссылками.
	// Здесь он аналогичен std::move, и move будет конкретнее.
	// К тому же, среди минусов я вижу дублирование типа функции, что не есть хорошо. 
	// Лучше, наверное переписать на универсальную ссылку так (см. код ниже). 
	// 		В таком случае юзер сможет передать как набор аргументов (в данном случае std::function), 
	// 		так и Task. При этом если он передаст lvalue, то аргументы скопируются, а если rvalue, то переместятся. 
	// 		И при всём при этом объект создаётся на месте, под капотом std::list, 
	//			из-за чего можно избежать накладных расходов на перемещение Task.
	/*
		template<typename ...TaskArgs>
		std::future<bool> Context::Post(TaskArgs&& ...args)
		{
			std::lock_guard<std::mutex> lock(mMutex);
			
			auto taskIter = mTasks.emplace(mTasks.begin(), std::forward<TaskArgs>(args) ...);
			myCondition.notify_one();

			return taskIter->get_future();
		}
	*/
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
		// NAMING: хочется по названию понять что именно лочим, лучше бы queueLock
		std::unique_lock<std::mutex> lock(mMutex);
		// TODO: зачем захватывать в лямбде всё по значению? Выглядит как то что тут достаточно захватить this [this]() { .. }
		myCondition.wait(lock, [=]() { return !mTasks.empty() || mStop || !mIsWork; });

		if(!mTasks.empty())
		{
			// TODO: предположим мы исполнили задачу, и ушли в долгое ожидание. 
			// Поскольку задача сохранена в currentTask, то все её ресурсы тоже будут держаться пока Run-ner не получит новую задачу.
			// 		Это означает, что, к примеру, всякие штуки из списка захвата лямбды могут держаться в памяти избыточно долго.
			// Поэтому преждевременная оптимизация - чаще всего не хорошо. Тут лучше просто создавать таск:
			// Task currentTask = std::move(mTasks.front());
			currentTask = std::move(mTasks.front());
			mTasks.pop();

			lock.unlock();

			currentTask();
		}
		// TODO: проверяем атомик переменную только под захваченным локом, в таком случае атомик переменная выглядит избыточной.
		// То есть выглядит как то, что достаточно одного мьютекса.
		else if(!mIsWork)
		{
			return;
		}
	}
}

void Context::Stop()
{
	if (mStop.exchange(true))
		// TODO: Комментария с пояснениями почему не нотифаим нет.
		return;

	// TODO: Комментария с пояснениями зачем нотифаим нет.
	myCondition.notify_all();
}

void Context::Restart()
{
	// TODO: можно проще и понятнее mStop = false;
	// TODO: Комментария с пояснениями почему не нотифаим нет.
	mStop.exchange(false);
}

// FORMAT: лучше переносить список инициализации на следующую строку
Context::Work::Work(Context& context): m_context(context)
{
	// TODO: можно проще и понятнее mIsWork = true;
	// TODO: work поменяли, но notify_all не сделали. Комментария с пояснениями почему решили не нотифаить.
	m_context.mIsWork.exchange(true);
}

Context::Work::~Work()
{
	// TODO: можно проще и понятнее mIsWork = false;
	// TODO: Комментария с пояснениями зачем нотифаить нет.
	m_context.mIsWork.exchange(false);
	m_context.myCondition.notify_all();
}
