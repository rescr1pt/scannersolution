#include "ThreadPoolQueue.h"

ThreadPoolQueue::ThreadPoolQueue()
{

}

ThreadPoolQueue::~ThreadPoolQueue()
{
    stop(true);
}

void ThreadPoolQueue::start(size_t threadCount, size_t poolMaxSize, IThreadPoolLocalContextCreator* threadLocalContextCreator)
{
    threadCount_ = threadCount;
    poolMaxSize_ = poolMaxSize;
    threadLocalContextCreator_ = threadLocalContextCreator;

    // before thread run
    if (threadLocalContextCreator_) {
        threadLocalContexts_.resize(threadCount_);
        for (size_t index = 0; index < threadCount_; ++index) {
            threadLocalContexts_[index] = std::move(threadLocalContextCreator_->create(index));
        }
    }

    for (size_t i = 0; i < threadCount_; ++i) {
        threads_.emplace_back(&ThreadPoolQueue::svc, this);
    }
}

void ThreadPoolQueue::stop(bool isForce)
{
    if (threads_.empty()) {
        return;
    }

    if (!isForce) {
        std::unique_lock<std::mutex> guard(queueMutex_);
        queueLimitCV_.wait(guard, [this] { return queue_.empty(); });
    }

    isStop_ = true;
    queueCV_.notify_all();

    for (auto& t : threads_) {
        t.join();
    }

    threads_.clear();
    threadLocalContexts_.clear();
}

void ThreadPoolQueue::putCommand(std::unique_ptr<IThreadPoolCommand> command, bool toTop)
{
    if (threads_.empty()) {
        return;
    }

    std::unique_lock<std::mutex> guard(queueMutex_);

    if (isStop_) {
        return;
    }

    queueLimitCV_.wait(guard, [this] { return queue_.size() < poolMaxSize_; });

    if (toTop) {
        queue_.push_front(std::move(command));
    }
    else {
        queue_.push_back(std::move(command));
    }

    guard.unlock();
    queueCV_.notify_one();
}

void ThreadPoolQueue::svc()
{
    static std::atomic_uint16_t globalThreadId_ = ATOMIC_VAR_INIT(0);
    uint16_t localTheradId = globalThreadId_++;

    IThreadPoolLocalContext* threadLocalContext = threadLocalContexts_[localTheradId].get();

    while (true) {
        std::unique_ptr<IThreadPoolCommand> command;
        // Get command
        {
            std::unique_lock<std::mutex> guard(queueMutex_);
            queueCV_.wait(guard, [this] { return !queue_.empty() || isStop_ == true; });
            if (isStop_ == true) {
                queueLimitCV_.notify_all();
                queue_.clear();
                break;
            }
            command = std::move(queue_.front());
            queue_.pop_front();

            command->onPopQueue(threadLocalContext);

            guard.unlock();
            queueLimitCV_.notify_all();
        }

        command->exec(threadLocalContext);
    }
}
