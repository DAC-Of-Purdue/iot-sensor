from fastapi import APIRouter

router = APIRouter(prefix="/history", tags=["Historical Data"])


@router.get("/", summary="Read historical data from the database")
def read_data():
    return "Hello"
