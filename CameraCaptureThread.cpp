#include "CameraCaptureThread.h"

#include <QImage>
#include <QMutex>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

static const int v4l2BufferNum = 2;

struct CameraCaptureThread::Private {
	QMutex mutex;
	QImage image;
	int fd;
	void *v4l2Buffer[v4l2BufferNum];
	uint32_t v4l2BufferSize[v4l2BufferNum];
	struct v4l2_format fmt;
};

CameraCaptureThread::CameraCaptureThread()
	: m(new Private)
{

}

CameraCaptureThread::~CameraCaptureThread()
{
	delete m;
}

void CameraCaptureThread::startCapture()
{
	m->fd = open("/dev/video0", O_RDWR);

	/* 1. フォーマット指定。640x480のRGB24形式でキャプチャしてください */
	memset(&m->fmt, 0, sizeof(m->fmt));
	m->fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	m->fmt.fmt.pix.width       = 640;
	m->fmt.fmt.pix.height      = 480;
	m->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	ioctl(m->fd, VIDIOC_S_FMT, &m->fmt);

	/* 2. バッファリクエスト。バッファを2面確保してください */
	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
	req.count  = v4l2BufferNum;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ioctl(m->fd, VIDIOC_REQBUFS, &req);

	/* 3. 確保されたバッファをユーザプログラムからアクセスできるようにmmapする */
	struct v4l2_buffer buf;
	for (uint32_t i = 0; i < v4l2BufferNum; i++) {
		/* 3.1 確保したバッファ情報を教えてください */
		memset(&buf, 0, sizeof(buf));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;
		ioctl(m->fd, VIDIOC_QUERYBUF, &buf);

		/* 3.2 取得したバッファ情報を基にmmapして、後でアクセスできるようにアドレスを保持っておく */
		m->v4l2Buffer[i] = mmap(NULL, buf.length, PROT_READ, MAP_SHARED, m->fd, buf.m.offset);
		m->v4l2BufferSize[i] = buf.length;
	}

	/* 4. バッファのエンキュー。指定するバッファをキャプチャするときに使ってください */
	for (uint32_t i = 0; i < v4l2BufferNum; i++) {
		memset(&buf, 0, sizeof(buf));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;
		ioctl(m->fd, VIDIOC_QBUF, &buf);
	}

	/* 5. ストリーミング開始。キャプチャを開始してください */
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(m->fd, VIDIOC_STREAMON, &type);

	/* この例だと2面しかないので、2フレームのキャプチャ(1/30*2秒?)が終わった後、新たにバッファがエンキューされるまでバッファへの書き込みは行われない、はず */
}

void CameraCaptureThread::copyBuffer()
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m->fd, &fds);

	/* 6. バッファに画データが書き込まれるまで待つ */
	while (select(m->fd + 1, &fds, NULL, NULL, NULL) < 0) {
		if (isInterruptionRequested()) return;
	}

	if (FD_ISSET(m->fd, &fds)) {
		/* 7. バッファのデキュー。もっとも古くキャプチャされたバッファをデキューして、そのインデックス番号を教えてください */
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		ioctl(m->fd, VIDIOC_DQBUF, &buf);

		/* 8. デキューされたバッファのインデックス(buf.index)と書き込まれたサイズ(buf.byteused)が返ってくる */

		/* 9. create QImage object */
		{
			QMutexLocker lock(&m->mutex);
			m->image = QImage((uchar const *)m->v4l2Buffer[buf.index], m->fmt.fmt.pix.width, m->fmt.fmt.pix.height, m->fmt.fmt.pix.width * 3, QImage::Format_RGB888);
		}

		/* 10. 先ほどデキューしたバッファを、再度エンキューする。カメラデバイスはこのバッファに対して再びキャプチャした画を書き込む */
		ioctl(m->fd, VIDIOC_QBUF, &buf);
	}
}

void CameraCaptureThread::stopCapture()
{
	/* 11. ストリーミング終了。キャプチャを停止してください */
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(m->fd, VIDIOC_STREAMOFF, &type);

	/* 12. リソース解放 */
	for (uint32_t i = 0; i < v4l2BufferNum; i++) munmap(m->v4l2Buffer[i], m->v4l2BufferSize[i]);

	/* 13. デバイスファイルを閉じる */
	close(m->fd);
}

void CameraCaptureThread::run()
{
	startCapture();
	while (1) {
		if (isInterruptionRequested()) break;
		copyBuffer();
	}
	stopCapture();
}

QImage CameraCaptureThread::image()
{
	QMutexLocker lock(&m->mutex);
	QImage im;
	std::swap(im, m->image);
	return im;
}
