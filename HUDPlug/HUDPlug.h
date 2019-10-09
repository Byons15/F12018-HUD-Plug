#pragma once
#include <windows.h>

static const char *CarStatusMutexName = "The Car status data mutex";
static const char *CarStatusMapViewName = "Car Status MapView Name";
static char *gameHwndMapViewName = "game Hwnd MapView Name";
static char *HUDPlugResourcePathMapViewName = "HUDPlugResourcePathMapViewName";

struct CarStatus
{
	float ersStoreEnergy = 0;  //ERS容量
	float ersHarvestedThisLap = 0; //本圈获取的ERS容量
	float ersDeployedThisLap = 0; //本圈部署的ERS容量
	unsigned short speed = 0;  // 速度
	unsigned short engineRPM = 0; //引擎转速
	unsigned char throttle = 0; // 油门量
	unsigned char brake = 0; // 制动量
	unsigned char revLightsPercent = 0; // 换挡指示灯
	unsigned char ersDeployMode = 0; // ERS部署模式
	unsigned char teamID = 0; // 车型 参见Packets.h
	char gear = 0; // 挡位
	bool drs = false;
	bool damage = false;
	bool show = false;  //是否显示HUD
};


