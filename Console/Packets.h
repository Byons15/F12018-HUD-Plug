#pragma once

#pragma pack(push)
#pragma pack(1)

typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef short int16;
typedef char int8;

struct PacketHeader
{
	uint16 m_packetFormat; // 2018
	uint8 m_packetVersion; //�����ݰ����͵İ汾������1��ʼ
	uint8 m_packetId; //���ݰ����͵ı�ʶ������μ�����
	uint64 m_sessionUID; //�Ự��Ψһ��ʶ��
	float m_sessionTime; //�Ựʱ���
	uint m_frameIdentifier; //������������֡�ı�ʶ��
	uint8 m_playerCarIndex; //�������������������
};

enum PACKETTYPE
{
	MOTION,
	SESSION,
	LAP_DATA,
	EVENT,
	PARTICIPANTS,
	CAR_SETUPS,
	CAR_TELEMETRY,
	CAR_STATUS,
};

struct CarMotionData
{
	float m_worldPositionX; //����ռ�Xλ��
	float m_worldPositionY; //����ռ�Yλ��
	float m_worldPositionZ; //����ռ�Zλ��
	float m_worldVelocityX; //����ռ��е��ٶ�X
	float m_worldVelocityY; //����ռ�Y���ٶ�
	float m_worldVelocityZ; //����ռ�Z�е��ٶ�
	int16 m_worldForwardDirX; //����ռ�ǰ��X���򣨹淶����
	int16 m_worldForwardDirY; //����ռ�ǰ��Y���򣨹淶����
	int16 m_worldForwardDirZ; //����ռ�ǰ��Z���򣨹淶����
	int16 m_worldRightDirX; //����ռ���X���򣨹淶����
	int16 m_worldRightDirY; //����ռ����ҵ�Y���򣨱�׼����
	int16 m_worldRightDirZ; //����ռ���Z���򣨱�׼����
	float m_gForceLateral; //����G�����
	float m_gForceLongitudinal; //����G������
	float m_gForceVertical; //��ֱG�����
	float m_yaw; //ƫ���ǣ����ȣ�
	float m_pitch; //�����ǣ����ȣ�
	float m_roll; //���ȵĹ�����
};

//Ƶ�ʣ��˵���ָ���ķ���
struct PacketMotionData
{
	PacketHeader m_header; //����

	CarMotionData m_carMotionData[20]; //��������������������

									   //���������������
	float m_suspensionPosition[4]; //ע�⣺���г������е�˳�����£�
	float m_suspensionVelocity[4]; // RL��RR��FL��FR
	float m_suspensionAcceleration[4]; // RL��RR��FL��FR
	float m_wheelSpeed[4]; //ÿ�����ֵ��ٶ�
	float m_wheelSlip[4]; //ÿ�����ֵĻ�����
	float m_localVelocityX; //�ֲ��ռ���ٶ�
	float m_localVelocityY; //�ֲ��ռ���ٶ�
	float m_localVelocityZ; //�ֲ��ռ���ٶ�
	float m_angularVelocityX; //���ٶ�x����
	float m_angularVelocityY; //���ٶ�y����
	float m_angularVelocityZ; //���ٶ�z����
	float m_angularAccelerationX; //���ٶ�x����
	float m_angularAccelerationY; //���ٶ�y����
	float m_angularAccelerationZ; //���ٶ�z����
	float m_frontWheelsAngle; //��ǰǰ�ֽǶȣ��Ի���Ϊ��λ��
};

struct MarshalZone
{
	float m_zoneStart; //ͨ��Ԫ˧����ʼ��һȦ�ķ�����0..1��
	int8 m_zoneFlag; // -1 =��Ч/δ֪��0 =�ޣ�1 =��ɫ��2 =��ɫ��3 =��ɫ��4 =��ɫ
};

//Ƶ�ʣ�ÿ��2��
struct PacketSessionData
{
	PacketHeader m_header; //����

	uint8 m_weather; //����-0 =�磬1 =���ƣ�2 =��
					 // 3 =С�꣬4 =���꣬5 =������
	int8 m_trackTemperature; //׷���¶� �����϶�Ϊ��λ
	int8 m_airTemperature; //���� �����϶�Ϊ��λ
	uint8 m_totalLaps; //�ⳡ��������Ȧ��
	uint16 m_trackLength; //������ȣ�����Ϊ��λ��
	uint8 m_sessionType; // 0 =δ֪��1 = P1��2 = P2��3 = P3��4 =��P
						 // 5 = Q1��6 = Q2��7 = Q3��8 = Short Q��9 = OSQ
						 // 10 = R��11 = R2��12 =��ʱ��
	int8 m_trackId; // -1��ʾδ֪��0-21��ʾ��Ŀ����μ���¼
	uint8 m_era; //ʱ����0 =�ִ���1 =����
	uint16 m_sessionTimeLeft; //�Ự��ʣ���ʱ�䣨����Ϊ��λ��
	uint16 m_sessionDuration; //�Ự����ʱ�䣨����Ϊ��λ��
	uint8 m_pitSpeedLimit; //�������ƣ���ÿСʱ������Ϊ��λ
	uint8 m_gamePaused; //��Ϸ�Ƿ���ͣ
	uint8 m_isSpectating; //����Ƿ��ڹۿ�
	uint8 m_spectatorCarIndex; //���ۿ�������������
	uint8 m_sliProNativeSupport; // SLI Pro֧�֣�0 =��Ч��1 =��Ч
	uint8 m_numMarshalZones; //�����Ԫ˧����
	MarshalZone m_marshalZones[21]; //Ԫ˧���б�C���21��
	uint8 m_safetyCarStatus; // 0 =û�а�ȫ����1 =��ȫ��ȫ��
							 // 2 =���ⰲȫ��
	uint8 m_networkGame; // 0 =���ߣ�1 =����
};

