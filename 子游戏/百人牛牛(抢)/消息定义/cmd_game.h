#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

#define UR_GAME_CONTROL					0x20000000L				//游戏特殊控制
//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID						104									//游戏 I D
#define GAME_NAME					TEXT("百人牛牛")					//游戏名字

//组件属性
#define GAME_PLAYER					MAX_CHAIR									//游戏人数
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//程序版本

//////////////////////////////////////////////////////////////////////////////////
//状态定义

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//等待开始
#define GAME_SCENE_PLACE_JETTON		GAME_STATUS_PLAY					//下注状态
#define GAME_SCENE_GAME_END			GAME_STATUS_PLAY+1					//结束状态


//区域索引
#define ID_TIAN_MEN					1									//天
#define ID_DI_MEN					2									//地
#define ID_XUAN_MEN					3									//玄
#define ID_HUANG_MEN				4									//黄

//玩家索引
#define BANKER_INDEX				0									//庄家索引
#define SHUN_MEN_INDEX				1									//顺门索引
#define DUI_MEN_INDEX				2									//对门索引
#define DAO_MEN_INDEX				3									//倒门索引
#define HUAN_MEN_INDEX				4									//倒门索引
#define MAX_INDEX					3									//最大索引

#define AREA_COUNT					4									//区域数目
#define CONTROL_AREA				4

//赔率定义
#define RATE_TWO_PAIR				12									//对子赔率
#define SERVER_LEN					32									//房间长度

//占座
#define SEAT_LEFT1_INDEX			0									//左一
#define SEAT_LEFT2_INDEX			1									//左二
#define SEAT_LEFT3_INDEX			2									//左三
#define SEAT_RIGHT1_INDEX			3									//右一
#define SEAT_RIGHT2_INDEX			4									//右二
#define SEAT_RIGHT3_INDEX			5									//右三
#define MAX_OCCUPY_SEAT_COUNT		6									//最大占位个数
#define SEAT_INVALID_INDEX			7									//无效索引

////////////////////////////////////////////////////////////////////////////////////
//宏定义
#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif

//超级抢庄
///////////////////////////////

//超级抢庄配置
typedef enum
{
	SUPERBANKER_VIPTYPE,
	SUPERBANKER_CONSUMETYPE
}SUPERBANKERTYPE;

//占位配置
typedef enum
{
	OCCUPYSEAT_VIPTYPE,
	OCCUPYSEAT_CONSUMETYPE,
	OCCUPYSEAT_FREETYPE
}OCCUPYSEATTYPE;

typedef enum
{
	VIP1_INDEX = 1,  //cbMemberOrder = 1 蓝钻会员
	VIP2_INDEX,		 //黄钻会员
	VIP3_INDEX,		 //白钻会员
	VIP4_INDEX,		 //红钻会员
	VIP5_INDEX,		 //VIP会员
	VIP_INVALID		 //无效
}VIPINDEX;

typedef struct
{
	SUPERBANKERTYPE		superbankerType;		//抢庄类型
	VIPINDEX			enVipIndex;				//VIP索引
	SCORE				lSuperBankerConsume;	//抢庄消耗
}SUPERBANKERCONFIG;

typedef struct
{
	OCCUPYSEATTYPE		occupyseatType;			//占位类型
	VIPINDEX			enVipIndex;				//VIP索引
	SCORE				lOccupySeatConsume;	    //占位消耗
	SCORE				lOccupySeatFree;	    //免费占位金币上限
	SCORE				lForceStandUpCondition;	//强制站立条件
}OCCUPYSEATCONFIG;

typedef enum
{
	ORDINARY_BANKER,		//普通庄家
	SUPERROB_BANKER,		//超级抢庄庄家
	INVALID_SYSBANKER		//无效类型(系统庄家)
}CURRENT_BANKER_TYPE;

/////////////////////////////////////////

