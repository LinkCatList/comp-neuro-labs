
import csv
from pathlib import Path

import matplotlib.pyplot as plt

BASE_DIR = Path(__file__).resolve().parent.parent
DATA_FILE = BASE_DIR / "data" / "measurements.csv"
PLOTS_DIR = BASE_DIR / "plots"

PLOTS_DIR.mkdir(parents=True, exist_ok=True)

time_ms = []
voltage_values = []

with open(DATA_FILE, "r", newline="") as file:
    reader = csv.DictReader(file)

    for row in reader:
        time_ms.append(float(row["time_ms"]))
        voltage_values.append(float(row["voltage_v"]))


time_s = [(t - time_ms[0]) / 1000 for t in time_ms]

plt.figure(figsize=(10, 5))
plt.plot(time_s, voltage_values, linewidth=1.5)

plt.title("Voltage vs Time")
plt.xlabel("Time (s)")
plt.ylabel("Voltage (V)")
plt.grid(True)
plt.tight_layout()

plt.savefig(PLOTS_DIR / "voltage_vs_time.png", dpi=300)
plt.close()
