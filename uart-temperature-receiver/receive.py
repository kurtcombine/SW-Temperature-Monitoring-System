"""
A GUI application to receive and display temperature data over a UART connection.
"""

import tkinter as tk
from tkinter import scrolledtext, ttk
import threading
from datetime import datetime
import argparse
import re
import serial

CMD_RE = re.compile(r"TEMPERATURE\(([^_)]*)_([^_)]*)_([^_)]*)\)")


class UARTTemperatureReceiver:
    """
    A GUI application to receive and display temperature data over a UART connection.

    Attributes:
        root (tk.Tk): The root Tkinter window.
        serial_port (str): The serial port to connect to.
        baud_rate (int): The baud rate for the serial connection.
        is_running (bool): A flag to indicate if the serial thread is running.
        data (dict): A dictionary storing temperature data for each device.
        serial_conn (serial.Serial): The serial connection object.
    """

    def __init__(self, root, port, baud_rate):
        """
        Initialize the UARTTemperatureReceiver application.

        Args:
            root (tk.Tk): The root Tkinter window.
            port (str): The serial port to connect to.
            baud_rate (int): The baud rate for the serial connection.
        """
        self.serial_port = port
        self.baud_rate = baud_rate
        self.is_running = False
        self.data = {}

        self.root = root
        self.root.title("UART Temperature Receiver")

        self.device_label = tk.Label(root, text="Device:")
        self.device_label.pack()
        self.device_var = tk.StringVar(root)
        self.device_select = ttk.Combobox(root, textvariable=self.device_var)
        self.device_var.set("ALL DEVICES")
        self.add_device("ALL DEVICES")
        del self.data["ALL DEVICES"]
        self.device_var.trace_add("write", self.on_device_change)
        self.device_select.pack()

        self.console = scrolledtext.ScrolledText(root, state="disabled")
        self.console.pack(expand=True, fill="both")

        self.status_label = tk.Label(root, text="Disconnected", fg="red")
        self.status_label.pack()

        self.start_serial_thread()

    def add_device(self, device):
        """
        Add a new device to the device list.

        Args:
            device (str): The name of the device to add.
        """
        self.data[device] = []
        self.device_select["values"] = (*self.device_select["values"], device)

    def on_device_change(self, *args):
        """
        Handle changes in the selected device.
        """
        self.console.config(state="normal")
        self.console.delete(1.0, tk.END)
        selected_device = self.device_var.get()

        if selected_device == "ALL DEVICES":
            all_data = [
                (timestamp, device, temperature)
                for device, records in self.data.items()
                for timestamp, temperature in records
            ]
            all_data.sort(key=lambda x: x[0])
            for timestamp, device, temperature in all_data:
                msg = (
                    f"[{timestamp.isoformat()} {device}] "
                    f"Temperature: {temperature} °C"
                )
                self.log_message(msg, date=False)
        else:
            for timestamp, temperature in self.data[selected_device]:
                self.log_message(
                    f"[{timestamp.isoformat()}] Temperature: {temperature} °C",
                    date=False,
                )

    def start_serial_thread(self):
        """
        Start the thread for reading data from the serial connection.
        """
        try:
            self.serial_conn = serial.Serial(
                self.serial_port, self.baud_rate, timeout=1
            )
            self.status_label.config(
                text=f"Connected to {self.serial_port}", fg="green"
            )
            self.is_running = True
            self.thread = threading.Thread(target=self.read_serial_data, daemon=True)
            self.thread.start()
        except serial.SerialException as e:
            self.log_message(f"Error connecting to {self.serial_port}: {e}")

    def read_serial_data(self):
        """
        Continuously read data from the serial connection.
        """
        while self.is_running:
            try:
                if self.serial_conn.in_waiting > 0:
                    raw_data = self.serial_conn.readline().decode("cp1252").strip()
                    device = self.parse_command(raw_data)

                    if device:
                        all_devices = self.device_var.get() == "ALL DEVICES"
                        if device == self.device_var.get() or all_devices:
                            timestamp, temperature = self.data[device][-1]
                            device_label = f" {device}" if all_devices else ""
                            self.log_message(
                                f"[{timestamp.isoformat()}{device_label}] "
                                f"Temperature: {temperature} °C",
                                date=False,
                            )
            except Exception as e:
                self.log_message(f"Error reading data: {e}")
                self.is_running = False

    def parse_command(self, command):
        """
        Parse a temperature command and update the data.

        Args:
            command (str): The received command string.

        Returns:
            str or None: The device name if the command is valid, otherwise None.
        """
        match = CMD_RE.match(command)
        if match:
            device, temperature, _ = match.groups()
            if device not in self.data:
                self.add_device(device)
            self.data[device].append((datetime.now(), float(temperature)))
            return device
        return None

    def log_message(self, message, date=True):
        """
        Log a message to the console.

        Args:
            message (str): The message to log.
            date (bool): Whether to include a timestamp in the log.
        """
        if date:
            message = f"[{datetime.now().isoformat()}] {message}"

        self.console.config(state="normal")
        self.console.insert(tk.END, message + "\n")
        self.console.see(tk.END)
        self.console.config(state="disabled")

    def on_close(self):
        """
        Handle application close event.
        """
        self.is_running = False
        if hasattr(self, "thread"):
            self.thread.join()
        if hasattr(self, "serial_conn") and self.serial_conn.is_open:
            self.serial_conn.close()
        self.root.destroy()


def main(port, baud_rate):
    """
    Entry point for the application.

    Args:
        port (str): The serial port to connect to.
        baud_rate (int): The baud rate for the serial connection.
    """
    root = tk.Tk()
    app = UARTTemperatureReceiver(root, port, baud_rate)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop()


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="UART Temperature Receiver")
    arg_parser.add_argument(
        "port",
        type=str,
        nargs="?",
        default="/dev/ttyUSB0",
        help="Serial port to connect to",
    )
    arg_parser.add_argument(
        "--baud-rate",
        type=int,
        default=9600,
        help="Baud rate for the serial connection",
    )
    args = arg_parser.parse_args()
    main(args.port, args.baud_rate)

# use `socat -d -d pty,raw,echo=0 pty,raw,echo=0`
