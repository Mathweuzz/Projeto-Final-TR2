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

def create_tank(name):
    c.execute('INSERT INTO tanks (name) VALUES (?)', (name,))
    conn.commit()
    print(f"Tank '{name}' created with ID {c.lastrowid}")

def insert_data(tank_id, level):
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    c.execute('INSERT INTO fuel_data (tank_id, datetime, level) VALUES (?, ?, ?)', (tank_id, now, level))
    conn.commit()
    print(f"Data inserted for tank {tank_id}: {now}, Level: {level}")

create_tank('Tanque 1')
create_tank('Tanque 17')
create_tank('Tanque 9')

insert_data(1, 22.3)
insert_data(1, 19.14)
insert_data(1, 12.18)
insert_data(2, 18.5)
insert_data(2, 13.2)
insert_data(3, 14.8)
insert_data(3, 17.9)

c.execute('SELECT * FROM tanks')
print("Tanks:")
for row in c.fetchall():
    print(row)

c.execute('SELECT * FROM fuel_data')
print("Fuel Data:")
for row in c.fetchall():
    print(row)

conn.close()
