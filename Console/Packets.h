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
	uint8 m_packetVersion; //此数据包类型的版本，均从1开始
	uint8 m_packetId; //数据包类型的标识符，请参见下文
	uint64 m_sessionUID; //会话的唯一标识符
	float m_sessionTime; //会话时间戳
	uint m_frameIdentifier; //检索到的数据帧的标识符
	uint8 m_playerCarIndex; //数组中玩家汽车的索引
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
	float m_worldPositionX; //世界空间X位置
	float m_worldPositionY; //世界空间Y位置
	float m_worldPositionZ; //世界空间Z位置
	float m_worldVelocityX; //世界空间中的速度X
	float m_worldVelocityY; //世界空间Y的速度
	float m_worldVelocityZ; //世界空间Z中的速度
	int16 m_worldForwardDirX; //世界空间前进X方向（规范化）
	int16 m_worldForwardDirY; //世界空间前进Y方向（规范化）
	int16 m_worldForwardDirZ; //世界空间前进Z方向（规范化）
	int16 m_worldRightDirX; //世界空间右X方向（规范化）
	int16 m_worldRightDirY; //世界空间向右的Y方向（标准化）
	int16 m_worldRightDirZ; //世界空间右Z方向（标准化）
	float m_gForceLateral; //横向G力组件
	float m_gForceLongitudinal; //纵向G力分量
	float m_gForceVertical; //垂直G力组件
	float m_yaw; //偏航角（弧度）
	float m_pitch; //俯仰角（弧度）
	float m_roll; //弧度的滚动角
};

//频率：菜单中指定的费率
struct PacketMotionData
{
	PacketHeader m_header; //标题

	CarMotionData m_carMotionData[20]; //赛道上所有汽车的数据

									   //仅额外的赛车数据
	float m_suspensionPosition[4]; //注意：所有车轮阵列的顺序如下：
	float m_suspensionVelocity[4]; // RL，RR，FL，FR
	float m_suspensionAcceleration[4]; // RL，RR，FL，FR
	float m_wheelSpeed[4]; //每个车轮的速度
	float m_wheelSlip[4]; //每个车轮的滑移率
	float m_localVelocityX; //局部空间的速度
	float m_localVelocityY; //局部空间的速度
	float m_localVelocityZ; //局部空间的速度
	float m_angularVelocityX; //角速度x分量
	float m_angularVelocityY; //角速度y分量
	float m_angularVelocityZ; //角速度z分量
	float m_angularAccelerationX; //角速度x分量
	float m_angularAccelerationY; //角速度y分量
	float m_angularAccelerationZ; //角速度z分量
	float m_frontWheelsAngle; //当前前轮角度（以弧度为单位）
};

struct MarshalZone
{
	float m_zoneStart; //通过元帅区开始的一圈的分数（0..1）
	int8 m_zoneFlag; // -1 =无效/未知，0 =无，1 =绿色，2 =蓝色，3 =黄色，4 =红色
};

//频率：每秒2次
struct PacketSessionData
{
	PacketHeader m_header; //标题

	uint8 m_weather; //天气-0 =晴，1 =轻云，2 =阴
					 // 3 =小雨，4 =大雨，5 =暴风雨
	int8 m_trackTemperature; //追踪温度 以摄氏度为单位
	int8 m_airTemperature; //气温 以摄氏度为单位
	uint8 m_totalLaps; //这场比赛的总圈数
	uint16 m_trackLength; //轨道长度（以米为单位）
	uint8 m_sessionType; // 0 =未知，1 = P1、2 = P2、3 = P3、4 =短P
						 // 5 = Q1，6 = Q2，7 = Q3，8 = Short Q，9 = OSQ
						 // 10 = R，11 = R2，12 =计时赛
	int8 m_trackId; // -1表示未知，0-21表示曲目，请参见附录
	uint8 m_era; //时代，0 =现代，1 =经典
	uint16 m_sessionTimeLeft; //会话中剩余的时间（以秒为单位）
	uint16 m_sessionDuration; //会话持续时间（以秒为单位）
	uint8 m_pitSpeedLimit; //坑速限制，以每小时公里数为单位
	uint8 m_gamePaused; //游戏是否暂停
	uint8 m_isSpectating; //玩家是否在观看
	uint8 m_spectatorCarIndex; //被观看的汽车的索引
	uint8 m_sliProNativeSupport; // SLI Pro支持，0 =无效，1 =有效
	uint8 m_numMarshalZones; //跟随的元帅区数
	MarshalZone m_marshalZones[21]; //元帅区列表–最多21个
	uint8 m_safetyCarStatus; // 0 =没有安全车，1 =完全安全车
							 // 2 =虚拟安全车
	uint8 m_networkGame; // 0 =离线，1 =在线
};