struct LapData
{
	float m_lastLapTime; //���һȦʱ�䣨����Ϊ��λ��
	float m_currentLapTime; //��ǰʱ�䣨����Ϊ��λ��
	float m_bestLapTime; //�Ự�����Ȧ�٣�����Ϊ��λ��
	float m_sector1Time; //����1ʱ�䣨����Ϊ��λ��
	float m_sector2Time; //����2��ʱ�䣨����Ϊ��λ��
	float m_lapDistance; //���복������Ϊ��λ�ĵ�ǰȦ�������C
						 //�����δ��Խ����Ϊ��
	float m_totalDistance; //�Ự�о������ܾ��루����Ϊ��λ���C����
						   //�����δ��Խ����Ϊ��
	float m_safetyCarDelta; //��ȫ������������
	uint8 m_carPosition; //����λ��
	uint8 m_currentLapNum; //��ǰȦ��
	uint8 m_pitStatus; // 0 =�ޣ�1 =��ʴ��2 =��������
	uint8 m_sector; // 0 =����1��1 =����2��2 =����3
	uint8 m_currentLapInvalid; //��ǰȦ��Ч-0 =��Ч��1 =��Ч
	uint8 m_penalties; //Ҫ��ӵ��ۼ�ʱ��ͷ�������Ϊ��λ��
	uint8 m_gridPosition; //������ʼ����������λ��
	uint8 m_driverStatus; //��ʻԱ״̬-0 =�ڳ��⣬1 =����Ȧ
						  // 2 =��վȦ��3 =��վȦ��4 =����
	uint8 m_resultStatus; //���״̬-0 =��Ч��1 =��Ч��2 =��Ч
						  // 3 =��ɣ�4 =ʧ��5 =δ����
						  // 6 =����
};

//Ƶ�ʣ���Ϸ�е��趨
struct PacketLapData
{
	PacketHeader m_header; //����

	LapData m_lapData[20]; //����������������Ȧ������
};

//Ƶ�ʣ��¼�����ʱ
struct PacketEventData
{
	PacketHeader m_header; //����
	uint8 m_eventStringCode[4]; //�¼��ַ������룬�μ�����
};

// PacketEventData��ʱ���ַ�������
#define SSTA "SSTA"  // �ڻỰ��ʼʱ����
#define SEND "SEND"  // �ڻỰ����ʱ����


struct ParticipantData
{
	uint8 m_aiControlled; //��������AI��1���������ˣ�0������
	uint8 m_driverId; //��������ID-����ĸ�¼
	uint8 m_teamId; //�Ŷ�ID-�μ���¼
	uint8 m_raceNumber; //�����ı������
	uint8 m_nationality; //��ʻԱ�Ĺ���
	char m_name[48]; //����������ΪUTF-8��ʽ-��ֹΪnull
					 //���̫������������U + 2026���ض�
};

//ÿ5��
struct PacketParticipantsData
{
	PacketHeader m_header; //����

	uint8 m_numCars; //�����е���������
	ParticipantData m_participants[20];
};

enum TREAM
{
	MERCEDES,
	FERRARI,
	REDBULL,
	WILLIAMS,
	FORCEINDIA,
	RENAULT,
	TOROROSSO,
	HASS,
	MCLAREN,
	SAUBER,
};

struct CarSetupData
{
	uint8 m_frontWing; //ǰ����
	uint8 m_rearWing; //������
	uint8 m_onThrottle; //������΢�ֵ��ڣ��ٷֱȣ�
	uint8 m_offThrottle; //�������رս����ţ��ٷֱȣ�
	float m_frontCamber; //ǰ����ǣ����ܼ�����״��
	float m_rearCamber; //������ǣ����ܼ�����״��
	float m_frontToe; //ǰ��ֺ�Ƕȣ����ܼ�����״��
	float m_rearToe; //���ֺ�Ƕȣ����ܼ�����״��
	uint8 m_frontSuspension; //ǰ����
	uint8 m_rearSuspension; // ������
	uint8 m_frontAntiRollBar; //ǰ�����
	uint8 m_rearAntiRollBar; //ǰ�����
	uint8 m_frontSuspensionHeight; //ǰ���߶�
	uint8 m_rearSuspensionHeight; //�����߶�
	uint8 m_brakePressure; //�ƶ�ѹ�����ٷֱȣ�
	uint8 m_brakeBias; //�ƶ�ƫ��ٷֱȣ�
	float m_frontTyrePressure; //ǰ��̥ѹ����PSI��
	float m_rearTyrePressure; //����̥ѹ����PSI��
	uint8 m_ballast; //������
	float m_fuelLoad; //ȼ�͸���
};

