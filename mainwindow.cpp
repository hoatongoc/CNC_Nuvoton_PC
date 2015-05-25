#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "motor_control.h"
#include "QString"
#include "QTimer"
#include "sampleobj.h"
#include <QMessageBox>
#include "math.h"
#include "QFile"
#include "common.h"
#include "QFileDialog"
#include "QTextStream"
#include "QDebug"

hid_device *handle;
short motorSpeed = 0;
char dir_x;
char dir_y;
char isRun;
SampleObj *objtest = new SampleObj();
unsigned char* buffer;
char SystemStatus = WAIT;
bool lock = false;
QStringList commandList;
int lineIndex=0;
char boardStatus = WAIT;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    buffer = objtest->getBuf();
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateGUI()));
    timer->start(33);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::updateGUI() {

    dir_x = buffer[7];
    dir_y = buffer[15];

    objtest->setCurrent_stepX(Convert_u8_u16(buffer[5],buffer[6]));
    objtest->setCurrent_stepY(Convert_u8_u16(buffer[13],buffer[14]));

    boardStatus = buffer[2];



    ui->statusLabel->setText(QString::number(boardStatus));
    ui->XlabelRemain->setText(QString::number(Convert_u8_u16(buffer[3],buffer[4])));
    ui->XlabelCur->setText(QString::number(objtest->getCurrent_stepX()));
    ui->XlabelDir->setText(QString::number(buffer[7]));
    ui->XlabelEnable->setText(QString::number(buffer[8]));
    ui->XlabelSpeed->setText(QString::number(Convert_u8_u16(buffer[9],buffer[10])));

    ui->YlabelRemain->setText(QString::number(Convert_u8_u16(buffer[11],buffer[12])));
    ui->YlabelCur->setText(QString::number(objtest->getCurrent_stepY()));
    ui->YlabelDir->setText(QString::number(buffer[15]));
    ui->YlabelEnable->setText(QString::number(buffer[16]));
    ui->YlabelSpeed->setText(QString::number(Convert_u8_u16(buffer[17],buffer[18])));

    if(boardStatus == NOT_FINISHED) {
        char xEnable = buffer[8];
        char yEnable = buffer[16];

        if(xEnable == PAUSE &&  yEnable == PAUSE) {

            SystemStatus = FINISHED;
            objtest->sendWorkingStatus(FINISHED);
        }
        lock = false;
    }
    else if (boardStatus == FINISHED) {
        SystemStatus =  WAIT;
        objtest->sendWorkingStatus(WAIT);
    }
    else if (boardStatus == WAIT) { // boardStatus == WAIT
        //process command
        if(lock == false) {
        std::vector<GCode> gcode = ProcessCommand();
        SystemStatus = NOT_FINISHED;
        lock = true;
        objtest->processCommand(gcode.at(0).command,gcode.at(0).value,gcode.at(1).value,gcode.at(2).value,0.0,0);
        objtest->sendWorkingStatus(NOT_FINISHED);
        }
    }


}


void MainWindow::on_connBtn_clicked()
{
    isRun = PAUSE;
    handle = hid_open(0x51a, 0x511b, NULL);
        if (handle)
        {


            QMessageBox mess;
            mess.setText("USB CONNECTED");
            mess.exec();

        }
        else
        {
            QMessageBox mess;
            mess.setText("OPEN ERROR");
            mess.exec();


        }
}

void MainWindow::on_dirBtn_clicked()
{

   char dir;
   int res;
   unsigned char buf[65];
   char motor_name;
   if(ui->mNumLineEdit->text() == "X"){
       motor_name = MC_X;
       dir = !dir_x;
   }
   else if(ui->mNumLineEdit->text() == "Y") {
        dir = !dir_y;
       motor_name = MC_Y;
    }

   //handle = hid_open(0x51a, 0x511b, NULL);
   if (!handle)
   {
       QMessageBox mess;
       mess.setText("OPEN ERROR");
       mess.exec();
   }
   else
   {
       buf[0] = 0x0;
       buf[1] = 0x13;
       buf[2] = 0x04;
       buf[3] = CMD_DIR;
       buf[4] = motor_name;
       buf[5] = dir;
       buf[6] = 0x0;
       buf[7] = 0x0;
       buf[8] = 0x0;
       buf[9] = 0x0;

       res = hid_write(handle, buf, 65);
       if (res == -1) {
           QMessageBox mess;
           mess.setText("ERROR HAPPENED");
           mess.exec();
       }
   }
}

void MainWindow::on_runBtn_clicked()
{
   int res;
   unsigned char buf[65];
   if(isRun == RUN) {
       isRun = PAUSE;
       buf[3] = CMD_PAUSE;
   }
   else {
       isRun = RUN;
       buf[3] = CMD_RUN;
   }



   //handle = hid_open(0x51a, 0x511b, NULL);
   if (!handle)
   {
       QMessageBox mess;
       mess.setText("OPEN ERROR");
       mess.exec();
   }
   else
   {
       buf[0] = 0x0;
       buf[1] = 0x13;
       buf[2] = 0x04;
       //buf[3] = 0xAA;
       buf[4] = 0x00;
       buf[5] = isRun;
       buf[6] = 0x0;
       buf[7] = 0x0;
       buf[8] = 0x0;
       buf[9] = 0x0;

       res = hid_write(handle, buf, 65);
       if (res == -1) {
           QMessageBox mess;
           mess.setText("CANT WRITE");
           mess.exec();
       }

       res = hid_read(handle, buf, 65);
       if(res<0) {
           QMessageBox mess;
           mess.setText("CANT READ");
           mess.exec();

       }


   }
}

