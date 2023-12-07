from fastapi import APIRouter, status
from fastapi.responses import Response
from . import schemas

router = APIRouter(prefix="/groot", tags=["Groot (automatic watering) project"])


@router.get(
    "/command",
    summary="Check water is needed",
    response_model=schemas.Command,
)
def read_command():
    """
    API to tell watering system to activate
    """
    return {"water": True}
