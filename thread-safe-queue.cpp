#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class AdvancedThreadSafeQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
    size_t max_size;

public:
    explicit AdvancedThreadSafeQueue(size_t max_size) : max_size(max_size) {}

    void push(T value) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return queue.size() < max_size; });
        queue.push(std::move(value));
        cv.notify_all();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });
        T value = std::move(queue.front());
        queue.pop();
        cv.notify_all();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }
};
