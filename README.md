# Interconnect Simulation for Multiprocessor Systems

**Course**: CE-4302 Computer Architecture II
**Institution**: Instituto Tecnológico de Costa Rica
**Students**: Esteban Chinchilla Rodriguez, Albert Vega Camacho, Abraham Venegas Mayorga
**Semester**: I-2025

## 📌 Project Overview

This project main objective is to apply the concepts of computer architecture II to the design and implementation of a simulation model of an interconnect as part of a multiprocessor (MP) system, in order to perform a quantitative and qualitative analysis of performance under different arbitration/communication schemes.

## 🧩 System Features

* Implemented in **C++** using **multithreading** capabilities (C++20).
* Developed and compiled using **g++ 13.3.0** on **Linux (Ubuntu)**.
* Interface created using **Qt Creator**.
* Includes support for two arbitration schemes:

  * FIFO (ignores QoS).
  * QoS-based priority.

* Simulation of message transfer times and memory access delays.
* Step-by-step (stepping) simulation support.
* Final report includes statistics for bandwidth, traffic, and METRIC TO BE DECIDE.

## 🧱 Project Structure

Each processing elemenet simulates a private instruction memory and cache and communicates with a shared memory via the Interconnect using the following message types:

* `WRITE_MEM`
* `READ_MEM`
* `BROADCAST_INVALIDATE`
* `INV_ACK`
* `INV_COMPLETE`
* `READ_RESP`
* `WRITE_RESP`

## 📂 Dependencies and Includes

The following standard headers are used in the project:

```cpp
#include <iostream>
#include <mutex>
#include <cstdint>
#include <cstring>
#include <vector>
#include <variant>
#include <array>
```

## 🧪 Workloads

Two distinct workloads were developed to test each arbitration scheme. Each workload uses all 8 PEs, with each executing at least 10 instructions.

## 📊 Output and Visualization

After each workload execution, the system generates:

* Bandwidth graphs.
* Traffic charts.
* METRIC TO BE DECIDED

## ⚙️ Compilation Instructions

### Requirements:

* Ubuntu Linux OS
* Qt Creator (recommended for building the interface)
* g++ 13.3.0 or compatible compiler

### Steps:

1. Clone or extract the project source code.
2. Open the project `.pro` file in **Qt Creator**.
3. Configure the kit (ensure it uses g++ 13.3.0).
4. Build the project via the **Build** menu.
5. Run the simulation from within Qt Creator or using the compiled executable from terminal.

### Terminal (alternative):

```bash
mkdir build && cd build
cmake ..
make
./main
```

