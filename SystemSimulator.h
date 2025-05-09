#ifndef SYSTEMSIMULATOR_H
#define SYSTEMSIMULATOR_H

#include "units/memory.h"
#include "units/executeunit.cpp"
#include "units/messagemanagement.h"
#include "units/schedulers/scheduler.h"
#include "units/datamessages.h"
#include "PEs/PE_class.h"
#include "units/clock.h"
#include "units/messagetimer.h"
#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <pthread.h>

class SystemSimulator {
public:
    SystemSimulator();
    ~SystemSimulator();
    void start();
    int step();
    void waitForThreads();

    // Puedes agregar getters para acceder a los estados si la interfaz los necesita

private:
    Clock clock;
    MessageTimer messageTimer;
    std::mutex messageTimerMutex;
    Scheduler<operation> operationScheduler;
    Scheduler<data_resp> responseScheduler;
    std::mutex operationSchedulerMutex;
    std::mutex responseSchedulerMutex;
    Memory mainMemory;
    ExecuteUnit executeUnit;
    std::array<PE, 4> pes;
    std::vector<PEThreadData> threadData;
    MessageManagementUnit messageManagementUnit;
    pthread_t threads[4];
    bool started = false;
};

#endif // SYSTEMSIMULATOR_H