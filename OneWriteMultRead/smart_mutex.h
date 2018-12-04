#ifndef SMART_MUTEX_H
#define SMART_MUTEX_H

#include <mutex> 
#include <iostream>

class smart_mutex {
private:

	int number_of_readers_;
	bool writer_is_active_;
	int number_of_expected_writers_;

	std::mutex mx_;
	std::condition_variable writing_thread_;
	std::condition_variable reading_threads_;


public:
	smart_mutex() {}

	void start_write() {
		{
			std::unique_lock<std::mutex> lock(mx_);
			number_of_expected_writers_++;
			writing_thread_.wait(lock, [&]() {return !writer_is_active_ && !number_of_readers_; });
			writer_is_active_ = true;
			std::cout << "Thread is writing\n";
			number_of_expected_writers_--;
		}
	}
	void stop_write() {
		{
			std::unique_lock<std::mutex> lock(mx_);
			writer_is_active_ = false;
		}
		if (number_of_expected_writers_ > 0) 
			writing_thread_.notify_one();
		else
			reading_threads_.notify_all();
		
	}

	void start_read() {
		{
			std::unique_lock<std::mutex> lock(mx_);
			reading_threads_.wait(lock, [&]() {return !writer_is_active_ && !number_of_expected_writers_; });
			number_of_readers_++;
		}
		std::cout << "Thread is reading\n";
	}
	void stop_read() {
		{
			std::unique_lock<std::mutex> lock(mx_);
			number_of_readers_--;
		}
		writing_thread_.notify_one();
	}
	
};

#endif