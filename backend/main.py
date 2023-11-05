from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from routers.historical_data import historical_data

description = """
Purdue Digital Agriculture club IoT API.
"""

origins = ["http://localhost:4200", "http://localhost:8005", "10.165.77.242:4200"]

app = FastAPI(
    title="Purdue DAC IoT API",
    description=description,
    version="0.2.0",
    contact={"name": "Tam Bureetes", "email": "tbureete@purdue.edu"},
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(historical_data.router)
