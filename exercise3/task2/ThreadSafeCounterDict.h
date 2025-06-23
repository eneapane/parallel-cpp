#pragma once

#include <functional>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <vector>



template<typename KeyType, typename ValueType, typename HashType = std::hash<KeyType>>
class ThreadSafeCounterDict {
private:
    struct Bucket {
        std::vector<std::pair<KeyType, ValueType>> entries;
        mutable std::shared_mutex mutex;
    };

    std::vector<Bucket> buckets;
    ValueType defaultValue;

    Bucket& get_bucket(const KeyType& key) {
        size_t hash = HashType{}(key);
        return buckets[hash % buckets.size()];
    }

    const Bucket& get_bucket(const KeyType& key) const {
        size_t hash = HashType{}(key);
        return buckets[hash % buckets.size()];
    }

public:
    ThreadSafeCounterDict(const ValueType& init_value, size_t num_buckets)
        : buckets(num_buckets), defaultValue(init_value) {}

    ValueType get_value_for_key(const KeyType& key) const {
        const Bucket& bucket = get_bucket(key);
        std::shared_lock lock(bucket.mutex);
        for (const auto& [k, v] : bucket.entries) {
            if (k == key)
                return v;
        }
        return defaultValue;
    }

    ValueType increase_value_for_key(const KeyType& key, const ValueType& increment) {
        Bucket& bucket = get_bucket(key);
        std::unique_lock lock(bucket.mutex);
        for (auto& [k, v] : bucket.entries) {
            if (k == key) {
                v += increment;
                return v;
            }
        }
        bucket.entries.emplace_back(key, increment);
        return increment;
    }

    std::vector<std::pair<KeyType, ValueType>> convert_to_pairs() const {
        std::vector<std::pair<KeyType, ValueType>> result;
        for (const auto& bucket : buckets) {
            std::shared_lock lock(bucket.mutex);
            result.insert(result.end(), bucket.entries.begin(), bucket.entries.end());
        }
        return result;
    }

};

