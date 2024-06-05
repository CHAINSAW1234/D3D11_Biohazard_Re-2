#pragma once
#include "Base.h"

BEGIN(Engine)

class CThread_Pool : public CBase {
public:
    CThread_Pool(size_t num_threads);
    ~CThread_Pool();

    // job 을 추가한다.
    void EnqueueJob(std::function<void()> job);
    bool AllJobsFinished();
public:
    static CThread_Pool* Create(_uint iNumThread);
private:
    // 총 Worker 쓰레드의 개수.
    size_t num_threads_;
    // Worker 쓰레드를 보관하는 벡터.
    std::vector<std::thread> worker_threads_;
    // 할일들을 보관하는 job 큐.
    std::queue<std::function<void()>> jobs_;
    // 위의 job 큐를 위한 cv 와 m.
    std::condition_variable cv_job_q_;
    std::mutex m_job_q_;
    std::atomic<size_t> active_jobs_;
    // 모든 쓰레드 종료
    bool stop_all;

    // Worker 쓰레드
    void WorkerThread();
};

END