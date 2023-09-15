from pydantic import BaseModel
from datetime import datetime


class HistoricalData(BaseModel):
    timestamp: datetime
    tag: str
    temperature: float
    humidity: float
