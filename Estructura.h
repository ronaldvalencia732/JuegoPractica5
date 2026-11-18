#ifndef ESTRUCTURA_H
#define ESTRUCTURA_H

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QPen>

// Esta clase representa cada muro o techo que protege al rival
class Estructura : public QGraphicsRectItem {
private:
    double resistencia; // Aguanta el bloque antes de romperse
    QGraphicsSimpleTextItem* textoVida; // El numerito que se ve encima del bloque

public:
    // Constructor: recibe donde va ubicado, su tamaño y cuanta resistencia tiene
    Estructura(double x, double y, double ancho, double alto, double vidaInicial)
        : QGraphicsRectItem(0, 0, ancho, alto), resistencia(vidaInicial) {

        setPos(x, y); // Lo ubicamos en el mapa
        setBrush(QBrush(Qt::lightGray)); // Le ponemos color gris claro para que se vea como muro
        setPen(QPen(Qt::black, 2)); // Borde negro para que resalte

        // Creamos el texto que muestra la vida y lo centramos mas o menos en el bloque
        textoVida = new QGraphicsSimpleTextItem(QString::number((int)resistencia), this);
        textoVida->setPos(ancho / 2 - 12, alto / 2 - 8);
    }

    // Funcion para quitarle vida cuando le pega una bala
    // Devuelve true si el bloque se destruyo por completo, o false si todavia aguanta
    bool recibirDano(double momentoImpacto) {
        // Definimos una constante para que el daño sea balanceado y no destruya todo de un tiro
        double factorConstante = 0.25;

        // Formula del taller: Daño = factor_constante * masa * velocidad (que ya viene en el momento)
        double dano = factorConstante * momentoImpacto;

        resistencia -= dano; // Le restamos el golpe a la resistencia que tenia

        // Si la resistencia llega a 0 o menos, ya se rompio el bloque
        if (resistencia <= 0) {
            resistencia = 0;
            textoVida->setText("0");
            return true; // Avisamos que hay que borrarlo de la pantalla
        }

        // Si sigue vivo, actualizamos el numerito que se ve encima
        textoVida->setText(QString::number((int)resistencia));
        return false;
    }
};

#endif