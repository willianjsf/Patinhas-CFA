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
        print(f"✓ IP local obtido: {ip}")
        return ip
    except Exception as e:
        print(f"✗ Erro ao obter IP local: {e}")
        return "0.0.0.0"

def broadcast_listener():
    local_ip = get_local_ip()
    print(f"\n🔊 Broadcast Listener iniciado na porta {BROADCAST_PORT}")
    print(f"   Responderá com IP: {local_ip}\n")

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.bind(("", BROADCAST_PORT))
    print(f"✓ Socket de broadcast vinculado à porta {BROADCAST_PORT}")

    while True:
        data, addr = sock.recvfrom(1024)
        msg = data.decode().strip()
        print(f"📨 Mensagem recebida de {addr}: {msg}")

        if msg == "DISCOVER_SERVER":
            resposta = f"SERVER_IP:{local_ip}"
            sock.sendto(resposta.encode(), addr)
            print(f"✓ Respondido para {addr}: {resposta}")

def ultimo_passo():
    global totalPassos
    return totalPassos

def carregar_dados():
    try:
        with open("animais.json", "r", encoding="utf-8") as f:
            dados = json.load(f)
            print(f"📂 Dados carregados de animais.json - {len(dados.get('pets', []))} pet(s)")
            return dados
    except FileNotFoundError:
        print("✗ Erro: arquivo animais.json não encontrado")
        raise
    except json.JSONDecodeError:
        print("✗ Erro: arquivo animais.json com formato JSON inválido")
        raise
    
def salvar_dados(dados):
    try:
        with open("animais.json", "w", encoding="utf-8") as f:
            json.dump(dados, f, ensure_ascii=False, indent=2)
        print("✓ Dados salvos em animais.json")
    except Exception as e:
        print(f"✗ Erro ao salvar dados: {e}")
        raise

@app.route('/', methods=['POST'])
def handle_post():
    print(f"\n📥 POST recebido - Atualizando passos")
    try:
        dados = carregar_dados()
        pet = dados["pets"][0]
        hoje = date.today().strftime("%Y-%m-%d")
        print(f"   Data atual: {hoje}")

        passosRecentes = request.get_data(as_text=True)
        print(f"   Passos recebidos: {passosRecentes}")

        diaEncontrado = False

        for item in pet["passos"]:
            if item["data"] == hoje:
                diaEncontrado = True
                passosHoje = item["num_passos"]
                passosAntigos = passosHoje
                passosHoje = passosHoje + int(passosRecentes)
                print(f"   ✓ Dia encontrado! {passosAntigos} + {passosRecentes} = {passosHoje} passos")
                item["num_passos"] = passosHoje
                salvar_dados(dados)
                break

        if diaEncontrado == False:
            print(f"   ℹ Dia não encontrado - Criando novo registro")
            novoDia = {
                "num_passos": int(passosRecentes),
                "data": hoje
            }
            dados["pets"][0]["passos"].append(novoDia)
            print(f"   ✓ Novo dia adicionado com {passosRecentes} passos")
            salvar_dados(dados)

        print(f"✓ POST processado com sucesso\n")
        return "OK", 200
    except Exception as e:
        print(f"✗ Erro ao processar POST: {e}\n")
        return "ERRO", 500

@app.route('/', methods=['GET'])
def handle_get():
    print(f"📤 GET recebido")
    try:
        ultimo = str(ultimo_passo())
        print(f"   Último passo: {ultimo}")
        print(f"✓ GET respondido\n")
        return ultimo, 200
    except Exception as e:
        print(f"✗ Erro ao processar GET: {e}\n")
        return "ERRO", 500

@app.route('/animalInfo', methods=['GET'])
def get_animal_info():
    print(f"📤 GET /animalInfo recebido")
    try:
        dados = carregar_dados()

        if not dados:
            print(f"✗ Banco de dados vazio")
            return jsonify({"erro": "Banco de dados vazio."}), 400

        num_pets = len(dados.get("pets", []))
        print(f"   ✓ Retornando informações de {num_pets} pet(s)")
        print(f"✓ GET /animalInfo respondido\n")
        return jsonify({
            "dados": dados["pets"]
        }), 200
    except Exception as e:
        print(f"✗ Erro ao processar GET /animalInfo: {e}\n")
        return jsonify({"erro": f"Erro ao processar requisição: {e}"}), 500


@app.route('/passosDia', methods=['GET'])
def get_passos_dia():
    print(f"📤 GET /passosDia recebido")
    try:
        dados = carregar_dados()
        pet = dados["pets"][0]

        dia = request.args.get('dia')
        print(f"   Data solicitada: {dia}")

        if not dia:
            print(f"✗ Parâmetro 'dia' não fornecido")
            return jsonify({"erro": "Parâmetro 'dia' é obrigatório."}), 400
        
        for item in pet["passos"]:
            if item["data"] == dia:
                print(f"   ✓ Registro encontrado: {item['num_passos']} passos")
                print(f"✓ GET /passosDia respondido\n")
                return jsonify({
                    "data": dia,
                    "num_passos": item["num_passos"]
                }), 200

        # Se não encontrar a data
        print(f"✗ Nenhum registro encontrado para {dia}")
        print(f"✓ GET /passosDia respondido (404)\n")
        return jsonify({
            "erro": f"Nenhum registro encontrado para a data {dia}"
        }), 404
    except Exception as e:
        print(f"✗ Erro ao processar GET /passosDia: {e}\n")
        return jsonify({"erro": f"Erro ao processar requisição: {e}"}), 500

if __name__ == '__main__':
    port=8080
    print("\n" + "="*50)
    print("🚀 INICIANDO SERVIDOR PATINHAS")
    print("="*50)
    
    local_ip = get_local_ip()
    contagem = ultimo_passo()
    print(f"📊 Total de passos em cache: {contagem}")
    
    print(f"\n🔄 Iniciando thread de broadcast...")
    udp_thread = threading.Thread(target=broadcast_listener, daemon=True) # inicia broadcast em thread separada
    udp_thread.start()
    print(f"✓ Thread de broadcast ativa")

    print(f"\n📡 Servidor Flask:")
    print(f"   → Endereço: {local_ip}:{port}")
    print(f"   → URL: http://{local_ip}:{port}")
    print(f"\n✓ Servidor pronto para receber requisições!")
    print("="*50 + "\n")
    app.run(host='0.0.0.0', port=port)
