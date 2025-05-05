#include <iostream>
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

// Estructuras de datos para cada tipo de mensaje
struct WriteMem {
    uint8_t src;
    uint32_t addr;
    uint8_t num_of_cache_lines;
    uint32_t start_cache_line;
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
    uint32_t cache_line;
    uint8_t qos;
};

// Mensaje genérico usando std::variant
using Message = std::variant<WriteMem, ReadMem, BroadcastInvalidate>;

// Interconnect que procesa los mensajes
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
                  << " START: 0x" << m.start_cache_line
                  << " QoS: 0x" << +m.qos << '\n';
    }

    void handle(const ReadMem& m) {
        std::cout << "[READ_MEM] SRC: 0x" << std::hex << +m.src
                  << " ADDR: 0x" << m.addr
                  << " SIZE: " << std::dec << m.size
                  << " QoS: 0x" << std::hex << +m.qos << '\n';
    }

    void handle(const BroadcastInvalidate& m) {
        std::cout << "[BROADCAST_INVALIDATE] SRC: 0x" << std::hex << +m.src
                  << " CACHE_LINE: 0x" << m.cache_line
                  << " QoS: 0x" << +m.qos << '\n';
    }
};

// Simulación simple de un PE ejecutando instrucciones
void simulate_pe(uint8_t pe_id, Interconnect& interconnect) {
    // Por ejemplo, ejecuta una WRITE_MEM y un READ_MEM
    WriteMem wm{pe_id, 0x1000, 2, 0x200, 0x10};
    ReadMem rm{pe_id, 0x1000, 64, 0x10};
    BroadcastInvalidate bi{pe_id, 0x200, 0x10};

    interconnect.process(wm);
    interconnect.process(rm);
    interconnect.process(bi);
}

int main() {
    Interconnect ic;
    for (uint8_t i = 0; i < 8; ++i) {
        simulate_pe(i, ic);
    }
    return 0;
}
