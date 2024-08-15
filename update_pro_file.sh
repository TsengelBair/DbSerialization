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
    # Копируем файлы в корень проекта
    echo "Copying files to project directory..."
    cp "$PB_HEADER" "$PROJECT_DIR"
    cp "$PB_SOURCE" "$PROJECT_DIR"

    # Убедитесь, что файлы уже добавлены в CMakeLists.txt (только если они отсутствуют)
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

    # Проверяем и добавляем подключение tableData.pb.h в fourthwindow.cpp
    if grep -Fxq "$HEADER_INCLUDE" "$CPP_FILE"; then
        echo "Header already included in $CPP_FILE"
    else
        echo "Adding header to $CPP_FILE..."
        sed -i "1i $HEADER_INCLUDE" "$CPP_FILE"
    fi

    # Добавление объявления слота в fourthwindow.h
    if grep -q "void on_serializeBtn_clicked();" "$HEADER_FILE"; then
        echo "Slot on_serializeBtn_clicked() already declared in $HEADER_FILE"
    else
        echo "Adding slot declaration to $HEADER_FILE..."
        sed -i '/private slots:/a\ \ \ \ void on_serializeBtn_clicked();' "$HEADER_FILE"
    fi

    # Добавление реализации слота в fourthwindow.cpp
    if grep -q "void MainWindow::on_serializeBtn_clicked()" "$CPP_FILE"; then
        echo "Slot on_serializeBtn_clicked() already implemented in $CPP_FILE"
    else
        echo "Adding slot implementation to $CPP_FILE..."
        cat <<EOL >> "$CPP_FILE"

void Fourthwindow::on_serializeBtn_clicked()
{
    // Объект для хранения всех полей бд для последующей сериализации
    TableDatas tableDatas;

    for (int row = 0; row < tableModel->rowCount(); ++row) {
        QSqlRecord record = tableModel->record(row);

        TableData* tableData = tableDatas.add_tabledata();
        tableData->set_id(record.value("id").toInt());
        tableData->set_name(record.value("name").toString().toStdString());
    }

    // Сериализуем, при использовании QString выдает ошибку, т.к. protobuf ожидает std::string
    std::string serializedData;
    if (!tableDatas.SerializeToString(&serializedData)){
        ui->statusLabel->setText("Не получилось сериализовать");
        return;
    };

    // Бинарный файл, в который запишем сериализованный объект
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
