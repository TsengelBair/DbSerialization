#include <QDebug>

#include "secondwindow.h"
#include "ui_secondwindow.h"

Secondwindow::Secondwindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Secondwindow),
  thirdWindow(nullptr)
{
  ui->setupUi(this);
  showDbs();
}

Secondwindow::~Secondwindow()
{
  delete ui;
}

void Secondwindow::showDbs()
{
  dbsNames.clear();
  while (ui->avaibleDbsLayout->count() > 1) {
      QWidget* widget = ui->avaibleDbsLayout->itemAt(1)->widget();
      ui->avaibleDbsLayout->removeWidget(widget);
      delete widget;
  }

  QSqlQuery query(DbConnection::getInstance()->getDatabase());
  query.prepare("SELECT datname FROM pg_database WHERE datistemplate = false;");

  if (!query.exec()) {
       qDebug() << "Ошибка:" << query.lastError().text();
       return;
   }

   while(query.next()){
       QString dbName = query.value(0).toString();
       if (dbName != "postgres"){
           dbsNames.push_back(dbName);
       };
   }

   for (const QString& dbName : dbsNames) {
       QPushButton* dbNameBtn = new QPushButton(dbName, this);
       ui->avaibleDbsLayout->addWidget(dbNameBtn);
       connect(dbNameBtn, &QPushButton::clicked, this, &Secondwindow::onDbBtnClicked);
   }
}

void Secondwindow::onDbBtnClicked()
{
  QPushButton* pushedBtn = qobject_cast<QPushButton*>(sender());

  if (pushedBtn) {
      QString parsedDbName = pushedBtn->text();
      DbConnection::getInstance()->setDatabaseConfig(parsedDbName);

      if (!thirdWindow) {
        thirdWindow = new Thirdwindow();
      };

      thirdWindow->show();
  };
}

