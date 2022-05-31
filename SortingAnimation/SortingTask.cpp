#include "SortingTask.h"

SortingTask::SortingTask(std::coroutine_handle<promise_type> handler) : co_handler(handler) { }

SortingTask::~SortingTask() {
	if ((bool)co_handler) {
		co_handler.destroy();
	}
}

SortingTask SortingTask::promise_type::get_return_object() {
	return SortingTask{
		std::coroutine_handle<promise_type>::from_promise(*this)
	};
}

std::suspend_always SortingTask::promise_type::initial_suspend() {
	return std::suspend_always{};
}

std::suspend_always SortingTask::promise_type::yield_value(pr a) {
	value = a;
	return std::suspend_always{};
}

std::suspend_always SortingTask::promise_type::final_suspend() const noexcept {
	return std::suspend_always{};
}

void SortingTask::promise_type::return_void() {
	return;
}

void SortingTask::promise_type::unhandled_exception() {
	exit(7777);
}