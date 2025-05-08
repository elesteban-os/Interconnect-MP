#include "messagemanagement.h"
#include <iostream>
#include <vector>


// Constructor
MessageManagementUnit::MessageManagementUnit(Scheduler<operation>* opScheduler, Scheduler<data_resp>* respScheduler, 
                                             std::mutex* opSchedulerMutex, std::mutex* respSchedulerMutex) {
    this->operationSchedulerMutex = opSchedulerMutex;
    this->responseSchedulerMutex = respSchedulerMutex;
    this->operationScheduler = opScheduler;
    this->responseScheduler = respScheduler;
}

// Method for processing READ_MEM messages
void MessageManagementUnit::processMessage(READ_MEM* readMessage) {
    std::lock_guard<std::mutex> lock(*operationSchedulerMutex); // Lock the mutex
    operation newOperation;
    newOperation.pe_ID = readMessage->SRC;
    newOperation.op_type = operation_type::READ;
    newOperation.address = readMessage->ADDR;
    newOperation.blocks = readMessage->SIZE / 4; // Convert bytes to blocks
    newOperation.QoS = readMessage->QoS;
    operationScheduler->addOperation(newOperation);
}

// Method for processing WRITE_MEM messages
void MessageManagementUnit::processMessage(WRITE_MEM* writeMessage) {
    std::lock_guard<std::mutex> lock(*operationSchedulerMutex); // Lock the mutex
    operation newOperation;
    newOperation.pe_ID = writeMessage->SRC;
    newOperation.op_type = operation_type::WRITE;
    newOperation.address = writeMessage->ADDR;
    newOperation.QoS = writeMessage->QoS;
    newOperation.blocks = writeMessage->NUM_CACHE_LINES * 4; // One cache line = 16 bytes
    newOperation.write_data = new block[newOperation.blocks];
    for (int i = 0; i < newOperation.blocks; ++i) {
        for (int j = 0; j < 4; ++j) {
            newOperation.write_data[i].byte[j] = writeMessage->data[i * 4 + j]; // Copy data from the message
        }
        
    }

    // Imprimir data   
    std::cout << "Data to write: ";
    for (int i = 0; i < newOperation.blocks; ++i) {
        std::cout << newOperation.write_data[i].word << " ";
    }
    std::cout << std::endl;

    // Add the new operation to the scheduler
    operationScheduler->addOperation(newOperation);
}

// Method for processing BROADCAST_INVALIDATE messages
void MessageManagementUnit::processMessage(BROADCAST_INVALIDATE* readMessage) {
    std::lock_guard<std::mutex> lock(*responseSchedulerMutex); // Lock the mutex
    data_resp newResponse;
    newResponse.pe_ID = readMessage->SRC;
    newResponse.op_type = operation_type::CACHE_INVALIDATE;
    newResponse.QoS = readMessage->QoS;
    newResponse.data = new block[1]; // Create a block for the response
    newResponse.data->byte[0] = readMessage->CACHE_LINE; // Store the cache line to invalidate

    // Add the new response to the scheduler
    responseScheduler->addOperation(newResponse);

    std::cout << "Processing BROADCAST_INVALIDATE message from PE: " << (int)readMessage->SRC << std::endl;
}

// Method for processing INV_ACK messages
void MessageManagementUnit::processMessage(INV_ACK* readMessage) {
    std::lock_guard<std::mutex> lock(*responseSchedulerMutex); // Lock the mutex
    data_resp newResponse;
    newResponse.pe_ID = readMessage->SRC;
    newResponse.op_type = operation_type::CACHE_ACK;
    newResponse.QoS = readMessage->QoS;
    newResponse.status = readMessage->STATUS; // Store the cache line status

    // Add the new response to the scheduler
    responseScheduler->addOperation(newResponse);

    std::cout << "Processing INV_ACK message from PE: " << (int)readMessage->SRC << std::endl;
}

