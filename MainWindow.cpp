#include "ImageWidget.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QKeyEvent>
#include <QFileDialog>
#include "CameraCaptureThread.h"
#include <QMessageBox>

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

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	int k = e->key();
	if (k == Qt::Key_S) {
		if (e->modifiers() & Qt::ControlModifier) {
			saveas();
		}
	}
	QMainWindow::keyPressEvent(e);

}

void MainWindow::saveas()
{
	QImage image = ui->widget->image();
	if (image.isNull()) return;
	QString path = QFileDialog::getSaveFileName(this, "Save as");
	if (path.isEmpty()) return;
	image.save(path);
}
