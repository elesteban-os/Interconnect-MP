#include "scheduler.h"

// Default constructor
template <typename T>
Scheduler<T>::Scheduler() {
    algorithm = new FIFO<T>(); // Default to FIFO algorithm
}

// Constructor with algorithm type
template <typename T>
Scheduler<T>::Scheduler(ScheduleType type) {
    setAlgorithm(type);
}

// Destructor
template <typename T>
Scheduler<T>::~Scheduler() {
    if (algorithm != nullptr) {
        delete algorithm;
    }
}

// Set the scheduling algorithm
template <typename T>
void Scheduler<T>::setAlgorithm(ScheduleType type) {
    if (algorithm != nullptr) {
        delete algorithm;
    }
    switch (type) {
        case ScheduleType::FIFO:
            algorithm = new FIFO<T>();
            break;
        case ScheduleType::PRIORITY:
            algorithm = new Priority<T>();
            break;
        default:
            algorithm = new FIFO<T>();
            break;
    }
}

// Add an operation to the scheduler
template <typename T>
void Scheduler<T>::addOperation(T new_operation) {
    this->schedule_queue.push(new_operation);
    if (algorithm->getType() == ScheduleType::PRIORITY) {
        this->schedule_queue_func();
    }
}

// Get the next operation without removing it
template <typename T>
T Scheduler<T>::getNextOperation() {
    if (!this->schedule_queue.empty()) {
        T next_operation = this->schedule_queue.front();
        return next_operation;
    } else {
        std::cout << "Queue is empty" << std::endl;
        T next_operation;
        next_operation.pe_ID = -1; // Set to an invalid ID
        return T(); // Return a default-constructed object
    }
}

// Remove and return the next operation
template <typename T>
T Scheduler<T>::popQueue() {
    if (!this->schedule_queue.empty()) {
        T next_operation = this->schedule_queue.front();
        this->schedule_queue.pop();
        return next_operation;
    } else {
        std::cout << "Queue is empty" << std::endl;
        return T(); // Return a default-constructed object
    }
}

// Reorganize the queue based on the scheduling algorithm
template <typename T>
void Scheduler<T>::schedule_queue_func() {
    std::vector<T> dataoperation;
    while (!this->schedule_queue.empty()) {
        dataoperation.push_back(this->schedule_queue.front());
        this->schedule_queue.pop();
    }
    this->schedule_queue = this->algorithm->schedule(dataoperation);
}