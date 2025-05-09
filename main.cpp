
#include <thread>
#include "units/memory.h"
#include "units/executeunit.cpp"
#include "units/messagemanagement.h"
#include "units/schedulers/scheduler.h"
#include "units/datamessages.h"
#include "PEs/PE_class.h"

// estructura para pasar datos a los hilos
struct PEThreadData {
    PE* pe;
    int id;
    MessageManagementUnit* mmu;
};

void* peTask(void* arg) {
    // logic for each PE
    
    PEThreadData* data = static_cast<PEThreadData*>(arg);
    PE* pe = data->pe;
    int id = data->id;

    uint8_t datos[Cache::BLOCK_SIZE];
    /*
    // Datos para llenar la caché (16 bytes por línea)
    
    for (int i = 0; i < Cache::BLOCK_SIZE; ++i) {
        datos[i] = i + 1; // Llenar con números del 1 al 16

        // Llenar la cache de 4 en 4 bytes
    }
    */

    // Llenar la cache con valores secuenciales en palabras de 4 bytes
    for (int i = 0; i < Cache::BLOCK_SIZE / 4; ++i) {
        uint32_t value = (i * 1024) * 2; // Llenar con números del 4 al 64
        std::memcpy(datos + i * 4, &value, sizeof(value)); // Copiar el valor en la posición correspondiente
    }

    // Llenar las 128 líneas de la caché
    for (int i = 0; i < Cache::BLOCKS; ++i) {
        //std::cout << "Escribiendo en la línea de caché " << i << "...\n";
        pe->readResp(datos, sizeof(datos)); // Usar readResp para escribir en la caché
    }

    // Escribir dos líneas adicionales para comprobar el sistema FIFO
    uint8_t extraData1[Cache::BLOCK_SIZE] = {1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0};

    //std::cout << "Escribiendo línea adicional 1 (FIFO)...\n";
    pe->readResp(extraData1, sizeof(extraData1)); // Escribir línea adicional 1

    /*
    // Verificar el contenido de la caché
    uint8_t buffer[Cache::BLOCK_SIZE];
    for (int i = 0; i < Cache::BLOCKS; ++i) {
        pe->cache.read(i, buffer, Cache::BLOCK_SIZE);
        std::cout << "Contenido de la línea " << i << ": ";
        for (int j = 0; j < Cache::BLOCK_SIZE; ++j) {
            std::cout << static_cast<int>(buffer[j]) << " ";
        }
        std::cout << "\n";
    }
    */


    // leer mensajes desde un archivo
    std::string filename = "PEs/instrucciones" + std::to_string(id) + ".txt";
    std::vector<Message> messages = pe->loadMessagesFromFile(filename);

    // procesar mensajes
    std::vector<ProcessedMessage> processedMessages = pe->processMessages(messages);
    // iterar sobre el vector de mensajes procesados y enviar un mensaje al MMU

    
    // iterar sobre el vector de mensajes procesados y enviarlos al MMU
    for (const auto& msg : processedMessages) {
        
        // castear el mensaje procesado y enviarlo al MMU
        std::visit([data](auto&& m) {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, WRITE_MEM>) {
                data->mmu->processMessage(m); // invocar al metodo que recibe los mensajes en el MMU
            } else if constexpr (std::is_same_v<T, READ_MEM>) {
                data->mmu->processMessage(m); // invocar al metodo que recibe los mensajes en el MMU
            } else if constexpr (std::is_same_v<T, BROADCAST_INVALIDATE>) {
                data->mmu->processMessage(m); // invocar al metodo que recibe los mensajes en el MMU
            }
        }, msg);
        
        // Esperar la respuesta del MMU
        data->pe->waiting = true;
        /*
        while (data->pe->waiting) {
            std::this_thread::yield();
        }
        */
    }
    
    return nullptr; // Terminar el hilo
}

// Prueba de calendarizador, memoria, unidad de ejecucion y unidad de gestion de mensajes
int main() {
    
    // Crear instancias de los calendarizadores
    Scheduler<operation> operationScheduler;
    Scheduler<data_resp> responseScheduler;

    // Crear mutexes para la sincronización
    std::mutex operationSchedulerMutex;
    std::mutex responseSchedulerMutex;

    

    // Crear memoria principal
    Memory mainMemory;

    // Crear una unidad de ejecución
    ExecuteUnit executeUnit(&operationScheduler, &responseScheduler, &mainMemory, &responseSchedulerMutex);

    std::array<PE, 4> pes = {PE(0), PE(1), PE(2), PE(3)};
    PEThreadData threadData[4];

    // Crear instancia de la unidad de gestión de mensajes
    MessageManagementUnit messageManagementUnit(&operationScheduler, &responseScheduler, 
        &operationSchedulerMutex, &responseSchedulerMutex, &pes);

    // Asignar el MMU a cada PE
    for (int i = 0; i < 4; ++i) {
        pes.at(i).mmu = &messageManagementUnit;
    }

    // crear hilos
    pthread_t threads[4];

    

    // Actualizar todas las unidades varias veces  
    
    
    

    for (int i = 0; i < 4; ++i) {
        threadData[i] = {&pes[i], i, &messageManagementUnit};
        pthread_create(&threads[i], nullptr, peTask, &threadData[i]);
    }

    // Esperar un tiempo para que los hilos se inicien
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    for (int i = 0; i < 100; ++i) {
        std::cout << "----- Ciclo: " << i << " -----" << std::endl;
        mainMemory.update(); // Actualizar la memoria principal
        executeUnit.update(); // Actualizar la unidad de ejecución
        messageManagementUnit.update(); // Actualizar la unidad de gestión de mensajes
    }

    // esperar a que todos los hilos terminen
    for (int i = 0; i < 4; ++i) {
        pthread_join(threads[i], nullptr);
    }


    

    
    /*
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
    */
    return 0;
    

}