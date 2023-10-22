# Backend Scirpts

## mqtt2influx

This script is for recording incomming data from sensors via MQTT protocol.
It digests the raw data and write down into InfluxDB.
The server is currenly running on Raspberry Pi 2.
The latest InfluxDB that can run on RPi2 (armhf) is version 1.6.

### Create a database in Influx DB

```bash
sudo apt install influxdb # install the database
sudo apt install influxdb-client # install cli client
influx # activate cli. It will connect to localhost by default
create database <database-name> # create a new database
```

### Create retention policies and continuous queries

```bash
create retention policy "one_day" on "dac" duration 1d replication 1 default

create continuous query "cq_1h" on "dac"
begin
select
mean("temperature") as "temperature",
mean("humidity") as "humidity"
into "abe3078_1h"
from "abe3078"
group by time(1h), *
end
```
