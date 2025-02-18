# Script to run on a laptop with the Bleak library, to connect to a WW500.
# This is a proof-of-concept that shows that a laptop can be used in place of a smartphone app
# 

import asyncio
from bleak import BleakClient, BleakScanner

# Nordic uart Service defined here: https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/libraries/bluetooth/services/nus.html
NUS_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"    

# Write data to the RX Characteristic to send it to the UART interface.
# Write or Write Without Response
RX_CHARACTERISTIC_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"   

# Enable notifications for the TX Characteristic to receive data from the application. 
# The application transmits all data that is received over UART as notifications.
TX_CHARACTERISTIC_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" 

DEVICE_NAME = "WILD-JYIC"  # Replace with your device name

# Handle disconnection events
def on_disconnect(client):
    print("Device disconnected")

async def main():
    print("*** Searching for WW500 by device name '",DEVICE_NAME,"' ***")
    # Define a function to filter devices based on the service UUID
    def filter_by_service_uuid(device, advertisement_data):
        return NUS_SERVICE_UUID in advertisement_data.service_uuids

    # Define a function to filter devices based on the device name
    def filter_by_device_name(device, advertisement_data):
        #return device.name == DEVICE_NAME
        return device.name is not None and device.name == DEVICE_NAME

    devices = await BleakScanner.discover(filter=filter_by_device_name)
    address = None
    for device in devices:
        print(f"Device found: {device.name} (MAC: {device.address})")
        address = device.address
        break

    if address is None:
        print("Device with the specified service UUID not found. Please check the UUID and try again.")
        return

    async with BleakClient(address, disconnected_callback=on_disconnect) as client:
        # Ensure the client is connected
        if not client.is_connected:
            print("Failed to connect to the device.")
            return

        print("Device connected")

        # Perform service discovery
        # deprecated :await client.get_services()
        # Access the services property
        services = client.services

        try:
            def notification_handler(sender, data):
                print("Notification received:", data.decode())

            # Set up and start notifications for the TX characteristic
            await client.start_notify(TX_CHARACTERISTIC_UUID, notification_handler)

            await asyncio.sleep(100)  # Keep the script running for 100 seconds

            # Stop notifications
            await client.stop_notify(TX_CHARACTERISTIC_UUID)

        except Exception as e:
            print(f"An error occurred: {e}")

asyncio.run(main())