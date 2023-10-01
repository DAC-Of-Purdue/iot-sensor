from pydantic import BaseModel
from datetime import datetime


class Message(BaseModel):
    message: str


class HistoricalDataPoint(BaseModel):
    time: datetime
    temperature: float
    humidity: float


class HistoricalDataPayload(BaseModel):
    sensor_name: str
    points: list[HistoricalDataPoint]
