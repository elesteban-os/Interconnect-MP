
#include "memory.h"
#include "executeunit.cpp"
#include "messagemanagement.h"
#include "schedulers/scheduler.h"
#include "datamessages.h"

// Prueba de calendarizador, memoria, unidad de ejecucion y unidad de gestion de mensajes
int main() {
    // Crear instancias de los calendarizadores
    Scheduler<operation> operationScheduler;
    Scheduler<data_resp> responseScheduler;

    // Crear mutexes para la sincronización
    std::mutex operationSchedulerMutex;
    std::mutex responseSchedulerMutex;

    // Crear instancia de la unidad de gestión de mensajes
    MessageManagementUnit messageManagementUnit(&operationScheduler, &responseScheduler, 
                                                &operationSchedulerMutex, &responseSchedulerMutex);

    // Crear memoria principal
    Memory mainMemory;

    // Crear una unidad de ejecución
    ExecuteUnit executeUnit(&operationScheduler, &responseScheduler, &mainMemory, &responseSchedulerMutex);
    
    // Simular la llegada de mensajes
    WRITE_MEM writeMessage(4, 16); // 4 cache lines, 16 bytes each
    writeMessage.SRC = 1; // ID del PE que envía el mensaje
    writeMessage.ADDR = 0x0000; // Dirección de memoria principal
    writeMessage.QoS = 1; // Prioridad del mensaje
    for (int i = 0; i < 32; ++i) {
        writeMessage.data[i] = i; // Datos a escribir
    }
    writeMessage.NUM_CACHE_LINES = 1; // Número de líneas de caché
    writeMessage.START_CACHE_LINE = 0; // Línea de caché inicial
    messageManagementUnit.processMessage(&writeMessage); // Procesar el mensaje de escritura

    // Simular la llegada de un mensaje de invalidación
    BROADCAST_INVALIDATE invalidateMessage;
    invalidateMessage.SRC = 2; // ID del PE que envía el mensaje
    invalidateMessage.CACHE_LINE = 0; // Línea de caché a invalidar
    invalidateMessage.QoS = 1; // Prioridad del mensaje
    messageManagementUnit.processMessage(&invalidateMessage); // Procesar el mensaje de invalidación

    // Simular la llegada de un mensaje de reconocimiento de invalidación
    INV_ACK ackMessage;
    ackMessage.SRC = 3; // ID del PE que envía el mensaje
    ackMessage.STATUS = 1; // Estado de la línea de caché (válida)
    ackMessage.QoS = 1; // Prioridad del mensaje
    messageManagementUnit.processMessage(&ackMessage); // Procesar el mensaje de reconocimiento de invalidación

    // Simular la llegada de un mensaje de lectura
    READ_MEM readMessage;
    readMessage.SRC = 4; // ID del PE que envía el mensaje
    readMessage.ADDR = 0x0004; // Dirección de memoria principal
    readMessage.SIZE = 16; // Cantidad de bytes a leer
    readMessage.QoS = 1; // Prioridad del mensaje
    messageManagementUnit.processMessage(&readMessage); // Procesar el mensaje de lectura

    // Actualizar todas las unidades varias veces       

    for (int i = 0; i < 30; ++i) {
        std::cout << "----- Ciclo: " << i << " -----" << std::endl;
        mainMemory.update(); // Actualizar la memoria principal
        executeUnit.update(); // Actualizar la unidad de ejecución
        messageManagementUnit.update(); // Actualizar la unidad de gestión de mensajes
    }

    return 0;
    

}