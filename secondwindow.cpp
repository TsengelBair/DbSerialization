#include <QDebug>

#include "secondwindow.h"
#include "ui_secondwindow.h"

Secondwindow::Secondwindow(QSqlDatabase &db, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Secondwindow),
  db(db),
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


  if (!db.isOpen()) {
      db.open();
  }

  QSqlQuery query(db);
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

      // Приходится дублировать настройки подключения, хотя по логике нужно только setDatabaseName
      // Позже пофиксить
      db = QSqlDatabase::addDatabase("QPSQL");
      db.setUserName("postgres");
      db.setHostName("localhost");
      db.setPassword("25944");
      db.setDatabaseName(parsedDbName);
      if (!thirdWindow) {
          qDebug() << db.databaseName();
          thirdWindow = new Thirdwindow(db);
      };
      thirdWindow->show();
  };
}

