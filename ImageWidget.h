#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>

class ImageWidget : public QWidget
{
	Q_OBJECT
private:
	QImage image_;
public:
	explicit ImageWidget(QWidget *parent = 0);

	void setImage(QImage image);
signals:

public slots:

protected:
	void paintEvent(QPaintEvent *event);
};

#endif // IMAGEWIDGET_H
