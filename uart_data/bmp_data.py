import csv
import struct
import serial  

PORT = 'COM6'
BAUD_RATE = 2400
CSV_FILE = 'bmp_sensor_data.csv'


with open(CSV_FILE, mode='a', newline='') as file:
    writer = csv.writer(file)
    if file.tell() == 0:
        writer.writerow(['Temperature_C', 'Pressure_Pa'])

HEADER = b'\xff\xff\xff'
FRAME_SIZE = 11  # 3 bytes header + 4 bytes temp + 4 bytes pressure

def stream_and_save():
    ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    buffer = bytearray()
    
    
    try:
        while True:
            # Read incoming bytes continuously
            chunk = ser.read(ser.in_waiting or 1)
            if chunk:
                buffer.extend(chunk)
            
            # Look for the header in the buffer
            while len(buffer) >= FRAME_SIZE:
                header_index = buffer.find(HEADER)
                
                # If header is not found, clear old junk except last 2 bytes (could be start of header)
                if header_index == -1:
                    buffer = buffer[-2:]
                    break
                
                # If header is found but not at the start, discard prior garbage bytes
                if header_index > 0:
                    buffer = buffer[header_index:]
                
                # Check if full frame (header + 8 payload bytes) is ready
                if len(buffer) < FRAME_SIZE:
                    break
                
                # Extract 11-byte frame and remove from buffer
                frame = buffer[:FRAME_SIZE]
                buffer = buffer[FRAME_SIZE:]
                
                # Decode payload (skip 3 header bytes)
                payload = frame[3:]
                raw_temp, raw_pressure = struct.unpack('>Ii', payload)
                
                temp_c = raw_temp / 10.0
                pressure_pa = raw_pressure
                
                # Print and append directly to CSV
                # print(f"Temp: {temp_c:.1f} °C | Pressure: {pressure_pa} Pa")
                
                with open(CSV_FILE, mode='a', newline='') as file:
                    writer = csv.writer(file)
                    writer.writerow([temp_c, pressure_pa])

    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        ser.close()

if __name__ == '__main__':
    stream_and_save()