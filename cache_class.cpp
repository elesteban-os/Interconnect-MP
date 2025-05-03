#include <iostream>
#include <array>
#include <cstdint>

// Clase Cache
class Cache {
public:
    /* atributos de la clase */
    static const int BLOCKS = 128;
    static const int BLOCK_SIZE = 16;
    std::array<std::array<uint8_t, BLOCK_SIZE>, BLOCKS> memory; // memoria de 128 bloques de 16 bytes (array)

    Cache() {
        // inicializar la memoria a cero
        for (auto& block : memory) {
            block.fill(0);
        }
    }

    /* métodos de la clase */

    /* método para escribir en la cache
     * parámetros: indice del bloque (blockIndex), dato a escribir (data), indice al ultimo elemento a copiar (len) 
     */

    void write(int blockIndex, const uint8_t* data, size_t len) {
        if (blockIndex >= 0 && blockIndex < BLOCKS && len <= BLOCK_SIZE) {
            std::copy(data, data + len, memory[blockIndex].begin()); // data + len es para decir hasta donde debe escribir del array a copiar (data)
        }
    }

    /* método para leer de la cache
     * parámetros: indice del bloque (blockIndex), puntero al arreglo donde se guardará lo leido, indice al ultimo elemento a leer (len) 
     */
    void read(int blockIndex, uint8_t* outBuffer, size_t len) const {
        if (blockIndex >= 0 && blockIndex < BLOCKS && len <= BLOCK_SIZE) {
            std::copy(memory[blockIndex].begin(), memory[blockIndex].begin() + len, outBuffer);
        }
    }
};