#include <iostream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include "CustomerThread.h"

std::vector<long long> all_latencies;   // global definition
std::mutex print_mutex;                 // global definition

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cout << "Usage: " << argv[0] 
                  << " [ip addr] [port #] [# customers] [# orders] [robot type]" 
                  << std::endl;
        return 0;
    }

    std::string server_ip = argv[1];
    int port = std::atoi(argv[2]);
    int num_customers = std::atoi(argv[3]);
    int num_orders = std::atoi(argv[4]);
    int robot_type = std::atoi(argv[5]);

    std::vector<std::shared_ptr<CustomerThread>> customers;
    std::vector<std::thread> threads;

    // Start timer for throughput measurement
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_customers; i++) {
        auto cust_ptr = std::make_shared<CustomerThread>(i, num_orders, robot_type, server_ip, port);
        customers.push_back(cust_ptr);
        threads.push_back(std::thread(&CustomerThread::run, cust_ptr.get()));
    }

    for (auto &t : threads) {
        t.join();
    }

    // End timer for throughput measurement
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Compute latency stats
    if (!all_latencies.empty()) {
        long long sum = 0;
        long long min_latency = all_latencies[0];
        long long max_latency = all_latencies[0];

        for (auto &lat : all_latencies) {
            sum += lat;
            if (lat < min_latency) min_latency = lat;
            if (lat > max_latency) max_latency = lat;
        }

        double avg_latency = static_cast<double>(sum) / all_latencies.size();
        double throughput = static_cast<double>(all_latencies.size()) / elapsed.count();

        // Print results: avg latency, min latency, max latency, throughput
        std::cout << avg_latency << "\t"
                  << min_latency << "\t"
                  << max_latency << "\t"
                  << throughput << std::endl;
    }

    return 0;
}
