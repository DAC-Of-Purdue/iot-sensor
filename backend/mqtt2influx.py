import paho.mqtt.subscribe as subscribe
from datetime import datetime
from influxdb import InfluxDBClient

MQTT_ROOT_TOPIC = "purdue-dac"


def on_message_mqtt(_, userdata, message):
    influx_client: InfluxDBClient = userdata["influx_client"]
    # extract sensor name from mqtt topic
    sensor_name = message.topic[len(MQTT_ROOT_TOPIC) + 1 :]
    # extract data in order: temperature, humidity, and timestamp
    data = message.payload.decode("UTF-8").split(":")
    # record timestamp as UTC
    timestamp = datetime.utcfromtimestamp(int(data[2])).isoformat()
    point = [
        {
            "measurement": "abe3078",
            "tags": {"sensor_name": sensor_name},
            "time": timestamp,
            "fields": {"temperature": float(data[0]), "humidity": float(data[1])},
        }
    ]
    # record data into influxdb
    influx_client.write_points(point)
    print(f"Record data from {sensor_name} at {timestamp}")


if __name__ == "__main__":
    influx_client = InfluxDBClient(database="raw")

    subscribe.callback(
        callback=on_message_mqtt,
        topics=f"{MQTT_ROOT_TOPIC}/#",
        hostname="66.253.158.154",
        userdata={"influx_client": influx_client},
    )
