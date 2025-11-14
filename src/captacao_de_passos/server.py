from flask import Flask, request, jsonify
from flask_cors import CORS
import socket
from datetime import datetime
import re
from datetime import date
from zeroconf import Zeroconf, ServiceInfo
import json
import threading

app = Flask(__name__)
totalPassos = 0
CORS(app)

BROADCAST_PORT = 50000

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

def broadcast_listener():
    local_ip = get_local_ip()
    print(f"Broadcast Listener jogando para o IP {local_ip}")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.bind(("", BROADCAST_PORT))

    while True:
        data, addr = sock.recvfrom(1024)
        msg = data.decode().strip()

        if msg == "DISCOVER_SERVER":
            resposta = f"SERVER_IP:{local_ip}"
            sock.sendto(resposta.encode(), addr)
            print(f"Respondido para {addr}: {resposta}")

def ultimo_passo():
    global totalPassos
    return totalPassos

def carregar_dados():
    with open("animais.json", "r", encoding="utf-8") as f:
        return json.load(f)
    
def salvar_dados(dados):
    with open("animais.json", "w", encoding="utf-8") as f:
        json.dump(dados, f, ensure_ascii=False, indent=2)

@app.route('/', methods=['POST'])
def handle_post():
    dados = carregar_dados()
    pet = dados["pets"][0]
    hoje = date.today().strftime("%Y-%m-%d")

    passosRecentes = request.get_data(as_text=True)
    # log_entry = f"{tmstp.timestamp()} - {passosRecentes} passos \nIP: {request.remote_addr}"
    # print(log_entry)

    diaEncontrado = False

    for item in pet["passos"]:
        if item["data"] == hoje:
            diaEncontrado = True
            passosHoje = item["num_passos"]
            passosHoje = passosHoje + int(passosRecentes)
            print(passosHoje)
            item["num_passos"] = passosHoje
            salvar_dados(dados)

    if diaEncontrado == False:
        novoDia = {
            "num_passos": int(passosRecentes),
            "data": hoje
        }
        dados["pets"][0]["passos"].append(novoDia)
        salvar_dados(dados)

    return "OK", 200

@app.route('/', methods=['GET'])
def handle_get():
    ultimo = str(ultimo_passo())
    print(ultimo)
    return ultimo, 200

@app.route('/animalInfo', methods=['GET'])
def get_animal_info():
    dados = carregar_dados()
    print(dados)

    if not dados:
        return jsonify({"erro": "Banco de dados vazio."}), 400

    return jsonify({
        "dados": dados["pets"]
    }), 200


@app.route('/passosDia', methods=['GET'])
def get_passos_dia():
    dados = carregar_dados()
    pet = dados["pets"][0]

    dia = request.args.get('dia')

    if not dia:
        return jsonify({"erro": "Parâmetro 'dia' é obrigatório."}), 400
    
    for item in pet["passos"]:
        if item["data"] == dia:
            return jsonify({
                "data": dia,
                "num_passos": item["num_passos"]
            }), 200

    # Se não encontrar a data
    return jsonify({
        "erro": f"Nenhum registro encontrado para a data {dia}"
    }), 404

if __name__ == '__main__':
    port=8080
    local_ip = get_local_ip()
    contagem = ultimo_passo()

    log.write(f"Servidor -> http://{local_ip}:" + str(port))
    
    udp_thread = threading.Thread(target=broadcast_listener, daemon=True) # inicia broadcast em thread separada
    udp_thread.start()

    print(f" → Endereço: {local_ip}:{port}")
    app.run(host='0.0.0.0', port=port)
