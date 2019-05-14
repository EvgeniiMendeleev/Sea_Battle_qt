#include "mainwindow.h"
#include "ui_mainwindow.h"

//Old official size of window: width = 580, height = 420
//New official size of window: width = 573, height = 415
//The newest than new official size of window: width = 580, height = 415

#define WidthOfFrame   580
#define HeightOfFrame 415

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //------Устанавливаем начальные параметры для главного окна и сцены------
    setFixedSize(QSize(WidthOfFrame, HeightOfFrame));

    ui->Frame->setFixedSize(QSize(WidthOfFrame, HeightOfFrame));
    ui->Frame->hide();
    ui->Frame->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    ui->Frame->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    ui->BattleButton->hide();
    //-----------------------------------------------------------------------

    //---Загружаем заранее картинки с попаданием и промахом---
    BUNG.load(":/img/красный-крестик.png");
    BUNG = BUNG.scaled(24, 24);

    NotBUNG.load(":/img/BlackDot.png");
    NotBUNG = NotBUNG.scaled(24, 24);
    //--------------------------------------------------------

    //Создаём сцену, на которой будет отображаться какой - либо фрейм.
    scene = new QGraphicsScene();

    //---Устанавливаем интервал таймера---
    MyTimer = new QTimer;
    MyTimer->setInterval(50);
    //------------------------------------
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Connection_clicked()
{
   //----Для начала получаем IP-адрес хоста, к которому хотим подключиться.---
   QString IPaddress = ui->String_For_IPaddress->text();

    if(IPaddress == NULL)
    {
        qDebug() << "No string";
        return;
    }

    QByteArray tmp = IPaddress.toLatin1();
    const char* IP = tmp.data();
    //-------------------------------------------------------------------------

    ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //Создаём сокет для общения с сервером.

    //------Если создание прошло неуспешно, то выводим сообщение.------
    if(ClientSocket < 0)
    {
        qDebug() << "Error with socket()";
    }
    //-----------------------------------------------------------------

    //------Формируем структуру с данным о хосте, к которому хотим подключиться.------
    struct sockaddr_in ServerAddr;

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(25567);
    inet_pton(AF_INET, IP, &(ServerAddr.sin_addr));
    //--------------------------------------------------------------------------------

    //---------------------------Пытаемся подключиться к серверу---------------------------
    if(::connect(ClientSocket, (struct sockaddr*) (&ServerAddr), sizeof(ServerAddr)) != -1)
    {
        /*
         * Подключаем сигнал истечения времени к ReadFromServer(), чтобы читать данные с сокета,
         * и делаем сокет неблокируемым, чтобы не подвисал клиент.
        */
        connect(MyTimer, &QTimer::timeout, this, &MainWindow::ReadFromServer);
        fcntl(ClientSocket, F_SETFL, O_NONBLOCK);

        MyTimer->start(); //Запускаем таймер.
    }
    else
    {
        QMessageBox::information(this, "Ошибка", "Введён неправильный адрес или сервер не запущен");
    }
    //-------------------------------------------------------------------------------------
}

