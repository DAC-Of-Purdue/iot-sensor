from fastapi import APIRouter
from . import crud, schemas

router = APIRouter(prefix="/history", tags=["Historical Data"])


@router.get(
    "/",
    summary="Read historical data from the database",
    response_model=list[schemas.HistoricalDataPayload],
)
def read_data():
    payload = crud.query_all_tags()
    print(payload)
    return payload
