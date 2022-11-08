#pragma once

#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>

class IThreadPoolLocalContext
{
public:
    virtual ~IThreadPoolLocalContext() {}
};

class IThreadPoolLocalContextCreator
{
public:
    virtual ~IThreadPoolLocalContextCreator() {}
    virtual std::unique_ptr<IThreadPoolLocalContext> create(size_t /*threadIndex*/) = 0;
};

class IThreadPoolCommand
{
public:
public:
    virtual ~IThreadPoolCommand() {}
    
    // Callback after get the command from the queue.
    // Note: Will be called before the queue mutex is released. 
    // This means that the callback implementation should not run for a long time, otherwise it affects the performance of the ThreadPool.  
    virtual void onPopQueue(IThreadPoolLocalContext* threadLocalContext) {}

    // 
    virtual void exec(IThreadPoolLocalContext* threadLocalContext) {}
    // virtual bool isGroup() const { return false; }
};

class ThreadPoolQueue
{
    typedef std::deque<std::unique_ptr<IThreadPoolCommand>> CommandQueue;

public:
    explicit ThreadPoolQueue();

    ~ThreadPoolQueue();

    void start(size_t threadCount, size_t poolMaxSize, IThreadPoolLocalContextCreator* threadLocalContextCreator);

    void stop(bool isForce);

    void putCommand(std::unique_ptr<IThreadPoolCommand> command, bool toTop);

    void svc();

private:
    size_t threadCount_ = 0;
    size_t poolMaxSize_ = 0;
    IThreadPoolLocalContextCreator* threadLocalContextCreator_ = nullptr;
    CommandQueue queue_;
    std::condition_variable queueCV_;
    std::condition_variable queueLimitCV_;
    std::mutex queueMutex_;
    std::vector<std::thread> threads_;
    std::vector<std::unique_ptr<IThreadPoolLocalContext>> threadLocalContexts_;
    std::atomic_bool isStop_ = ATOMIC_VAR_INIT(0);
};