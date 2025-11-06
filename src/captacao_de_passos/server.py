from flask import Flask, request
from flask_cors import CORS
import socket
from datetime import datetime
import re

app = Flask(__name__)
totalPassos = 0
CORS(app)

def get_local_ip(): 
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except:
        return "0.0.0.0"

log = open("log.txt", "a+", buffering=1)
log.seek(0)

def ultimo_passo():
    global totalPassos
    return totalPassos
#     linhas = log.readlines()
#     ultimo = linhas[-1]
#     match = re.search(r"(\d+(?:\.\d+)?)\s*passos", ultimo)
#     if match:
#         return int(match.group(1))
#     return 0

@app.route('/', methods=['POST'])
def handle_post():
    global totalPassos
    tmstp = datetime.now()
    passosRecentes = request.get_data(as_text=True)
    totalPassos += int(passosRecentes)
    log_entry = f"{tmstp.timestamp()} - {passosRecentes} passos \nIP: {request.remote_addr}"
    print(log_entry)
    log.write(log_entry)
    return "OK", 200

@app.route('/', methods=['GET'])
def handle_get():
    ultimo = str(ultimo_passo())
    print(ultimo)
    return ultimo, 200

if __name__ == '__main__':
    local_ip = get_local_ip()
    contagem = ultimo_passo()
    print(f"Contagem anterior -> {contagem}")
    print(f"Servidor -> http://{local_ip}:5000")
    log.write(f"Servidor -> http://{local_ip}:5000")
    app.run(host='0.0.0.0', port=5000, debug=False)
