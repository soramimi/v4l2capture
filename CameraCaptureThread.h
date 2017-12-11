#ifndef CAMERACAPTURETHREAD_H
#define CAMERACAPTURETHREAD_H

#include <QThread>

class CameraCaptureThread : public QThread {
private:
	struct Private;
	Private *m;

	void startCapture();
	void copyBuffer();
	void stopCapture();

protected:
	void run();

public:
	CameraCaptureThread();
	~CameraCaptureThread();
	QImage image();
};

#endif // CAMERACAPTURETHREAD_H
