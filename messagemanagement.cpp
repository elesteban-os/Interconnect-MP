
#include "temppe.cpp"   // Incluir temporalmente este archivo
#include "schedulers/scheduler.cpp" // Incluir el archivo de schedulers
#include "datamessages.h"   // Incluir el archivo de mensajes de datos
#include <vector>

class MessageManagementUnit {
    private:
        Scheduler<operation>* operationScheduler;
        Scheduler<data_resp>* responseScheduler;
        // std::vector<PE> vectorPE;                   // Vector de Processing Elements
        
    public:
        // Constructor 
        MessageManagementUnit(Scheduler<operation>* opScheduler, Scheduler<data_resp>* respScheduler) {
            this->operationScheduler = opScheduler;
            this->responseScheduler = respScheduler;
        }

        // Destructor
        ~MessageManagementUnit() {
            delete operationScheduler;
            delete responseScheduler;
        }

        // Metodo para un mensaje de READ (overloading)
        void processMessage(READ_MEM* readMessage) {
            operation newOperation;
            newOperation.pe_ID = readMessage->SRC;
            newOperation.op_type = operation_type::READ;
            newOperation.address = readMessage->ADDR;
            newOperation.blocks = readMessage->SIZE / 4; // Convertir bytes a bloques
            newOperation.QoS = readMessage->QoS;

            operationScheduler->addOperation(newOperation);
        }




        
};