# 🎬 Video Streaming Simulator

A comprehensive C++ simulation of adaptive bitrate (ABR) streaming algorithms and network conditions for video playback. This project demonstrates real-world video streaming technologies like those used by Netflix, YouTube, and other streaming platforms.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Technologies](#technologies)
- [Installation](#installation)
- [Usage](#usage)
- [Algorithms](#algorithms)
- [Network Presets](#network-presets)
- [Architecture](#architecture)
- [Sample Output](#sample-output)
- [Contributing](#contributing)
- [License](#license)

## 🎯 Overview

This simulator models the complete video streaming pipeline including:
- Network bandwidth fluctuations
- Adaptive bitrate selection algorithms
- Buffer management and rebuffering detection
- Quality metrics and performance analysis

Perfect for understanding how streaming services dynamically adjust video quality based on network conditions to provide smooth playback experiences.

## ✨ Features

- **Real-time Network Simulation**: Dynamic bandwidth fluctuations with realistic patterns
- **Multiple ABR Algorithms**: 
  - Always-Best-Fit (aggressive quality maximization)
  - Buffer-Based Adaptation (balanced approach)
- **Network Presets**: Home WiFi, Mobile LTE, Poor Connection, Rural WiFi, 5G, Custom
- **Detailed Analytics**: 
  - Average/min/max bitrate tracking
  - Rebuffer detection and counting
  - Quality score calculation
  - Data transfer metrics
- **Interactive Menu System**: Easy-to-use CLI interface
- **Comparative Analysis**: Run multiple algorithms side-by-side

## 🛠 Technologies

- **Language**: C++ (C++11 or higher)
- **Data Structures**: Queues, Vectors
- **Concepts**: 
  - Object-Oriented Programming
  - Network simulation
  - Real-time systems
  - Adaptive algorithms
  - Buffer management

## 📦 Installation

### Prerequisites

- C++ compiler (g++, clang++, or MSVC)
- Make (optional, for using Makefile)

### Clone the Repository

```bash
git clone https://github.com/yourusername/video-streaming-simulator.git
cd video-streaming-simulator
```

### Build the Project

#### Using Make:
```bash
make
```

#### Manual Compilation:
```bash
g++ -std=c++11 src/simulator.cpp -o simulator
```

#### For Windows (MinGW):
```bash
g++ -std=c++11 src/simulator.cpp -o simulator.exe
```

## 🚀 Usage

### Running the Simulator

```bash
./simulator
```

### Example Workflow

1. **Select Network Preset**: Choose from 5 predefined network conditions or create custom settings
2. **Choose Algorithm**: Select Always-Best-Fit, Buffer-Based, or compare both
3. **View Simulation**: Watch real-time streaming with bandwidth changes, buffer levels, and chunk downloads
4. **Analyze Results**: Review detailed statistics including quality scores and rebuffer events

### Interactive Menu

```
VIDEO STREAM BUFFERING & ADAPTIVE BITRATE SIMULATOR
============================================================
NETWORK PRESET MENU:
1. Home WiFi (Stable: 1000-5000 kbps, Low fluctuation)
2. Mobile LTE (Moderate: 500-3000 kbps, Medium fluctuation)
3. Poor Connection (Unstable: 200-1500 kbps, High fluctuation)
4. Rural WiFi (Limited: 300-2000 kbps, Medium fluctuation)
5. 5G Network (Excellent: 2000-10000 kbps, Low fluctuation)
6. Custom Network Settings
7. Exit
```

## 🧠 Algorithms

### 1. Always-Best-Fit Algorithm

**Strategy**: Aggressively selects the highest possible bitrate based on available bandwidth.

**Characteristics**:
- Maximizes video quality
- Higher risk of rebuffering during bandwidth drops
- Best for stable, high-bandwidth networks
- Quality levels: 480p (SD), 720p (HD), 1080p (Full HD), 2160p (4K)

**Use Case**: Ideal for users with stable connections who prioritize quality.

### 2. Buffer-Based Adaptation Algorithm

**Strategy**: Dynamically adjusts bitrate based on buffer occupancy levels.

**Buffer Thresholds**:
- **Critical** (< 3s): Drops to 480p to prevent rebuffering
- **Low** (< 10s): Conservative 720p quality
- **Normal** (< 20s): Balanced 1080p quality
- **Healthy** (≥ 20s): Maximum quality within bandwidth limits

**Characteristics**:
- Balances quality and playback smoothness
- Reduces rebuffering events
- Adapts to network instability
- Better user experience on variable connections

**Use Case**: Recommended for mobile networks and unstable connections.

## 🌐 Network Presets

| Preset | Min BW | Max BW | Fluctuation | Use Case |
|--------|--------|--------|-------------|----------|
| **Home WiFi** | 1000 kbps | 5000 kbps | Low (±200) | Stable home internet |
| **Mobile LTE** | 500 kbps | 3000 kbps | Medium (±400) | Mobile data streaming |
| **Poor Connection** | 200 kbps | 1500 kbps | High (±600) | Congested/weak networks |
| **Rural WiFi** | 300 kbps | 2000 kbps | Medium (±350) | Limited broadband |
| **5G Network** | 2000 kbps | 10000 kbps | Low (±150) | Next-gen mobile |
| **Custom** | User-defined | User-defined | User-defined | Research & testing |

## 🏗 Architecture

### Core Components

```
┌─────────────────────────────────────────────────┐
│          VideoStreamingEngine                    │
│  - Buffer management (queue)                     │
│  - Playback simulation                           │
│  - Statistics tracking                           │
└─────────────────┬───────────────────────────────┘
                  │
        ┌─────────┴─────────┐
        │                   │
┌───────▼─────────┐  ┌──────▼──────────────┐
│ NetworkSimulator│  │ ABR Algorithm       │
│ - Bandwidth     │  │ - Bitrate selection │
│ - Fluctuations  │  │ - Strategy logic    │
└─────────────────┘  └─────────────────────┘
```

### Class Structure

- **`VideoChunk`**: Represents a video segment with metadata
- **`NetworkSimulator`**: Models dynamic network conditions
- **`AdaptiveBitrateAlgorithm`**: Abstract base class for ABR strategies
- **`AlwaysBestFit`**: Aggressive quality-maximizing algorithm
- **`BufferBasedAdaptation`**: Conservative buffer-aware algorithm
- **`VideoStreamingEngine`**: Main orchestrator managing streaming lifecycle

## 📊 Sample Output

```
VIDEO STREAMING SIMULATION
Algorithm: Buffer-Based Adaptation
Buffer Capacity: 30000 ms | Video Duration: 60 seconds
========================================================
Time(ms)   ChunkID    Bitrate(k)  Size(KB)   Buffer(ms)     Status         NetworkBW(kbps)
--------------------------------------------------------
0          1          1080        270        2000           DOWNLOADED     2400
2000       1          1080        270        0              PLAYING        2300
2100       2          1080        270        2000           DOWNLOADED     2350

STREAMING STATISTICS - Buffer-Based Adaptation
========================================================
Average Bitrate: 1245.50 kbps
Min/Max Bitrate: 720 / 2160 kbps
Total Data Transferred: 18.75 MB
Rebuffers Encountered: 1
Total Rebuffer Time: 500 ms
Video Quality Score: 52.25 / 100
```


