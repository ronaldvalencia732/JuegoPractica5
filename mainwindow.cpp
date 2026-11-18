#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QList>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), balaActual(nullptr), turnoJugador(1), tiempoVuelo(0) {
    // Configuramos el tamaño de la ventana principal
    setWindowTitle("Practica 5 - Juego de Estrategia por Turnos");
    resize(920, 620);

    // Creamos el widget central y los layouts para organizar la pantalla y los botones abajo
    QWidget* central = new QWidget(this);
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(central);

    // Creamos la escena de 900x500 donde ocurre la fisica
    escena = new QGraphicsScene(0, 0, 900, 500, this);
    vista = new QGraphicsView(escena, this);
    vista->setRenderHint(QPainter::Antialiasing); // Para que los circulos no se vean pixelados
    layoutPrincipal->addWidget(vista);

    // Barra inferior con los controles del usuario
    QHBoxLayout* layoutControles = new QHBoxLayout();

    lblTurno = new QLabel("Turno: Jugador 1 (Izquierda)", this);
    inputAngulo = new QLineEdit(this);
    inputAngulo->setPlaceholderText("Angulo (ej: 45)");

    inputVelocidad = new QLineEdit(this);
    inputVelocidad->setPlaceholderText("Velocidad (ej: 85)");

    btnDisparar = new QPushButton("Disparar!", this);

    layoutControles->addWidget(lblTurno);
    layoutControles->addWidget(new QLabel("Angulo:", this));
    layoutControles->addWidget(inputAngulo);
    layoutControles->addWidget(new QLabel("Velocidad:", this));
    layoutControles->addWidget(inputVelocidad);
    layoutControles->addWidget(btnDisparar);

    layoutPrincipal->addLayout(layoutControles);
    setCentralWidget(central);

    // Ponemos los bloques y personajes en el mapa
    armarEscenario();

    // Conectamos el boton de disparar con su funcion
    connect(btnDisparar, &QPushButton::clicked, this, &MainWindow::iniciarDisparo);

    // Configuramos el reloj para actualizar la fisica cada 16 milisegundos
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
}

MainWindow::~MainWindow() {}

void MainWindow::armarEscenario() {
    // --- BASE DEL JUGADOR 1 ---
    // Pared izquierda, pared derecha y techo
    escena->addItem(new Estructura(80, 380, 40, 120, 200));
    escena->addItem(new Estructura(200, 380, 40, 120, 200));
    escena->addItem(new Estructura(80, 340, 160, 40, 100));
    escena->addItem(new Rival(145, 470, 1)); // Jugador 1 protegido adentro

    // Cañon izquierdo (un cuadro visual de donde salen las balas)
    escena->addRect(10, 260, 30, 15, QPen(Qt::black), QBrush(Qt::darkBlue));

    // --- BASE DEL JUGADOR 2 ---
    escena->addItem(new Estructura(660, 380, 40, 120, 200));
    escena->addItem(new Estructura(780, 380, 40, 120, 200));
    escena->addItem(new Estructura(660, 340, 160, 40, 100));
    escena->addItem(new Rival(725, 470, 2)); // Jugador 2 protegido adentro

    // Cañon derecho
    escena->addRect(860, 260, 30, 15, QPen(Qt::black), QBrush(Qt::darkRed));
}

