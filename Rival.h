#ifndef RIVAL_H
#define RIVAL_H

#include <QGraphicsEllipseItem>
#include <QBrush>

// Clase sencilla para representar al personaje enemigo que esta escondido
class Rival : public QGraphicsEllipseItem {
private:
    int numeroJugador; // Para saber si es el jugador 1 o el jugador 2

public:
    Rival(double x, double y, int jugador) : QGraphicsEllipseItem(0, 0, 30, 30), numeroJugador(jugador) {
        setPos(x, y); // Lo ponemos dentro de su casa

        // Le ponemos color azul al jugador 1 y rojo al jugador 2 para diferenciarlos
        if (jugador == 1) {
            setBrush(QBrush(Qt::blue));
        } else {
            setBrush(QBrush(Qt::red));
        }
    }

    int getJugador() const { return numeroJugador; }
};

#endif