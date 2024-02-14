#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    // Add new work item to the pool
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args) {
        auto task = std::make_shared<std::packaged_task<typename std::result_of<F(Args...)>::type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

};

#endif // THREADPOOL_H
