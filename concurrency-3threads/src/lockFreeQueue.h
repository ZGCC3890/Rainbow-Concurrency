//
// Created by ZGCC on 25-1-15.
//

#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

#include <atomic>
#include <mutex>

// 参考linux内核中的kfifo算法，利用环形缓冲区实现单生产者单消费者无锁队列
template <typename T>
class LockFreeQueue {
public:
    // 构造函数，size 必须是 2 的幂次方
    explicit LockFreeQueue(size_t size)
            : capacity(size), buffer(size), mask(size - 1), head(0), tail(0) {
        if ((size & (size - 1)) != 0) {
            throw std::invalid_argument("Size must be a power of 2");
        }
    }

    // 禁止拷贝和赋值
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

    // 入队操作（生产者）
    bool enqueue(const T& value) {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) & mask;

        // 队列已满
        if (next_tail == head.load(std::memory_order_acquire)) {
            expand();
        }

        buffer[current_tail] = value;
        tail.store(next_tail, std::memory_order_release);
        return true;
    }

    // 出队操作（消费者）
    bool dequeue(T& value) {
        size_t current_head = head.load(std::memory_order_relaxed);

        // 队列为空
        if (current_head == tail.load(std::memory_order_acquire)) {
            return false;
        }

        value = buffer[current_head];
        head.store((current_head + 1) & mask, std::memory_order_release);
        return true;
    }

private:
    size_t capacity;                        // 当前容量
    std::vector<T> buffer;                  // 缓冲区
    size_t mask;                            // 用于环形索引的掩码
    std::atomic<size_t> head, tail;         // 队列头和尾的原子变量
    std::mutex expand_lock;                 // 扩容锁，确保只有一个线程进行扩容

    // 扩容函数
    void expand() {
        std::lock_guard<std::mutex> lock(expand_lock);

        // 双重检查，避免重复扩容
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t current_head = head.load(std::memory_order_relaxed);
        if (((current_tail + 1) & mask) != current_head) {
            return; // 扩容期间可能已有空间
        }

        // 新容量为旧容量的两倍
        size_t new_capacity = capacity * 2;
        std::vector<T> new_buffer(new_capacity);

        // 迁移数据
        size_t current_size = (current_tail - current_head + capacity) & mask;
        for (size_t i = 0; i < current_size; ++i) {
            new_buffer[i] = buffer[(current_head + i) & mask];
        }

        // 更新缓冲区和相关状态
        buffer = std::move(new_buffer);
        head.store(0, std::memory_order_relaxed);
        tail.store(current_size, std::memory_order_relaxed);
        capacity = new_capacity;
        mask = new_capacity - 1;
    }
};
#endif //LOCKFREEQUEUE_H
