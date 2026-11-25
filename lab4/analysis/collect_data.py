
import serial
import csv
import time

PORT = "/dev/ttyUSB0"
BAUD = 115200
OUTPUT = "lab4/data/measurements.csv"

with serial.Serial(PORT, BAUD, timeout=1) as ser:
    time.sleep(2)

    with open(OUTPUT, "w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["time_ms", "raw", "voltage_v"])

        try:
            while True:
                line = ser.readline().decode(
                    "utf-8", errors="ignore"
                ).strip()

                parts = line.split(",")
                if len(parts) != 3:
                    continue

                try:
                    time_ms = int(parts[0])
                    raw = int(parts[1])
                    voltage = float(parts[2])
                except ValueError:
                    continue

                writer.writerow([time_ms, raw, voltage])
                file.flush()