void MainWindow::Preparing_for_Battle()
{
    //-----------------Загружаем бэкграунд и подгоняем под размер фрейма.-----------------
    frame.load(":/img/PreparingForBattle.png");
    frame = frame.scaled(ui->Frame->geometry().width(), ui->Frame->geometry().height());
    scene->addPixmap(frame);
    //------------------------------------------------------------------------------------

    //-------Выделяем память для работы с сеткой кораблей и обрабатываем ошибку.-------
    memID = shmget(IPC_PRIVATE, 100 * sizeof(int16_t), 0600|IPC_CREAT|IPC_EXCL);

    if(memID < 0)
    {
        qDebug() << "Error with shmget()";
    }
    //---------------------------------------------------------------------------------

    //---Изначально инициализируем массив нулями, обозначающие, что в клетке ничего нету.---
    table = (int16_t*)shmat(memID, 0, 0);

    for(int i = 0; i < 100; i++)
    {
        table[i] = 0;
    }
    //--------------------------------------------------------------------------------------

    //Отображаем все корабли. Всего их 10:
    //1 - четырёхпалубник
    //2 - трёхпалубника
    //3 - двухпалубников
    //4 - однопалубников
    MyShips.resize(10);

    //Отображаем четырёхпалубник
    MyShips[0] = new Ship(4, 10, memID);
    MyShips[0]->set_x0(28); //10
    MyShips[0]->set_y0(149); //92
    MyShips[0]->setPos(28, 149);
    scene->addItem(MyShips[0]);

    //Отображаем трёхпалубники
    for(int i = 1; i < 3; i++)
    {
        MyShips[i] = new Ship(3, 7 + i ,memID);
        MyShips[i]->set_x0(28 + (i - 1) * 100);
        MyShips[i]->set_y0(196);
        MyShips[i]->setPos(28 + (i - 1) * 100, 196);
        scene->addItem(MyShips[i]);
    }

    //Отображаем двухпалубники
    for(int i = 3; i < 6; i++)
    {
        MyShips[i] = new Ship(2, 2 + i ,memID);
        MyShips[i]->set_x0(28 + (i - 3) * 70);
        MyShips[i]->set_y0(243);
        MyShips[i]->setPos(28 + (i - 3) * 70, 243);
        scene->addItem(MyShips[i]);
    }

    //Отображаем однопалубники
    for(int i = 6; i < 10; i++)
    {
        MyShips[i] = new Ship(1, i - 5, memID);
        MyShips[i]->set_x0(28 + (i - 6) * 40);
        MyShips[i]->set_y0(290);
        MyShips[i]->setPos(28 + (i - 6) * 40, 290);
        scene->addItem(MyShips[i]);
    }

    ui->BattleButton->show();         //Отображаем кнопку "В бой!".

    //Отображаем фрейм с подготовкой к битве.
    ui->Frame->setScene(scene);
    ui->Frame->show();
}

void MainWindow::Main_Menu_off()
{
    ui->BackgroundForMainMenu->hide();
    ui->String_For_IPaddress->hide();
    ui->Connection->hide();
    ui->Name->hide();
}

void MainWindow::on_BattleButton_clicked()
{
    //----Проверяем наличие всех кораблей на сетке.----
    for(int i = 0; i < 10; i++)
    {
        if(!MyShips[i]->onTable())
        {
            QMessageBox::information(this, "Ошибка", "Не все корабли расставлены на поле");
            return;
        }
    }
    //-------------------------------------------------

    for(int i = 0; i < 10; i++)
    {
        MyShips[i]->hide();
    }

    send(ClientSocket, table, 100 * sizeof(int16_t), MSG_NOSIGNAL);     //Отправляем расположение кораблей на сервер.
    shmctl(memID, IPC_RMID, 0);

    ui->BattleButton->hide();
}

void MainWindow::BATTLE()
{
    //Загружаем бэкграунд для отображения "Моего флота" и "Флота противника".
    frame.load(":/img/BATTLEFIELD.jpg");
    frame = frame.scaled(ui->Frame->geometry().width(), ui->Frame->geometry().height());
    scene->addPixmap(frame);

    //Отображаем корабли на своём поле.
    for(int i = 0; i < 10; i++)
    {
        QPixmap ship;

        if(i == 0)
        {
           ship.load(":/img/4ship.jpg");
           ship = ship.scaled(98, 28);
        }
        else if((i >= 1) && (i <= 2))
        {
            ship.load(":/img/3ship.png");
            ship = ship.scaled(75, 28);
        }
        else if(i >= 3 && i <= 5)
        {
            ship.load(":/img/2ship.png");
            ship = ship.scaled(51, 28);
        }
        else if(i >= 6 && i <= 9)
        {
            ship.load(":/img/1ship.png");
            ship = ship.scaled(24, 30);
        }

        QGraphicsPixmapItem* item = scene->addPixmap(ship);

        if(MyShips[i]->Horisontal())
        {
            item->setPos(20 + 24 * MyShips[i]->get_x(), 103 + 23 * MyShips[i]->get_y());
        }
        else
        {
            item->setRotation(90);
            item->setPos(23 + 24 * MyShips[i]->get_x() + 26, 103 + 23 * MyShips[i]->get_y());
        }
    }

    for(int i = 0; i < 10; i++)
    {
        delete MyShips[i];
    }

    //----Готовим поле. по которому будут производиться выстрелы.----
    Table = new BattleTable();
    Table->setPos(322, 102);
    connect(Table, &BattleTable::fire, this, &MainWindow::SendFire);
    scene->addItem(Table);
    //---------------------------------------------------------------

    ui->Frame->setScene(scene);
    ui->Frame->show();
}

