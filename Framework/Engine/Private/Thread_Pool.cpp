#include "Thread_Pool.h"

CThread_Pool::CThread_Pool(size_t num_threads)
    : num_threads_(num_threads), stop_all(false), active_jobs_(0) {
    worker_threads_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i) {
        worker_threads_.emplace_back([this]() { this->WorkerThread(); });
    }
}

void CThread_Pool::WorkerThread() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_job_q_);
            cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });
            if (stop_all && this->jobs_.empty()) {
                return;
            }
            job = std::move(jobs_.front());
            jobs_.pop();
            ++active_jobs_;  // �۾� ���� �� Ȱ�� �۾� �� ����
        }

        job();  // �۾� ����

        {
            std::lock_guard<std::mutex> lock(m_job_q_);
            --active_jobs_;  // �۾� �Ϸ� �� Ȱ�� �۾� �� ����
            if (jobs_.empty() && active_jobs_ == 0) {
                cv_job_q_.notify_all();  // ��� �۾��� �Ϸ�Ǿ����� �˸�
            }
        }
    }
}

CThread_Pool::~CThread_Pool() {
    stop_all = true;
    cv_job_q_.notify_all();

    for (auto& t : worker_threads_) {
        t.join();
    }
}

void CThread_Pool::EnqueueJob(std::function<void()> job) {
    if (stop_all) {
        throw std::runtime_error("ThreadPool ��� ������");
    }
    {
        std::lock_guard<std::mutex> lock(m_job_q_);
        jobs_.push(std::move(job));
    }
    cv_job_q_.notify_one();
}

bool CThread_Pool::AllJobsFinished() {
    std::unique_lock<std::mutex> lock(m_job_q_);
    return jobs_.empty() && active_jobs_ == 0;
}

CThread_Pool* CThread_Pool::Create(_uint iNumThread)
{
    CThread_Pool* pInstance = new CThread_Pool(iNumThread);

    return pInstance;
}