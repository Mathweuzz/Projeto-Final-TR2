from flask import Flask, render_template, jsonify, request
from flask_socketio import SocketIO, emit
from datetime import datetime
import sqlite3

app = Flask(__name__)
app.config['SECRET_KEY'] = 'mysecret'
socketio = SocketIO(app)

def get_data():
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()
    query = '''
        SELECT 
            fuel_data.datetime, 
            fuel_data.level, 
            tanks.name AS tank_name 
        FROM 
            fuel_data 
        JOIN 
            tanks ON fuel_data.tank_id = tanks.id 
        ORDER BY 
            fuel_data.datetime DESC
    '''
    c.execute(query)
    data = c.fetchall()
    conn.close()
    return data

def get_filtered_data(start_date, end_date):
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()

    start_date = f"{start_date} 00:00:00"
    end_date = f"{end_date} 23:59:59"

    query = '''
        SELECT 
            fuel_data.datetime, 
            fuel_data.level, 
            tanks.name AS tank_name 
        FROM 
            fuel_data 
        JOIN 
            tanks ON fuel_data.tank_id = tanks.id 
        WHERE 
            fuel_data.datetime BETWEEN ? AND ?
        ORDER BY 
            fuel_data.datetime DESC
    '''
    c.execute(query, (start_date, end_date))
    data = c.fetchall()
    conn.close()
    return data 
 

@app.route('/data')
def data():
    data = get_data()
    return jsonify(data)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data/filtered')
def filtered_data():
    start_date = request.args.get('start_date')
    end_date = request.args.get('end_date')

    if start_date and end_date:
        filtered_data = get_filtered_data(start_date, end_date)
        return jsonify(filtered_data)
    else:
        return jsonify({'error': 'Invalid date range'}), 400

@app.route('/update')
def update():
    data = get_data()
    socketio.emit('update', data)
    return 'Data updated and sent via WebSocket'

def create_tank(id):
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()
    name = f"Tanque {id}"
    c.execute('INSERT INTO tanks (id, name) VALUES (?, ?)', (id, name))
    conn.commit()
    conn.close()
    print(f"Tank '{name}' created with ID {id}")
    return id

@app.route('/insert', methods=['POST'])
def insert_data():
    tank_id = request.form.get('tank_id')
    level = request.form.get('level')
    if tank_id and level:
        conn = sqlite3.connect('fuel_tank_data.db')
        c = conn.cursor()
        c.execute("SELECT 1 FROM tanks WHERE id = ?", (tank_id,))
        if not c.fetchone():
            tank_id = create_tank(tank_id)
        now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        c.execute('INSERT INTO fuel_data (tank_id, datetime, level) VALUES (?, ?, ?)', (tank_id, now, level))
        conn.commit()
        conn.close()
        return jsonify({'message': 'Data inserted successfully'})
    else:
        return jsonify({'error': 'Invalid data'}), 400
    

@app.route('/data/filtered/<tank_id>')
def filtered_data_by_tank(tank_id):
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()
    c.execute("""
        SELECT 
            fuel_data.datetime, 
            fuel_data.level, 
            tanks.name AS tank_name 
        FROM 
            fuel_data 
        JOIN 
            tanks ON fuel_data.tank_id = tanks.id 
        WHERE 
            fuel_data.tank_id = ?
        ORDER BY 
            fuel_data.datetime DESC
    """, (tank_id,))
    data = c.fetchall()
    conn.close()
    return jsonify(data)
    
@app.route('/tanks')
def get_tanks():
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()
    c.execute('SELECT id, name FROM tanks')
    tanks = c.fetchall()
    conn.close()
    return jsonify(tanks)

if __name__ == '__main__':
    print("Starting Flask server with SocketIO")
    socketio.run(app, debug=True)
