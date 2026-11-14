import csv
import matplotlib.pyplot as plt

time = []
signal = []
noisy_signal = []
filtered_signal = []

with open("../data/signal.csv", "r") as file:
    reader = csv.DictReader(file)

    for row in reader:
        time.append(float(row["time"]))
        signal.append(float(row["signal"]))
        noisy_signal.append(float(row["noisy_signal"]))
        if row["filtered_signal"]:
            filtered_signal.append(float(row["filtered_signal"]))

fig, axes = plt.subplots(4, 1, figsize=(12, 9))
fig.subplots_adjust(hspace=0.5)

axes[0].plot(time, signal, label="Original signal", linewidth=2)
axes[1].plot(time, noisy_signal, label="Noisy signal", linewidth=2)
axes[2].plot(time[4:], filtered_signal, label="Filtered signal", linewidth=2)
axes[3].plot(time, noisy_signal, label="Noisy signal", linewidth=2)
axes[3].plot(time[4:], filtered_signal, label="Filtered signal", linewidth=2)

axes[0].set_title("Original signal")
axes[1].set_title("Noisy signal")
axes[2].set_title("Moving average filter")
axes[3].set_title("Noisy vs filtered signal")

for ax in axes:
    ax.legend(loc="upper right")
    ax.set_xlabel("Time (seconds)")
    ax.set_ylabel("Amplitude (microvolts)")
    ax.grid(alpha=0.2, linestyle="--")

plt.savefig("../plots/original_vs_noisy.png")