#ifndef MESSAGEMANAGEMENT_H
#define MESSAGEMANAGEMENT_H

#include "schedulers/scheduler.h"
#include "../PEs/PE_class.h"
#include "datamessages.h"
#include <mutex>

// Forward declarations for message types
struct READ_MEM;
struct WRITE_MEM;
struct BROADCAST_INVALIDATE;
struct INV_ACK;

struct invalidation_message {
    int pe_id;
    int numPEInvalidations;
};

class PE; // Forward declaration of PE class

class MessageManagementUnit {
    private:
        Scheduler<operation>* operationScheduler;
        Scheduler<data_resp>* responseScheduler;
        std::mutex* operationSchedulerMutex; // Mutex for thread safety
        std::mutex* responseSchedulerMutex;  // Mutex for thread safety
        std::array<PE, 4> *pes;
        std::vector<invalidation_message> invalidationMessages;

    public:
        // Constructor
        MessageManagementUnit(Scheduler<operation>* opScheduler, Scheduler<data_resp>* respScheduler, 
                              std::mutex* opSchedulerMutex, std::mutex* respSchedulerMutex);

        MessageManagementUnit(Scheduler<operation>* opScheduler, Scheduler<data_resp>* respScheduler, 
                              std::mutex* opSchedulerMutex, std::mutex* respSchedulerMutex, std::array<PE, 4> *pes);

        // Methods for processing different message types
        void processMessage(READ_MEM readMessage);
        void processMessage(WRITE_MEM writeMessage);
        void processMessage(BROADCAST_INVALIDATE readMessage);
        void processMessage(INV_ACK readMessage);
        void update();
};

#endif // MESSAGEMANAGEMENT_H