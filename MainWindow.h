#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
	Ui::MainWindow *ui;

	struct Private;
	Private *m;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

protected:
	void timerEvent(QTimerEvent *event);
};

#endif // MAINWINDOW_H
