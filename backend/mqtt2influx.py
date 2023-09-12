import os
import paho.mqtt.subscribe as subscribe
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS, WriteApi
from dotenv import load_dotenv
from datetime import datetime

MQTT_ROOT_TOPIC = "purdue-dac"
ORG = "Digital Agriculture Club"


def on_message_mqtt(_, userdata, message):
    write_api: WriteApi = userdata["influx_write_api"]
    bucket = userdata["bucket"]
    point = Point("ABE3078")
    # extract sensor name from mqtt topic
    sensor_name = message.topic[len(MQTT_ROOT_TOPIC) + 1 :]
    point = point.tag("sensor_name", sensor_name)
    # extract data in order: temperature, humidity, and timestamp
    data = message.payload.decode("UTF-8").split(":")
    point = point.field("temperature", data[0]).field("humidity", data[1])
    # record timestamp as UTC
    timestamp = datetime.utcfromtimestamp(int(data[2])).isoformat()
    point = point.time(time=timestamp, write_precision="s")
    # record data into influxdb
    write_api.write(bucket=bucket, org=ORG, record=point)
    print(f"Record data from {sensor_name} at {timestamp}")

if __name__ == "__main__":
    load_dotenv()

    token = os.environ.get("INFLUXDB_TOKEN")
    url = "http://localhost:8086"

    influx_client = InfluxDBClient(url=url, token=token, org=ORG)
    write_api = influx_client.write_api(write_options=SYNCHRONOUS)
    bucket = "sensor-data"

    subscribe.callback(
        callback=on_message_mqtt,
        topics=f"{MQTT_ROOT_TOPIC}/#",
        hostname="66.253.158.154",
        userdata={"influx_write_api": write_api, "bucket": bucket},
    )
