#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>
using namespace std;

// Video chunk structure
struct VideoChunk {
    int chunk_id;
    int bitrate;           // kbps (quality level)
    int size_kb;           // kilobytes
    int duration_ms;       // milliseconds (typically 2000ms = 2 seconds)
    int requested_time;
    int received_time;
    int played_time;
    bool is_downloaded;
};

// Network condition simulator
class NetworkSimulator {
private:
    int current_bandwidth;  // kbps
    int min_bandwidth;
    int max_bandwidth;
    int fluctuation_rate;
    
public:
    NetworkSimulator(int min_bw, int max_bw, int fluct) 
        : min_bandwidth(min_bw), max_bandwidth(max_bw), 
          fluctuation_rate(fluct), current_bandwidth(min_bw) {}
    
    // Simulate network fluctuations (realistic bandwidth changes)
    int simulateBandwidth(int current_time) {
        // Add random fluctuations every second
        if (current_time % 1000 == 0) {
            int change = (rand() % (fluctuation_rate * 2)) - fluctuation_rate;
            current_bandwidth = max(min_bandwidth, 
                                   min(max_bandwidth, current_bandwidth + change));
        }
        return current_bandwidth;
    }
    
    int getBandwidth() { return current_bandwidth; }
    void setBandwidth(int bw) { current_bandwidth = bw; }
    void setParameters(int min_bw, int max_bw, int fluct) {
        min_bandwidth = min_bw;
        max_bandwidth = max_bw;
        fluctuation_rate = fluct;
        current_bandwidth = min_bw;
    }
};

// Adaptive Bitrate Algorithm
class AdaptiveBitrateAlgorithm {
public:
    virtual int selectBitrate(int buffer_level, int bandwidth, 
                             int min_bitrate, int max_bitrate) = 0;
    virtual string getName() = 0;
    virtual ~AdaptiveBitrateAlgorithm() {}
};

// Always-Best-Fit Algorithm: Select highest bitrate possible
class AlwaysBestFit : public AdaptiveBitrateAlgorithm {
public:
    int selectBitrate(int buffer_level, int bandwidth, 
                     int min_bitrate, int max_bitrate) override {
        // Always try to get the best quality
        // Safety: ensure we have enough bandwidth
        vector<int> bitrates = {480, 720, 1080, 2160};
        
        for (int i = bitrates.size() - 1; i >= 0; i--) {
            if (bitrates[i] <= bandwidth && bitrates[i] <= max_bitrate) {
                return bitrates[i];
            }
        }
        return min_bitrate;
    }
    
    string getName() override { return "Always-Best-Fit"; }
};

// Buffer-Based Adaptation: Balance quality with buffer safety
class BufferBasedAdaptation : public AdaptiveBitrateAlgorithm {
public:
    int selectBitrate(int buffer_level, int bandwidth,
                     int min_bitrate, int max_bitrate) override {
        vector<int> bitrates = {480, 720, 1080, 2160};
        
        // Buffer thresholds (in ms)
        int CRITICAL_BUFFER = 3000;   // 3 seconds
        int LOW_BUFFER = 10000;       // 10 seconds
        int NORMAL_BUFFER = 20000;    // 20 seconds
        
        // Bitrate quality levels
        if (buffer_level < CRITICAL_BUFFER) {
            // Rebuffer risk: go to lowest quality
            return 480;
        } else if (buffer_level < LOW_BUFFER) {
            // Low buffer: conservative approach
            return 720;
        } else if (buffer_level < NORMAL_BUFFER) {
            // Normal operation: medium quality
            return 1080;
        } else {
            // Healthy buffer: maximize quality within bandwidth limits
            for (int i = bitrates.size() - 1; i >= 0; i--) {
                if (bitrates[i] <= bandwidth && bitrates[i] <= max_bitrate) {
                    return bitrates[i];
                }
            }
        }
        return max_bitrate;
    }
    
    string getName() override { return "Buffer-Based Adaptation"; }
};

// Video Streaming Engine
class VideoStreamingEngine {
private:
    queue<VideoChunk> buffer;
    vector<VideoChunk> streaming_history;
    
    int buffer_capacity_ms;      // max 30 seconds
    int current_buffer_level;
    int total_time_played;
    int rebuffer_count;
    int rebuffer_time_ms;
    
    NetworkSimulator* network;
    AdaptiveBitrateAlgorithm* abr_algorithm;
    
    // Bitrate levels (kbps)
    int bitrates[4] = {480, 720, 1080, 2160};
    
public:
    VideoStreamingEngine(int buffer_cap, NetworkSimulator* net, 
                        AdaptiveBitrateAlgorithm* algo)
        : buffer_capacity_ms(buffer_cap), network(net), 
          abr_algorithm(algo), current_buffer_level(0),
          total_time_played(0), rebuffer_count(0), rebuffer_time_ms(0) {}
    
