#ifndef MESSAGE_INTERPRETER_H
#define MESSAGE_INTERPRETER_H

#include <string>
#include <vector>
#include <variant>
#include <cstdint>

// Tipos de mensajes
enum class MessageType {
    WRITE_MEM,
    READ_MEM,
    BROADCAST_INVALIDATE
};

// Estructuras de datos
struct WriteMem {
    uint8_t src;
    uint16_t addr;
    uint8_t num_of_cache_lines;
    uint8_t start_cache_line;
    uint8_t qos;
};

struct ReadMem {
    uint8_t src;
    uint16_t addr;
    uint16_t size;
    uint8_t qos;
};

struct BroadcastInvalidate {
    uint8_t src;
    uint8_t cache_line;
    uint8_t qos;
};

using Message = std::variant<WriteMem, ReadMem, BroadcastInvalidate>;

class MessageInterpreter {
public:
    // Carga mensajes desde un archivo
    std::vector<Message> load_messages_from_file(const std::string& filename);

    // Procesa un mensaje y lo imprime
    void process_message(const Message& msg) const;

private:
    // Convierte un string a un valor numérico
    uint32_t parse_value(const std::string& token) const;
};

#endif // MESSAGE_INTERPRETER_H