void MainWindow::SendFire(int16_t x, int16_t y)
{
    //Здесь мы просто формируем структуру Shot для отправки выстрела на сервер
    Shot InfoForServer;
    InfoForServer.PosX = x;
    InfoForServer.PosY = y;

    send(ClientSocket, &InfoForServer, sizeof(InfoForServer), MSG_NOSIGNAL);
}

void MainWindow::ReadFromServer()
{
    int16_t buffer[31];

    if(recv(ClientSocket, buffer, sizeof(int16_t), MSG_NOSIGNAL) > 0)
    {
        Msg_type type = static_cast<Msg_type>(buffer[0]);

        if(type == state_for_client)
        {
            recv(ClientSocket, &buffer[1], sizeof(int16_t), MSG_NOSIGNAL);
            StateOfClient = static_cast<states>(buffer[1]);

            qDebug() << "state = " << StateOfClient;

            if(StateOfClient == WaitingOfConnection)
            {
                //Отключаем главное меню, чтобы перейти к подготовке к битве
                Main_Menu_off();

                frame.load(":/img/WaitingOfConnection.jpg");
                frame = frame.scaled(ui->Frame->geometry().width(), ui->Frame->geometry().height());
                scene->addPixmap(frame);

                ui->Frame->setScene(scene);
                ui->Frame->show();
            }
            else if(StateOfClient == PlacingShips)
            {
                Preparing_for_Battle();
            }
            else if(StateOfClient == WaitingOfReadyPlayer)
            {
                frame.load(":/img/WaitingOfReadyPlayer.jpg");
                frame = frame.scaled(ui->Frame->geometry().width(), ui->Frame->geometry().height());
                scene->addPixmap(frame);

                ui->Frame->setScene(scene);
                ui->Frame->show();
            }
            else if(StateOfClient == Battle)
            {
                BATTLE();
            }
            else if(StateOfClient == Win)
            {
                frame.load(":/img/Winner.jpg");
                frame = frame.scaled(ui->Frame->geometry().width(), ui->Frame->geometry().height());
                scene->addPixmap(frame);

                ui->Frame->setScene(scene);
                ui->Frame->show();

                delete Table;
            }
            else if(StateOfClient == Lose)
            {
                frame.load(":/img/Loser.jpg");
                frame = frame.scaled(ui->Frame->geometry().width(), ui->Frame->geometry().height());
                scene->addPixmap(frame);

                ui->Frame->setScene(scene);
                ui->Frame->show();

                delete Table;
            }
        }
        else if(type == result_of_shot)
        {
            recv(ClientSocket, &buffer[1], sizeof(int16_t), MSG_NOSIGNAL);
            ResultOfShot Result = static_cast<ResultOfShot>(buffer[1]);

            if(Result == not_hit)
            {
                recv(ClientSocket, &buffer[2], sizeof(Message) - 2 * sizeof(int16_t), MSG_NOSIGNAL);

                Message* MsgFromServer = convertToMessage(buffer);

                int16_t PosX = MsgFromServer->PosX;
                int16_t PosY = MsgFromServer->PosY;

                QGraphicsPixmapItem* item = scene->addPixmap(NotBUNG);
                item->setPos(322 + 24 * PosX, 102 + 24 * PosY);

                delete MsgFromServer;
            }
            else if(Result == hit)
            {
                recv(ClientSocket, &buffer[2], sizeof(Message) - 2 * sizeof(int16_t), MSG_NOSIGNAL);

                Message* MsgFromServer = convertToMessage(buffer);

                int16_t PosX = MsgFromServer->PosX;
                int16_t PosY = MsgFromServer->PosY;

                QGraphicsPixmapItem* item = scene->addPixmap(BUNG);
                item->setPos(322 + 24 * PosX, 102 + 24 * PosY);

                delete MsgFromServer;
            }
            else if(Result == kill)
            {
                recv(ClientSocket, &buffer[2], sizeof(DotsAroundShip) - 2 * sizeof(int16_t), MSG_NOSIGNAL);
                DotsAroundShip* Dots = convertToDAS(buffer);

                for(int i = 0; i < Dots->CountOfDots; i++)
                {
                    QGraphicsPixmapItem* item = scene->addPixmap(NotBUNG);
                    item->setPos(322 + 24 * Dots->CoorDots[i].PosX, 102 + 24 * Dots->CoorDots[i].PosY);
                }

                delete Dots;
            }
        }
        else if(type == enemy_shot)
        {
            recv(ClientSocket, &buffer[1], sizeof(int16_t), MSG_NOSIGNAL);
            ResultOfShot Result = static_cast<ResultOfShot>(buffer[1]);

            if(Result == not_hit)
            {
                recv(ClientSocket, &buffer[2], sizeof(Message) - 2 * sizeof(int16_t), MSG_NOSIGNAL);
                Message* MsgFromServer = convertToMessage(buffer);

                int16_t PosX = MsgFromServer->PosX;
                int16_t PosY = MsgFromServer->PosY;

                QGraphicsPixmapItem* item = scene->addPixmap(NotBUNG);
                item->setPos(20 + 24 * PosX, 102 + 24 * PosY);

                delete MsgFromServer;
            }
            else if(Result == hit)
            {
                recv(ClientSocket, &buffer[2], sizeof(Message) - 2 * sizeof(int16_t), MSG_NOSIGNAL);
                Message* MsgFromServer = convertToMessage(buffer);

                int16_t PosX = MsgFromServer->PosX;
                int16_t PosY = MsgFromServer->PosY;

                QGraphicsPixmapItem* item = scene->addPixmap(BUNG);
                item->setPos(20 + 24 * PosX, 102 + 24 * PosY);

                delete MsgFromServer;
            }
            else if(Result == kill)
            {
                recv(ClientSocket, &buffer[2], sizeof(DotsAroundShip) - 2 * sizeof(int16_t), MSG_NOSIGNAL);
                DotsAroundShip* Dots = convertToDAS(buffer);

                for(int i = 0; i < Dots->CountOfDots; i++)
                {
                    QGraphicsPixmapItem* item = scene->addPixmap(NotBUNG);
                    item->setPos(20 + 24 * Dots->CoorDots[i].PosX, 102 + 24 * Dots->CoorDots[i].PosY);
                }

                delete Dots;
            }
        }
    }
}

DotsAroundShip* MainWindow::convertToDAS(int16_t* buffer)
{
    DotsAroundShip* NewMessage = new DotsAroundShip;

    NewMessage->type = buffer[0];
    NewMessage->Result = buffer[1];
    NewMessage->CountOfDots = buffer[2];

    int count = 3;

    for(int i = 0; i < NewMessage->CountOfDots; i++)
    {
        NewMessage->CoorDots[i].PosX = buffer[count];
        NewMessage->CoorDots[i].PosY = buffer[count + 1];

        count += 2;
    }

    return NewMessage;
}

Message* MainWindow::convertToMessage(int16_t* buffer)
{
    Message* NewMessage = new Message;

    NewMessage->type = buffer[0];
    NewMessage->Result = buffer[1];
    NewMessage->PosX = buffer[2];
    NewMessage->PosY = buffer[3];

    return NewMessage;
}
