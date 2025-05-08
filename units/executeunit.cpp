
#include <iostream>
#include "schedulers/scheduler.h"
#include "memory.h"
#include <mutex>

class ExecuteUnit {
    private:
        operation operationToMemory;
        Scheduler<operation>* operationScheduler = nullptr;
        Scheduler<data_resp>* responseScheduler = nullptr;
        std::mutex* responseSchedulerMutex; // Mutex for thread safety
        Memory *memory = nullptr;
        bool busy = false;

    public:
        ExecuteUnit(Scheduler<operation>* operationScheduler, Scheduler<data_resp>* respScheduler, Memory* mem, std::mutex* respSchedulerMutex) {
            this->responseSchedulerMutex = respSchedulerMutex;
            this->responseScheduler = respScheduler;
            this->operationScheduler = operationScheduler;
            this->memory = mem;
        }

        ~ExecuteUnit() {
            //delete responseScheduler;
            //delete memory;
        }

        void executeOperation(operation op) {
            this->operationToMemory = op;
            this->busy = true;
            if (op.op_type == operation_type::READ) {
                memory->readData(op.address, op.pe_ID, op.blocks, op.QoS);
            } else if (op.op_type == operation_type::WRITE) {
                memory->writeData(op.address, op.pe_ID, op.write_data, op.blocks, op.QoS);
            }
        }

        void update() {
            if (!this->busy) {
                operation op = operationScheduler->getNextOperation();
                if (op.pe_ID != -1) {
                    executeOperation(op);
                }
            } else if (memory->get_op_ready()) {
                std::lock_guard<std::mutex> lock(*responseSchedulerMutex); // Lock the mutex

                data_resp* response = memory->get_data_response();
                responseScheduler->addOperation(*response);
                this->busy = false;

                // Eliminar operacion de la cola de operaciones
                operationScheduler->popQueue();

            }
        }


};

