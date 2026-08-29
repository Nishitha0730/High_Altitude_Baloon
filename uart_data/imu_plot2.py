from collections import deque
import csv
import queue
import struct
import threading
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import serial

PORT = "COM6"
BAUD_RATE = 2400
OUTPUT_FILE = "imu_data_raw.csv"
HEADER = b"\xff\xff\xff"
PAYLOAD_SIZE = 14
DISPLAY_POINTS = 100  # Number of live points to display on the plot window


data_queue = queue.Queue()
is_running = True

# Thread-safe plot buffers
x_data = deque(maxlen=DISPLAY_POINTS)
ax_data = deque(maxlen=DISPLAY_POINTS)
ay_data = deque(maxlen=DISPLAY_POINTS)
az_data = deque(maxlen=DISPLAY_POINTS)
gx_data = deque(maxlen=DISPLAY_POINTS)
gy_data = deque(maxlen=DISPLAY_POINTS)
gz_data = deque(maxlen=DISPLAY_POINTS)
temp_data = deque(maxlen=DISPLAY_POINTS)


def get_synced_payload(ser):
    sync_count = 0

    while is_running:
        b = ser.read(1)
        if not b:
            return None  # Serial read timed out

        # Count consecutive header bytes
        if b == b"\xff":
            sync_count += 1
        else:
            sync_count = 0

        # Header match found
        if sync_count == 3:
            payload = ser.read(PAYLOAD_SIZE) # 14
            if len(payload) == PAYLOAD_SIZE:
                return payload
            return None

    return None


def serial_reader_thread():
    global is_running

    try:
        ser = serial.Serial(PORT, BAUD_RATE, timeout=0.5)
        print(f"Connected to {PORT} at {BAUD_RATE} baud.")
        ser.reset_input_buffer()

        with open(OUTPUT_FILE, mode="w", newline="") as file:
            writer = csv.writer(file)
            writer.writerow(["ax", "ay", "az", "temp", "gx", "gy", "gz"])
            print("streaming IMU data")

            sample_count = 0
            while is_running:
                payload_bytes = get_synced_payload(ser)

                if payload_bytes and len(payload_bytes) == PAYLOAD_SIZE:
                    row = list(struct.unpack(">7h", payload_bytes))

                    # Raw Accelerometer conversion (g)
                    ax = row[0] / 16384.0
                    ay = row[1] / 16384.0
                    az = row[2] / 16384.0

                    # Raw Temperature conversion (°C)
                    temp = row[3] / 340.0 + 36.53

                    # Raw Gyroscope conversion (°/s) - No Bias Offset
                    gx = row[4] / 131.0
                    gy = row[5] / 131.0
                    gz = row[6] / 131.0

                    raw_row = [
                        round(ax, 4),
                        round(ay, 4),
                        round(az, 4),
                        round(temp, 2),
                        round(gx, 3),
                        round(gy, 3),
                        round(gz, 3),
                    ]

                    # Save raw data to CSV
                    writer.writerow(raw_row)
                    file.flush()

                    # Push data point to GUI thread queue
                    sample_count += 1
                    data_queue.put((sample_count, raw_row))

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    finally:
        if "ser" in locals() and ser.is_open:
            ser.close()
            print("Serial port closed.")



if __name__ == "__main__":
    # Start reader thread
    t = threading.Thread(target=serial_reader_thread, daemon=True)
    t.start()

    # Setup Matplotlib Figure
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

    # Plot Lines Setup
    (line_ax,) = ax1.plot([], [], "r-", label="ax (g)")
    (line_ay,) = ax1.plot([], [], "g-", label="ay (g)")
    (line_az,) = ax1.plot([], [], "b-", label="az (g)")
    ax1.set_ylabel("Accel (g)")
    ax1.set_title("Live Raw IMU Data")
    ax1.legend(loc="upper right")
    ax1.grid(True)

    (line_gx,) = ax2.plot([], [], color="orange", label="gx (°/s)")
    (line_gy,) = ax2.plot([], [], color="purple", label="gy (°/s)")
    (line_gz,) = ax2.plot([], [], color="brown", label="gz (°/s)")
    ax2.set_ylabel("Gyro (°/s)")
    ax2.legend(loc="upper right")
    ax2.grid(True)

    (line_temp,) = ax3.plot([], [], color="darkred", label="Temp (°C)")
    ax3.set_xlabel("Sample Index")
    ax3.set_ylabel("Temp (°C)")
    ax3.legend(loc="upper right")
    ax3.grid(True)

    def update_plot(frame):
        # Empty queue into display buffers
        while not data_queue.empty():
            idx, sample = data_queue.get()
            x_data.append(idx)
            ax_data.append(sample[0])
            ay_data.append(sample[1])
            az_data.append(sample[2])
            temp_data.append(sample[3])
            gx_data.append(sample[4])
            gy_data.append(sample[5])
            gz_data.append(sample[6])

        if x_data:
            # Update Line Data
            line_ax.set_data(x_data, ax_data)
            line_ay.set_data(x_data, ay_data)
            line_az.set_data(x_data, az_data)

            line_gx.set_data(x_data, gx_data)
            line_gy.set_data(x_data, gy_data)
            line_gz.set_data(x_data, gz_data)

            line_temp.set_data(x_data, temp_data)

            # Adjust axes dynamically
            for ax in (ax1, ax2, ax3):
                ax.set_xlim(min(x_data), max(x_data) + 1)
                ax.relim()
                ax.autoscale_view(scalex=False, scaley=True)

        return (
            line_ax,
            line_ay,
            line_az,
            line_gx,
            line_gy,
            line_gz,
            line_temp,
        )

    # Start Animation Loop
    ani = animation.FuncAnimation(
        fig, update_plot, interval=100, blit=False, cache_frame_data=False
    )

    try:
        plt.tight_layout()
        plt.show()
    except KeyboardInterrupt:
        pass
    finally:
        is_running = False
        print("Application closed.")