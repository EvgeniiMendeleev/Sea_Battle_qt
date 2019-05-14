#include "battletable.h"

BattleTable::BattleTable(QObject *parent) : QObject(parent)
{
}

void BattleTable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    /*
     * Здесь мы просто посылаем сигнал о том, что был произведён выстрел,
     * и отправляем координаты выстрела в слот SendFire(), описанный в файле mainwindow.cpp,
     * для дальнейшей отправки на сервер.
    */
   emit fire(event->pos().x() / 24, event->pos().y() / 24);
}

QRectF BattleTable::boundingRect() const
{
    //Формируем квадрат поля противника для вычисления координат.
    return QRectF(0, 0, 240, 240);
}

void BattleTable::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
