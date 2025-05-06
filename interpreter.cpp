#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include <iomanip>

// Tipos de mensajes
enum class MessageType {
    WRITE_MEM,
    READ_MEM,
    BROADCAST_INVALIDATE
};

// Estructuras de datos
struct WriteMem {
    uint8_t src;
    uint32_t addr;
    uint8_t num_of_cache_lines;
    uint8_t start_cache_line;
    uint8_t qos;
};

struct ReadMem {
    uint8_t src;
    uint32_t addr;
    uint32_t size;
    uint8_t qos;
};

struct BroadcastInvalidate {
    uint8_t src;
    uint8_t cache_line;
    uint8_t qos;
};

using Message = std::variant<WriteMem, ReadMem, BroadcastInvalidate>;

// Interconnect
class Interconnect {
public:
    void process(const Message& msg) {
        std::visit([&](auto&& m) { handle(m); }, msg);
    }

private:
    void handle(const WriteMem& m) {
        std::cout << "[WRITE_MEM] SRC: 0x" << std::hex << +m.src
                  << " ADDR: 0x" << m.addr
                  << " LINES: " << +m.num_of_cache_lines
                  << " START: 0x" << +m.start_cache_line
                  << " QoS: " << +m.qos << '\n';
    }

    void handle(const ReadMem& m) {
        std::cout << "[READ_MEM] SRC: 0x" << std::hex << +m.src
                  << " ADDR: 0x" << m.addr
                  << " SIZE: " << std::dec << m.size
                  << " QoS: " << std::dec << +m.qos << '\n';
    }

    void handle(const BroadcastInvalidate& m) {
        std::cout << "[BROADCAST_INVALIDATE] SRC: 0x" << std::hex << +m.src
                  << " CACHE_LINE: 0x" << +m.cache_line
                  << " QoS: " << +m.qos << '\n';
    }
};

// Función auxiliar para convertir strings
uint32_t parse_value(const std::string& token) {
    return token.find("0x") == 0 ? std::stoul(token, nullptr, 16) : std::stoul(token);
}

// Función principal de carga de instrucciones
void process_instructions(const std::string& filename, Interconnect& ic) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Eliminar comas
        for (char& c : line) {
            if (c == ',') c = ' ';
        }

        std::istringstream ss(line);
        std::string token;
        ss >> token;

        // Saltar líneas tipo PE2 (no se usan como source)
        if (token.rfind("PE", 0) == 0) continue;

        if (token == "WRITE_MEM") {
            std::string src_s, addr_s, lines_s, start_s, qos_s;
            ss >> src_s >> addr_s >> lines_s >> start_s >> qos_s;

            WriteMem wm;
            wm.src = parse_value(src_s);
            wm.addr = parse_value(addr_s);
            wm.num_of_cache_lines = parse_value(lines_s);
            wm.start_cache_line = parse_value(start_s);
            wm.qos = parse_value(qos_s);
            ic.process(wm);

        } else if (token == "READ_MEM") {
            std::string src_s, addr_s, size_s, qos_s;
            ss >> src_s >> addr_s >> size_s >> qos_s;

            ReadMem rm;
            rm.src = parse_value(src_s);
            rm.addr = parse_value(addr_s);
            rm.size = parse_value(size_s);
            rm.qos = parse_value(qos_s);
            ic.process(rm);

        } else if (token == "BROADCAST_INVALIDATE") {
            std::string src_s, cl_s, qos_s;
            ss >> src_s >> cl_s >> qos_s;

            BroadcastInvalidate bi;
            bi.src = parse_value(src_s);
            bi.cache_line = parse_value(cl_s);
            bi.qos = parse_value(qos_s);
            ic.process(bi);
        }
    }
}



int main() {
    Interconnect ic;
    process_instructions("instrucciones.txt", ic);
    return 0;
}
