from flask import Flask, request
import socket

app = Flask(__name__)

def get_local_ip(): 
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except:
        return "0.0.0.0"

@app.route('/', methods=['POST'])
def handle_post():
    print(f"Data: {request.get_data(as_text=True)}")
    print(f"IP: {request.remote_addr}")
    return "OK", 200

if __name__ == '__main__':
    local_ip = get_local_ip()
    print(f"Servidor -> http://{local_ip}:5000")
    app.run(host='0.0.0.0', port=5000, debug=False)
