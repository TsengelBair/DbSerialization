#ifndef FOURTHWINDOW_H
#define FOURTHWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>

namespace Ui {
  class Fourthwindow;
}

class Fourthwindow : public QWidget
{
    Q_OBJECT

  public:
    explicit Fourthwindow(QSqlDatabase &db, QString &parsedTableName, QWidget *parent = nullptr);
    ~Fourthwindow();

  private slots:
    void on_makeProtoFileBtn_clicked();

  private:
    Ui::Fourthwindow *ui;
    QSqlDatabase db;
    QString tableName;
    QSqlTableModel* tableModel;

    // Названия столбцов и их тип данных для генерации .proto файла
    QStringList columnNames;
    QList<QVariant::Type> columnTypes;

    void showTableView();
};

#endif // FOURTHWINDOW_H
