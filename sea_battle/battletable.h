#ifndef BATTLETABLE_H
#define BATTLETABLE_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <stdint.h>

using namespace std;

class BattleTable : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit BattleTable(QObject *parent = 0);

private:
    QRectF boundingRect() const;        /*
                                         * Функция отрисовывает квадрат, по которому будет происходить
                                         * выстрел, для отправки координат на сервер.
                                        */
    void mousePressEvent(QGraphicsSceneMouseEvent* event);  //Функция, по щелчку мыши которая вычисляет координаты выстрела в вражеское поле.
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

signals:
    void fire(int16_t x, int16_t y); //Сигнал отправляет координаты x и y в слот SendFire(), описанный в mainwindow.cpp, для отправки на сервер.

public slots:

private slots:
};

#endif // BATTLETABLE_H
