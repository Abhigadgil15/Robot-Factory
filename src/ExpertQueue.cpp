#include "ExpertQueue.h"

void ExpertQueue::push(ExpertRequest&& req) {
    std::lock_guard<std::mutex> lock(mtx);
    queue.push(std::move(req));
    cv.notify_one();
}

ExpertRequest ExpertQueue::pop() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return !queue.empty(); });
    ExpertRequest req = std::move(queue.front());
    queue.pop();
    return req;
}

bool ExpertQueue::empty() {
    std::lock_guard<std::mutex> lock(mtx);
    return queue.empty();
}