    // Calculate chunk size based on bitrate and duration
    int calculateChunkSize(int bitrate, int duration_ms) {
        // bitrate in kbps, duration in ms
        // size = (bitrate * duration) / 8000
        return (bitrate * duration_ms) / 8000;
    }
    
    // Download a chunk from network
    VideoChunk downloadChunk(int chunk_id, int current_time, int selected_bitrate) {
        int chunk_size = calculateChunkSize(selected_bitrate, 2000);  // 2 sec chunks
        int bandwidth = network->getBandwidth();
        
        // Download time = size / bandwidth (in ms)
        int download_time = (chunk_size * 8) / (bandwidth / 1000);
        
        VideoChunk chunk = {
            chunk_id,
            selected_bitrate,
            chunk_size,
            2000,  // 2 second chunks
            current_time,
            current_time + download_time,
            0,
            true
        };
        
        return chunk;
    }
    
    // Simulate playback
    void simulatePlayback(int simulation_duration_ms, int chunks_needed) {
        cout << "\n" << string(120, '=') << endl;
        cout << "VIDEO STREAMING SIMULATION" << endl;
        cout << "Algorithm: " << abr_algorithm->getName() << endl;
        cout << "Buffer Capacity: " << buffer_capacity_ms << " ms | "
             << "Video Duration: " << (chunks_needed * 2) << " seconds" << endl;
        cout << string(120, '=') << endl;
        
        int current_time = 0;
        int next_chunk_to_request = 1;
        int next_chunk_to_play = 1;
        
        // Print header
        cout << left << setw(10) << "Time(ms)"
             << setw(12) << "ChunkID"
             << setw(12) << "Bitrate(k)"
             << setw(12) << "Size(KB)"
             << setw(15) << "Buffer(ms)"
             << setw(15) << "Status"
             << setw(20) << "NetworkBW(kbps)" << endl;
        cout << string(120, '-') << endl;
        
        // Simulation loop
        while (current_time < simulation_duration_ms) {
            // Update network conditions
            int bandwidth = network->simulateBandwidth(current_time);
            
            // Request new chunks if buffer not full
            if (next_chunk_to_request <= chunks_needed && 
                current_buffer_level < buffer_capacity_ms) {
                
                // Select bitrate based on ABR algorithm
                int selected_bitrate = abr_algorithm->selectBitrate(
                    current_buffer_level, bandwidth, 480, 2160);
                
                // Download chunk
                VideoChunk chunk = downloadChunk(next_chunk_to_request, 
                                                current_time, selected_bitrate);
                
                // Add to buffer when download completes
                if (chunk.received_time <= current_time + 100) {  // Within timeframe
                    buffer.push(chunk);
                    current_buffer_level += chunk.duration_ms;
                    streaming_history.push_back(chunk);
                    
                    cout << left << setw(10) << current_time
                         << setw(12) << next_chunk_to_request
                         << setw(12) << selected_bitrate
                         << setw(12) << chunk.size_kb
                         << setw(15) << current_buffer_level
                         << setw(15) << "DOWNLOADED"
                         << setw(20) << bandwidth << endl;
                    
                    next_chunk_to_request++;
                }
            }
            
            // Play chunk if available
            if (!buffer.empty() && current_time % 2000 == 0) {
                VideoChunk played = buffer.front();
                buffer.pop();
                current_buffer_level -= played.duration_ms;
                total_time_played += played.duration_ms;
                
                cout << left << setw(10) << current_time
                     << setw(12) << played.chunk_id
                     << setw(12) << played.bitrate
                     << setw(12) << played.size_kb
                     << setw(15) << current_buffer_level
                     << setw(15) << "PLAYING"
                     << setw(20) << bandwidth << endl;
                
                next_chunk_to_play++;
            }
            
            // Rebuffer detection
            if (buffer.empty() && next_chunk_to_request <= chunks_needed) {
                rebuffer_count++;
                rebuffer_time_ms += 500;
                cout << left << setw(10) << current_time
                     << setw(12) << "-"
                     << setw(12) << "-"
                     << setw(12) << "-"
                     << setw(15) << current_buffer_level
                     << setw(15) << "REBUFFERING!"
                     << setw(20) << bandwidth << endl;
            }
            
            current_time += 100;  // 100ms simulation steps
        }
    }
    
