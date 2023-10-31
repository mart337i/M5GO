import urequests
import time
from m5stack import lcd, btnA
from udatetime import datetime

BASE_URL = "http://meo.local"

class Sensor:
    def __init__(self, name="", building_id=1, facility_id=1, sensor_id=None):
        self.name = urequests.get(f"{BASE_URL}/get_target_name/").text
        self.building_id = urequests.get(f"{BASE_URL}/get_target_building/").text
        self.facility_id = urequests.get(f"{BASE_URL}/get_target_facility/").text
        
        sensor_data = {
            "name": self.name,
            "building_id": self.building_id
        }
        
        url = f"{BASE_URL}/sensor/"
        response = urequests.post(url, json=sensor_data)
        if response.status_code == 200:
            content = response.json()
            self.sensor_id = content["id"]
        else:
            self.sensor_id = None

class SensorType:
    TEMPERATURE = "temperature"
    HUMIDITY = "humidity"

class SensorValue:
    id = None
    type = None
    max_value = None
    low_value = None
    value = None
    datetime = None
    sensor_id = None

global_sensor = Sensor(name="", building_id=1, facility_id=1, sensor_id=0)

def send_alarm(message):
    url = f"{BASE_URL}/alarm/"
    data = {
        "type": "failure",
        "message": message,
        "sonor_id" : global_sensor.sensor_id
    }
    response = urequests.post(url, json=data)
    if response.status_code != 200:
        lcd.print(f"Alarm sent: {response.status_code}, {response.text}")
    else:
        lcd.print(f"Alarm sent: {response.status_code}, {response.text}")

def get_temp_humidity():
    try:
        # Placeholder for reading temperature and humidity from M5GO's DHT sensor
        temperature = 25.0  # Example temperature value
        humidity = 60.0     # Example humidity value

        lcd.print(f"Temp : {temperature}.\nhum: {humidity}")
        return temperature, humidity

    except Exception as e:
        send_alarm(f"Error with M5GO sensor: {e}")
        return None, None

def post_temp_humidity_data(sensor=global_sensor):
    url = f"{BASE_URL}/sensor_value/"
    temperature, humidity = get_temp_humidity()
    if not temperature or not humidity:
        lcd.print(f"Value not set for temperature or humidity")
        return

    # Creating temperature data instance
    temp_data = SensorValue()
    temp_data.type = SensorType.TEMPERATURE
    temp_data.value = temperature
    temp_data.datetime = datetime.utcnow().isoformat()
    temp_data.sensor_id = sensor.sensor_id
    temp_data.max_value = 100
    temp_data.low_value = 0

    data = {
        "name": sensor.name,
        "sensor_value": temp_data.__dict__,
        "building_id": sensor.building_id
    }

    response = urequests.post(url, json=data)
    lcd.print(f"Posted temperature to API: {response.status_code}, {response.text}")

    # Creating humidity data instance
    humidity_data = SensorValue()
    humidity_data.type = SensorType.HUMIDITY
    humidity_data.value = humidity
    humidity_data.datetime = datetime.utcnow().isoformat()
    humidity_data.sensor_id = sensor.sensor_id
    humidity_data.max_value = 100
    humidity_data.low_value = 0

    data["sensor_value"] = humidity_data.__dict__
    response = urequests.post(url, json=data)
    lcd.print(f"Posted humidity to API: {response.status_code}, {response.text}")

def main():
    lcd.clear()
    lcd.print("Starting application")
    if global_sensor.sensor_id == 0:
        return exit(-1)

    while True:
        if btnA.isPressed():
            post_temp_humidity_data()
        time.sleep(1)

if __name__ == "__main__":
    main()