#include <QDebug>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    secondWindow(nullptr)
{
  ui->setupUi(this);

  db = QSqlDatabase::addDatabase("QPSQL");
  db.setUserName("postgres");
  db.setHostName("localhost");
  db.setPassword("25944");

  if (!db.open()) {
      qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
      ui->statusbar->showMessage("Ошибка");
  } else {
      qDebug() << "Успешное подключение к базе данных";
      ui->statusbar->showMessage("Подключение установлено");
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_openEditorBtn_clicked()
{
  if (!secondWindow) {
      secondWindow = new Secondwindow(db);
  }
  secondWindow->show();
}

