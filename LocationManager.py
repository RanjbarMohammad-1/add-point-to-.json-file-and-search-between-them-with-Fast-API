
import json
import os


class LocationManager:

    def __init__(self, filename="points.json"):
        self.filename = filename

        if not os.path.exists(self.filename):
            self._save_data({"points": []})

    def _read_data(self):
        with open(self.filename, "r", encoding="utf-8") as file:
            return json.load(file)

    def _save_data(self, data):
        with open(self.filename, "w", encoding="utf-8") as file:
            json.dump(
                data,
                file,
                indent=4,
                ensure_ascii=False
            )

    def add_point(self, name, latitude, longitude):

        name = name.strip()

        if not name:
            raise ValueError("Point name cannot be empty")

        if not -90 <= latitude <= 90:
            raise ValueError("Invalid latitude")

        if not -180 <= longitude <= 180:
            raise ValueError("Invalid longitude")

        data = self._read_data()

        for point in data["points"]:
            if point["name"].casefold() == name.casefold():
                raise ValueError("Point already exists")

        point = {
            "name": name,
            "latitude": latitude,
            "longitude": longitude
        }

        data["points"].append(point)

        self._save_data(data)

        return point

    def search_by_name(self, name):

        name = name.strip()

        if not name:
            return None

        data = self._read_data()

        for point in data["points"]:

            if point["name"].casefold() == name.casefold():
                return point

        return None
