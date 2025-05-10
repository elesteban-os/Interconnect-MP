# Ejecutar el .ui con el siguiente comando

import sys
from PyQt5 import QtWidgets
from PyQt5.uic import loadUi
import subprocess
import threading

class ControladorC:
    def __init__(self, text_edit):
        self.text_edit = text_edit
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
        # Iniciar el proceso y permitir interacción con stdin
        self.process = subprocess.Popen(
            ["./main"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        # Leer la salida del proceso en tiempo real (opcional)
        for line in self.process.stdout:
             # Usar QtWidgets.QApplication.processEvents() para actualizar la UI
            QtWidgets.QApplication.processEvents()
            # Agregar el texto al QTextEdit
            self.text_edit.append(line.strip())

    def sendEnterToTerminal(self):
        # Enviar una letra al proceso en ejecución
        if self.process and self.process.stdin:
            self.process.stdin.write("\n")  # Escribir la letra "A"
            self.process.stdin.flush()       # Asegurarse de que se envíe inmediatamente

class MyApp(QtWidgets.QMainWindow):
        def __init__(self):
            super(MyApp, self).__init__()
            loadUi("mainwindow.ui", self)
            self.controlador = ControladorC(self.textEdit)


            # Conectar el botón a la función
            self.uploadButton1.clicked.connect(self.controlador.run) 
            self.nextButton.clicked.connect(self.controlador.sendEnterToTerminal)

def run_ui():
    app = QtWidgets.QApplication(sys.argv)
    window = MyApp()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    run_ui()