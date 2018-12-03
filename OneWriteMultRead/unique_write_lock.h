#ifndef UNIQUE_WRITE_LOCK_H
#define UNIQUE_WRITE_LOCK_H

#include "smart_mutex.h"

class unique_write_lock {

private:
	smart_mutex* mx_;

public:
	unique_write_lock(smart_mutex& mx) : mx_(&mx) {
		mx_->start_write();
	}

	unique_write_lock(const unique_write_lock&) = delete;

	unique_write_lock(unique_write_lock && obj) : mx_(obj.mx_) {
		obj.mx_ = nullptr;
	}

	unique_write_lock& operator=(unique_write_lock const & obj) = delete;

	unique_write_lock& operator=(unique_write_lock&& obj) {
		mx_ = obj.mx_;
		obj.mx_ = nullptr;
		return *this;
	}

	~unique_write_lock() {
		mx_->stop_write();
	}
};

#endif