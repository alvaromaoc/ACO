#include <queue>
#include <list>

#ifndef UNIQUE_QUEUE_H
#define UNIQUE_QUEUE_H

class unique_queue {
private:
    std::queue<int, std::list<int>> queue;
    std::vector<bool> inQueue;

public:

    unique_queue(int max, std::list<int> init);
    
    unique_queue(int max);

    void push(int elem);

    int front();

    void pop();

    bool empty();
};

#endif //UNIQUE_QUEUE_H