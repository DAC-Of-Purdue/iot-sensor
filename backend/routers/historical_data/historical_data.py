from fastapi import APIRouter
from . import crud

router = APIRouter(prefix="/history", tags=["Historical Data"])


@router.get("/", summary="Read historical data from the database")
def read_data():
    # crud.load_all()
    return "Hello"
