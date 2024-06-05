#pragma once
#include "Base.h"

BEGIN(Engine)

class CThread_Pool : public CBase {
public:
    CThread_Pool(size_t num_threads);
    ~CThread_Pool();

    // job �� �߰��Ѵ�.
    void EnqueueJob(std::function<void()> job);
    bool AllJobsFinished();
public:
    static CThread_Pool* Create(_uint iNumThread);
private:
    // �� Worker �������� ����.
    size_t num_threads_;
    // Worker �����带 �����ϴ� ����.
    std::vector<std::thread> worker_threads_;
    // ���ϵ��� �����ϴ� job ť.
    std::queue<std::function<void()>> jobs_;
    // ���� job ť�� ���� cv �� m.
    std::condition_variable cv_job_q_;
    std::mutex m_job_q_;
    std::atomic<size_t> active_jobs_;
    // ��� ������ ����
    bool stop_all;

    // Worker ������
    void WorkerThread();
};

END