from fastapi import APIRouter, status
from fastapi.responses import Response
from . import crud, schemas

router = APIRouter(prefix="/history", tags=["Historical Data"])


@router.get(
    "/",
    summary="All devices",
    response_model=list[schemas.HistoricalDataPayload],
)
def read_data(period: str | None = None):
    """
    Read historical data.
    By default, the API will return data from all devices in past 5 minutes.
    Optional query parameters:
    - **period**: query period. For example, "30m" for 30 minutes or "2h" for 2 hours.
    """
    return crud.query_data(period=period)


@router.get(
    "/{sensor_name}",
    summary="One device",
    response_model=list[schemas.HistoricalDataPoint],
    responses={status.HTTP_204_NO_CONTENT: {}},
)
def read_data_device(sensor_name: str, period: str | None = None):
    """
    Read historical data.
    By default, the API will return data from a specific device in past 5 minutes.
    Optional query parameters:
    - **period**: query period. For example, "30m" for 30 minutes or "2h" for 2 hours.
    """
    data = crud.query_data(period=period, sensor_name=sensor_name)
    return Response(status_code=status.HTTP_204_NO_CONTENT) if len(data) == 0 else data
