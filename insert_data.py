import sqlite3
from datetime import datetime

conn = sqlite3.connect('fuel_tank_data.db')
c = conn.cursor()

c.execute('''
    CREATE TABLE IF NOT EXISTS fuel_data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        datetime TEXT NOT NULL,
        level INTEGER NOT NULL
    )
''')

def insert_data(level):
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    c.execute('INSERT INTO fuel_data (datetime, level) VALUES (?, ?)', (now, level))
    conn.commit()
    print(f"Data inserted: {now}, Level: {level}")

while True:
    try:
        level = int(input("Enter fuel level: "))
        insert_data(level)
    except ValueError:
        print("Invalid input. Please enter an integer value for the fuel level.")
    except KeyboardInterrupt:
        break

conn.close()
