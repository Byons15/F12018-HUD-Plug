#pragma once
#include <windows.h>

static const char *CarStatusMutexName = "The Car status data mutex";
static const char *CarStatusMapViewName = "Car Status MapView Name";
static char *gameHwndMapViewName = "game Hwnd MapView Name";
static char *HUDPlugResourcePathMapViewName = "HUDPlugResourcePathMapViewName";

struct CarStatus
{
	float ersStoreEnergy = 0;  //ERS����
	float ersHarvestedThisLap = 0; //��Ȧ��ȡ��ERS����
	float ersDeployedThisLap = 0; //��Ȧ�����ERS����
	unsigned short speed = 0;  // �ٶ�
	unsigned short engineRPM = 0; //����ת��
	unsigned char throttle = 0; // ������
	unsigned char brake = 0; // �ƶ���
	unsigned char revLightsPercent = 0; // ����ָʾ��
	unsigned char ersDeployMode = 0; // ERS����ģʽ
	unsigned char teamID = 0; // ���� �μ�Packets.h
	char gear = 0; // ��λ
	bool drs = false;
	bool damage = false;
	bool show = false;  //�Ƿ���ʾHUD
};