struct tagCustomAndroid
{
	//坐庄
	bool							nEnableRobotBanker;				//是否做庄
	LONGLONG						lRobotBankerCountMin;			//坐庄次数
	LONGLONG						lRobotBankerCountMax;			//坐庄次数
	LONGLONG						lRobotListMinCount;				//列表人数
	LONGLONG						lRobotListMaxCount;				//列表人数
	LONGLONG						lRobotApplyBanker;				//最多申请个数
	LONGLONG						lRobotWaitBanker;				//空盘重申

	//下注
	LONGLONG						lRobotMinBetTime;				//下注筹码个数
	LONGLONG						lRobotMaxBetTime;				//下注筹码个数
	LONGLONG						lRobotMinJetton;				//下注筹码金额
	LONGLONG						lRobotMaxJetton;				//下注筹码金额
	LONGLONG						lRobotBetMinCount;				//下注机器人数
	LONGLONG						lRobotBetMaxCount;				//下注机器人数
	LONGLONG						lRobotAreaLimit;				//区域限制

	//存取款
	LONGLONG						lRobotScoreMin;					//金币下限
	LONGLONG						lRobotScoreMax;					//金币上限
	LONGLONG						lRobotBankGetMin;				//取款最小值(非庄)
	LONGLONG						lRobotBankGetMax;				//取款最大值(非庄)
	LONGLONG						lRobotBankGetBankerMin;			//取款最小值(坐庄)
	LONGLONG						lRobotBankGetBankerMax;			//取款最大值(坐庄)
	LONGLONG						lRobotBankStoMul;				//存款百分比

	//构造函数
	tagCustomAndroid()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		nEnableRobotBanker = true;
		lRobotBankerCountMin = 5;
		lRobotBankerCountMax = 10;
		lRobotListMinCount = 2;
		lRobotListMaxCount = 5;
		lRobotApplyBanker = 5;
		lRobotWaitBanker = 3;

		lRobotMinBetTime = 6;
		lRobotMaxBetTime = 8;
		lRobotMinJetton = 100;
		lRobotMaxJetton = 5000000;
		lRobotBetMinCount = 4;
		lRobotBetMaxCount = 8;
		lRobotAreaLimit = 10000000;
		
		lRobotScoreMin = 1000000;
		lRobotScoreMax = 100000000;
		lRobotBankGetMin = 100;
		lRobotBankGetMax = 30000000;
		lRobotBankGetBankerMin = 10000000;
		lRobotBankGetBankerMax = 50000000;
		lRobotBankStoMul = 50;
	}
};

//机器人信息
struct tagRobotInfo
{
	int nChip[8];														//筹码定义
	int nAreaChance[AREA_COUNT];										//区域几率
	TCHAR szCfgFileName[MAX_PATH];										//配置文件
	int	nMaxTime;														//最大赔率

	tagRobotInfo()
	{
		int nTmpChip[8] = {100, 1000, 10000, 50000, 100000, 500000, 1000000, 5000000};
		int nTmpAreaChance[AREA_COUNT] = {1, 1, 1, 1};

		nMaxTime = 10;
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
	}
};

//记录信息
struct tagServerGameRecord
{
	bool							bWinShunMen;						//顺门胜利
	bool							bWinDuiMen;							//对门胜利
	bool							bWinDaoMen;							//倒门胜利
	bool							bWinHuang;							//倒门胜利
};

//下注信息
struct tagUserBet
{
	TCHAR							szNickName[32];						//用户昵称
	DWORD							dwUserGameID;						//用户ID
	LONGLONG						lUserStartScore;					//用户金币
	LONGLONG						lUserWinLost;						//用户金币
	LONGLONG						lUserBet[AREA_COUNT+1];				//用户下注
};

//下注信息数组
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//库存控制
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2
//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				99									//游戏空闲
#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			104									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			106									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_CANCEL_BANKER			108									//取消申请


#define SUB_S_AMDIN_COMMAND			110									//管理员命令
#define SUB_S_UPDATE_STORAGE        111									//更新库存
#define SUB_S_SEND_USER_BET_INFO    112									//发送下注
#define SUB_S_ADVANCE_OPENCARD		113									//提前开牌

