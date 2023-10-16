import socket
import sqlite3
import random
import string

HOST = '127.0.0.1'
PORT = 4444

DATABASE_FILE = 'user_database.db'

def generate_session_token():
    token_length = 16
    characters = string.ascii_letters + string.digits
    return ''.join(random.choice(characters) for _ in range(token_length))

def authenticate_user(username, password):
    conn = sqlite3.connect(DATABASE_FILE)
    cursor = conn.cursor()
    cursor.execute("SELECT password FROM users WHERE username=?", (username,))
    row = cursor.fetchone()
    if row and row[0] == password:
        return True
    else:
        return False

def store_session_token(username, token):
    conn = sqlite3.connect(DATABASE_FILE)
    cursor = conn.cursor()
    cursor.execute("INSERT INTO sessions (username, token) VALUES (?, ?)", (username, token))
    conn.commit()
    conn.close()

def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(5)

    print(f"Server listening on {HOST}:{PORT}")

    while True:
        client_socket, addr = server_socket.accept()
        print(f"Accepted connection from {addr}")

        data = client_socket.recv(1024).decode()

        if not data:
            break

        username, password = data.split()

        if authenticate_user(username, password):
            session_token = generate_session_token()
            store_session_token(username, session_token)
            response = f"Username = {username} \npassword = {password}\nSession token = {session_token}"
        else:
            response = "Invalid creds"

        client_socket.send(response.encode())
        client_socket.close()

if __name__ == '__main__':
    main()
