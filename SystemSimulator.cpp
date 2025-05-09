#include "SystemSimulator.h"
#include <iostream>
#include <chrono>
#include <thread>

SystemSimulator::SystemSimulator()
    : messageTimer(&clock),
      mainMemory(&clock),
      executeUnit(&operationScheduler, &responseScheduler, &mainMemory, &responseSchedulerMutex, &messageTimer),
      pes{PE(0, &messageTimer), PE(1, &messageTimer), PE(2, &messageTimer), PE(3, &messageTimer)},
      threadData(4),
      messageManagementUnit(&operationScheduler, &responseScheduler, &operationSchedulerMutex, &responseSchedulerMutex, &pes, &threadData, &messageTimer)
{
    for (int i = 0; i < 4; ++i) {
        pes.at(i).mmu = &messageManagementUnit;
    }
}

SystemSimulator::~SystemSimulator() {
    waitForThreads();
}

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

void SystemSimulator::start() {
    if (started) return;
    started = true;
    for (int i = 0; i < 4; ++i) {
        threadData[i] = {&pes[i], i, &messageManagementUnit, &messageTimer};
        pthread_create(&threads[i], nullptr, peTask, &threadData[i]);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void SystemSimulator::step() {
    std::cout << "----- Ciclo: " << clock.getCycle() << " -----" << std::endl;
    messageTimer.update();
    mainMemory.update();
    executeUnit.update();
    messageManagementUnit.update();
    clock.update();
}

void SystemSimulator::waitForThreads() {
    if (!started) return;
    for (int i = 0; i < 4; ++i) {
        pthread_join(threads[i], nullptr);
    }
}