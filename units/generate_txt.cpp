#include "generate_txt.h"

void saveVectorToFile(const std::vector<int>& data, const std::string& filename) {
    // Abrir/crear el archivo en modo escritura
    std::ofstream outFile(filename);
    
    // Verificar si el archivo se abrió correctamente
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo" << std::endl;
        return;
    }

    // Escribir los datos separados por espacios
    for (size_t i = 0; i < data.size(); i++) {
        outFile << data[i];
        // Agregar espacio después de cada número excepto el último
        if (i < data.size() - 1) {
            outFile << " ";
        }
    }

    // Cerrar el archivo
    outFile.close();
}