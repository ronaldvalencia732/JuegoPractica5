#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QGraphicsEllipseItem>
#include <QBrush>
#include <cmath>

// Esta clase maneja el movimiento parabolico y los choques de la bala
class Proyectil : public QGraphicsEllipseItem {
private:
    double vx, vy;          // Velocidad en horizontal (X) y vertical (Y)
    double masa;            // Masa de la bala para calcular que tan duro pega
    double gravedad;        // Gravedad que jala la bala hacia abajo
    double restitucion;     // Coeficiente de restitucion (e) para perder energia al chocar con muros

public:
    Proyectil(double x, double y, double velocidadInicial, double anguloGrados, double m)
        : QGraphicsEllipseItem(0, 0, 14, 14), masa(m), gravedad(9.8), restitucion(0.6) {

        setPos(x, y); // Sale desde la posicion del cañon
        setBrush(QBrush(Qt::darkRed)); // Color rojo oscuro para la bala

        // Pasamos el angulo de grados a radianes
        double radianes = anguloGrados * M_PI / 180.0;

        // Sacamos las componentes de velocidad inicial en X y en Y
        vx = velocidadInicial * std::cos(radianes);
        // El eje Y crece hacia abajo, por eso va negativo para que dispare hacia arriba
        vy = -velocidadInicial * std::sin(radianes);
    }

    // Actualiza donde va la bala en cada paso de tiempo (dt)
    void mover(double dt) {
        // La gravedad va aumentando la velocidad hacia abajo
        vy += gravedad * dt;

        // Movemos la bala sumando lo que avanzo en X y en Y
        setPos(x() + vx * dt, y() + vy * dt);
    }

    // Calcula el momento lineal (P = masa * velocidad total) cuando choca
    double calcularMomento() const {
        double velocidadTotal = std::sqrt(vx * vx + vy * vy);
        return masa * velocidadTotal;
    }

    // Choque elastico contra las paredes laterales de la ventana
    void rebotarParedLateral() {
        vx = -vx; // Solo invertimos la direccion horizontal sin perder fuerza
    }

    // Choque elastico contra el techo de la ventana
    void rebotarTecho() {
        vy = -vy; // Invertimos la velocidad vertical sin perder fuerza
    }

    // Choque inelastico contra los bloques de defensa (pierde energia)
    void rebotarObstaculo(bool choqueHorizontal) {
        if (choqueHorizontal) {
            vx = -vx * restitucion; // Rebota en X pero mas lento
        } else {
            vy = -vy * restitucion; // Rebota en Y pero mas lento
        }
    }

    // Para saber que tan rapido se mueve todavia (sirve para acabar el turno si ya casi no se mueve)
    double getRapidez() const {
        return std::sqrt(vx * vx + vy * vy);
    }
};

#endif