#ifndef UNIQUE_READ_LOCK_H
#define UNIQUE_READ_LOCK_H

#include "smart_mutex.h"

class unique_read_lock {

private:
	smart_mutex* mx_;

public:
	unique_read_lock(smart_mutex& mx): mx_(&mx) {
		mx_->start_read();
	}

	unique_read_lock(const unique_read_lock&) = delete;

	~unique_read_lock() {
		mx_->stop_read();
	}
};

#endif