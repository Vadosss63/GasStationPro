#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <qthread.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readData();

private slots:
    void on_pushButton_clicked();

signals:
    void dataRead(QString data);
private:
    void connectToPort();

    QSerialPort serialPort;

    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
