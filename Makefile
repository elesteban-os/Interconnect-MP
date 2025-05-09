CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread

# Archivos fuente principales
SRCS = main.cpp \
       PEs/PE_class.cpp \
       units/memory.cpp \
       units/messagemanagement.cpp \
       units/datamessages.cpp \
       units/schedulers/scheduler.cpp \
       units/cache_class.cpp \
       units/interpreter.cpp \
       units/executeunit.cpp

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Ejecutable final
TARGET = interconnect_mp

all: 
    $(TARGET)
	$(TARGET): $(OBJS)
    $(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
    rm -f $(OBJS) $(TARGET)