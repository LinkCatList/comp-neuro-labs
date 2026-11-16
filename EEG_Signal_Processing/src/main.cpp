
#include <iostream>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <random>
#include <vector>

std::vector<double> MovingAverage(const std::vector<double>& signal, int windowSize) {
    if (signal.empty() || windowSize <= 0 || static_cast<std::size_t>(windowSize) > signal.size()) {
        return {};
    }

    const auto window = static_cast<std::size_t>(windowSize);
    std::vector<double> result;
    result.reserve(signal.size() - window + 1);
    const auto signalSize = signal.size();
    double sum = 0;

    for (std::size_t i = 0; i < window; ++i) {
        sum += signal[i];
    }

    result.emplace_back(sum / windowSize);

    for (std::size_t i = window; i < signalSize; ++i) {
        sum -= signal[i - window];
        sum += signal[i];
        result.emplace_back(sum / windowSize);
    }

    return result;
}

double CalculateMSE(const std::vector<double>& original, const std::vector<double>& processed) {
    if (original.empty() || processed.empty() || original.size() != processed.size()) {
        return -1;
    }

    double sum = 0;
    for (std::size_t i = 0; i < original.size(); ++i) {
        sum += (original[i] - processed[i]) * (original[i] - processed[i]);
    }
    return sum / processed.size();
}

int main() {
    constexpr double samplingRate = 256;
    constexpr double duration = 5;
    const std::vector<int> windowSizes = {5, 15, 31};
    constexpr int sampleCount = samplingRate * duration;

    std::ofstream file("../data/signal.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open ../data/signal.csv\n";
        return 1;
    }
    file << "time,signal,noisy_signal,filtered_signal\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> noise(0.0, 5.0);

    std::vector<double> signal;
    std::vector<double> noisySignal;
    const double pi = std::acos(-1);

    for (int i = 0; i < sampleCount; ++i) {
        double time = i / samplingRate;
        signal.emplace_back(30 * std::sin(2 * pi * 10 * time) + 15 * std::sin(2 * pi * 20 * time));
        noisySignal.emplace_back(signal.back() + noise(gen));
    }

    std::vector<double> filteredSignal = MovingAverage(noisySignal, windowSizes[0]);

    for (int i = 0; i < sampleCount; ++i) {
        double time = i / samplingRate;
        file << time << "," << signal[i] << "," << noisySignal[i];
        if (i >= windowSizes[0] - 1) {
            file << "," << filteredSignal[i - windowSizes[0] + 1];
        }
        file << "\n";
    }

    std::ofstream mseFile("../data/signals_mse.csv");
    if (!mseFile.is_open()) {
        std::cerr << "Failed to open ../data/signals_mse.csv\n";
        return 1;
    }
    mseFile << "window_size,mse_before,mse_after\n";

    const double mseBefore = CalculateMSE(signal, noisySignal);
    for (std::size_t i = 0; i < windowSizes.size(); ++i) {
        filteredSignal = MovingAverage(noisySignal, windowSizes[i]);
        mseFile << windowSizes[i] << "," << mseBefore << "," << CalculateMSE(std::vector<double>(signal.begin() + windowSizes[i] - 1, signal.end()), filteredSignal) << "\n";
    }
}
