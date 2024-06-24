import serial
import requests

url = 'http://localhost:5000/insert'
arduino = serial.Serial('/dev/ttyACM0', 9600)

while True:
    if arduino.in_waiting > 0:
        # Read data from serial port
        tank_id, level = arduino.readline().decode().strip().split("|")
        print(tank_id, level)

        try:
            tank_id = int(tank_id)
            level = float(level)

            # Send data to server
            print("Sending data to server")
            payload = {
                "tank_id": tank_id,
                "level": level
            }
            response = requests.post(url, data=payload)

            # Print response from server
            print(response.text)
        except ValueError:
            continue

        

