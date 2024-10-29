#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dbmanager.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);


	DBManager::instance()->updateDB();
	m_catalogModel = new MainCatalogModel();
	ui->listView->setModel(m_catalogModel);

}

MainWindow::~MainWindow()
{
	delete ui;
}

