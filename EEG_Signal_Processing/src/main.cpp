#include <iostream>
#include <cmath>
#include <fstream>
#include <random>
#include <vector>

std::vector<double> MovingAverage(const std::vector<double>& signal, int windowSize) {
    if (signal.empty() || windowSize <= 0 || windowSize > static_cast<int>(signal.size())) {
        return {};
    }

    std::vector<double> result;
    int signalSize = static_cast<int>(signal.size());
    double sum = 0;

    for (int i = 0; i < windowSize; ++i) {
        sum += signal[i];
    }

    result.emplace_back(sum / windowSize);

    for (int i = windowSize; i < signalSize; ++i) {
        sum -= signal[i - windowSize];
        sum += signal[i];
        result.emplace_back(sum / windowSize);
    }

    return result;
}

int main() {
    const double samplingRate = 256;
    const double duration = 5;
    const int windowSize = 5;
    const int sampleCount = samplingRate * duration;

    std::ofstream file("../data/signal.csv");
    file << "time,signal,noisy_signal,filtered_signal\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> noise(0.0, 5.0);

    std::vector<double> signal;
    std::vector<double> noisySignal;

    for (int i = 0; i < sampleCount; ++i) {
        double time = i / samplingRate;
        signal.emplace_back(30 * std::sin(2 * std::acos(-1) * 10 * time) + 15 * std::sin(2 * std::acos(-1) * 20 * time));
        noisySignal.emplace_back(signal.back() + noise(gen));
    }

    std::vector<double> filteredSignal = MovingAverage(noisySignal, windowSize);

    for (size_t i = 0; i < sampleCount; ++i) {
        double time = i / samplingRate;
        file << time << "," << signal[i] << "," << noisySignal[i];
        if (i >= windowSize - 1) {
            file << "," << filteredSignal[i - windowSize + 1];
        }
        file << "\n";
    }
}
