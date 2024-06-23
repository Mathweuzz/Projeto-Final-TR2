import serial
import requests

url = 'http://localhost:5000/insert'
arduino = serial.Serial('/dev/ttyACM0', 9600)

while True:
    if arduino.in_waiting > 0:
        # Read data from serial port
        data = arduino.readline().decode().strip()
        print(data)

        try:
            level = float(data)

            # Send data to server
            print("Sending data to server")
            payload = {"level": level}
            response = requests.post(url, data=payload)

            # Print response from server
            print(response.text)
        except ValueError:
            continue

        

