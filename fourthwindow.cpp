#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QSqlRecord>
#include <QSqlField>

#include "fourthwindow.h"
#include "ui_fourthwindow.h"

Fourthwindow::Fourthwindow(QSqlDatabase &db, QString &parsedTableName, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Fourthwindow),
  db(db),
  tableName(parsedTableName)
{
  ui->setupUi(this);
  showTableView();
}

Fourthwindow::~Fourthwindow()
{
  delete ui;
}

void Fourthwindow::showTableView()
{
  if (!db.isOpen()) {
      db.open();
  }

  tableModel = new QSqlTableModel(this, db);
  tableModel->setTable(tableName);
  tableModel->select();

  // Заполняем столбцы и их тип данных
  for (int i = 0; i < tableModel->columnCount(); ++i) {
      columnNames << tableModel->headerData(i, Qt::Horizontal).toString();
      columnTypes << tableModel->record().field(i).type();
  }

  for (int i = 0; i < columnNames.count(); ++i) {
      qDebug() << columnNames[i] << "\n";
      qDebug() << columnTypes[i] << "\n";

  };

  ui->tableView->setModel(tableModel);
}


void Fourthwindow::on_makeProtoFileBtn_clicked()
{
  // Создаем строку, содержащую представление прото файла
    QString protoFileContent = "syntax = \"proto3\";\n\n";
    int fieldNumber = 1;
    protoFileContent += "message TableData {\n";

    for (int i = 0; i < columnNames.size(); ++i) {
        QString fieldType;
        switch (columnTypes[i]) {
            case QVariant::Int:
                fieldType = "int32";
                break;
            case QVariant::Double:
                fieldType = "double";
                break;
            case QVariant::String:
                fieldType = "string";
                break;
            default:
                fieldType = "string";
        }

        protoFileContent += "    " + fieldType + " " + columnNames[i] + " = " + QString::number(i+1) + ";\n";
    }

  protoFileContent += "}\n\n";

  protoFileContent += "message TableDatas {\n";
  protoFileContent += "    repeated TableData tableData = " + QString::number(fieldNumber++) + ";\n";
  protoFileContent += "}";

  // Записываем эту строку в прото файл
  QFile protoFile("tableData.proto");
  if (protoFile.open(QIODevice::WriteOnly)) {
      QTextStream out(&protoFile);
      out << protoFileContent;
      protoFile.close();
  } else {
      qDebug() << "Не удалось открыть файл";
      return;
  };

  // Запускаем protoc для компиляции .proto файла
  QProcess process;
  QStringList arguments;
  arguments << "--cpp_out=." << "tableData.proto";

  process.start("protoc", arguments);

  if (!process.waitForFinished()) {
      qDebug() << "Ошибка при выполнении protoc:" << process.errorString();
      ui->statusLabel->setText("Файл .proto не был сгенерирован");
  } else {
      qDebug() << "protoc завершился успешно.";
      qDebug() << process.readAllStandardOutput();
      ui->statusLabel->setText("Файл .proto был успешно сгенерирован");
  }

  // Запускаем скрипт, который обновит про файл и добавит два сгенерированных pb файла в проект
      QProcess scriptProcess;
      scriptProcess.start("/bin/bash", QStringList() << "/home/installer/manager-cmake/update_pro_file.sh");

      if (!scriptProcess.waitForFinished()) {
          qDebug() << "Ошибка при выполнении скрипта:" << scriptProcess.errorString();
          ui->statusLabel->setText("Скрипт не был выполнен");
      } else {
          qDebug() << "Скрипт успешно выполнен.";
          qDebug() << scriptProcess.readAllStandardOutput();
          ui->statusLabel->setText("Файлы .pro успешно обновлены");
      }
}
