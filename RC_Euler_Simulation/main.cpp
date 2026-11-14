#include <iostream>
#include <fstream>

int main() {
    std::ofstream file("rc_simulation.csv");

    const double UIn = 10;
    double UC = 2;
    const double R = 2000;
    const double C = 0.0001;
    const double Dt = 0.01;
    const double DURATION = 1;

    const double RC = R * C;

    file << "time,RC\n";
    file << "0.0," << RC << "\n";

    for (int i = 0; i < DURATION / Dt; ++i) {
        double UNew = UC + (UIn - UC) / RC * Dt;
        UC = UNew;
        file << (i + 1) * Dt << "," << UC << "\n";
    }
}
