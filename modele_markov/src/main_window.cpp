#include "mainwindow.h"

MainWindow::MainWindow()
{
	// Widget prncipal
	
	m_top = new QWidget(this);
	setCentralWidget(m_top);
	
	// Nom et taille de la fenêtre
	
	this->setWindowTitle("Analyseur Syntaxical");
	this->resize(900, 600);
	this->move(0,0);
	this->setWindowIcon(QIcon("Images/icone_v4.png"));
	
	// Layout appliqué au top
	
	m_box = new QVBoxLayout(m_top);

	// Marche pas sur Linux
	
	setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{
	delete m_top;
}

void MainWindow::ouvrir()
{

}
