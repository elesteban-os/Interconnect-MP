#include <iostream>
#include "PE_class.cpp"

int main() {
    
    // Crear un objeto PE
    PE pe(0);
    
    // Datos para llenar la caché (16 bytes por línea)
    uint8_t data[Cache::BLOCK_SIZE];
    for (int i = 0; i < Cache::BLOCK_SIZE; ++i) {
        data[i] = i + 1; // Llenar con números del 1 al 16
    }

    // Llenar las 128 líneas de la caché
    for (int i = 0; i < Cache::BLOCKS; ++i) {
        std::cout << "Escribiendo en la línea de caché " << i << "...\n";
        pe.readResp(data, sizeof(data)); // Usar readResp para escribir en la caché
    }

    // Escribir dos líneas adicionales para comprobar el sistema FIFO
    uint8_t extraData1[20] = {101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120};
    uint8_t extraData2[Cache::BLOCK_SIZE] = {201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216};

    std::cout << "Escribiendo línea adicional 1 (FIFO)...\n";
    pe.readResp(extraData1, sizeof(extraData1)); // Escribir línea adicional 1

    std::cout << "Escribiendo línea adicional 2 (FIFO)...\n";
    pe.readResp(extraData2, sizeof(extraData2)); // Escribir línea adicional 2

    // Verificar el contenido de la caché
    uint8_t buffer[Cache::BLOCK_SIZE];
    for (int i = 0; i < Cache::BLOCKS; ++i) {
        pe.cache.read(i, buffer, Cache::BLOCK_SIZE);
        std::cout << "Contenido de la línea " << i << ": ";
        for (int j = 0; j < Cache::BLOCK_SIZE; ++j) {
            std::cout << static_cast<int>(buffer[j]) << " ";
        }
        std::cout << "\n";
    }

    
    WRITE_MEM msg = pe.writeMem(pe.id, 4096, 3, 0, 1); // Llamar a la función writeMem y guardar el mensaje
    std::cout << "PE " << pe.id << " mensaje enviado: SRC=" << static_cast<int>(msg.SRC) << ", ADDR=" << static_cast<int>(msg.ADDR) << ", NUM_CACHE_LINES=" << static_cast<int>(msg.NUM_CACHE_LINES)
              << ", START_CACHE_LINE=" << static_cast<int>(msg.START_CACHE_LINE) << ", QoS=" << static_cast<int>(msg.QoS) << "\n";
    std::cout << "PE " << pe.id << " datos a escribir: ";
    for (int i = 0; i < msg.NUM_CACHE_LINES * Cache::BLOCK_SIZE; ++i) {
        std::cout << static_cast<int>(msg.data[i]) << " "; // se hace cast de uint8_t a int para mostrar el valor en consola
    }
    std::cout << "\n";

    return 0;
}