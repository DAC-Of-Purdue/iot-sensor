from fastapi import FastAPI

from routers.historical_data import historical_data

description = """
Purdue Digital Agriculture club IoT API.
"""

app = FastAPI(
    title="Purdue DAC IoT API",
    description=description,
    version="0.1.0",
    contact={"name": "Tam Bureetes", "email": "tbureete@purdue.edu"},
)

app.include_router(historical_data.router)
