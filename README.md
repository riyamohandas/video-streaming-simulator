# Video Stream Buffering & Adaptive Bitrate Simulator

## Executive Summary
Simulates real-time adaptive bitrate (ABR) streaming used by Netflix, YouTube, and OTT platforms.

## Technologies
- C++17
- Data Structures (Queues, Vectors)
- Object-Oriented Design
- Network Simulation
- Streaming Algorithms

## Compilation
```bash
g++ -std=c++17 src/simulator.cpp -o build/streamer.exe
```

## Execution
```bash
./build/streamer.exe
```

## Features
- Two ABR Algorithms (Always-Best-Fit, Buffer-Based Adaptation)
- Network Bandwidth Simulation
- Real-time Buffer Management
- Comprehensive Metrics & Statistics
- Rebuffering Detection

## Project Structure
