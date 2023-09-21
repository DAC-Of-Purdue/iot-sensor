from fastapi import APIRouter
from . import crud, schemas

router = APIRouter(prefix="/history", tags=["Historical Data"])


@router.get(
    "/",
    summary="Read historical data from the database",
    response_model=list[schemas.HistoricalDataPayload],
)
def read_data(period: str | None = None):
    """
    Read historical data. 
    By default, the API will return data from all devices in past 5 minutes.
    Optional query parameters:
    - **period**: query period. For example, "30m" for 30 minutes or "2h" for 2 hours.
    """
    return crud.query_all_tags(period=period)
