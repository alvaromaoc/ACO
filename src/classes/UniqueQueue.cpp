#include "UniqueQueue.h"

unique_queue::unique_queue(int max, std::list<int> init) {
    queue = std::queue<int, std::list<int>>(init);
    inQueue = std::vector<bool>(max);
}

unique_queue::unique_queue(int max) {
    queue = std::queue<int, std::list<int>>();
    inQueue = std::vector<bool>(max);
}

void unique_queue::push(int elem) {
    if (!inQueue[elem]) {
        queue.push(elem);
        inQueue[elem] = true;
    }
}

int unique_queue::front() {
    return queue.front();
}

void unique_queue::pop() {
    int elem = queue.front();
    inQueue[elem] = false;
    queue.pop();
}

bool unique_queue::empty() {
    return queue.empty();
}