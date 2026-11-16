# TCP server za test WiFi + motori.
# Pokretanje:  python tcp_server2.py
# Nakon prve poruke sa vozila: čeka 3 s, šalje START, čeka 8 s, šalje STOP.
# Sve što vozilo pošalje ispisuje se na ekran.

import socket
import threading
import time

PORT = 8084

srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
srv.bind(("0.0.0.0", PORT))
srv.listen(1)

print("Cekam vezu na portu", PORT, "...")
conn, addr = srv.accept()
print("Spojen:", addr)

prva = threading.Event()


def prijem():
    while True:
        try:
            data = conn.recv(1024)
        except OSError:
            break
        if not data:
            print("Veza zatvorena.")
            break
        print("Primljeno:", data.decode(errors="replace").strip())
        prva.set()


threading.Thread(target=prijem, daemon=True).start()

prva.wait()
time.sleep(3)
conn.sendall(b"START\n")
print("Poslano: START")

time.sleep(8)
conn.sendall(b"STOP\n")
print("Poslano: STOP")

time.sleep(5)
print("Kraj testa.")
