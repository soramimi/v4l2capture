#include "ImageWidget.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMutex>
#include <QThread>

#include "CameraCaptureThread.h"

struct MainWindow::Private {
	CameraCaptureThread capture_thread;
};

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m(new Private)
{
	ui->setupUi(this);
	m->capture_thread.start();

	startTimer(1000 / 30);
}

MainWindow::~MainWindow()
{
	m->capture_thread.requestInterruption();
	m->capture_thread.wait();
	delete m;
	delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	QImage image = m->capture_thread.image();
	if (!image.isNull()) {
		ui->widget->setImage(image);
	}
}
