#include <QDebug>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    secondWindow(nullptr)
{
  ui->setupUi(this);

  DbConnection* dbConnection = DbConnection::getInstance();
  if (!dbConnection->getDatabase().isOpen()) {
    ui->statusbar->showMessage("Ошибка при подключении к бд");
  } else {
    ui->statusbar->showMessage("Успешное подключение");
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_openEditorBtn_clicked()
{
  if (!secondWindow) {
      secondWindow = new Secondwindow();
  }

  secondWindow->show();
}

