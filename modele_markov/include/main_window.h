#ifndef mainwindow_h
#define mainwindow_h

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>

class MainWindow : public QMainWindow
{
	private:
		QWidget *m_top;
		QVBoxLayout *m_box;
		
	public:
		MainWindow();
		~MainWindow();
		
		void ouvrir();
};

#endif