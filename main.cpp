#include <array>
#include <iostream>
#include <pthread.h>
#include <unistd.h> // Para usar sleep()
#include "PE_class.cpp"

// número de PEs
constexpr int NUM_PES = 8;

// estructura para pasar datos a los hilos
struct PEThreadData {
    PE* pe;
    int id;
};

// función que ejecutará cada hilo
void* peTask(void* arg) {
    PEThreadData* data = static_cast<PEThreadData*>(arg);
    PE* pe = data->pe;
    int id = data->id;

    // datos a escribir en la caché (puede ser cualquier numero pero se escoge el id del PE +1+2+3 para que no se repitan)
    uint8_t writeData[Cache::BLOCK_SIZE] = {static_cast<uint8_t>(id), static_cast<uint8_t>(id + 1), static_cast<uint8_t>(id + 2)};
    uint8_t readData[Cache::BLOCK_SIZE] = {0}; // buffer para leer datos inicializado a cero

    // escribir en la caché
    std::cout << "PE " << id << " escribiendo en su caché...\n";
    pe->cache.write(0, writeData, 3);
    sleep(1); // Delay de 1 segundo

    // leer de la caché
    std::cout << "PE " << id << " leyendo de su caché...\n";
    pe->cache.read(0, readData, 3);
    sleep(1); // Delay de 1 segundo

    // mostrar los datos leídos
    std::cout << "PE " << id << " datos leídos: ";
    for (int i = 0; i < 3; ++i) {
        std::cout << static_cast<int>(readData[i]) << " "; // se hace cast de uint8_t a int para mostrar el valor en consola
    }
    std::cout << "\n";

    return nullptr;
}

int main() {
    // crear 8 PEs
    std::array<PE, NUM_PES> pes = {PE(0), PE(1), PE(2), PE(3), PE(4), PE(5), PE(6), PE(7)};

    // crear hilos
    pthread_t threads[NUM_PES];
    PEThreadData threadData[NUM_PES];

    for (int i = 0; i < NUM_PES; ++i) {
        threadData[i] = {&pes[i], i};
        pthread_create(&threads[i], nullptr, peTask, &threadData[i]);
    }

    // esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_PES; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return 0;
}