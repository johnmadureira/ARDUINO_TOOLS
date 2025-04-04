


# Função para tentar se conectar automaticamente ao Arduino Leonardo
def conectar_arduino():
    portas = [f"/dev/ttyUSB{i}" for i in range(256)]  # Lista de possíveis portas em sistemas Linux
    portas += [f"COM{i}" for i in range(1, 256)]  # Lista de portas em sistemas Windows

    for porta in portas:
        try:
            ser = serial.Serial(porta, 9600, timeout=0.1) 
            print(f'Arduino conectado na porta {porta}')
            return ser
        except serial.SerialException:
            continue  # Se falhar, tenta a próxima porta

    print("Não foi possível conectar ao Arduino.")
    return None