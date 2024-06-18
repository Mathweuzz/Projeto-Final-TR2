import sqlite3
from datetime import datetime, timedelta
import random

conn = sqlite3.connect('fuel_tank_data.db')
c = conn.cursor()

c.execute('''
    CREATE TABLE IF NOT EXISTS fuel_data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        datetime TEXT NOT NULL,
        level INTEGER NOT NULL
    )
''')

def insert_data(datetime_str, level):
    c.execute('INSERT INTO fuel_data (datetime, level) VALUES (?, ?)', (datetime_str, level))
    conn.commit()

start_date = datetime(2010, 1, 1)
end_date = datetime(2023, 12, 31)
delta = timedelta(days=1)

current_date = start_date
while current_date <= end_date:
    level = random.randint(0, 100)
    
    datetime_str = current_date.strftime("%Y-%m-%d %H:%M:%S")
    
    insert_data(datetime_str, level)
    
    current_date += delta


conn.close()