    // Display statistics
    void displayStatistics() {
        cout << "\n" << string(120, '=') << endl;
        cout << "STREAMING STATISTICS - " << abr_algorithm->getName() << endl;
        cout << string(120, '=') << endl;
        
        if (streaming_history.empty()) {
            cout << "No streaming data available." << endl;
            return;
        }
        
        double avg_bitrate = 0;
        int min_bitrate = streaming_history[0].bitrate;
        int max_bitrate = streaming_history[0].bitrate;
        int total_data = 0;
        
        cout << left << setw(15) << "Chunk ID"
             << setw(15) << "Bitrate(kbps)"
             << setw(15) << "Size(KB)"
             << setw(15) << "Download(ms)"
             << setw(15) << "Quality" << endl;
        cout << string(75, '-') << endl;
        
        for (const auto& chunk : streaming_history) {
            int download_time = chunk.received_time - chunk.requested_time;
            string quality;
            
            if (chunk.bitrate == 480) quality = "SD";
            else if (chunk.bitrate == 720) quality = "HD";
            else if (chunk.bitrate == 1080) quality = "Full HD";
            else quality = "4K";
            
            cout << left << setw(15) << chunk.chunk_id
                 << setw(15) << chunk.bitrate
                 << setw(15) << chunk.size_kb
                 << setw(15) << download_time
                 << setw(15) << quality << endl;
            
            avg_bitrate += chunk.bitrate;
            min_bitrate = min(min_bitrate, chunk.bitrate);
            max_bitrate = max(max_bitrate, chunk.bitrate);
            total_data += chunk.size_kb;
        }
        
        cout << string(75, '=') << endl;
        cout << fixed << setprecision(2);
        cout << "\nSUMMARY METRICS:\n";
        cout << "Average Bitrate: " << (avg_bitrate / streaming_history.size()) << " kbps\n";
        cout << "Min/Max Bitrate: " << min_bitrate << " / " << max_bitrate << " kbps\n";
        cout << "Total Data Transferred: " << (total_data / 1024.0) << " MB\n";
        cout << "Rebuffers Encountered: " << rebuffer_count << "\n";
        cout << "Total Rebuffer Time: " << rebuffer_time_ms << " ms\n";
        cout << "Video Quality Score: ";
        
        // Quality score calculation
        double quality_score = (avg_bitrate / 2160.0) * 100;
        if (rebuffer_count > 0) quality_score -= (rebuffer_count * 10);
        quality_score = max(0.0, quality_score);
        
        cout << quality_score << " / 100\n";
        cout << string(75, '=') << "\n" << endl;
    }
};

// Function to display main menu
void displayMainMenu() {
    cout << "\n" << string(60, '=') << endl;
    cout << "VIDEO STREAM BUFFERING & ADAPTIVE BITRATE SIMULATOR" << endl;
    cout << "Next-Generation Media Platform Streaming Engine" << endl;
    cout << string(60, '=') << endl;
    cout << "NETWORK PRESET MENU:" << endl;
    cout << "1. Home WiFi (Stable: 1000-5000 kbps, Low fluctuation)" << endl;
    cout << "2. Mobile LTE (Moderate: 500-3000 kbps, Medium fluctuation)" << endl;
    cout << "3. Poor Connection (Unstable: 200-1500 kbps, High fluctuation)" << endl;
    cout << "4. Rural WiFi (Limited: 300-2000 kbps, Medium fluctuation)" << endl;
    cout << "5. 5G Network (Excellent: 2000-10000 kbps, Low fluctuation)" << endl;
    cout << "6. Custom Network Settings" << endl;
    cout << "7. Exit" << endl;
    cout << string(60, '-') << endl;
}

// Function to display algorithm selection menu
int displayAlgorithmMenu() {
    int choice;
    cout << "\nALGORITHM SELECTION:" << endl;
    cout << "1. Always-Best-Fit Algorithm" << endl;
    cout << "2. Buffer-Based Adaptation Algorithm" << endl;
    cout << "3. Both (Compare Algorithms)" << endl;
    cout << "Enter your choice (1-3): ";
    cin >> choice;
    return choice;
}

// Function to get custom network settings
void getCustomSettings(int& min_bw, int& max_bw, int& fluct, int& buffer_cap, 
                      int& sim_time, int& num_chunks) {
    cout << "\nCUSTOM NETWORK SETTINGS:" << endl;
    cout << "Enter minimum bandwidth (kbps): ";
    cin >> min_bw;
    cout << "Enter maximum bandwidth (kbps): ";
    cin >> max_bw;
    cout << "Enter fluctuation rate (kbps): ";
    cin >> fluct;
    cout << "Enter buffer capacity (ms): ";
    cin >> buffer_cap;
    cout << "Enter simulation time (ms): ";
    cin >> sim_time;
    cout << "Enter number of chunks: ";
    cin >> num_chunks;
}

