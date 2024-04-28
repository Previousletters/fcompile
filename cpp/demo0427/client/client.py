import socket
import numpy as np
import time
import torch
from transformers import AutoTokenizer




def encode_command(cmd, attrs):
    return "command%sattrs%04d" % (cmd, len(attrs))


def decode_command(encode_message):
    message = str(encode_message, 'utf-8')
    if "command" == message[:7]:
        return message[7:11], message[14:15]
    else:
        print(message)
        return "Error", 0


def run_model(client, input_ids, token, kvcache):
    attrs = np.array([token, kvcache], dtype="uint16").tobytes()
    attrs += input_ids.numpy().astype("uint16").tobytes()
    client.sendall(bytes(encode_command("rmod", attrs), 'utf-8') + attrs)
    strClientData = client.recv(1024)
    state = np.frombuffer(strClientData, dtype="uint16")
    return int(state[0])


def run_model_kvcache(client, input_ids, token, kvcache):
    attrs = np.array([token, kvcache], dtype="uint16").tobytes()
    attrs += input_ids.numpy().astype("uint16").tobytes()
    client.sendall(bytes(encode_command("rkvc", attrs), 'utf-8') + attrs)
    strClientData = client.recv(1024)
    ids = np.frombuffer(strClientData, dtype="uint16")
    ids_len = ids[0]
    ids = ids[1:1+ids_len].astype("int32").tolist()
    strClientData = client.recv(1024)
    state = np.frombuffer(strClientData, dtype="uint16")
    return int(state[0]), ids_len, ids


def run_model_kvcache_show(client, input_ids, token, kvcache):
    attrs = np.array([token, kvcache], dtype="uint16").tobytes()
    attrs += input_ids.numpy().astype("uint16").tobytes()
    client.sendall(bytes(encode_command("rkvs", attrs), 'utf-8') + attrs)
    strClientData = client.recv(1024)
    ids = np.frombuffer(strClientData, dtype="uint16")
    ids_len = ids[0]
    state = ids[0]
    ids = ids[1:1+ids_len].astype("int32").tolist()
    return state, ids_len, ids


def main_test_without_kvcache(toknizer, client):
    query = "你好"
    generated_text = ""

    while True:
        prompt = "[Round {}]\n\n问：{}\n\n答：".format(1, query) + generated_text
        inputs = tokenizer([prompt], return_tensors="pt")
        next_id = run_model(client, inputs["input_ids"], inputs["input_ids"].shape[1], 0)
        if next_id == 2:
            break
        next_tokens = tokenizer.decode([next_id])
        next_tokens = ' ' if len(next_tokens) == 0 else next_tokens
        generated_text += next_tokens

    print(prompt)
    client.sendall(bytes("quit", 'utf-8'))
    client.close()


def main_kvcache(tokenizer, client):
    while True:
        query = input("User: ")
        if query == "exit" or query == "quit":
            break
        generated_text = ""
        prompt = "[Round {}]\n\n问：{}\n\n答：".format(1, query)
        inputs = tokenizer([prompt], return_tensors="pt")
        state, ids_len, ids = run_model_kvcache(client, inputs["input_ids"], inputs["input_ids"].shape[1], 0)
        generated_text = tokenizer.decode(ids)
        print("FPGA: ", generated_text)
    client.sendall(bytes("quit", 'utf-8'))
    client.close()


def main_kvcache_show(tokenizer, client):
    while True:
        query = input("User: ")
        if query == "exit" or query == "quit":
            break
        prompt = "[Round {}]\n\n问：{}\n\n答：".format(1, query)
        inputs = tokenizer([prompt], return_tensors="pt")
        print("FPGA: ", end="")
        while True:
            state, ids_len, ids = run_model_kvcache_show(client, inputs["input_ids"], inputs["input_ids"].shape[1], 0)
            if state == 0:
                break
            generated_text = tokenizer.decode(ids)
            print(generated_text, end="")
    client.sendall(bytes("quit", 'utf-8'))
    client.close()


if __name__ == "__main__":
    tokenizer = AutoTokenizer.from_pretrained("mytokens", trust_remote_code=True, local_files_only=True)
    client = socket.socket()
    client.connect(("10.20.72.156", 8123))
    print('连接到 FPGA ChatGLM2')
    main_kvcache(tokenizer, client)
