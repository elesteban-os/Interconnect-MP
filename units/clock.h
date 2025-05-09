#ifndef CLOCK_H
#define CLOCK_H

#include <condition_variable>

class Clock {
    private:
        bool clk = false; // Clock state
        int cycle = 0;    // Cycle counter
        std::mutex clockMutex; // Mutex for thread safety
        std::condition_variable clockCV; // Condition variable for synchronization

    public:
        Clock();          // Constructor
        ~Clock();         // Destructor

        void update();    // Update the clock
        bool getClock();  // Get the current clock state
        int getCycle();   // Get the current cycle count
        void waitForCycle(int targetCycle); // Wait for the next cycle
};

#endif // CLOCK_H