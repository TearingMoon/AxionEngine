#include "JobSystem.hpp"

#include <algorithm>

namespace Axion
{

JobSystem::JobSystem(std::size_t workerCount)
{
    if (workerCount == 0)
    {
        const unsigned int hardware = std::thread::hardware_concurrency();
        workerCount = hardware > 1 ? hardware - 1 : 1;
    }
    workers_.reserve(workerCount);
    for (std::size_t i = 0; i < workerCount; ++i)
        workers_.emplace_back(&JobSystem::WorkerLoop, this);
}

JobSystem::~JobSystem()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopping_ = true;
    }
    condition_.notify_all();
    for (auto& worker : workers_)
        worker.join();
}

void JobSystem::WorkerLoop()
{
    while (true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]() {
                return stopping_ || !jobs_.empty();
            });
            if (stopping_ && jobs_.empty())
                return;
            job = std::move(jobs_.front());
            jobs_.pop();
        }
        job();
    }
}

} // namespace Axion
