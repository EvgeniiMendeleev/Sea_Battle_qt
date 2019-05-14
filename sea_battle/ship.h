#ifndef SHIP_H
#define SHIP_H
#include <QMouseEvent>
#include <QObject>
#include <QGraphicsItem>
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <sys/shm.h>
#include <stdint.h>

using namespace std;

class Ship: public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit Ship(int16_t TypeOfShip, int16_t ID, int memID, QObject* parent = 0);

    void set_x0(int x);     //Устанавливает координату X0 корабля на сцене.
    void set_y0(int y);     //Устанавливает координату Y0 корабля на сцене.
    int get_x();            //Получить координату X на сетке. Возвращает -1 в случае, если корабль расположен не на сетке.
    int get_y();            //Получить координату Y на сетке. Возвращает -1 в случае, если корабль расположен не на сетке.
    bool checkPlace(int16_t* table, int i0, int j0);                  //Возвращает true, если корабль можно расположить в данной области.
    bool onTable();         //Функция для определения, находится ли корабль на сетке или нет.
    bool aroundShip(int16_t* table, int i0, int j0, int k, int l);    //Функция для проверки на рядом стоящие корабли.
    void clearFromShip();   //Функция для очистки информации в буфере разделяемой памяти о расположении корабля в случае его перестановки.
    bool Horisontal();      //Функция для определения: горизонтален ли корабль на поле.

    ~Ship();
signals:

private:
    QRectF boundingRect() const;    //Функция для создания прямоугольника, чтобы навесить текстуру корабля.
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);     //Функция отрисовки текстуры корабля.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);           //Функция для перемещения корабля мышкой.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);          //Функция для захвата корабля мышкой.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);        //Функция для расположения корабля на сцене и игровой сетке.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);    //Функция для переворота корабля.

    int16_t typeOfShip;                 //Тип корабля.
    int16_t shipID;                     //Дескриптор корабля для его обработки на сервере.
    QPixmap TextureOfShip;              //Текстура корабля.
    int width;                          //Ширина текстуры корабля.
    int height = 42;                    //Высота текстуры корабля.
    int x, y;                           //Координаты на сетке.
    int x0, y0;                         //Координаты появления корабля.

    bool isHorisontal = true;           /*
                                         * Переменная, отвечающая за состояние корабля в пространстве:
                                         * горизонтален он или вертикален.
                                        */

    bool onPlace = false;               //Переменная, отвечающая за состояние на сетке: положен на игровую сетку или нет.
    int centerX, centerY;               //Для корректировки захвата корабля.
    int memId;                          //Дескриптор разделяемой памяти с информацией о расположении кораблей.

public slots:

};

#endif // SHIP_H
