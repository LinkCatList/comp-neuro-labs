import csv
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

time = []
euler = []
exact = []
error = []

with open("rc_simulation.csv", "r") as file:
    reader = csv.DictReader(file)

    for row in reader:
        time.append(float(row["time"]))
        euler.append(float(row["euler"]))
        exact.append(float(row["exact"]))
        error.append(float(row["error"]))

plt.plot(time, euler, label="Euler method")
plt.plot(time, exact, label="Exact solution")
plt.xlabel("Time (seconds)")
plt.ylabel("Voltage (volts)")
plt.title("UC circuit: Euler vs Exact")
plt.legend()
plt.grid(True)
plt.savefig("capacitor_charging.png")

plt.figure()
plt.plot(time, error, label="Error")
plt.title("UC circuit: error Euler vs Exact")
plt.xlabel("Time (seconds)")
plt.ylabel("Error (volts)")
plt.legend()
plt.grid(True)
plt.savefig("capacitor_charging_error.png")