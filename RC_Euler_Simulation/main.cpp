#include <iostream>
#include <fstream>
#include <cmath>

int main() {
    std::ofstream file("rc_simulation.csv");

    const double UIn = 10;
    const double UC_0 = 2;
    double UC = 2;
    const double R = 2000;
    const double C = 0.0001;
    const double Dt = 0.01;
    const double DURATION = 1;

    const double RC = R * C;

    double UCExact = UC;

    file << "time,euler,exact,error\n";
    file << "0.0," << UC << "," << UCExact << "," << "0" << "\n";

    for (int i = 0; i < DURATION / Dt; ++i) {
        double UNew = UC + (UIn - UC) / RC * Dt;
        UC = UNew;
        double time = (i + 1) * Dt;
        UCExact = UIn + (UC_0 - UIn) * std::exp(-time / RC);
        file << time << "," << UC << "," << UCExact << "," << std::abs(UC - UCExact) << "\n";
    }
}
