import sqlite3

conn = sqlite3.connect('fuel_tank_data.db')
c = conn.cursor()

c.execute('SELECT * FROM fuel_data')
rows = c.fetchall()

for row in rows:
    print(row)

conn.close()
