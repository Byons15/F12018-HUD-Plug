#include "Receiver.h"
#include <QtNetwork>
#include <cstdlib>
#include <Windows.h>

Receiver::Receiver(QThread *workingThread)
	: QObject(0), mutexCarStatus(0), updated(false), firstReceive(true)
{
	// 创建状态数据访问互斥体和共享内存，如果失败了就什么也不做
	mutexCarStatus = CreateMutexA(0, FALSE, CarStatusMutexName);
	if (mutexCarStatus == 0) return;

	mapView = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CarStatus), CarStatusMapViewName);
	if (mapView == 0) return;

	// 创建套接字并连接相应的信号槽，同时移动到工作线程
	udp = new QUdpSocket();
	receivedThread = workingThread;
	connect(udp, SIGNAL(readyRead()), this, SLOT(processPendingDatagram()), Qt::AutoConnection);
	connect(receivedThread, SIGNAL(finished()), udp, SLOT(deleteLater()));
	udp->bind(QHostAddress(QHostAddress::Any), 20777);
	udp->moveToThread(receivedThread);
	
	// 把对象的销毁器连接到工作线程，并将对象移动到工作线程
	connect(receivedThread, SIGNAL(finished()), this, SLOT(deleteLater()));
	moveToThread(receivedThread);

	updateTimer = new QTimer();
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(CheckUpdated()));
	updateTimer->start(100);
	updateTimer->moveToThread(workingThread);
}

Receiver::~Receiver()
{
}

void Receiver::CheckUpdated()
{

	// 如果100ms以来, 数据没有被更新过,那么关闭HUD
	if (!updated)
	{
		if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
			return;
		void *buffer = MapViewOfFile(mapView, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		CarStatus &carStatus = *((CarStatus *)buffer);

		carStatus.show = false;

		UnmapViewOfFile(buffer);
		ReleaseMutex(mutexCarStatus);
	}

	updated = false;
}

void Receiver::processPendingDatagram()
{
	while (udp->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(udp->pendingDatagramSize());
		udp->readDatagram(datagram.data(), datagram.size());
		//TODO::处理数据报
		if (datagram.size() <= sizeof(PacketHeader))
			break;

		PacketHeader *header = (PacketHeader*)(datagram.data());

		if (header->m_packetFormat != 2018)
			break;

		updated = true;

		if (firstReceive)
		{
			emit FirstReceive();
			firstReceive = false;
		}

		const uint8 &playerIndex = header->m_playerCarIndex;
		switch (header->m_packetId)
		{
		case MOTION:
			break;
		case SESSION:
			{
				auto *data = (PacketSessionData*)(datagram.data());

				if (data->m_gamePaused)
				{
					//好像游戏暂停时不会发送SESSION消息
					//所以要是有SESSION消息, m_gamePaused基本都是false
				}
				else
				{
				}
			}
			break;
		case LAP_DATA:
			{
				auto *data = (PacketLapData*)(datagram.data());
				
				if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
					break;
				void *buffer = MapViewOfFile(mapView, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				CarStatus &carStatus = *((CarStatus *)buffer);

				carStatus.show = data->m_lapData[playerIndex].m_driverStatus && carStatus.teamID >= MERCEDES && carStatus.teamID <= SAUBER;

				UnmapViewOfFile(buffer);
				ReleaseMutex(mutexCarStatus);
			}
			break;
		case EVENT:
			{
				auto *data = (PacketEventData*)(datagram.data());
				if (data->m_eventStringCode[0] == 'S' &&
					data->m_eventStringCode[1] == 'S' &&
					data->m_eventStringCode[2] == 'T' &&
					data->m_eventStringCode[3] == 'A')
				{
					if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
						break;

					void *buffer = MapViewOfFile(mapView, FILE_MAP_ALL_ACCESS, 0, 0, 0);
					CarStatus &carStatus = *((CarStatus *)buffer);

					carStatus.show = true;

					UnmapViewOfFile(buffer);
					ReleaseMutex(mutexCarStatus);
				}
				else if (data->m_eventStringCode[0] == 'S' &&
					data->m_eventStringCode[1] == 'E' &&
					data->m_eventStringCode[2] == 'N' &&
					data->m_eventStringCode[3] == 'D')
				{
					if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
						break;

					void *buffer = MapViewOfFile(mapView, FILE_MAP_ALL_ACCESS, 0, 0, 0);
					CarStatus &carStatus = *((CarStatus *)buffer);

					carStatus.show = false;

					UnmapViewOfFile(buffer);
					ReleaseMutex(mutexCarStatus);
				}
			}
			break;
		case PARTICIPANTS:
			{
				auto data = (PacketParticipantsData *)(datagram.data());

				if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
					break;

				void *buffer = MapViewOfFile(mapView, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				CarStatus &carStatus = *((CarStatus *)buffer);

				carStatus.teamID = data->m_participants[playerIndex].m_teamId;

				UnmapViewOfFile(buffer);
				ReleaseMutex(mutexCarStatus);
			}
			break;
		case CAR_SETUPS:
			break;
		case CAR_TELEMETRY:
			// 获取车辆状态
			{
				auto *data = (PacketCarTelemetryData*)(datagram.data());
				
				if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
					break;

				void *buffer = MapViewOfFile(mapView, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				CarStatus &carStatus = *((CarStatus *)buffer);

				carStatus.speed = data->m_carTelemetryData[playerIndex].m_speed;
				carStatus.brake = data->m_carTelemetryData[playerIndex].m_brake;
				carStatus.drs = data->m_carTelemetryData[playerIndex].m_drs;
				carStatus.engineRPM = data->m_carTelemetryData[playerIndex].m_engineRPM;
				carStatus.gear = data->m_carTelemetryData[playerIndex].m_gear;
				carStatus.throttle = data->m_carTelemetryData[playerIndex].m_throttle;
				carStatus.revLightsPercent = data->m_carTelemetryData[playerIndex].m_revLightsPercent;

				UnmapViewOfFile(buffer);
				ReleaseMutex(mutexCarStatus);
			}
			break;
		case CAR_STATUS:
			{
				auto *data = (PacketCarStatusData*)(datagram.data());

				if (WaitForSingleObject(mutexCarStatus, 8) != WAIT_OBJECT_0)
					break;

				void *buffer = MapViewOfFile(mapView, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				CarStatus &carStatus = *((CarStatus *)buffer);

				carStatus.ersDeployedThisLap = data->m_carStatusData[playerIndex].m_ersDeployedThisLap;
				carStatus.ersHarvestedThisLap = data->m_carStatusData[playerIndex].m_ersHarvestedThisLapMGUH 
											  + data->m_carStatusData[playerIndex].m_ersHarvestedThisLapMGUK;
				carStatus.ersStoreEnergy = data->m_carStatusData[playerIndex].m_ersStoreEnergy;
				carStatus.ersDeployMode = data->m_carStatusData[playerIndex].m_ersDeployMode;
				carStatus.gear = data->m_carStatusData[playerIndex].m_pitLimiterStatus ? -2 : carStatus.gear;
				carStatus.damage = data->m_carStatusData[playerIndex].m_frontLeftWingDamage
								|| data->m_carStatusData[playerIndex].m_frontRightWingDamage
								|| data->m_carStatusData[playerIndex].m_rearWingDamage;
				
				UnmapViewOfFile(buffer);
				ReleaseMutex(mutexCarStatus);
			}
			break;
		default:
			// 不是游戏遥测数据
			break;
		}
	}
}
