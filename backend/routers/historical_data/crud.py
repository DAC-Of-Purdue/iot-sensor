import os
from influxdb import InfluxDBClient
from dotenv import load_dotenv

def load_all(): 

    load_dotenv()

    # select * from abe3078 where  time > now() - 1m group by "sensor_name"
    influx_client = InfluxDBClient(host=os.environ.get("INFLUX_HOST"), database="dac")
    result = influx_client.query('select * from abe3078 limit 5')
    print(result)

if __name__ == '__main__':
    load_all()