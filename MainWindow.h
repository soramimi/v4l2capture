#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
	Ui::MainWindow *ui;

	struct Private;
	Private *m;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void saveas();
private slots:

protected:
	void timerEvent(QTimerEvent *event);

	// QWidget interface
protected:
	void keyPressEvent(QKeyEvent *);
};

#endif // MAINWINDOW_H
