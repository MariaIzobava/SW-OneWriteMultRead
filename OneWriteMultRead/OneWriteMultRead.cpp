// OneWriteMultRead.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <mutex>
#include <cassert>
#include <sstream>
#include <vector>
#include <algorithm>
#include "unique_read_lock.h"
#include "unique_write_lock.h"
#include "one_shot.h"

one_shot one_shot_signal;

smart_mutex mx;

void write(int id) {
	one_shot_signal.wait();
		
	unique_write_lock lock(mx);

	std::stringstream ss;
	ss << "writing";
	for (int i = 0; i < id; i++)
		ss << "    ";
	ss << id << std::endl;
	std::cout << ss.str();
	std::this_thread::sleep_for(std::chrono::seconds(2));

}


void read(int id) {
	one_shot_signal.wait();

	unique_read_lock lock(mx);

	std::stringstream ss;
	ss << "reading";
	for (int i = 0; i < id; i++)
		ss << "    ";
	ss << id << std::endl;
	std::cout << ss.str();
	std::this_thread::sleep_for(std::chrono::seconds(4));
	
}


int main()
{

	std::vector<std::thread> threads;
	threads.reserve(20);

	threads.emplace_back(write, 1);

	threads.emplace_back(read, 1);

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
