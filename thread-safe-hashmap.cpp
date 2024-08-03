#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <mutex>
#include <functional>

template <typename Key, typename Value>
class AdvancedThreadSafeHashMap {
private:
    std::vector<std::unordered_map<Key, Value>> buckets;
    std::vector<std::shared_mutex> locks;
    size_t num_buckets;

    size_t get_bucket_index(const Key& key) const {
        return std::hash<Key>()(key) % num_buckets;
    }

public:
    explicit AdvancedThreadSafeHashMap(size_t num_buckets) : num_buckets(num_buckets) {
        buckets.resize(num_buckets);
        locks.resize(num_buckets);
    }

    void insert(Key key, Value value) {
        size_t index = get_bucket_index(key);
        std::unique_lock<std::shared_mutex> lock(locks[index]);
        buckets[index][key] = std::move(value);
    }

    bool get(const Key& key, Value& value) const {
        size_t index = get_bucket_index(key);
        std::shared_lock<std::shared_mutex> lock(locks[index]);
        auto it = buckets[index].find(key);
        if (it != buckets[index].end()) {
            value = it->second;
            return true;
        }
        return false;
    }

    void erase(const Key& key) {
        size_t index = get_bucket_index(key);
        std::unique_lock<std::shared_mutex> lock(locks[index]);
        buckets[index].erase(key);
    }

    bool contains(const Key& key) const {
        size_t index = get_bucket_index(key);
        std::shared_lock<std::shared_mutex> lock(locks[index]);
        return buckets[index].find(key) != buckets[index].end();
    }
};