struct LapData
{
	float m_lastLapTime; //最后一圈时间（以秒为单位）
	float m_currentLapTime; //当前时间（以秒为单位）
	float m_bestLapTime; //会话的最佳圈速（以秒为单位）
	float m_sector1Time; //扇区1时间（以秒为单位）
	float m_sector2Time; //扇区2的时间（以秒为单位）
	float m_lapDistance; //距离车辆以米为单位的当前圈数附近–
						 //如果尚未跨越线则为负
	float m_totalDistance; //会话中经过的总距离（以米为单位）–可以
						   //如果尚未跨越线则为负
	float m_safetyCarDelta; //安全车的秒数增量
	uint8 m_carPosition; //赛车位置
	uint8 m_currentLapNum; //当前圈数
	uint8 m_pitStatus; // 0 =无，1 =点蚀，2 =凹坑区域
	uint8 m_sector; // 0 =扇区1，1 =扇区2，2 =扇区3
	uint8 m_currentLapInvalid; //当前圈无效-0 =有效，1 =无效
	uint8 m_penalties; //要添加的累计时间惩罚（以秒为单位）
	uint8 m_gridPosition; //车辆开始比赛的网格位置
	uint8 m_driverStatus; //驾驶员状态-0 =在车库，1 =飞行圈
						  // 2 =出站圈，3 =入站圈，4 =赛道
	uint8 m_resultStatus; //结果状态-0 =无效，1 =无效，2 =有效
						  // 3 =完成，4 =失格，5 =未分类
						  // 6 =退休
};

//频率：游戏中的设定
struct PacketLapData
{
	PacketHeader m_header; //标题

	LapData m_lapData[20]; //赛道上所有汽车的圈数数据
};

//频率：事件发生时
struct PacketEventData
{
	PacketHeader m_header; //标题
	uint8 m_eventStringCode[4]; //事件字符串代码，参见下文
};

// PacketEventData的时间字符串代码
#define SSTA "SSTA"  // 在会话开始时发送
#define SEND "SEND"  // 在会话结束时发送


struct ParticipantData
{
	uint8 m_aiControlled; //车辆是由AI（1）还是由人（0）控制
	uint8 m_driverId; //驱动程序ID-请参阅附录
	uint8 m_teamId; //团队ID-参见附录
	uint8 m_raceNumber; //汽车的比赛编号
	uint8 m_nationality; //驾驶员的国籍
	char m_name[48]; //参与者名称为UTF-8格式-终止为null
					 //如果太长，将被…（U + 2026）截断
};

//每5秒
struct PacketParticipantsData
{
	PacketHeader m_header; //标题

	uint8 m_numCars; //数据中的汽车数量
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
	uint8 m_frontWing; //前翼航空
	uint8 m_rearWing; //后翼航空
	uint8 m_onThrottle; //节气门微分调节（百分比）
	uint8 m_offThrottle; //差速器关闭节气门（百分比）
	float m_frontCamber; //前外倾角（悬架几何形状）
	float m_rearCamber; //后方外倾角（悬架几何形状）
	float m_frontToe; //前脚趾角度（悬架几何形状）
	float m_rearToe; //后脚趾角度（悬架几何形状）
	uint8 m_frontSuspension; //前悬架
	uint8 m_rearSuspension; // 后悬挂
	uint8 m_frontAntiRollBar; //前防倾杆
	uint8 m_rearAntiRollBar; //前防倾杆
	uint8 m_frontSuspensionHeight; //前座高度
	uint8 m_rearSuspensionHeight; //后座高度
	uint8 m_brakePressure; //制动压力（百分比）
	uint8 m_brakeBias; //制动偏差（百分比）
	float m_frontTyrePressure; //前轮胎压力（PSI）
	float m_rearTyrePressure; //后轮胎压力（PSI）
	uint8 m_ballast; //镇流器
	float m_fuelLoad; //燃油负荷
};

