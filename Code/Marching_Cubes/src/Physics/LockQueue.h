#ifndef LOCKQUEUE_H
#define LOCKQUEUE_H
#include <queue>
#include <optional>

#include "mingw.mutex.h"
#include "mingw.thread.h"
template<typename T> class LockQueue {
    public:
        LockQueue() = default;
        std::optional<T> pop() {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty()) {
                return {};
            }
            T result = queue.front();
            queue.pop();
            return result;
        };
        void push(const T &item) {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(item);
        }
    private:

        std::queue<T> queue;
        std::mutex mutex;
};

#endif