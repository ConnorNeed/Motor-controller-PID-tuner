import sys
import serial
import serial.tools.list_ports
from PyQt6.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel, QPushButton, QComboBox
from PyQt6.QtCore import QThread, pyqtSignal, QDateTime
import pyqtgraph as pg

class SerialReader(QThread):
    data_received = pyqtSignal(str)

    def __init__(self, port, baudrate=115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.running = True

    def run(self):
        try:
            ser = serial.Serial(self.port, self.baudrate, timeout=1)
            while self.running:
                if ser.in_waiting:
                    line = ser.readline().decode('utf-8').strip()
                    self.data_received.emit(line)
        except serial.SerialException as e:
            print(f"Serial error: {e}")

    def stop(self):
        self.running = False
        self.quit()
        self.wait()

class SerialMonitor(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.serial_thread = None
        self.target_data = []
        self.velocity_data = []
        self.time_data = []
        self.best_error_data = []
        self.target_val = 0.0

    def initUI(self):
        layout = QVBoxLayout()
        
        self.port_selector = QComboBox()
        self.refresh_ports()
        layout.addWidget(self.port_selector)
        
        self.start_button = QPushButton("Start")
        self.start_button.clicked.connect(self.start_serial)
        layout.addWidget(self.start_button)
        
        self.stop_button = QPushButton("Stop")
        self.stop_button.clicked.connect(self.stop_serial)
        self.stop_button.setEnabled(False)
        layout.addWidget(self.stop_button)
        
        self.labels = {}
        fields = ["Generation", "Iteration", "PID", "Target", "Velocity", "LastError", "PWM"]
        for field in fields:
            label = QLabel(f"{field}: N/A")
            layout.addWidget(label)
            self.labels[field] = label
        
        self.vel_target_graph_widget = pg.PlotWidget()
        self.vel_target_graph_widget.setLabel('left', 'Value')
        self.vel_target_graph_widget.setLabel('bottom', 'Time')
        self.vel_target_graph_widget.setTitle("Time vs Target & Velocity")
        self.vel_target_graph_curve_target = self.vel_target_graph_widget.plot([], [], pen='r', symbol='o', name="Target")
        self.vel_target_graph_curve_velocity = self.vel_target_graph_widget.plot([], [], pen='b', symbol='x', name="Velocity")
        layout.addWidget(self.vel_target_graph_widget)

        self.gen_error_graph_widget = pg.PlotWidget()
        self.gen_error_graph_widget.setLabel('left', 'Error')
        self.gen_error_graph_widget.setLabel('bottom', 'Generation')
        self.gen_error_graph_widget.setTitle("Generation vs Best Error")
        self.gen_error_graph_curve = self.gen_error_graph_widget.plot([], [], pen='r', symbol='o')
        layout.addWidget(self.gen_error_graph_widget)

        self.setLayout(layout)
        self.setWindowTitle("Serial Monitor")

    def refresh_ports(self):
        self.port_selector.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.port_selector.addItem(port.device)

    def start_serial(self):
        port = self.port_selector.currentText()
        if port:
            self.serial_thread = SerialReader(port)
            self.serial_thread.data_received.connect(self.update_values)
            self.serial_thread.start()
            self.start_button.setEnabled(False)
            self.stop_button.setEnabled(True)

    def stop_serial(self):
        if self.serial_thread:
            self.serial_thread.stop()
            self.serial_thread = None
            self.start_button.setEnabled(True)
            self.stop_button.setEnabled(False)
        self.clear_data()
    
    def clear_data(self):
        self.target_data.clear()
        self.velocity_data.clear()
        self.time_data.clear()
        self.best_error_data.clear()
        for label in self.labels.values():
            label.setText(f"{label.text().split(':')[0]}: N/A")
        self.vel_target_graph_curve_target.clear()
        self.vel_target_graph_curve_velocity.clear()
        self.gen_error_graph_curve.clear()
        self.gen_error_graph_widget.clear()

    def update_values(self, data):
        print(f"Received data: {data}")
        try:
            data = data.split(')')
            values = data[1].split(':')
            header = values[0].strip()
            value = values[1].strip()
            self.handle_update(header, value)
        except Exception as e:
            print(f"Error processing data: {e}")
    
    def handle_update(self, header, value):
        current_time = QDateTime.currentDateTime().toMSecsSinceEpoch() / 1000.0
        
        if header == "Target":
            self.target_val = float(value)
            self.labels["Target"].setText(f"Target: {value}")
        elif header == "Velocity":
            self.velocity_data.append(float(value))
            self.time_data.append(current_time)
            self.target_data.append(self.target_val)
            self.labels["Velocity"].setText(f"Velocity: {value}")
            self.update_graph()
        elif header == "Generation":
            self.labels["Generation"].setText(f"Generation: {value}")
        elif header == "Iteration":
            self.labels["Iteration"].setText(f"Iteration: {value}")
            self.time_data.clear()
            self.target_data.clear()
            self.velocity_data.clear()
        elif header == "BestError":
            self.best_error_data.append(float(value))
            self.update_graph()
        elif header == "LastError":
            self.labels["LastError"].setText(f"Last Error: {value}")
        elif header == "PID":
            self.labels["PID"].setText(f"PID: {value}")
        elif header == "PWM":
            self.labels["PWM"].setText(f"PWM: {value}")
        else:
            print(f"Unknown header: {header}")
    
    def update_graph(self):
        if self.time_data and self.target_data:
            self.vel_target_graph_curve_target.setData(self.time_data, self.target_data)
        if self.time_data and self.velocity_data:
            self.vel_target_graph_curve_velocity.setData(self.time_data, self.velocity_data)
        
        if self.best_error_data:
            self.gen_error_graph_curve.setData(range(len(self.best_error_data)), self.best_error_data)
            self.gen_error_graph_widget.setXRange(0, len(self.best_error_data))
            self.gen_error_graph_widget.setYRange(min(self.best_error_data), max(self.best_error_data))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SerialMonitor()
    window.show()
    sys.exit(app.exec())