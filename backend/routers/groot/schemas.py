from pydantic import BaseModel


class Message(BaseModel):
    message: str


class Command(BaseModel):
    water: bool
