#include <mutex>

struct one_shot {

private:
	std::condition_variable cv_;
	std::mutex mx_;
	bool f = false;

public:
	void wait() {
		std::unique_lock<std::mutex> lock(mx_);
		cv_.wait(lock, [&]() {return f; });
	}

	void signal() {
		{
			std::lock_guard<std::mutex> lock(mx_);
			f = true;
		}

		cv_.notify_all();
	}
};