#include "ship.h"
#include <QDebug>

Ship::Ship(int16_t TypeOfShip, int16_t ID, int memID, QObject* parent):QObject(parent), QGraphicsItem()
{
    centerY = 21;
    typeOfShip = TypeOfShip;
    memId = memID;
    shipID = ID;

    //В зависимости от типа корабля создаём объект с определённым набором параметров.
    switch(TypeOfShip)
    {
        case 1:
            width = 31;

            centerX = width / 2;
            TextureOfShip.load(":/img/1ship.png");
            TextureOfShip = TextureOfShip.scaled(width, height, Qt::KeepAspectRatio);
            break;

        case 2:
            width = 62;

            centerX = width / 2;
            TextureOfShip.load(":/img/2ship.png");
            TextureOfShip = TextureOfShip.scaled(width, height, Qt::KeepAspectRatio);
            break;

        case 3:
            width = 93;

           centerX = width / 2;
            TextureOfShip.load(":/img/3ship.png");
            TextureOfShip = TextureOfShip.scaled(width, height, Qt::KeepAspectRatio);
            break;

        case 4:
            width = 124;

            centerX = width / 2;
            TextureOfShip.load(":/img/4ship.jpg");
            TextureOfShip = TextureOfShip.scaled(width, height, Qt::KeepAspectRatio);
            break;
    }
}

Ship::~Ship()
{

}

QRectF Ship::boundingRect() const
{
    return QRectF (0, 0, width, height);
}

void Ship::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawPixmap(0, 0, TextureOfShip, 0, 0, width, height);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void Ship::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    this->setPos(mapToScene(event->pos().x() - centerX, event->pos().y() - centerY));
}

void Ship::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    setZValue(1);

    if(onPlace)
    {
        clearFromShip();
    }

    this->setCursor(QCursor(Qt::ClosedHandCursor));
    Q_UNUSED(event);
}

void Ship::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    double k, m;           //Строка - столбец сетки с кораблями.

    if(isHorisontal)
    {
        m = (this->pos().x() + height / 12 - 250) / 31;
        k = (this->pos().y() + height / 3 - 86) / 30;

        if(k >= 0 && m >= 0)
        {
            int i = k;
            int j = m;

            if((j + typeOfShip - 1 >= 10) || (i >= 10))
            {
                this->setPos(x0, y0);
                this->x = -1;
                this->y = -1;
                onPlace = false;
            }
            else
            {
                int16_t* table = (int16_t*)shmat(memId, 0, 0);

                if(checkPlace(table, i, j))
                {
                    this->setPos(250 + 31 * j + 2, 86 + 30 * i);

                    for(int l = j; l < j + typeOfShip; l++)
                    {
                        table[10 * i + l] = shipID;
                    }

                    this->x = j;//this->pos().x();
                    this->y = i;//this->pos().y();
                    this->onPlace = true;
                }
                else
                {
                    //qDebug() << "Another ship is near!\n";
                    this->setPos(x0, y0);
                    this->x = -1;
                    this->y = -1;
                    onPlace = false;
                }
            }
        }
        else
        {
            this->setPos(x0, y0);
            this->x = -1;
            this->y = -1;
            this->onPlace = false;
        }
    }
    else
    {
        m = (this->pos().x() - height / 2 - 248) / 31;
        k = (this->pos().y() + 18 - 86) / 30;

        if(k >= 0 && m >= 0)
        {
            int i = k;
            int j = m;

            if((i + typeOfShip - 1 >= 10) || (j >= 10))
            {
                this->setPos(x0, y0);
                this->x = -1;
                this->y = -1;
                onPlace = false;
            }
            else
            {
                int16_t* table = (int16_t*)shmat(memId, 0, 0);

                if(checkPlace(table, i, j))
                {
                    this->setPos(248 + 31 * j + 38, 86 + 30 * i);

                    for(int k = i; k < i + typeOfShip; k++)
                    {
                        table[10 * k + j] = shipID;
                    }

                    this->x = j; //this->pos().x() - 31;
                    this->y = i;
                    this->onPlace = true;
                }
                else
                {
                    this->setPos(this->x0, this->y0);
                    this->x = -1;
                    this->y = -1;
                    this->onPlace = false;
                    //qDebug() << "Another ship is near!\n";
                }
            }
        }
        else
        {
            this->setPos(this->x0, this->y0);
            this->x = -1;
            this->y = -1;
            this->onPlace = false;
        }
    }

   /* qDebug() << "Type: " << typeOfShip;
    qDebug() << "xSHIP = " << this->x;
    qDebug() << "ySHIP = " << this->y;
    qDebug() << "\n";*/

    setZValue(0);
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event);
}

void Ship::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if(onPlace)
    {
        clearFromShip();
    }

    if(isHorisontal)
    {
        setZValue(1);
        setRotation(90);
        isHorisontal = false;
    }
    else
    {
        setZValue(0);
        setRotation(0);
        isHorisontal = true;
    }

    Q_UNUSED(event);
}

