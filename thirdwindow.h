#ifndef THIRDWINDOW_H
#define THIRDWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>

#include "dbconnection.h"
#include "fourthwindow.h"

namespace Ui {
  class Thirdwindow;
}

class Thirdwindow : public QWidget
{
    Q_OBJECT

  public:
    explicit Thirdwindow(QWidget *parent = nullptr);
    ~Thirdwindow();

  private slots:
    void onTableBtnClicked();

  private:
    Ui::Thirdwindow *ui;
    QVector<QString>tableNames;
    Fourthwindow* fourthWindow;

    void showTables();
};

#endif // THIRDWINDOW_H
