from flask import Flask, request, jsonify
from flask_cors import CORS
import socket
from datetime import datetime
import re
from datetime import date
from zeroconf import Zeroconf, ServiceInfo
import json

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

    zeroconf = Zeroconf()
    info = ServiceInfo(
        "_http._tcp.local.",
        "backend.local._http._tcp.local.",
        addresses=[socket.inet_aton(local_ip)],
        port=port,
        properties={},
        server="backend.local."
    )
    zeroconf.register_service(info)

    log.write(f"Servidor -> http://{local_ip}:" + str(port))
    
    print(f" → Endereço: {local_ip}:{port}")
    print(f" → Servidor DNS-SD: {info.server}")
    app.run(host='0.0.0.0', port=port)
