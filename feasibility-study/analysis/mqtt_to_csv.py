import csv
import json
from datetime import datetime
from pathlib import Path
import paho.mqtt.client as mqtt

#Configurazione del broker
BROKER = "localhost"
PORT = 1883
TOPIC = "wifi_sensing/rssi"

# Percorso del dataset
BASE_DIR = Path(__file__).resolve().parent.parent
CSV_PATH = BASE_DIR / "datasets" / "dataset_rssi.csv"


def initialize_csv():
    CSV_PATH.parent.mkdir(parents=True, exist_ok=True)

    if not CSV_PATH.exists() or CSV_PATH.stat().st_size == 0:
        with open(CSV_PATH, "w", newline="", encoding="utf-8") as file:
            writer = csv.writer(file)
            writer.writerow(["timestamp", "rssi"])


def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Connesso al broker MQTT")
        client.subscribe(TOPIC)
        print(f"In ascolto sul topic: {TOPIC}")
    else:
        print(f"Connessione fallita: {reason_code}")



def on_message(client, userdata, msg):
    raw = msg.payload.decode("utf-8")

    print(f"DEBUG - Topic: {msg.topic}")
    print(f"DEBUG - Payload ricevuto: {raw!r}")

    try:
        # Decodifica JSON
        data = json.loads(raw)

        # Estrazione dei singoli valori
        rssi = int(data["rssi"])
        uptime_ms = int(data["uptime_ms"])

        timestamp = datetime.now().astimezone().isoformat(
            timespec="milliseconds"
        )

        with open(CSV_PATH, "a", newline="", encoding="utf-8") as file:
            writer = csv.writer(file)
            writer.writerow([timestamp, rssi, uptime_ms])

        print(f"OK | RSSI: {rssi} | Uptime: {uptime_ms}")

    except (json.JSONDecodeError, ValueError, KeyError, TypeError) as e:
        print(f"ERRORE: {type(e).__name__}: {e}")


def main():
    initialize_csv()

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(BROKER, PORT, 60)

    print("Avvio acquisizione RSSI...")

    try:
        client.loop_forever()
    except KeyboardInterrupt:
        print("\nAcquisizione terminata")
    finally:
        client.disconnect()


if __name__ == "__main__":
    main()