//每5秒发送一次
struct PacketCarSetupData
{
	PacketHeader m_header; //标题
	CarSetupData m_carSetups[20];
};

//频率：菜单中指定的费率
struct CarTelemetryData
{
	uint16 m_speed; //车速
	uint8 m_throttle; //应用的油门量（0 - 100）
	int8 m_steer; //转向（-100（左全锁）到100（右全锁））
	uint8 m_brake; //施加的制动量（0到100）
	uint8 m_clutch; //施加的离合器数量（0到100）
	int8 m_gear; //挡位（1-8，N=0, R=-1）
	uint16 m_engineRPM; //引擎RPM
	uint8 m_drs; // 0 =关闭，1 =开启
	uint8 m_revLightsPercent; //换挡指示灯（百分比）
	uint16 m_brakesTemperature[4]; //制动温度（摄氏度）
	uint16 m_tyresSurfaceTemperature[4]; //轮胎表面温度（摄氏度）
	uint16 m_tyresInnerTemperature[4]; //轮胎内部温度（摄氏度）
	uint16 m_engineTemperature; //发动机温度（摄氏度）
	float m_tyresPressure[4]; //胎压（PSI）
};

struct PacketCarTelemetryData
{
	PacketHeader m_header; //标题

	CarTelemetryData m_carTelemetryData[20];

	uint32 m_buttonStatus; //位标志，指定正在使用的按钮
						   //当前按下-请参阅附录
};

struct CarStatusData
{
	uint8 m_tractionControl; // 0（关闭）-2（高）
	uint8 m_antiLockBrakes; // 0（关闭）-1（开启）
	uint8 m_fuelMix; //燃油混合-0 =稀油，1 =标准，2 =浓，3 =最大
	uint8 m_frontBrakeBias; //前刹车偏压（百分比）
	uint8 m_pitLimiterStatus; //坑限制器状态-0 =关闭，1 =开启
	float m_fuelInTank; //当前的燃料质量
	float m_fuelCapacity; // 燃料容量
	uint16 m_maxRPM; //汽车最高转速，转速限制器
	uint16 m_idleRPM; //汽车怠速RPM
	uint8 m_maxGears; //最大齿轮数
	uint8 m_drsAllowed; // 0 =不允许，1 =允许，-1 =未知
	uint8 m_tyresWear[4]; //轮胎磨损率
	uint8 m_tyreCompound; //现代-0 =超柔软，1 =超柔软
						  // 2 =超级软，3 =软，4 =中，5 =硬
						  // 6 =超硬，7 =中间，8 =湿
						  //经典-0-6 =干燥，7-8 =湿
	uint8 m_tyresDamage[4]; //轮胎损坏（百分比）
	uint8 m_frontLeftWingDamage; //左前翼的伤害（百分比）
	uint8 m_frontRightWingDamage; //右前翼的伤害（百分比）
	uint8 m_rearWingDamage; //后翼损坏（百分比）
	uint8 m_engineDamage; //引擎损坏（百分比）
	uint8 m_gearBoxDamage; //齿轮箱损坏（百分比）
	uint8 m_exhaustDamage; //排气损坏（百分比）
	int8 m_vehicleFiaFlags; // -1 =无效/未知，0 =无，1 =绿色
							// 2 =蓝色，3 =黄色，4 =红色
	float m_ersStoreEnergy; //焦耳的ERS能量存储
	uint8 m_ersDeployMode; // ERS部署模式，0 =无，1 =低，2 =中
						   // 3 =高，4 =超车，5 =热圈
	float m_ersHarvestedThisLapMGUK; // MGU-K在这一圈收获了ERS能量
	float m_ersHarvestedThisLapMGUH; // MGU-H在这一圈收获了ERS能量
	float m_ersDeployedThisLap; //在这一圈部署ERS能量
};

//每秒2次
struct PacketCarStatusData
{
	PacketHeader m_header; //标题

	CarStatusData m_carStatusData[20];
};

#pragma pack(pop)