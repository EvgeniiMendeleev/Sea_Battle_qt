#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QPixmap>
#include <QDebug>
#include <QVector>
#include <QMessageBox>
#include <QTimer>
#include <sys/shm.h>
#include <battletable.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

//--Библиотеки для работы с сокетами Беркли--
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
//-------------------------------------------

#include <ship.h>

using namespace std;

namespace Ui {
class MainWindow;
}

enum ResultOfShot {not_hit,
                   hit,
                   kill};   //Результат выстрела

enum Msg_type {result_of_shot,
               enemy_shot,
               state_for_client,
               error};      //Тип сообщений, отправляемых с сервера клиенту.

enum states {WaitingOfReadyPlayer,
             WaitingOfConnection,
             Win,
             Lose,
             PlacingShips,
             Battle};         //Состояния клиента, для отображения соответствующих сообщений на экране.

struct Shot         //Выстрел, отправляемый на сервер.
{
    int16_t PosX;
    int16_t PosY;
};

struct Message      //Сообщение, получаемое с сервера.
{
    int16_t type;
    int16_t Result;
    int16_t PosX;
    int16_t PosY;
};

struct DotsAroundShip
{
    int16_t type;
    int16_t Result;
    int16_t CountOfDots;
    Shot CoorDots[14];
};

struct StateForClient   //Сообщение о состоянии клиента для вывода промежуточных окон.
{
    int16_t type;
    int16_t state;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     void Preparing_for_Battle();                                //Функция для отображения фрейма с нераставленными кораблями и полем.
     void Main_Menu_off();                                       //Функция для скрытия кадра главного меню.
     void Main_Menu_on();                                        //Функция для отображения кадра главного меню.
     void BATTLE();                                              //Функция для отображения кадра с битвой.
     Message* convertToMessage(int16_t* buffer);                 //Функция для конвертации данных, отпраленных с сервера, в структуру для удобства работы.
     DotsAroundShip* convertToDAS(int16_t* buffer);
private slots:
    void on_Connection_clicked();                //Кнопка подключения в главном меню.
    void on_BattleButton_clicked();              //Кнопка готовности на этапе расстановки кораблей.
    void SendFire(int16_t x, int16_t y);         //Отправка координат выстрела серверу.
    void ReadFromServer();                       //Чтение данных с сокета, на который пришли данные с сервера.

private:
    Ui::MainWindow *ui;     //Главное меню.
    int ClientSocket;       //Сокет клиента, с помощью которого будет общение с сервером.

    QTimer* MyTimer;        //Таймер, по истечению которого будут читаться данные с сокета.

    //---Переменные для отображения какого - либо фрейма---
    QPixmap frame;
    QGraphicsScene* scene;
    //-----------------------------------------------------

    //---Картинки для отображения результата выстрела-----
    QPixmap BUNG;               //Попал
    QPixmap NotBUNG;            //Молоко
    //----------------------------------------------------

    QVector<Ship*> MyShips; //Вектор всех кораблей (всего их 10)
    //i = 0 - четырёхпалубник
    //i = 1-2 - трёхпалубники
    //i = 3-5 - двухпалубники
    //i = 6-9 - однопалубники

    //----Переменные для работы с таблицей кораблей----
    //--------(используется разделяемая память)--------
    /*
     * Используется разделяемая память из тех соображений, что
     * не только данному процессу нужна информация о кораблях,
     * чтобы потом отправить их на сервер, но и процессу, описанному в файле
     * ship.cpp в функции mouseReleaseEvent(), чтобы выполнить проверку на размещение корабля
     * в данные клетки. Также помогает избежать ошибок при приёме данных на сервер.
    */
    int memID;
    int16_t* table;
    //-------------------------------------------------

    BattleTable* Table;        //Поле, на котором вычисляются координаты выстрела для противника.
    states StateOfClient;      //Переменная для хранения состояния клиента.
};

#endif // MAINWINDOW_H
