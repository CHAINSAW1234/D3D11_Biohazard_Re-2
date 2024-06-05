#pragma once
#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CThread_Pool : public CBase {
public:
	CThread_Pool(size_t num_threads);
	~CThread_Pool();

	template <class F, class... Args>
	std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(
		F&& f, Args&&... args);

public:
	static CThread_Pool* Create();
private:
	size_t num_threads_;
	std::vector<std::thread> worker_threads_;
	std::queue<std::function<void()>> jobs_;
	std::condition_variable cv_job_q_;
	std::mutex m_job_q_;

	bool stop_all;

	void WorkerThread();
};

int work(int t, int id) {
	printf("%d start \n", id);
	std::this_thread::sleep_for(std::chrono::seconds(t));
	printf("%d end after %ds\n", id, t);
	return t + id;
}

int main() {
	CThread_Pool::CThread_Pool pool(3);

	std::vector<std::future<int>> futures;
	for (int i = 0; i < 10; i++) {
		futures.emplace_back(pool.EnqueueJob(work, i % 3 + 1, i));
	}
	for (auto& f : futures) {
		printf("result : %d \n", f.get());
	}
}

END