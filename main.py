# Ejecutar el .ui con el siguiente comando

import sys
from PyQt5 import QtWidgets
from PyQt5.uic import loadUi
from PyQt5.QtCore import Qt
import subprocess
import threading
import graficas

class ControladorC:
    def __init__(self):
        self.process = None
        pass

    def run(self):
        # Enviar comando a la terminal para ejecutar el script
        #subprocess.run(["g++", "main.cpp", "units/memory.cpp", "units/executeunit.cpp", "units/messagemanagement.cpp", "units/schedulers/scheduler.cpp", "PEs/PE_class.cpp", "PEs/interpreter.cpp", "units/clock.cpp", "units/messagetimer.cpp", "SystemSimulator.cpp", "-o", "main", "-pthread"])  # Compilar el archivo C++
        
        # Ejecutar la funcion execute() de la clase ControladorC por medio de un hilo
        thread = threading.Thread(target=self.execute)
        thread.start()
        
        
        
        pass

    def execute(self):
        # Iniciar el proceso y ejecutarlo en consola
        self.process = subprocess.Popen(["./main"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        # Leer la salida del proceso en tiempo real
        while True:
            output = self.process.stdout.readline()
            if output == "" and self.process.poll() is not None:
                break
            if output:
                print(output.strip())

        
        

    def sendEnterToTerminal(self):
        # Ensure the process is running
        if self.process is None or self.process.poll() is not None:
            print("El proceso no está en ejecución.")
            return
        try:
            if self.process.stdin:
                self.process.stdin.write("\n")  # Send Enter
                self.process.stdin.flush()      # Ensure it is sent immediately
        except BrokenPipeError:
            print("Error: Broken pipe. The process may have terminated.")

    def send1terminal(self):
        # Ensure the process is running
        if self.process is None or self.process.poll() is not None:
            print("El proceso no está en ejecución.")
            return
        try:
            if self.process.stdin:
                self.process.stdin.write("1\n")  # Send Enter
                self.process.stdin.flush()      # Ensure it is sent immediately
        except BrokenPipeError:
            print("Error: Broken pipe. The process may have terminated.")

    def send2terminal(self):
        # Ensure the process is running
        if self.process is None or self.process.poll() is not None:
            print("El proceso no está en ejecución.")
            return
        try:
            if self.process.stdin:
                self.process.stdin.write("2\n")  # Send Enter
                self.process.stdin.flush()      # Ensure it is sent immediately
        except BrokenPipeError:
            print("Error: Broken pipe. The process may have terminated.")


    def graficar1(self):
        # Llamar a la función de graficar desde el módulo graficas
        filename = 'interconnect_data.txt'
        ciclos, mensajes = graficas.load_data(filename)
        graficas.plot_mensajes_vs_ciclos(ciclos, mensajes, "Mensajes")

    def graficar2(self):
        # Llamar a la función de graficar desde el módulo graficas
        filename = 'interconnect_data_bytes.txt'
        ciclos, mensajes = graficas.load_data(filename)
        graficas.plot_mensajes_vs_ciclos(ciclos, mensajes, "Bytes")

    def graficar3(self):
        # Llamar a la función de graficar desde el módulo graficas
        filename = 'interconnect_data_PEs.txt'
        ciclos, mensajes = graficas.load_data(filename)
        graficas.plot_mensajes_vs_ciclos(ciclos, mensajes, "Mensajes por PE")

    

class MyApp(QtWidgets.QMainWindow):
        def __init__(self):
            super(MyApp, self).__init__()
            loadUi("mainwindow.ui", self)
            self.controlador = ControladorC()


            # Conectar el botón a la función
            self.ICButton.clicked.connect(self.controlador.run) 

            # Conectar el evento enter a la función sendEnterToTerminal
            

            self.nextButton.clicked.connect(self.controlador.sendEnterToTerminal)
            self.GRAPHICButton.clicked.connect(self.controlador.graficar1)
            self.GRAPHICButton2.clicked.connect(self.controlador.graficar2)
            self.GRAPHICButton2_2.clicked.connect(self.controlador.graficar3)
            self.PEButton1.clicked.connect(self.controlador.send1terminal)
            self.PEButton2.clicked.connect(self.controlador.send2terminal)
        
        def keyPressEvent(self, event):
            if event.key() == Qt.Key_Return or event.key() == Qt.Key_Enter:
                # Llamar a la función cuando se presiona Enter
                self.controlador.sendEnterToTerminal()

def run_ui():
    app = QtWidgets.QApplication(sys.argv)
    window = MyApp()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    run_ui()