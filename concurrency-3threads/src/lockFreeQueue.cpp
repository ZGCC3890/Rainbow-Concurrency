//
// Created by ZGCC on 25-1-14.
//
#include "utils.h"
// 构造函数
template <typename T>
LockFreeQueue<T>::LockFreeQueue(size_t capacity)
        : buffer(capacity + 1), head(0), tail(0), capacity(capacity + 1) {}

// 入队
template <typename T>
void LockFreeQueue<T>::enqueue(const T& value) {
    size_t currentTail = tail.load(std::memory_order_relaxed);
    size_t nextTail = (currentTail + 1) % capacity;

    if (nextTail == head.load(std::memory_order_acquire)) {
        throw std::overflow_error("Queue is full");
    }

    buffer[currentTail] = value;
    tail.store(nextTail, std::memory_order_release);
}

// 出队
template <typename T>
T LockFreeQueue<T>::dequeue() {
    size_t currentHead = head.load(std::memory_order_relaxed);

    if (currentHead == tail.load(std::memory_order_acquire)) {
        throw std::underflow_error("Queue is empty");
    }

    T value = buffer[currentHead];
    head.store((currentHead + 1) % capacity, std::memory_order_release);
    return value;
}

template <typename T>
bool LockFreeQueue<T>::isEmpty(){
    size_t currentHead = head.load(std::memory_order_relaxed);
    return currentHead == tail.load(std::memory_order_acquire);
}