#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    // Inicializa todo el entorno grafico de Qt
    QApplication a(argc, argv);

    // Crea y muestra la ventana del juego
    MainWindow w;
    w.show();

    // Mantiene el programa abierto
    return a.exec();
}