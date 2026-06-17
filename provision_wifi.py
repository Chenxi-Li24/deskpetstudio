"""Send WiFi credentials to DeskPet ESP32-S3 via BLE."""
import asyncio
import json
from bleak import BleakClient, BleakScanner

SERVICE_UUID = "12345678-1234-1234-1234-123456789abc"
CHAR_WIFI_CFG = "abcd0002-1234-1234-1234-123456789abc"
SSID = "OmniX_Race"
PASS = "12345678"

async def main():
    print(f"Scanning for DeskPet-Mini...")
    device = await BleakScanner.find_device_by_name("DeskPet-Mini", timeout=10)
    if not device:
        print("ERROR: DeskPet-Mini not found. Is the device powered on and advertising?")
        return
    print(f"Found: {device.name} ({device.address})")

    async with BleakClient(device) as client:
        print("Connected!")
        payload = json.dumps({"ssid": SSID, "pass": PASS})
        data = payload.encode()
        print(f"Sending: {payload}")
        await client.write_gatt_char(CHAR_WIFI_CFG, data, response=True)
        print("WiFi credentials sent! The device should connect now.")
        await asyncio.sleep(1)

asyncio.run(main())
