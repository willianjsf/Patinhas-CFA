from flask import Flask, request
import socket
from datetime import datetime
import re

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

log = open("log.txt", "a", buffering=1) 

def ultimo_passo():
    linhas = log.readline()
    ultimo = linhas[-1]
    match = re.search(r"(\d+(?:\.\d+)?)\s*passos", ultimo)
    if match:
        return float(match.group(1))
    return 0

@app.route('/', methods=['POST'])
def handle_post():
    tmstp = datetime.now()
    log_entry = f"{tmstp.timestamp()} - {request.get_data(as_text=True)} passos \n IP: {request.remote_addr}"
    printf(log_entry)
    log.write(log_entry)
    return "OK", 200

if __name__ == '__main__':
    local_ip = get_local_ip()
    contagem = ultimo_passo()
    print(f"Contagem anterior -> {contagem}")
    print(f"Servidor -> http://{local_ip}:5000")
    log.write(f"Servidor -> http://{local_ip}:5000))
    app.run(host='0.0.0.0', port=5000, debug=False)
