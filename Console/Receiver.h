#pragma once

#include <QUdpSocket>
#include "Packets.h"
#include <QThread>
#include "HUDPlug.h"
#include <Windows.h>

class QTimer;

class Receiver : public QObject
{
	Q_OBJECT

public:
	Receiver(QThread *workingThread);
	~Receiver();

signals:
	void FirstReceive();

private slots:
	void processPendingDatagram();
	void CheckUpdated();

private:
	QUdpSocket *udp;
	QThread *receivedThread;
	QTimer *updateTimer;
	HANDLE mutexCarStatus;
	HANDLE mapView;
	bool updated;
	bool firstReceive;
};
