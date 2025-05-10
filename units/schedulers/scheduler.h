#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>
#include <iostream>
#include "schedulealgorithms.cpp" // Include the header for scheduling algorithms

template <typename T>
class Scheduler {
private:
    ScheduleAlgorithm<T>* algorithm = nullptr; // Template-based algorithm pointer
    std::queue<T> schedule_queue;             // Queue for operations

public:
    Scheduler();
    ~Scheduler();

    void setAlgorithm(int typeAlg);
    void addOperation(T new_operation);
    T getNextOperation();
    T popQueue();
    void schedule_queue_func();
};

#endif // SCHEDULER_H