from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from routers.historical_data import historical_data
from routers.groot import groot

description = """
Purdue Digital Agriculture club IoT API.
"""

app = FastAPI(
    title="Purdue DAC IoT API",
    description=description,
    version="0.2.2",
    contact={"name": "Tam Bureetes", "email": "tbureete@purdue.edu"},
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(historical_data.router)
app.include_router(groot.router)
