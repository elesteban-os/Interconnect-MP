#include <queue>
#include <vector>
#include <algorithm>
#include <stack>

enum class ScheduleType {
    FIFO,
    PRIORITY
};

// Clase abstracta para los algoritmos (template)
template <typename T>
class ScheduleAlgorithm {
public:
    virtual std::queue<T> schedule(std::vector<T> dataoperation) = 0;
    virtual ScheduleType getType() = 0;
    virtual ~ScheduleAlgorithm() {};
};

// Algoritmo FIFO (template)
template <typename T>
class FIFO : public ScheduleAlgorithm<T> {
public:
    std::queue<T> schedule(std::vector<T> dataoperation) override {
        std::queue<T> q;
        for (T p : dataoperation) {
            q.push(p);
        }
        return q;
    }

    ScheduleType getType() override {
        return ScheduleType::FIFO;
    }
};

// Algoritmo Priority (template)
template <typename T>
class Priority : public ScheduleAlgorithm<T> {
public:
    std::queue<T> schedule(std::vector<T> dataoperation) override {
        std::queue<T> q;
        std::sort(dataoperation.begin(), dataoperation.end(), [](const T& a, const T& b) {
            return a.QoS < b.QoS; // Sort by QoS
        });
        for (T p : dataoperation) {
            q.push(p);
        }
        return q;
    }

    ScheduleType getType() override {
        return ScheduleType::PRIORITY;
    }
};
