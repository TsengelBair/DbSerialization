#include <QDebug>

#include "thirdwindow.h"
#include "ui_thirdwindow.h"

Thirdwindow::Thirdwindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Thirdwindow),
  fourthWindow(nullptr)
{
  ui->setupUi(this);
  showTables();
}

Thirdwindow::~Thirdwindow()
{
  delete ui;
}

void Thirdwindow::showTables()
{
  QSqlQuery query(DbConnection::getInstance()->getDatabase());
  if (!query.exec("SELECT tablename FROM pg_catalog.pg_tables WHERE schemaname = 'public'")) {
      qDebug() << "Ошибка:" << query.lastError().text();
      return;
  }

  while (query.next()){
      QString tableName = query.value(0).toString();
      qDebug() << tableName;
      tableNames.push_back(tableName);
  }

  for (const QString& tableName : tableNames) {
      qDebug() << tableName << " ";
      QPushButton* tableNameBtn = new QPushButton(tableName, this);
      ui->avaibleTablesLayout->addWidget(tableNameBtn);
      connect(tableNameBtn, &QPushButton::clicked, this, &Thirdwindow::onTableBtnClicked);
  }
}

void Thirdwindow::onTableBtnClicked()
{
  QPushButton* pushedBtn = qobject_cast<QPushButton*>(sender());
  if (pushedBtn) {
      QString parsedTableName = pushedBtn->text();

      if (!fourthWindow) {
          fourthWindow = new Fourthwindow(parsedTableName);
      };

      fourthWindow->show();
  };
}
