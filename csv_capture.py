import serial
import csv
import time
import sys


SERIAL_PORT = "/dev/cu.usbserial-1320"  
BAUD_RATE = 115200
CSV_FILE = "ble_scan_data.csv"
TARGET_SAMPLES = 5000  # samples

print(f"--- Starting BLE CSV Capture ---")
print(f"Targeting exactly {TARGET_SAMPLES} samples...")
print(f"Data will stream into: {CSV_FILE}\n")

sample_count = 0  

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    ser.flushInput()
    
    with open(CSV_FILE, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(["Timestamp_MS", "MAC_Address", "RSSI_dBm", "System_Time"])
        
        # Loop until our sample counter hits the target limit
        while sample_count < TARGET_SAMPLES:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                
                if not line:
                    continue  
                
                parts = line.split(',')
                
                if len(parts) == 3:
                    timestamp_ms = parts[0]
                    mac_address = parts[1]
                    rssi_val = parts[2]
                    
                    if ":" in mac_address:
                        current_wall_time = time.strftime("%Y-%m-%d %H:%M:%S")
                        
                        writer.writerow([timestamp_ms, mac_address, rssi_val, current_wall_time])
                        file.flush()  
                        
                        # Increment the counter
                        sample_count += 1
                        
                        # Print live progress status to the screen
                        print(f"[{sample_count}/{TARGET_SAMPLES}] Saved | MAC: {mac_address} | RSSI: {rssi_val} dBm")
                else:
                    # Print regular system boot logs without counting them as data samples
                    print(f"[ESP32 LOG] {line}")
                    
            except UnicodeDecodeError:
                continue

    print(f"\n🎉 Success! Collected exactly {TARGET_SAMPLES} samples.")
    print(f"Data is safely saved inside '{CSV_FILE}'.")
    sys.exit(0)

except serial.SerialException as e:
    print(f"\n❌ Serial Port Error: Could not connect to {SERIAL_PORT}.")
    sys.exit(1)

except KeyboardInterrupt:
    print(f"\n Stopping early. Captured {sample_count} samples so far inside {CSV_FILE}.")
    sys.exit(0)