# flake8: noqa: E402
# pylint: disable=C0114, C0116, C0115, C0413, E1101
import tkinter as tk
from tkinter import scrolledtext
import threading
from datetime import datetime
import argparse
import re
import traceback
import serial

CMD_RE = re.compile(r"TEMPERATURE\(([^_)]*)_([^_)]*)_([^_)]*)\)")


class UARTTemperatureReceiver:
    def __init__(self, root, port, baud_rate):
        self.serial_port = port
        self.baud_rate = baud_rate
        self.is_running = False

        self.data = {}

        self.root = root
        self.root.title("UART Temperature Receiver")

        self.device_label = tk.Label(root, text="Device:")
        self.device_label.pack()
        self.device_var = tk.StringVar(root)
        self.device_select = tk.OptionMenu(root, self.device_var, [])
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
        self.data[device] = []
        self.device_select["menu"].add_command(
            label=device, command=tk._setit(self.device_var, device)
        )

    def on_device_change(self, *args):
        self.console.config(state="normal")
        self.console.delete(1.0, tk.END)
        device = self.device_var.get()
        if device == "ALL DEVICES":
            data = [
                (date, dev, temp)
                for dev, data in self.data.items()
                for date, temp in data
            ]
            data.sort(key=lambda x: x[0])
            for date, dev, temp in data:
                self.log_message(
                    f"[{date.isoformat()} {dev}] Temperature: {temp} °C", date=False
                )
        else:
            for date, temp in self.data[device]:
                self.log_message(
                    f"[{date.isoformat()}] Temperature: {temp} °C", date=False
                )

    def start_serial_thread(self):
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
            err = traceback.format_exception_only(type(e), e)[0].strip()
            self.log_message(f"Error connecting to {self.serial_port}: {err}")

    def read_serial_data(self):
        while self.is_running:
            try:
                if self.serial_conn.in_waiting > 0:
                    data = self.serial_conn.readline().decode("cp1252").strip()
                    dev = self.parse_command(data)
                    all_devices = self.device_var.get() == "ALL DEVICES"
                    if dev is not None and (
                        dev == self.device_var.get() or all_devices
                    ):
                        date, temp = self.data[dev][-1]
                        dev_stamp = f" {dev}" if all_devices else ""
                        self.log_message(
                            f"[{date.isoformat()}{dev_stamp}] Temperature: {temp} °C",
                            date=False,
                        )
            except Exception as e:
                err = traceback.format_exception_only(type(e), e)[0].strip()
                self.log_message(f"Error reading data: {err}")
                self.is_running = False

    def parse_command(self, command):
        match = CMD_RE.match(command)
        if match:
            device, temperature, _precision = match.groups()
            if device not in self.data:
                self.add_device(device)
            self.data[device].append((datetime.now(), float(temperature)))
            return device
        return None

    def log_message(self, message, date=True):
        if date:
            message = f"[{datetime.now().isoformat()}] {message}"

        self.console.config(state="normal")
        self.console.insert(tk.END, message + "\n")
        self.console.see(tk.END)
        self.console.config(state="disabled")

    def on_close(self):
        self.is_running = False
        if hasattr(self, "thread"):
            self.thread.join()
        if hasattr(self, "serial_conn") and self.serial_conn.is_open:
            self.serial_conn.close()
        self.root.destroy()


def main(port, baud_rate):
    root = tk.Tk()
    app = UARTTemperatureReceiver(root, port, baud_rate)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop()


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("port", type=str, nargs="?", default="/dev/ttyUSB0")
    arg_parser.add_argument("--baud-rate", type=int, default=9600)
    args = arg_parser.parse_args()
    main(args.port, args.baud_rate)
