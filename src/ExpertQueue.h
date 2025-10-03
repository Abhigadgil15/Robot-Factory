#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include "Order.h"

struct ExpertRequest {
    Robot robot;
    std::promise<Robot> completionPromise;
};

class ExpertQueue {
private:
    std::queue<ExpertRequest> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void push(ExpertRequest&& req);
    ExpertRequest pop();
    bool empty();
};
