#include <stack>
#include <mutex>
#include <condition_variable>
#include <atomic>

template <typename T>
class AdvancedThreadSafeStack {
private:
    std::stack<T> stack;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> is_locked;

public:
    AdvancedThreadSafeStack() : is_locked(false) {}

    void push(T value) {
        while (is_locked.exchange(true));
        {
            std::lock_guard<std::mutex> lock(mtx);
            stack.push(std::move(value));
        }
        is_locked = false;
        cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !stack.empty(); });
        T value = std::move(stack.top());
        stack.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return stack.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return stack.size();
    }
};