bool Ship::checkPlace(int16_t* table, int i0, int j0)
{
    //Определяем в каком месте сетки располагается корабль в зависимости от положения в пространстве.
    if(isHorisontal)
    {
        if(j0 - 1 < 0 && i0 - 1 < 0)    //Верхний левый угол
        {
            if(!aroundShip(table, i0, j0, i0 + 1, j0 + typeOfShip)) return false;
        }
        else if(j0 + typeOfShip >= 10 && i0 - 1 < 0)    //Верхний правый угол
        {
            if(!aroundShip(table, i0, j0 - 1, i0 + 1, j0 + typeOfShip - 1)) return false;
        }
        else if(i0 - 1 < 0)    //Верх
        {
            if(!aroundShip(table, i0, j0 - 1, i0 + 1, j0 + typeOfShip)) return false;
        }
        else if(j0 - 1 < 0 && i0 + 1 >= 10) //Нижний левый угол
        {
            if(!aroundShip(table, i0 - 1, j0, i0, j0 + typeOfShip)) return false;
        }
        else if(j0 + typeOfShip >= 10 && i0 + 1 >= 10) //Нижний правый угол
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0, j0 + typeOfShip - 1)) return false;
        }
        else if(i0 + 1 >= 10) //Низ
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0, j0 + typeOfShip)) return false;
        }
        else if(j0 - 1 < 0) //Сбоку слева
        {
            if(!aroundShip(table, i0 - 1, j0, i0 + 1, j0 + typeOfShip)) return false;
        }
        else if(j0 + typeOfShip >= 10) //Сбоку справа
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0 + 1, j0 + typeOfShip - 1)) return false;
        }
        else //Не по краям
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0 + 1, j0 + typeOfShip)) return false;
        }
    }
    else
    {
        if(j0 - 1 < 0 && i0 - 1 < 0)    //Верхний левый угол
        {
            if(!aroundShip(table, i0, j0, i0 + typeOfShip, j0 + 1)) return false;
        }
        else if(j0 + 1 >= 10 && i0 - 1 < 0)    //Верхний правый угол
        {
            if(!aroundShip(table, i0, j0 - 1, i0 + typeOfShip, j0)) return false;
        }
        else if(i0 - 1 < 0)    //Верх
        {
            if(!aroundShip(table, i0, j0 - 1, i0 + typeOfShip, j0 + 1)) return false;
        }
        else if(j0 - 1 < 0 && i0 + typeOfShip >= 10) //Нижний левый угол
        {
            if(!aroundShip(table, i0 - 1, j0, i0 + typeOfShip, j0 + 1)) return false;
        }
        else if(j0 + 1 >= 10 && i0 + typeOfShip >= 10) //Нижний правый угол
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0 + typeOfShip, j0)) return false;
        }
        else if(i0 + typeOfShip >= 10) //Низ
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0 + typeOfShip - 1, j0 + 1)) return false;
        }
        else if(j0 - 1 < 0) //Сбоку слева
        {
            if(!aroundShip(table, i0 - 1, j0, i0 + typeOfShip, j0 + 1)) return false;
        }
        else if(j0 + 1 >= 10) //Сбоку справа
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0 + typeOfShip, j0)) return false;
        }
        else //Не по краям
        {
            if(!aroundShip(table, i0 - 1, j0 - 1, i0 + typeOfShip, j0 + 1)) return false;
        }
    }

    return true;
}

bool Ship::onTable()
{
    if(this->onPlace) return true;
    else return false;
}

bool Ship::aroundShip(int16_t* table, int i0, int j0, int k, int l)
{   
    //Проверяем наличие кораблей в данной области.
    // Например, проверим область вокруг двухпалубника с координатами
    // (x,y), (x, y + 1) на наличие какого - либо корабля в области j0 <= j <= l
    // и i0 <= i <= k, когда двухпалубник горизонтален.

    /*
     *       j0              l
     *    ........................
     * i0 ... 0  0  0  0  0  0 ...
     *    ... 0  4  4  4  4  0 ...
     * k  ... 0  0  0  0  0  0 ...
     *    ........................
    */

    for(int i = i0; i <= k; i++)
    {
        for(int j = j0; j <= l; j++)
        {
            if(table[10 * i + j] != 0)
                return false;
        }
    }

    return true;
}

void Ship::clearFromShip()
{
    int16_t* table = (int16_t*)shmat(memId, 0, 0);

    int i0 = y; //(y - 86) / 30;
    int j0 = x; //(x - 250) / 31;

    if(isHorisontal)
    {
        for(int j = j0; j < j0 + typeOfShip; j++)
        {
            table[10 * i0 + j] = 0;
        }
    }
    else
    {
        for(int i = i0; i < i0 + typeOfShip; i++)
        {
            table[10 * i + j0] = 0;
        }
    }
}

bool Ship::Horisontal()
{
    return this->isHorisontal;
}

void Ship::set_x0(int x)
{
    this->x0 = x;
}

void Ship::set_y0(int y)
{
    this->y0 = y;
}

int Ship::get_x()
{
    return this->x;
}

int Ship::get_y()
{
    return this->y;
}