// Metodo para actualizar la unidad con los mensajes de respuesta
void MessageManagementUnit::update() {
    std::lock_guard<std::mutex> lock(*responseSchedulerMutex); // Lock the mutex
    
    // Comprobar si hay mensajes en la cola de respuestas
    data_resp response = responseScheduler->getNextOperation();
    if (response.pe_ID != -1) {     
        // Procesar el mensaje de respuesta
        std::cout << "Processing response for PE: " << response.pe_ID << " --> ";

        // Casos según el tipo de operación
        switch (response.op_type) {
            case operation_type::CACHE_ACK:
                // Inv Ack: SRC, DEST, STATUS, QoS
                std::cout << "Cache ACK received from PE:" << (int)response.pe_ID << " with status: " << (int)response.status << std::endl;
                // Procesar el ACK y contar hasta que se haga un COMPLETE (calendarizarlo en respuestas y colocar QoS) (SRC y DEST)
                break;
            case operation_type::CACHE_INVALIDATE:
                // Broadcast Invalidate: SRC, DEST, CACHE_LINE, QoS
                std::cout << "Cache Invalidate received from PE: "<< (int)response.pe_ID << " for cache line: " << (int)response.data->byte[0] << std::endl;
                
                // Llamar a la función de invalidación de cache de cada PE
                DATA_RESP_PE invalidatePEMEssages;
                invalidatePEMEssages.OPERATION_TYPE = OPERATION_TYPE_PE::CACHE_INVALIDATE; // Para que el PE sepa el tipo de funcion
                invalidatePEMEssages.CACHE_LINE = response.data->byte[0]; // CACHE_LINE a invalidar
                invalidatePEMEssages.SRC = response.pe_ID; // ID del PE que recibe el mensaje

                // Enviar el mensaje
                // Obtener el ID del PE que recibe el mensaje por medio del response* y enviar el mensaje
 
                break;
            case operation_type::READ:
                // Read resp: DST, DATA, QoS
                std::cout << "Read operation completed for PE: " << response.pe_ID << std::endl;
                // Process read data if available
                if (response.data != nullptr) {
                    std::cout << "Data read: ";
                    for (int i = 0; i < response.blocks; ++i) {
                        std::cout << (int)response.data[i].word << " ";
                    }
                    std::cout << std::endl;

                    // Send the data to the PE
                    DATA_RESP_PE readPEMessage;
                    readPEMessage.OPERATION_TYPE = OPERATION_TYPE_PE::RESP_READ; // Para que el PE sepa el tipo de funcion
                    readPEMessage.DATA = new uint8_t[response.blocks * 4]; // Dynamically allocate memory for the data
                    for (int i = 0; i < response.blocks; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            readPEMessage.DATA[i * 4 + j] = response.data[i].byte[j]; // Copy data to the message
                        }
                    }

                    readPEMessage.DATA_SIZE = response.blocks * 4; // Size of the data
                    readPEMessage.SRC = response.pe_ID; // ID del PE que recibe el mensaje

                    // Obtener el ID del PE que recibe el mensaje por medio del response* y enviar el mensaje

                }
                break;
            case operation_type::WRITE:
                // Write resp: DST, STATUS, QoS
                // La respuesta solo ocupa el STATUS
                // Enviar el mensaje de respuesta al PE
                DATA_RESP_PE writePEMessage;
                writePEMessage.OPERATION_TYPE = OPERATION_TYPE_PE::RESP_WRITE; // Para que el PE sepa el tipo de funcion
                writePEMessage.STATUS = response.status; // STATUS de la respuesta
                writePEMessage.SRC = response.pe_ID; // ID del PE que recibe el mensaje

                // Obtener el ID del PE que recibe el mensaje por medio del response* y enviar el mensaje

                std::cout << "Write operation completed for PE: " << response.pe_ID << " with STATUS: " << (int)response.status << std::endl;
                break;
            default:
                std::cout << "Unknown response type" << std::endl;
                break;
        }

        // Quitar el mensaje de la cola de respuestas
        responseScheduler->popQueue();
        
    }
}