#include "Thread_Pool.h"
#include "thread_pool.h"

CThread_Pool::CThread_Pool(size_t num_threads)
	: num_threads_(num_threads), stop_all(false) {
	worker_threads_.reserve(num_threads_);
	for (size_t i = 0; i < num_threads_; ++i) {
		worker_threads_.emplace_back([this]() { this->WorkerThread(); });
	}
}

void CThread_Pool::WorkerThread() {
	while (true) {
		std::unique_lock<std::mutex> lock(m_job_q_);
		cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });
		if (stop_all && this->jobs_.empty()) {
			return;
		}

		std::function<void()> job = std::move(jobs_.front());
		jobs_.pop();
		lock.unlock();

		job();
	}
}

CThread_Pool::~CThread_Pool() {
	stop_all = true;
	cv_job_q_.notify_all();

	for (auto& t : worker_threads_) {
		t.join();
	}
}

template <class F, class... Args>
std::future<typename std::result_of<F(Args...)>::type> CThread_Pool::EnqueueJob(
	F&& f, Args&&... args) {
	if (stop_all) {
		throw std::runtime_error("CThread_Pool »ç¿ë ÁßÁöµÊ");
	}

	using return_type = typename std::result_of<F(Args...)>::type;
	auto job = std::make_shared<std::packaged_task<return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	std::future<return_type> job_result_future = job->get_future();
	{
		std::lock_guard<std::mutex> lock(m_job_q_);
		jobs_.push([job]() { (*job)(); });
	}
	cv_job_q_.notify_one();

	return job_result_future;
}

CThread_Pool* CThread_Pool::Create()
{
	return nullptr;
}