//ÿ5�뷢��һ��
struct PacketCarSetupData
{
	PacketHeader m_header; //����
	CarSetupData m_carSetups[20];
};

//Ƶ�ʣ��˵���ָ���ķ���
struct CarTelemetryData
{
	uint16 m_speed; //����
	uint8 m_throttle; //Ӧ�õ���������0 - 100��
	int8 m_steer; //ת��-100����ȫ������100����ȫ������
	uint8 m_brake; //ʩ�ӵ��ƶ�����0��100��
	uint8 m_clutch; //ʩ�ӵ������������0��100��
	int8 m_gear; //��λ��1-8��N=0, R=-1��
	uint16 m_engineRPM; //����RPM
	uint8 m_drs; // 0 =�رգ�1 =����
	uint8 m_revLightsPercent; //����ָʾ�ƣ��ٷֱȣ�
	uint16 m_brakesTemperature[4]; //�ƶ��¶ȣ����϶ȣ�
	uint16 m_tyresSurfaceTemperature[4]; //��̥�����¶ȣ����϶ȣ�
	uint16 m_tyresInnerTemperature[4]; //��̥�ڲ��¶ȣ����϶ȣ�
	uint16 m_engineTemperature; //�������¶ȣ����϶ȣ�
	float m_tyresPressure[4]; //̥ѹ��PSI��
};

struct PacketCarTelemetryData
{
	PacketHeader m_header; //����

	CarTelemetryData m_carTelemetryData[20];

	uint32 m_buttonStatus; //λ��־��ָ������ʹ�õİ�ť
						   //��ǰ����-����ĸ�¼
};

struct CarStatusData
{
	uint8 m_tractionControl; // 0���رգ�-2���ߣ�
	uint8 m_antiLockBrakes; // 0���رգ�-1��������
	uint8 m_fuelMix; //ȼ�ͻ��-0 =ϡ�ͣ�1 =��׼��2 =Ũ��3 =���
	uint8 m_frontBrakeBias; //ǰɲ��ƫѹ���ٷֱȣ�
	uint8 m_pitLimiterStatus; //��������״̬-0 =�رգ�1 =����
	float m_fuelInTank; //��ǰ��ȼ������
	float m_fuelCapacity; // ȼ������
	uint16 m_maxRPM; //�������ת�٣�ת��������
	uint16 m_idleRPM; //��������RPM
	uint8 m_maxGears; //��������
	uint8 m_drsAllowed; // 0 =������1 =����-1 =δ֪
	uint8 m_tyresWear[4]; //��̥ĥ����
	uint8 m_tyreCompound; //�ִ�-0 =������1 =������
						  // 2 =������3 =��4 =�У�5 =Ӳ
						  // 6 =��Ӳ��7 =�м䣬8 =ʪ
						  //����-0-6 =���7-8 =ʪ
	uint8 m_tyresDamage[4]; //��̥�𻵣��ٷֱȣ�
	uint8 m_frontLeftWingDamage; //��ǰ����˺����ٷֱȣ�
	uint8 m_frontRightWingDamage; //��ǰ����˺����ٷֱȣ�
	uint8 m_rearWingDamage; //�����𻵣��ٷֱȣ�
	uint8 m_engineDamage; //�����𻵣��ٷֱȣ�
	uint8 m_gearBoxDamage; //�������𻵣��ٷֱȣ�
	uint8 m_exhaustDamage; //�����𻵣��ٷֱȣ�
	int8 m_vehicleFiaFlags; // -1 =��Ч/δ֪��0 =�ޣ�1 =��ɫ
							// 2 =��ɫ��3 =��ɫ��4 =��ɫ
	float m_ersStoreEnergy; //������ERS�����洢
	uint8 m_ersDeployMode; // ERS����ģʽ��0 =�ޣ�1 =�ͣ�2 =��
						   // 3 =�ߣ�4 =������5 =��Ȧ
	float m_ersHarvestedThisLapMGUK; // MGU-K����һȦ�ջ���ERS����
	float m_ersHarvestedThisLapMGUH; // MGU-H����һȦ�ջ���ERS����
	float m_ersDeployedThisLap; //����һȦ����ERS����
};

//ÿ��2��
struct PacketCarStatusData
{
	PacketHeader m_header; //����

	CarStatusData m_carStatusData[20];
};

#pragma pack(pop)