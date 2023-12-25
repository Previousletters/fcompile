import socket
import numpy as np
import time

server = socket.socket()
server.bind(("127.0.0.1", 8000))
server.listen(5)
print('server waiting...')


def encode_command(cmd, attrs):
    return "command%sattrs%04d%s" % (cmd, len(attrs), attrs)


def decode_command(message):
    if "command" == message[:7]:
        return message[7:11], message[14:15]
    else:
        print(message)
        return "Error", 0


def load_model(client, name):
    client.sendall(bytes(encode_command("load", name), 'utf-8'))
    strClientData = client.recv(1024)
    state = decode_command(str(strClientData, 'utf-8'))
    print(state)


def run_model(client, name):
    client.sendall(bytes(encode_command("rmod", name), 'utf-8'))
    strClientData = client.recv(1024)
    state = decode_command(str(strClientData, 'utf-8'))
    print(state)


def set_inputs(client, index, data: np.array):
    data = data.tobytes()
    times, dlast = (len(data) + 1023) // 1024, len(data) % 1024
    if dlast == 0:
        times -= 1
        dlast = 1024
    attrs = "%04d%04d%04d" % (index, times, dlast)
    client.sendall(bytes(encode_command("iptr", attrs), 'utf-8'))
    strClientData = client.recv(1024)
    state = decode_command(str(strClientData, 'utf-8'))
    if state:
        for n in range(times-1):
            # time.sleep(0.01)
            client.sendall(data[n*1024:(n+1)*1024])
            strClientData = client.recv(1024)
            state = decode_command(str(strClientData, 'utf-8'))
        client.sendall(data[-dlast:])
        strClientData = client.recv(1024)
        state = decode_command(str(strClientData, 'utf-8'))
        print(state)


def get_outputs(client, index, times, dlast):
    attrs = "%04d%04d%04d" % (index, times, dlast)
    client.sendall(bytes(encode_command("optr", attrs), 'utf-8'))
    data = b""
    for n in range(times-1):
        time.sleep(0.01)
        data += client.recv(1024)
    data += client.recv(1024)
    data = data[:(times-1)*1024+dlast]
    strClientData = client.recv(1024)
    state = decode_command(str(strClientData, 'utf-8'))
    print(state)
    return np.frombuffer(data, dtype="int16")


def process(data, weight, tin=128, tout=32):
    Tout = tout
    Tinf = tin
    Tino = tin
    H, W = 1, data.shape[0]
    OH, OW = H, W

    C = data.shape[1]

    SP_Tout = (OH*OW + Tout - 1) // Tout
    SC_Tinf = (    C + Tinf - 1) // Tinf
    ST_Tout = ( Tino + Tout - 1) // Tout
    reshape_dt = np.zeros((SC_Tinf, H, W, Tinf), dtype="int16")
    for sc in range(SC_Tinf):
        for h in range(H):
            for w in range(W):
                for ti in range(Tinf):
                    if sc*Tinf+ti < C:
                        reshape_dt[sc, h, w, ti] = data[w][sc*Tinf+ti]

    if weight is not None:
        O = weight.shape[1]
        SO_Tino = (    O + Tino - 1) // Tino
        reshape_wt = np.zeros((SO_Tino, SC_Tinf, ST_Tout, Tout, Tinf), dtype="int16")
        for so in range(SO_Tino):
            for sc in range(SC_Tinf):
                for st in range(ST_Tout):
                    for to in range(Tout):
                        for ti in range(Tinf):
                            if so*ST_Tout*Tout + st*Tout + to < O and sc*Tinf + ti < C:
                                reshape_wt[so, sc, st, to, ti] = weight[sc*Tinf + ti, so*ST_Tout*Tout + st*Tout + to]
        return reshape_dt, reshape_wt
    return reshape_dt


while True:
    client, addr = server.accept()
    data = np.random.randint(-10, 10, (1, 3), "int16")
    weight = np.random.randint(-10, 10, (3, 2), "int16")
    result = data @ weight
    load_model(client, "./model.so")
    time.sleep(0.01)
    set_inputs(client, 0, data)
    time.sleep(0.01)
    set_inputs(client, 1, weight)
    time.sleep(0.01)
    run_model(client, "./model.so")
    time.sleep(0.01)
    output = get_outputs(client, 2, 1, 4)
    print(np.sum(result.flatten() != output.flatten()))
    client.sendall(bytes("quit", 'utf-8'))
    client.close()
