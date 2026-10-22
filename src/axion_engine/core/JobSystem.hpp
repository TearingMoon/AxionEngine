#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace Axion
{

/**
 * @brief Small worker pool for CPU-only jobs.
 *
 * Jobs must not touch SDL objects or mutate Scene/GameObject state directly.
 * Use it for isolated preparation work and merge results on the main thread.
 */
class JobSystem
{
public:
    explicit JobSystem(std::size_t workerCount = 0);
    ~JobSystem();

    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;

    template <typename Function>
    auto Submit(Function&& function) -> std::future<std::invoke_result_t<Function>>
    {
        using Result = std::invoke_result_t<Function>;
        auto task = std::make_shared<std::packaged_task<Result()>>(
            std::forward<Function>(function));
        std::future<Result> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            jobs_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return result;
    }

    template <typename Function>
    void ParallelFor(std::size_t count, Function&& function)
    {
        if (count == 0)
            return;
        std::vector<std::future<void>> futures;
        futures.reserve(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            futures.push_back(Submit([index, &function]() {
                function(index);
            }));
        }
        for (auto& future : futures)
            future.get();
    }

private:
    void WorkerLoop();

    std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<std::function<void()>> jobs_;
    std::vector<std::thread> workers_;
    bool stopping_ = false;
};

} // namespace Axion
