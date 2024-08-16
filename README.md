## Подключение к таблице из бд через интерфейс

![1](/img/image.png)

![2](/img/image-1.png)

![3](/img/3.jpg)

![4](/img/4.jpg)

### После нажатия на кнопку Make proto file сработает метод, внутри которого:

1) Создастся строка, хранящая в себе синтаксис прото файла;

2) Создастся прото файл, в который запишется строка из предыдущего этапа 

3) Запустится процесс для компиляции прото файла 

4) Запустится скрипт, который добавит в проект сгенерированные прото файлы, на основе которых и будут сериализовываться данные из таблицы, также в хэдер добавится объявление слота для обработки кнопки Serialize и в cpp реализация (все через bash скрипт)

Метод 

```c++
void Fourthwindow::on_makeProtoFileBtn_clicked()
{
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

  QFile protoFile("tableData.proto");
  if (protoFile.open(QIODevice::WriteOnly)) {
      QTextStream out(&protoFile);
      out << protoFileContent;
      protoFile.close();
  } else {
      qDebug() << "Не удалось открыть файл";
      return;
  };

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
      // bash скрипт
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
```

Bash скрипт 

```bash
#!/bin/bash

PROJECT_DIR="/home/installer/manager-cmake"
CPP_FILE="$PROJECT_DIR/fourthwindow.cpp"
HEADER_FILE="$PROJECT_DIR/fourthwindow.h"
BUILD_DIR="/home/installer/BUILDS/build-manager-cmake-Desktop-Debug"
PB_HEADER="$BUILD_DIR/tableData.pb.h"
PB_SOURCE="$BUILD_DIR/tableData.pb.cc"
HEADER_INCLUDE='#include "tableData.pb.h"'

echo "Checking for generated files in $BUILD_DIR..."

if [ -f "$PB_HEADER" ]; then
    echo "Found $PB_HEADER"
else
    echo "$PB_HEADER not found."
fi

if [ -f "$PB_SOURCE" ]; then
    echo "Found $PB_SOURCE"
else
    echo "$PB_SOURCE not found."
fi

if [ -f "$PB_HEADER" ] && [ -f "$PB_SOURCE" ]; then
    echo "Copying files to project directory..."
    cp "$PB_HEADER" "$PROJECT_DIR"
    cp "$PB_SOURCE" "$PROJECT_DIR"

    if grep -q "tableData.pb.cc" "$PROJECT_DIR/CMakeLists.txt"; then
        echo "tableData.pb.cc already included in CMakeLists.txt"
    else
        echo "Adding tableData.pb.cc to CMakeLists.txt..."
        sed -i '/set(PROJECT_SOURCES/ a\        tableData.pb.cc' "$PROJECT_DIR/CMakeLists.txt"
    fi

    if grep -q "tableData.pb.h" "$PROJECT_DIR/CMakeLists.txt"; then
        echo "tableData.pb.h already included in CMakeLists.txt"
    else
        echo "Adding tableData.pb.h to CMakeLists.txt..."
        sed -i '/set(PROJECT_SOURCES/ a\        tableData.pb.h' "$PROJECT_DIR/CMakeLists.txt"
    fi

    if grep -Fxq "$HEADER_INCLUDE" "$CPP_FILE"; then
        echo "Header already included in $CPP_FILE"
    else
        echo "Adding header to $CPP_FILE..."
        sed -i "1i $HEADER_INCLUDE" "$CPP_FILE"
    fi

    if grep -q "void on_serializeBtn_clicked();" "$HEADER_FILE"; then
        echo "Slot on_serializeBtn_clicked() already declared in $HEADER_FILE"
    else
        echo "Adding slot declaration to $HEADER_FILE..."
        sed -i '/private slots:/a\ \ \ \ void on_serializeBtn_clicked();' "$HEADER_FILE"
    fi

    if grep -q "void MainWindow::on_serializeBtn_clicked()" "$CPP_FILE"; then
        echo "Slot on_serializeBtn_clicked() already implemented in $CPP_FILE"
    else
        echo "Adding slot implementation to $CPP_FILE..."
        cat <<EOL >> "$CPP_FILE"

void Fourthwindow::on_serializeBtn_clicked()
{
    TableDatas tableDatas;

    for (int row = 0; row < tableModel->rowCount(); ++row) {
        QSqlRecord record = tableModel->record(row);

        TableData* tableData = tableDatas.add_tabledata();
        tableData->set_id(record.value("id").toInt());
        tableData->set_name(record.value("name").toString().toStdString());
    }

    std::string serializedData;
    if (!tableDatas.SerializeToString(&serializedData)){
        ui->statusLabel->setText("Не получилось сериализовать");
        return;
    };

    QFile file("tableData.bin");
    if (!file.open(QIODevice::WriteOnly)){
        ui->statusLabel->setText("Не удалось открыть файл для записи");
        return;
    };

    file.write(serializedData.c_str(), serializedData.size());
    file.close();

    ui->statusLabel->setText("Данные успешно сериализованы");
}
EOL
    fi
else
    echo "Generated files not found."
fi
```

После генерации прото файла нужно перезапустить приложение и уже после перезапуска можно сериализовывать 

![5](/img/5.jpg)

В папке с билдом проекта появится сериализованный файл, содержащий бинарное предстваление данных из таблицы

![6](/img/6.jpg)

![7](/img/7.jpg)

