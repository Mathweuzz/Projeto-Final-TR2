import sqlite3
from datetime import datetime

conn = sqlite3.connect('fuel_tank_data.db')

c = conn.cursor()

c.execute('''
    CREATE TABLE IF NOT EXISTS tanks (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL
    )
''')

c.execute('''
    CREATE TABLE IF NOT EXISTS fuel_data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        tank_id INTEGER NOT NULL,
        datetime TEXT NOT NULL,
        level REAL NOT NULL,
        FOREIGN KEY (tank_id) REFERENCES tanks(id)
    )
''')

def create_tank(id):
    name = f"Tanque {id}"
    c.execute("INSERT INTO tanks (id, name) VALUES (?, ?)", (id, name))
    conn.commit()
    print(f"Tank '{name}' created with ID {id}")
    return id

def insert_data(tank_id, level):
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    c.execute('INSERT INTO fuel_data (tank_id, datetime, level) VALUES (?, ?, ?)', (tank_id, now, level))
    conn.commit()
    print(f"Data inserted for tank {tank_id}: {now}, Level: {level}")

def add_data():
    while True:
        try:
            tank_id = int(input("Enter tank ID: "))
            level = float(input("Enter fuel level: "))

            c.execute("SELECT 1 FROM tanks WHERE id = ?", (tank_id,))
            if not c.fetchone():
                tank_id = create_tank(tank_id)
            
            insert_data(tank_id, level)
        except ValueError:
            print("Invalid input ID")
        except KeyboardInterrupt:
            print("Exit")
            break

def delete_tank(tank_id):
    c.execute('DELETE FROM tanks WHERE id = ?', (tank_id,))
    c.execute("DELETE FROM fuel_data WHERE tank_id = ?", (tank_id,))
    conn.commit()
    print(f"Tank with ID {tank_id} deleted")

delete_tank(9)


add_data()

c.execute("SELECT * FROM tanks")
print("Tanks:")
for row in c.fetchall():
    print(row)

c.execute("SELECT * FROM fuel_data")
print("Fuel Data:")
for row in c.fetchall():
    print(row)

conn.close()