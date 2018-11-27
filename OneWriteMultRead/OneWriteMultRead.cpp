// OneWriteMultRead.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <mutex>
#include <cassert>
#include <sstream>
#include <vector>
#include <algorithm>

std::mutex mx_;
std::condition_variable writer_;
std::condition_variable readers_;
bool writer_is_active_ = false;
int number_of_readers = 0;

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

one_shot one_shot_signal;

void write(int id) {
	one_shot_signal.wait();
		{
			std::unique_lock<std::mutex> lock(mx_);
			writer_is_active_ = true;
			writer_.wait(lock, []() {return !number_of_readers; });
			std::cout << "Thread #" << id << " gonna write for " << 3 << " seconds\n";
			std::this_thread::sleep_for(std::chrono::seconds(3));
			writer_is_active_ = false;
		}
	readers_.notify_all();

}


void read(int id) {
	one_shot_signal.wait();
	{
		std::unique_lock<std::mutex> lock(mx_);
		readers_.wait(lock, []() { return !writer_is_active_; });
		number_of_readers++;
	}
	std::stringstream stream;
	for (int i = 0; i < id; i++)
	stream << "    ";
	stream << "read " << id << "\n";
	std::cout << stream.str();
	number_of_readers--;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	writer_.notify_all();
}

const int n_readers = 5;
const int n_writers = 6;


int main()
{

	std::vector<std::thread> threads;
	threads.reserve(20);

	threads.emplace_back(read, 1);

	threads.emplace_back(write, 1);

	threads.emplace_back(read, 2);

	threads.emplace_back(write, 2);

	threads.emplace_back(write, 3);

	threads.emplace_back(read, 3);

	threads.emplace_back(write, 4);

	threads.emplace_back(write, 5);

	threads.emplace_back(read, 4);

	one_shot_signal.signal();

	std::for_each(threads.begin(), threads.end(), [](auto& x) {x.join(); });
	getchar();
}