#define SUB_S_SUPERROB_BANKER		114									//超级抢庄
#define SUB_S_CURSUPERROB_LEAVE		115									//超级抢庄玩家离开
#define SUB_S_OCCUPYSEAT			116									//占位
#define SUB_S_OCCUPYSEAT_FAIL		117									//占位失败
#define SUB_S_UPDATE_OCCUPYSEAT		118									//更新占位

//超级抢庄
struct CMD_S_SuperRobBanker
{
	bool					bSucceed;
	WORD					wApplySuperRobUser;
	WORD					wCurSuperRobBankerUser;
};

//超级抢庄玩家离开
struct CMD_S_CurSuperRobLeave
{
	WORD					wCurSuperRobBankerUser;
};


//更新库存
struct CMD_S_UpdateStorage
{
	BYTE                            cbReqType;						//请求类型
	LONGLONG						lStorageStart;				//起始库存
	LONGLONG						lStorageDeduct;				//库存衰减
	LONGLONG						lStorageCurrent;				//当前库存
	LONGLONG						lStorageMax1;					//库存上限1
	LONGLONG						lStorageMul1;					//系统输分概率1
	LONGLONG						lStorageMax2;					//库存上限2
	LONGLONG						lStorageMul2;					//系统输分概率2
};

//发送下注
struct CMD_S_SendUserBetInfo
{
	LONGLONG						lUserStartScore[GAME_PLAYER];				//起始分数
	LONGLONG						lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];//个人总注
};

//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
	BYTE cbResult;
#define CR_ACCEPT  2			//接受
#define CR_REFUSAL 3			//拒绝
	BYTE cbExtendData[20];			//附加数据
};
//失败结构
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lJettonArea;						//下注区域
	LONGLONG						lPlaceScore;						//当前下注
};

//更新积分
struct CMD_S_ChangeUserScore
{
	WORD							wChairID;							//椅子号码
	DOUBLE							lScore;								//玩家积分

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	DOUBLE							lCurrentBankerScore;				//庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	WORD							wCancelUser;					//取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//当庄玩家
	LONGLONG						lBankerScore;						//庄家金币
	CURRENT_BANKER_TYPE				typeCurrentBanker;					//当前庄家类型
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//玩家信息
	LONGLONG						lUserMaxScore;						//玩家金币

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家成绩
	LONGLONG						lBankerScore;						//庄家分数
	INT								nEndGameMul;						//提前开牌百分比
	bool							bEnableSysBanker;					//系统做庄

	//控制信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
	bool							bGenreEducate;						//练习模式
	
	SUPERBANKERCONFIG				superbankerConfig;					//抢庄配置
	WORD							wCurSuperRobBankerUser;
	CURRENT_BANKER_TYPE				typeCurrentBanker;					//当前庄家类型

	OCCUPYSEATCONFIG				occupyseatConfig;							//占位配置
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID

	tagCustomAndroid				CustomAndroid;						//机器人配置
};

//游戏状态
struct CMD_S_StatusPlay
{
	//全局下注
	LONGLONG						lAllJettonScore[AREA_COUNT+1];		//全体总注

	//玩家下注
	LONGLONG						lUserJettonScore[AREA_COUNT+1];		//个人总注

	//玩家积分
	LONGLONG						lUserMaxScore;						//最大下注							

	//控制信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制

	//扑克信息
	BYTE							cbTableCardArray[5][5];				//桌面扑克

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家赢分
	LONGLONG						lBankerScore;						//庄家分数
	INT								nEndGameMul;						//提前开牌百分比
	bool							bEnableSysBanker;					//系统做庄

	//结束信息
	LONGLONG						lEndBankerScore;					//庄家成绩
	LONGLONG						lEndUserScore;						//玩家成绩
	LONGLONG						lEndUserReturnScore;				//返回积分
	LONGLONG						lEndRevenue;						//游戏税收

	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态

	//房间信息
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称
	bool							bGenreEducate;						//练习模式
	
	SUPERBANKERCONFIG				superbankerConfig;					//抢庄配置
	WORD							wCurSuperRobBankerUser;
	CURRENT_BANKER_TYPE				typeCurrentBanker;					//当前庄家类型

	OCCUPYSEATCONFIG				occupyseatConfig;							//占位配置
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID

	LONGLONG						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];	//占位玩家成绩

	tagCustomAndroid				CustomAndroid;						//机器人配置
};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
	INT64                           nListUserCount;						//列表人数
	LONGLONG						lStorageStart;						//
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家位置
	LONGLONG						lBankerScore;						//庄家金币
	LONGLONG						lUserMaxScore;						//我的金币
	BYTE							cbTimeLeave;						//剩余时间	
	bool							bContiueCard;						//继续发牌
	int								nChipRobotCount;					//人数上限 (下注机器人)
	int								nAndriodApplyCount;					//机器人列表人数

	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
	bool							bIsAndroid;							//是否机器人
	bool							bAndroid;						//机器标识
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	BYTE							cbTimeLeave;						//剩余时间

	//扑克信息
	BYTE							cbTableCardArray[5][5];				//桌面扑克
	BYTE							cbLeftCardCount;					//扑克数目

	BYTE							bcFirstCard;

	//庄家信息
	LONGLONG						lBankerScore;						//庄家成绩
	LONGLONG						lBankerTotallScore;					//庄家成绩
	INT								nBankerTime;						//做庄次数

	//玩家成绩
	LONGLONG						lUserScore;							//玩家成绩
	LONGLONG						lUserReturnScore;					//返回积分

	//全局信息
	LONGLONG						lRevenue;							//游戏税收

	LONGLONG						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];	//占位玩家成绩
};

//占位
struct CMD_S_OccupySeat
{
	WORD							wOccupySeatChairID;							//申请占位玩家ID
	BYTE							cbOccupySeatIndex;							//占位索引
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
};

//占位失败
struct CMD_S_OccupySeat_Fail
{
	WORD							wAlreadyOccupySeatChairID;					//已申请占位玩家ID
	BYTE							cbAlreadyOccupySeatIndex;					//已占位索引
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
};

//更新占位
struct CMD_S_UpdateOccupySeat
{
	WORD							wOccupySeatChairIDArray[MAX_OCCUPY_SEAT_COUNT];	//占位椅子ID
	WORD							wQuitOccupySeatChairID;						//申请退出占位玩家
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_BANKER			2									//申请庄家
#define SUB_C_CANCEL_BANKER			3									//取消申请
#define SUB_C_CONTINUE_CARD			4									//继续发牌
#define SUB_C_AMDIN_COMMAND			5									//管理员命令
#define SUB_C_UPDATE_STORAGE        6									//更新库存

#define SUB_C_SUPERROB_BANKER		7									//超级抢庄
#define SUB_C_OCCUPYSEAT			8									//占位
#define SUB_C_QUIT_OCCUPYSEAT		9									//退出占位


//客户端消息
#define IDM_ADMIN_COMMDN			WM_USER+1000
#define IDM_UPDATE_STORAGE			WM_USER+1001

//控制区域信息
struct tagControlInfo
{
	BYTE cbControlArea[MAX_INDEX];			//控制区域
};

struct tagAdminReq
{
	BYTE							m_cbExcuteTimes;					//执行次数
	BYTE							m_cbControlStyle;					//控制方式
#define		CS_BANKER_LOSE    1
#define		CS_BANKER_WIN	  2
#define		CS_BET_AREA		  3
	bool							m_bWinArea[3];						//赢家区域
};


struct CMD_C_AdminReq
{
	BYTE cbReqType;
#define RQ_SET_WIN_AREA		1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//附加数据
};
//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
};

//更新库存
struct CMD_C_UpdateStorage
{
	BYTE                            cbReqType;						//请求类型
	LONGLONG						lStorageDeduct;					//库存衰减
	LONGLONG						lStorageCurrent;				//当前库存
	LONGLONG						lStorageMax1;					//库存上限1
	LONGLONG						lStorageMul1;					//系统输分概率1
	LONGLONG						lStorageMax2;					//库存上限2
	LONGLONG						lStorageMul2;					//系统输分概率2
};

//占位
struct CMD_C_OccupySeat
{
	WORD							wOccupySeatChairID;				//占位玩家
	BYTE							cbOccupySeatIndex;				//占位索引
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif