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
create database raw # first create a raw database
use raw # change workspace
# the raw data will be keep only for a week
create retention policy "one_week" on "raw" duration 1w replication 1 default

create database dac # this is the main database
use dac # change workspace
# this database is 1-hour average data which will be kept for 1 year
create retention policy "one_year" on "dac" duration 52w replication 1 default

use raw # back to raw data
create continuous query "cq_1h_dac" on "raw" # create continuous query 
begin
select mean(*) # average all data 
# dump average data into dac database with one year retention policy
into "dac"."one_year".:MEASUREMENT 
from /.*/ # query from current database (raw)
group by time(1h), * # query every 1 hour and preserve tags (*)
end
```