// Function to run simulation with given parameters
void runSimulation(NetworkSimulator& network, int buffer_capacity, 
                  int simulation_time, int total_chunks, int algorithm_choice) {
    
    if (algorithm_choice == 1 || algorithm_choice == 3) {
        // Always-Best-Fit Algorithm
        cout << "\n--- TEST 1: ALWAYS-BEST-FIT ALGORITHM ---\n" << endl;
        AlwaysBestFit abf_algo;
        VideoStreamingEngine engine1(buffer_capacity, &network, &abf_algo);
        engine1.simulatePlayback(simulation_time, total_chunks);
        engine1.displayStatistics();
    }
    
    if (algorithm_choice == 2 || algorithm_choice == 3) {
        // Reset network for fair comparison
        network.setBandwidth(network.getBandwidth());
        
        // Buffer-Based Adaptation Algorithm
        cout << "\n--- TEST 2: BUFFER-BASED ADAPTATION ALGORITHM ---\n" << endl;
        BufferBasedAdaptation bba_algo;
        VideoStreamingEngine engine2(buffer_capacity, &network, &bba_algo);
        engine2.simulatePlayback(simulation_time, total_chunks);
        engine2.displayStatistics();
    }
}

// Main function with interactive menu
int main() {
    srand(time(0));
    
    int main_choice, algorithm_choice;
    bool exit_program = false;
    
    // Default parameters
    int min_bw = 500, max_bw = 5000, fluct = 300;
    int buffer_capacity = 30000;  // 30 seconds
    int simulation_time = 60000;  // 60 seconds
    int total_chunks = 30;        // 30 chunks = 60 seconds of video
    
    NetworkSimulator network(min_bw, max_bw, fluct);
    
    while (!exit_program) {
        displayMainMenu();
        cout << "Enter your choice (1-7): ";
        cin >> main_choice;
        
        switch (main_choice) {
            case 1: // Home WiFi
                min_bw = 1000; max_bw = 5000; fluct = 200;
                buffer_capacity = 30000;
                simulation_time = 60000;
                total_chunks = 30;
                cout << "\n✓ Home WiFi preset selected!" << endl;
                break;
                
            case 2: // Mobile LTE
                min_bw = 500; max_bw = 3000; fluct = 400;
                buffer_capacity = 30000;
                simulation_time = 60000;
                total_chunks = 30;
                cout << "\n✓ Mobile LTE preset selected!" << endl;
                break;
                
            case 3: // Poor Connection
                min_bw = 200; max_bw = 1500; fluct = 600;
                buffer_capacity = 30000;
                simulation_time = 60000;
                total_chunks = 30;
                cout << "\n✓ Poor Connection preset selected!" << endl;
                break;
                
            case 4: // Rural WiFi
                min_bw = 300; max_bw = 2000; fluct = 350;
                buffer_capacity = 30000;
                simulation_time = 60000;
                total_chunks = 30;
                cout << "\n✓ Rural WiFi preset selected!" << endl;
                break;
                
            case 5: // 5G Network
                min_bw = 2000; max_bw = 10000; fluct = 150;
                buffer_capacity = 30000;
                simulation_time = 60000;
                total_chunks = 30;
                cout << "\n✓ 5G Network preset selected!" << endl;
                break;
                
            case 6: // Custom Settings
                getCustomSettings(min_bw, max_bw, fluct, buffer_capacity, 
                                simulation_time, total_chunks);
                cout << "\n✓ Custom settings applied!" << endl;
                break;
                
            case 7: // Exit
                exit_program = true;
                cout << "\nThank you for using the Video Streaming Simulator!" << endl;
                continue;
                
            default:
                cout << "Invalid choice! Please try again." << endl;
                continue;
        }
        
        if (main_choice >= 1 && main_choice <= 6) {
            // Set network parameters
            network.setParameters(min_bw, max_bw, fluct);
            
            // Get algorithm choice
            algorithm_choice = displayAlgorithmMenu();
            
            if (algorithm_choice >= 1 && algorithm_choice <= 3) {
                // Run simulation
                runSimulation(network, buffer_capacity, simulation_time, 
                            total_chunks, algorithm_choice);
                
                cout << "\n✓ Simulation completed successfully!" << endl;
                cout << "✓ Demonstrates: Video Streaming, ABR Algorithms, Buffer Management" << endl;
                cout << "✓ Technologies: C++, Queues, Timers, OS Concepts, Network Simulation\n" << endl;
            } else {
                cout << "Invalid algorithm choice!" << endl;
            }
        }
        
        // Pause before showing menu again
        if (!exit_program) {
            cout << "Press Enter to continue...";
            cin.ignore();
            cin.get();
        }
    }
    
    return 0;
}
