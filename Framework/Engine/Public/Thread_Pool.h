#pragma once
#include "Base.h"

BEGIN(Engine)

class CThread_Pool : public CBase {
public:
    CThread_Pool(size_t num_threads);
    ~CThread_Pool();

    void EnqueueJob(std::function<void()> job);
    bool AllJobsFinished();
public:
    static CThread_Pool* Create(_uint iNumThread);
private:
    size_t num_threads_;
    std::vector<std::thread> worker_threads_;
    std::queue<std::function<void()>> jobs_;
    std::condition_variable cv_job_q_;
    std::mutex m_job_q_;
    std::atomic<size_t> active_jobs_;
    bool stop_all;
    void WorkerThread();
};

END