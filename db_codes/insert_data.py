import sqlite3
from datetime import datetime

conn = sqlite3.connect('fuel_tank_data.db')
c = conn.cursor()

# Altera a coluna 'level' para REAL
c.execute('''
    CREATE TABLE IF NOT EXISTS fuel_data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        datetime TEXT NOT NULL,
        level REAL NOT NULL  -- Altera para REAL
    )
''')

def insert_data(level):
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    c.execute('INSERT INTO fuel_data (datetime, level) VALUES (?, ?)', (now, level))
    conn.commit()
    print(f"Data inserted: {now}, Level: {level}")

while True:
    try:
        level = float(input("Enter fuel level: "))  # Converte para float
        insert_data(level)
    except ValueError:
        print("Invalid input. Please enter a numeric value for the fuel level.")
    except KeyboardInterrupt:
        break

conn.close()