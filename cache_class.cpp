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
    std::array<uint8_t, BLOCKS> validity; // array para almacenar el estado de validez de cada línea (1 = válida, 0 = inválida)

    Cache() {
        // inicializar la memoria a cero
        for (auto& block : memory) {
            block.fill(0);
        }
        validity.fill(0); // Inicializar todas las líneas como inválidas
    }

    /* métodos de la clase */

    /* método para escribir 1 linea (bloque) en la cache
     * parámetros: indice del bloque (blockIndex), dato a escribir (data), indice al ultimo elemento a copiar (len) 
     */

    void write(uint8_t blockIndex, const uint8_t* data, uint8_t len) {
        if (blockIndex >= 0 && blockIndex < BLOCKS && len <= BLOCK_SIZE) {
            std::copy(data, data + len, memory[blockIndex].begin()); // data + len es para decir hasta donde debe escribir del array a copiar (data)
            validity[blockIndex] = 1; // Marcar la línea como válida después de escribir
        }
    }

    /* método para leer 1 linea (bloque) de la cache
     * parámetros: indice del bloque (blockIndex), puntero al arreglo donde se guardará lo leido, indice al ultimo elemento a leer (len) 
     */
    void read(int blockIndex, uint8_t* outBuffer, uint8_t len) const {
        if (blockIndex >= 0 && blockIndex < BLOCKS && len <= BLOCK_SIZE) {
            if (validity[blockIndex] == 1) { // Solo leer si la línea es válida
                std::copy(memory[blockIndex].begin(), memory[blockIndex].begin() + len, outBuffer);
            } else {
                std::cerr << "Error: Intento de leer una línea de caché inválida " << blockIndex << "\n";
            }
        }
    }

    /* método para escribir uno o varios bloques en la cache
     * parámetros: índice del bloque inicial (startBlockIndex), puntero al buffer de entrada (data), número de bloques a escribir (numBlocks)
     */
    void writeCacheLines(uint8_t startBlockIndex, const uint8_t* data, uint8_t numBlocks) {
        for (size_t i = 0; i < numBlocks; ++i) {
            write(startBlockIndex + i, data + (i * BLOCK_SIZE), BLOCK_SIZE);
        }
    }

    /* método para leer uno o varios bloques de la cache
     * parámetros: índice del bloque inicial (startBlockIndex), puntero al buffer de salida (outBuffer), número de bloques a leer (numBlocks)
     */
    void readCacheLines(uint8_t startBlockIndex, uint8_t* outBuffer, uint8_t numBlocks) const {
        for (size_t i = 0; i < numBlocks; ++i) {
            read(startBlockIndex + i, outBuffer + (i * BLOCK_SIZE), BLOCK_SIZE);
        }
    }

    /* método para invalidar una línea de caché
     * parámetros: índice del bloque a invalidar (blockIndex)
     */ 
    void invalidate(uint8_t blockIndex) {
        if (blockIndex >= 0 && blockIndex < BLOCKS) {
            validity[blockIndex] = 0; // Marcar la línea como inválida
        }
    }

    /* método para verificar si una línea es válida
     * parámetros: índice del bloque a verificar (blockIndex)
     */
    bool isValid(uint8_t blockIndex) const {
        if (blockIndex >= 0 && blockIndex < BLOCKS) {
            return validity[blockIndex] == 1;
        }
        return false;
    }
};