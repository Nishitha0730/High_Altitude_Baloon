import csv
import struct

# Raw serial frame including the header
raw_frame = bytes([
    0xFF, 0xFF, 0xFF,  # Header
    0x20, 0xBA,        # AC1 (signed int16)
    0xFB, 0xC4,        # AC2 (signed int16)
    0xC7, 0x02,        # AC3 (signed int16)
    0x80, 0xAF,        # AC4 (unsigned int16)
    0x60, 0x0A,        # AC5 (unsigned int16)
    0x40, 0x87,        # AC6 (unsigned int16)
    0x19, 0x73,        # B1  (signed int16)
    0x00, 0x27,        # B2  (signed int16)
    0x80, 0x00,        # MB  (signed int16)
    0xD1, 0xF6,        # MC  (signed int16)
    0x0C, 0x00         # MD  (signed int16)
])

# 1. Strip the 3-byte header
payload = raw_frame[3:]


ac1, ac2, ac3, ac4, ac5, ac6, b1, b2, mb, mc, md = struct.unpack('>hhhHHHhhhhh', payload)

calibration_data = {
    'AC1': ac1, 'AC2': ac2, 'AC3': ac3,
    'AC4': ac4, 'AC5': ac5, 'AC6': ac6,
    'B1': b1,   'B2': b2,   'MB': mb,
    'MC': mc,   'MD': md
}

# 3. Write data to a CSV file
output_file = 'bmp_calibration.csv'
with open(output_file, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['Parameter', 'Parsed_Value'])
    

    for name, value in calibration_data.items():
        writer.writerow([name, value])