void MainWindow::iniciarDisparo() {
    // Si ya hay una bala en el aire, no dejamos disparar otra
    if (balaActual != nullptr) return;

    // Leemos los numeros que escribio el usuario
    bool okAng, okVel;
    double angulo = inputAngulo->text().toDouble(&okAng);
    double velocidad = inputVelocidad->text().toDouble(&okVel);

    // Validamos que haya escrito numeros validos
    if (!okAng || !okVel || velocidad <= 0) {
        QMessageBox::warning(this, "Error", "Por favor ingresa un angulo y velocidad validos.");
        return;
    }

    // Definimos desde donde sale la bala y hacia donde apunta segun el turno
    double xSalida, anguloReal;
    if (turnoJugador == 1) {
        xSalida = 45;          // Sale del cañon izquierdo
        anguloReal = angulo;   // Dispara hacia la derecha
    } else {
        xSalida = 840;         // Sale del cañon derecho
        anguloReal = 180.0 - angulo; // Invertimos el angulo para que dispare hacia la izquierda
    }

    // Creamos la bala con masa 2.0 y la agregamos a la pantalla
    balaActual = new Proyectil(xSalida, 255, velocidad, anguloReal, 2.0);
    escena->addItem(balaActual);

    // Bloqueamos el boton mientras la bala vuela
    btnDisparar->setEnabled(false);
    tiempoVuelo = 0;
    timer->start(16); // Arrancamos la animacion
}

void MainWindow::actualizarJuego() {
    if (!balaActual) return;

    // Avanzamos la fisica en un intervalo pequeño dt = 0.08
    balaActual->mover(0.08);
    tiempoVuelo++;

    // 1. COLISION CONTRA LAS PAREDES DE LA CAJA (Elastica)
    if (balaActual->x() <= 0 || balaActual->x() + 14 >= 900) {
        balaActual->rebotarParedLateral();
    }
    if (balaActual->y() <= 0) {
        balaActual->rebotarTecho();
    }

    // 2. REVISAR SI CHOCO CONTRA UN BLOQUE O CONTRA UN RIVAL
    QList<QGraphicsItem*> colisiones = balaActual->collidingItems();
    for (QGraphicsItem* item : colisiones) {

        // Revisamos si el objeto con el que choco es un Rival
        Rival* rivalTocado = dynamic_cast<Rival*>(item);
        if (rivalTocado != nullptr) {
            // Verificamos que no se haya disparado a si mismo
            if (rivalTocado->getJugador() != turnoJugador) {
                timer->stop();
                QMessageBox::information(this, "Victoria!",
                                         QString("¡El Jugador %1 ha ganado la partida!").arg(turnoJugador));
                close(); // Cerramos el juego al ganar
                return;
            }
        }

        // Revisamos si el objeto con el que choco es una Estructura (muro)
        Estructura* muro = dynamic_cast<Estructura*>(item);
        if (muro != nullptr) {
            // Calculamos la fuerza del impacto y le hacemos daño al bloque
            double momento = balaActual->calcularMomento();
            bool destruido = muro->recibirDano(momento);

            // Hacemos que la bala rebote perdiendo energia (inelastico)
            // Miramos si pego por los lados o por arriba/abajo comparando centros
            double diffX = std::abs((balaActual->x() + 7) - (muro->x() + muro->rect().width() / 2));
            double diffY = std::abs((balaActual->y() + 7) - (muro->y() + muro->rect().height() / 2));

            balaActual->rebotarObstaculo(diffX > diffY);

            // Si la resistencia del bloque llego a 0, lo quitamos de la escena
            if (destruido) {
                escena->removeItem(muro);
                delete muro;
            }
            break; // Salimos del ciclo para no procesar doble choque en el mismo instante
        }
    }

    // 3. CONDICION DE FIN DE TURNO
    // El turno acaba si la bala toca el suelo (y >= 486) o si ya lleva mucho tiempo rebotando
    if (balaActual->y() >= 486 || tiempoVuelo > 500) {
        cambiarTurno();
    }
}

void MainWindow::cambiarTurno() {
    timer->stop(); // Frenamos la animacion

    // Borramos la bala actual de la pantalla y de la memoria
    escena->removeItem(balaActual);
    delete balaActual;
    balaActual = nullptr;

    // Cambiamos de jugador
    if (turnoJugador == 1) {
        turnoJugador = 2;
        lblTurno->setText("Turno: Jugador 2 (Derecha)");
    } else {
        turnoJugador = 1;
        lblTurno->setText("Turno: Jugador 1 (Izquierda)");
    }

    // Volvemos a habilitar el boton para el siguiente disparo
    btnDisparar->setEnabled(true);
}