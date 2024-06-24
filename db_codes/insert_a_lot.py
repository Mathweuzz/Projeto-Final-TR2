import sqlite3

def check_data():
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()
    
    c.execute('SELECT * FROM tanks')
    tanks = c.fetchall()
    print("Tanks:")
    for tank in tanks:
        print(tank)
    
    c.execute('SELECT * FROM fuel_data')
    fuel_data = c.fetchall()
    print("Fuel Data:")
    for data in fuel_data:
        print(data)
    
    conn.close()

check_data()
