#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "Proyectil.h"
#include "Estructura.h"
#include "Rival.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QGraphicsScene* escena;      // El mundo donde se dibujan las cosas
    QGraphicsView* vista;        // La camara que nos deja ver la escena
    QTimer* timer;               // El reloj que hace que el juego se anime
    Proyectil* balaActual;       // Puntero a la bala que esta volando en este momento

    // Controles para que el jugador escriba y dispare
    QLineEdit* inputAngulo;
    QLineEdit* inputVelocidad;
    QPushButton* btnDisparar;
    QLabel* lblTurno;

    int turnoJugador;            // 1 para Jugador 1, 2 para Jugador 2
    int tiempoVuelo;             // Contador para evitar que un turno dure infinito rebotando

    void armarEscenario();       // Metodo para poner los muros, cañones y rivales
    void cambiarTurno();         // Metodo para pasarle el control al otro jugador

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void iniciarDisparo();       // Se ejecuta al darle clic al boton Disparar
    void actualizarJuego();      // Se ejecuta para mover la bala y ver choques
};

#endif