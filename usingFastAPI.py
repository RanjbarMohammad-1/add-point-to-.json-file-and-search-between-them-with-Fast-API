
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field

from LocationManager import LocationManager


# Create FastAPI application
app = FastAPI(
    title="Location Management API",
    description="API for managing geographical points",
    version="1.0.0"
)

# Create LocationManager object
manager = LocationManager()


# Request model
class PointRequest(BaseModel):
    name: str = Field(min_length=1)
    latitude: float = Field(ge=-90, le=90)
    longitude: float = Field(ge=-180, le=180)


# Add a new point
@app.post("/points", status_code=201)
def add_point(point: PointRequest):

    name = point.name.strip()

    if not name:
        raise HTTPException(
            status_code=422,
            detail="Point name cannot be empty"
        )

    try:
        result = manager.add_point(
            name,
            point.latitude,
            point.longitude
        )

        return {
            "success": True,
            "message": "Point added successfully",
            "point": result
        }

    except ValueError as error:
        raise HTTPException(
            status_code=409,
            detail=str(error)
        )


# Search for a point by name
@app.get("/points/{name}")
def search_point(name: str):

    result = manager.search_by_name(name)

    if result is None:
        raise HTTPException(
            status_code=404,
            detail="Point not found"
        )

    return {
        "success": True,
        "point": result
    }
