// OneWriteMultRead.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <mutex>
#include <cassert>
#include <time.h>
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

public:
	void wait() {
		std::unique_lock<std::mutex> lock(mx_);
		cv_.wait(lock);
	}

	void signal() {
		cv_.notify_one();
	}
};

one_shot one_shot_signal;

void write(int id) {
	one_shot_signal.wait();
		{
			std::unique_lock<std::mutex> lock(mx_);
			writer_is_active_ = true;
			writer_.wait(lock, []() {return !number_of_readers; });
			int timer_ = 2;
			std::cout << "Thread #" << id << " gonna write for " << timer_ << " seconds\n";
			std::this_thread::sleep_for(std::chrono::seconds(timer_));
			writer_is_active_ = false;
		}

}


void read(int id) {
	one_shot_signal.wait();
		while (writer_is_active_) {}
		number_of_readers++;
		std::stringstream stream;
		stream << "Thread #" << id << " gonna read for 1 seconds\n";
		std::cout << stream.str();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		number_of_readers--;
		writer_.notify_all();
}



const int n_readers = 5;
const int n_writers = 6;


int main()
{
	srand(time(NULL));
	
	std::vector<std::thread> readers, writers;
	readers.reserve(n_readers);
	writers.reserve(n_writers);
	for (int i = 0; i < n_writers; i++)
		writers.emplace_back(write, i + 1);
	for (int i = 0; i < n_readers; i++)
		readers.emplace_back(read, i + 1);
	one_shot_signal.signal();
	
	std::for_each(readers.begin(), readers.end(), [](auto& x) {x.join(); });
	std::for_each(writers.begin(), writers.end(), [](auto& x) {x.join(); });

}

