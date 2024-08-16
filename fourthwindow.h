#ifndef FOURTHWINDOW_H
#define FOURTHWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>

#include "dbconnection.h"

namespace Ui {
  class Fourthwindow;
}

class Fourthwindow : public QWidget
{
    Q_OBJECT

  public:
    explicit Fourthwindow(QString &parsedTableName, QWidget *parent = nullptr);
    ~Fourthwindow();

  private slots:
    void on_makeProtoFileBtn_clicked();

  private:
    Ui::Fourthwindow *ui;
    QString tableName;
    QSqlTableModel* tableModel;

    // Названия столбцов и их тип данных для генерации .proto файла
    QStringList columnNames;
    QList<QVariant::Type> columnTypes;

    void showTableView();
};

#endif // FOURTHWINDOW_H
