import os
from influxdb import InfluxDBClient
from influxdb.resultset import ResultSet


def query_data(period: str | None, sensor_name: str | None = None, raw_data: bool = False) -> list[dict]:
    payload = []
    database = "raw" if raw_data else "dac"
    if period is None:
        period = "5m" if raw_data else "12h"
    try:
        influx_client = InfluxDBClient(database=database, timeout=10)
        # With specific sensor name
        if sensor_name:
            result: ResultSet = influx_client.query(
                f"""select * from abe3078 where time > now() - {period} and "sensor_name" = '{sensor_name}';"""
            )
            # In case of wrong sensor name
            if len(result) == 0:
                return []

            for data in result.items()[0][1]:
                del data["sensor_name"]
                payload.append(data)

        # All sensors
        else:
            result: ResultSet = influx_client.query(
                f'select * from abe3078 where time > now() - {period} group by "sensor_name";'
            )
            for data in result.items():
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
    query_data()
