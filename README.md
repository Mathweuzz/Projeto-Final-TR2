## How to execute the project
### For users who want to send data through arduinos
1. Connect **receiver** arduino on USB port
2. Upload [receiver.ino](./lora_codes/receiver.ino) using **Arduino IDE**
3. Connect **transmitter** arduino on other USB port
4. Upload [transmitter.ino](./lora_codes/transmitter.ino) using **Arduino IDE**
5. Install flask dependencies
6. Run ```python3 serial_port_read.py```
7. Run ```python3 app.py```
8. Clink on this [link](http://localhost:5000/)

### For regular users
1. Run ```python3 app.py```
2. Clink on this [link](http://localhost:5000/)
