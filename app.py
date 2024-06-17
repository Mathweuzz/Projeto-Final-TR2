from flask import Flask, render_template, jsonify, request
from flask_socketio import SocketIO, emit
import sqlite3

app = Flask(__name__)
app.config['SECRET_KEY'] = 'mysecret'
socketio = SocketIO(app)

def get_data():
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()
    c.execute('SELECT datetime, level FROM fuel_data ORDER BY datetime DESC')
    data = c.fetchall()
    conn.close()
    return data

def get_filtered_data(start_date, end_date):
    conn = sqlite3.connect('fuel_tank_data.db')
    c = conn.cursor()
    c.execute("SELECT datetime, level FROM fuel_data WHERE datetime BETWEEN ? AND ? ORDER BY datetime DESC",(start_date, end_date))
    data = c.fetchall()
    conn.close()
    return data 

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def data():
    data = get_data()
    return jsonify(data)

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

if __name__ == '__main__':
    print("Starting Flask server with SocketIO")
    socketio.run(app, debug=True)
