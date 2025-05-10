
#include <thread>
#include <variant>
#include <condition_variable>
#include "units/memory.h"
#include "units/executeunit.cpp"
#include "units/messagemanagement.h"
#include "units/schedulers/scheduler.h"
#include "units/datamessages.h"
#include "PEs/PE_class.h"
#include "units/clock.h"
#include "units/messagetimer.h"


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

    struct Visitor {
        MessageManagementUnit* mmu;
        void operator()(const WRITE_MEM& msg) const {
            mmu->processMessage(msg);
        }
        void operator()(const READ_MEM& msg) const {
            mmu->processMessage(msg);
        }
        void operator()(const BROADCAST_INVALIDATE& msg) const {
            mmu->processMessage(msg);
        }
    };
    
    // iterar sobre el vector de mensajes procesados y enviarlos al MMU
    for (const auto& msg : processedMessages) {
        
        
        // Obtener el tamano de bloques a escribir
        int numCacheLines = 0;
        if (std::holds_alternative<WRITE_MEM>(msg)) {
            numCacheLines = std::get<WRITE_MEM>(msg).NUM_CACHE_LINES;
        } else numCacheLines = -1;

        int time;
        time = (numCacheLines == -1) ? data->messageTimer->getCycles() + 2 : data->messageTimer->getCycles() + (numCacheLines * 4) + 1; // 4 ciclos por cada bloque a escribir
        std::cout << "Ciclo hasta enviar mensaje del PE " << id << ": " << time - 1 << std::endl;
        data->messageTimer->getClock()->waitForCycle(time); // Esperar un ciclo para simular el tiempo de procesamiento
        //std::cout << "Despues del clock para el PE " << id << std::endl;

        std::visit(Visitor{data->mmu}, msg); // Enviar el mensaje al MMU
        
        // Esperar la respuesta del MMU
        data->pe->waiting = true;
        // Esperar la respuesta del MMU
        data->responseReady = false;
        std::cout << "Esperando respuesta del MMU para el PE " << id << std::endl;
        std::unique_lock<std::mutex> lock(data->mtx);
        data->cv.wait(lock, [data]() { return data->responseReady; });

        // Procesar la respuesta
        std::cout << "Respuesta recibida para el PE " << id << std::endl;

        // Reiniciar la bandera para la próxima iteración
        data->responseReady = false;
        /*
        while (data->pe->waiting) {
            std::this_thread::yield();
        }
        */
    }

    // Instrucciones terminadas
    std::cout << "Instrucciones terminadas para el PE " << id << std::endl;
    data->pe->finished = true;
    
    return nullptr; // Terminar el hilo
}

// Prueba de calendarizador, memoria, unidad de ejecucion y unidad de gestion de mensajes
int main() {    
    // Crear un reloj
    Clock clock;
    // Crear un temporizador de mensajes
    MessageTimer messageTimer(&clock);
    std::mutex messageTimerMutex;
    
    // Crear instancias de los calendarizadores
    Scheduler<operation> operationScheduler;
    Scheduler<data_resp> responseScheduler;

    // Crear mutexes para la sincronización
    std::mutex operationSchedulerMutex;
    std::mutex responseSchedulerMutex;

    // Crear memoria principal
    Memory mainMemory(&clock);

    // Crear una unidad de ejecución
    ExecuteUnit executeUnit(&operationScheduler, &responseScheduler, &mainMemory, &responseSchedulerMutex, &messageTimer);
    int NUMPES = 8; // Número de PEs
    std::array<PE, 8> pes = {PE(0, &messageTimer), PE(1, &messageTimer), PE(2, &messageTimer), PE(3, &messageTimer), 
        PE(4, &messageTimer), PE(5, &messageTimer) , PE(6, &messageTimer), PE(7, &messageTimer)};
    //std::array<PE, 1> pes = {PE(0, &messageTimer)};

    
    std::vector<PEThreadData> threadData(NUMPES); // Vector para almacenar los datos de los hilos

    // Crear instancia de la unidad de gestión de mensajes
    MessageManagementUnit messageManagementUnit(&operationScheduler, &responseScheduler, 
        &operationSchedulerMutex, &responseSchedulerMutex, &pes, &threadData, &messageTimer);

    // Asignar el MMU a cada PE
    for (int i = 0; i < NUMPES; ++i) {
        pes.at(i).mmu = &messageManagementUnit;
    }

    // crear hilos
    pthread_t threads[NUMPES];

    

    // Actualizar todas las unidades varias veces  
    for (int i = 0; i < NUMPES; ++i) {
        threadData[i] = {&pes[i], i, &messageManagementUnit, &messageTimer};
        pthread_create(&threads[i], nullptr, peTask, &threadData[i]);
    }

    // Esperar un tiempo para que los hilos se inicien
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //for (int i = 0; i < 10000; ++i) {
    while(!pes[0].finished || !pes[1].finished || !pes[2].finished || !pes[3].finished) {
        
        std::cout << "----- Ciclo: " << clock.getCycle() << " -----" << std::endl;
        messageTimer.update(); // Actualizar el temporizador de mensajes
        mainMemory.update(); // Actualizar la memoria principal
        executeUnit.update(); // Actualizar la unidad de ejecución
        messageManagementUnit.update(); // Actualizar la unidad de gestión de mensajes
        clock.update(); // Actualizar el reloj
        // Avanzar el ciclo presionando enter
        std::cout << "Presione enter para continuar..." << std::endl;
        std::cin.get();
    }

    // esperar a que todos los hilos terminen
    for (int i = 0; i < 4; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Imprimir las caches de cada PE
    for (int i = 0; i < NUMPES; ++i) {
        std::cout << "Contenido de la caché del PE " << i << ": \n";
        for (int j = 0; j < Cache::BLOCKS; ++j) {
            std::cout << "Linea " << j << ", Validez: " << (int)pes[i].cache.validity[j] << " Datos: ";
            for (int k = 0; k < Cache::BLOCK_SIZE; ++k) {
                std::cout << (int)pes[i].cache.memory[j][k] << " " ;
            }
            std::cout << " \n" ;
        }
    }
    std::cout << "Fin de la simulación." << std::endl;


    

    
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