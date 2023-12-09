from fastapi import APIRouter, status
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


@router.post(
    "/command",
    summary="Check watering is performed",
    status_code=status.HTTP_201_CREATED
)
def feedback(command: schemas.Command):
    """
    API to get the feedback from watering system
    """
    print(command)
    print("Done watering ")
    
    return 
