// Imports
#include <queue>
#include "schedulealgorithms.cpp"
#include <vector>
#include <iostream>

template <typename T>
class Scheduler {
private:
    ScheduleAlgorithm<T>* algorithm = nullptr; // Template-based algorithm pointer
    std::queue<T> schedule_queue;             // Queue for operations

public:
    Scheduler() {
        algorithm = new FIFO<T>(); // Default to FIFO algorithm
    }

    Scheduler(ScheduleType type) {
        setAlgorithm(type);
    }

    ~Scheduler() {
        if (algorithm != nullptr) {
            delete algorithm;
        }
    }

    void setAlgorithm(ScheduleType type) {
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

    void addOperation(T new_operation) {
        this->schedule_queue.push(new_operation);
        if (algorithm->getType() == ScheduleType::PRIORITY) {
            this->schedule_queue_func();
        }
    }

    T getNextOperation() {
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

    T popQueue() {
        if (!this->schedule_queue.empty()) {
            T next_operation = this->schedule_queue.front();
            this->schedule_queue.pop();
            return next_operation;
        } else {
            std::cout << "Queue is empty" << std::endl;
            return T(); // Return a default-constructed object
        }
    }

    void schedule_queue_func() {
        std::vector<T> dataoperation;
        while (!this->schedule_queue.empty()) {
            dataoperation.push_back(this->schedule_queue.front());
            this->schedule_queue.pop();
        }
        this->schedule_queue = this->algorithm->schedule(dataoperation);
    }
};

// Probar el calendarizador
int mainSche() {
    // Crear un scheduler
    Scheduler<operation> scheduler(ScheduleType::FIFO);
    
    /*

    // Crear algunos procesos
    std::vector<operation> operations;
    operations.push_back(operation::withMaxTime_Deadline_Period(1, 20, 1, 4));
    operations.push_back(operation::withMaxTime_Deadline_Period(2, 20, 2, 8));
    operations.push_back(operation::withMaxTime_Deadline_Period(3, 20, 4, 16));
    
    // Calendarizar los procesos
    std::queue<operation> scheduled_operations = scheduler.schedule(operations);
    
    // Imprimir los procesos programados
    while (!scheduled_operations.empty()) {
        operation p = scheduled_operations.front();
        scheduled_operations.pop();
        std::cout << "operation ID: " << p.getoperationID() << ", Remaining time: " << p.getRemainingTime() << ", Deadline: " << p.getRemainingDeadline() << std::endl;
        //std::cout << p.getoperationID() << ", "  ;
    }
    */
    return 0;

}

