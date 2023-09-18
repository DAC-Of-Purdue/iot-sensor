import os
from influxdb import InfluxDBClient
from influxdb.resultset import ResultSet


def query_all_tags(period: str = "5m") -> list[dict]:
    payload = []
    try:
        influx_client = InfluxDBClient(database="dac", timeout=10)
        result: ResultSet = influx_client.query(
            f'select * from abe3078 where time > now() - {period} group by "sensor_name";'
        )
        for data in result.items():
            print(data)
            payload.append(
                {
                    "sensor_name": data[0][1]["sensor_name"],
                    "points": [point for point in data[1]],
                }
            )
    except Exception as e:
        print(e)

    return payload


if __name__ == "__main__":
    query_all_tags()