void MainWindow::on_stepBtn_clicked()
{
    unsigned short value;
    QString str = ui->stepLineEdit->text();
    value = str.toUShort();
    char high =0;
    char low=0;
    Convert_u16_u8(value,high,low);
    int res;
    unsigned char buf[65];

    char motor_name;
    if(ui->mNumLineEdit->text() == "X") motor_name = MC_X;
    else if(ui->mNumLineEdit->text() == "Y") motor_name = MC_Y;

    if (!handle)
    {
        QMessageBox mess;
        mess.setText("OPEN ERROR");
        mess.exec();
    }
    else
    {
        buf[0] = 0x0;
        buf[1] = 0x13;
        buf[2] = 0x04;
        buf[3] = CMD_STEP;
        buf[4] = high;
        buf[5] = low;
        buf[6] = dir_x;
        buf[7] = 0x01;
        buf[8] = 0xF4;
        buf[9] = high;
        buf[10] = low;
        buf[11] = dir_y;
        buf[12] = 0x01;
        buf[13] = 0xF4;

        res = hid_write(handle, buf, 65);
        if (res == -1) {
            QMessageBox mess;
            mess.setText("CANT WRITE");
            mess.exec();
        }

        res = hid_read(handle, buf, 65);
        if(res<0) {
            QMessageBox mess;
            mess.setText("CANT READ");
            mess.exec();

        }
    }
}

void MainWindow::on_speedBtn_clicked()
{
    unsigned short value;
     QString str = ui->speedLineEdit->text();
     value = str.toUShort();
     char high =0;
     char low=0;
    Convert_u16_u8(value,high,low);
    int res;
    unsigned char buf[65];

    char motor_name;
    if(ui->mNumLineEdit->text() == "X") motor_name = MC_X;
    else if(ui->mNumLineEdit->text() == "Y") motor_name = MC_Y;

    if (!handle)
    {
        QMessageBox mess;
        mess.setText("OPEN ERROR");
        mess.exec();
    }
    else
    {
        buf[0] = 0x0;
        buf[1] = 0x13;
        buf[2] = 0x04;
        buf[3] = CMD_SPEED;
        buf[4] = motor_name;
        buf[5] = high;
        buf[6] = low;
        buf[7] = 0x0;
        buf[8] = 0x0;
        buf[9] = 0x0;

        res = hid_write(handle, buf, 65);
        if (res == -1) {
            QMessageBox mess;
            mess.setText("CANT WRITE");
            mess.exec();
        }

        res = hid_read(handle, buf, 65);
        if(res<0) {
            QMessageBox mess;
            mess.setText("CANT READ");
            mess.exec();

        }
    }
}

void MainWindow::on_statusBtn_clicked()
{
        objtest->startTimer();
}


void MainWindow::on_OpenButton_clicked()
{
    //Open dialog
     QString filename=QFileDialog::getOpenFileName(
                 this,
                 tr("Open File"),
                 "C://",
                 "All File (*.*);;Text file (*.txt)"
                 );

   //get the chosen file and read from it
   QFile file(filename);
   if(!file.open(QIODevice::ReadOnly))
       QMessageBox::information(0,"info",file.errorString());

   QTextStream in(&file);
   while (!in.atEnd())
   {
       ui->textEdit->setText(in.readAll());
   }
   file.close();
}

void MainWindow::on_pushButton_clicked()
{
        InitializeCommands(commandList, ui->textEdit->toPlainText());


}

void MainWindow::on_runDrawBtn_clicked()
{

}


void MainWindow::InitializeCommands(QStringList& cList, QString sourceText) {
    cList = sourceText.split(QRegularExpression("[\r\n]"),QString::SkipEmptyParts);
}


std::vector<GCode>MainWindow::ProcessCommand() {

    std::vector<GCode> gCodeList (3);

    for(int i = lineIndex;i<commandList.length();i++) // moi i la 1 line
    {
        lineIndex++;
        if(commandList.at(i).left(1)==";") continue; // new nhu line bat dau = dau ; thi line do la comment, skip qua
        QStringList element = commandList.at(i).split(QRegularExpression("G+|\\sX+|\\sY"),QString::SkipEmptyParts);
        // element la tap hop các thanh phan trong 1 line
        for(int y=0;y<element.length();y++) // check cac thanh phan cua line theo thu tu G X Y
        {
//             QString test = element.at(y);
//             ui->textExtract->append(test);

            switch (y){
            case 0:
            {
                GCode *g = &gCodeList.at(y);
                g->command = "G";
                g->value = element.at(y).toFloat();
                qDebug()<<element.at(y);
                ui->textExtract->append(g->command);
                ui->textExtract->append(QString::number(g->value));
                //command name
                break;
            }
            case 1:
            {
                GCode *g = &gCodeList.at(y);
                g->command = "X";
                g->value = element.at(y).toFloat();
                ui->textExtract->append(g->command);
                ui->textExtract->append(QString::number(g->value));
                // X coor
                qDebug() << element.at(y);
                break;
            }

            case 2: {
                GCode *g = &gCodeList.at(y);
                g->command = "Y";
                g->value = element.at(y).toFloat();
                ui->textExtract->append(g->command);
                ui->textExtract->append(QString::number(g->value));

                 // Y coor
                qDebug()<<element.at(y);
                break;
            }
            }

//         objtest->processCommand(temp_vec.at(0).command, temp_vec.at(0).value, temp_vec.at(1).value, temp_vec.at(2).value, 0.0, i);
       }

        break;
    }
    return gCodeList;
}
