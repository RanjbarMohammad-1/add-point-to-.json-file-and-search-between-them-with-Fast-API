from LocationManager import LocationManager
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field



def main():


    ##this part is for ordinary usage without Fast API
    manager = LocationManager()

    # Add points
    try:
        # manager.add_point("تهران", 35.6892, 51.3890)
        # manager.add_point("Berlin", 52.5200, 13.4050)
        manager.add_point("1تهران", 35.6892, 51.3890)
        manager.add_point("1Berlin", 52.5200, 13.4050)
    except ValueError as error:
        print(error)

    # # Search for a Persian point
    result = manager.search_by_name("تهران")

    if result:
        print("Point found:")
        print(result)
    else:
        print("Point not found")

    # # Search for an English point
    # result = manager.search_by_name("Berlin")

    # if result:
    #     print("Point found:")
    #     print(result)
    # else:
    #     print("Point not found")





if __name__=='__main__':
    main()