#ifndef CLOCK_H
#define CLOCK_H

class Clock {
    private:
        bool clk = false; // Clock state
        int cycle = 0;    // Cycle counter

    public:
        Clock();          // Constructor
        ~Clock();         // Destructor

        void update();    // Update the clock
        bool getClock();  // Get the current clock state
        int getCycle();   // Get the current cycle count
};

#endif // CLOCK_H