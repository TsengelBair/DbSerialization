#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>

#include "thirdwindow.h"

namespace Ui {
  class Secondwindow;
}

class Secondwindow : public QWidget
{
    Q_OBJECT

  public:
    explicit Secondwindow(QSqlDatabase &db, QWidget *parent = nullptr);
    ~Secondwindow();

  private slots:
    void onDbBtnClicked();

  private:
    Ui::Secondwindow *ui;
    QSqlDatabase db;
    QVector<QString>dbsNames;
    Thirdwindow* thirdWindow;

    void showDbs();
};

#endif // SECONDWINDOW_H
