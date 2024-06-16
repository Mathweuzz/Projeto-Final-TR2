from flask import Flask, render_template, jsonify
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

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def data():
    data = get_data()
    return jsonify(data)

@app.route('/update')
def update():
    data = get_data()
    socketio.emit('update', data)
    return 'Data updated and sent via WebSocket'

if __name__ == '__main__':
    print("Starting Flask server with SocketIO")
    socketio.run(app, debug=True)
