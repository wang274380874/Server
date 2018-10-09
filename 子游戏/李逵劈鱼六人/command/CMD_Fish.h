
#ifndef CMD_FISH_H_
#define CMD_FISH_H_

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
// 服务定义

//edit by yang yang  2015年10月9日  星期五  14:26:46
#define  DEF_GAME_PLATFORM_CHANGYOU           1
#define  DEF_GAME_PLATFORM_LONGJIANG          2

#define KIND_ID               510
#define GAME_NAME             TEXT("捕鱼六人")
#define GAME_PLAYER           6
#define VERSION_SERVER        PROCESS_VERSION(6,0,3)       // PROCESS_VERSION(1,0,0)
#define VERSION_CLIENT        PROCESS_VERSION(6,0,3)       // PROCESS_VERSION(1,0,0)

// 简单的版本检测
// 如果客户端有更新应该更改这个值.如果只修改EXE的版本而网络数据包没修改的话，更新客户端是没什么意义的，因为EXE的版本是可以直接被修改的
// 只要保存以前的EXE版本,更新完之后再换回去，如果有BUG的话等于还是没修复。
#define GAME_VERSION          30

#ifndef SCORE
#define SCORE LONGLONG
#endif

const int kResolutionWidth = 1366;
const int kResolutionHeight = 768;

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#define M_PI_2  1.57079632679489661923f
#define M_PI_4  0.785398163397448309616f
#define M_1_PI  0.318309886183790671538f
#define M_2_PI  0.636619772367581343076f
#endif
const float kChairDefaultAngle[GAME_PLAYER] = { M_PI, M_PI, M_PI, 0, 0, 0 };

enum TraceType {
  TRACE_LINEAR = 0,
  TRACE_BEZIER
};

struct FPoint {
  float x;
  float y;
};

struct FPointAngle {
  float x;
  float y;
  float angle;
};

//////////////////////////////////////////////////////////////////////////
// 游戏定义

/*
// 座位号
-------------
    0   1   2 
7               3                
    6   5   4
-------------
*/

enum SceneKind {
  SCENE_KIND_1 = 0,
  SCENE_KIND_2,
  SCENE_KIND_3,
  SCENE_KIND_4,
  //SCENE_KIND_5,

  SCENE_KIND_COUNT
};

enum FishKind {
  FISH_KIND_1 = 0,
  FISH_KIND_2,
  FISH_KIND_3,
  FISH_KIND_4,
  FISH_KIND_5,
  FISH_KIND_6,
  FISH_KIND_7,
  FISH_KIND_8,
  FISH_KIND_9,
  FISH_KIND_10,
  FISH_KIND_11,
  FISH_KIND_12,
  FISH_KIND_13,
  FISH_KIND_14,
  FISH_KIND_15,
  FISH_KIND_16,
  FISH_KIND_17,
  FISH_KIND_18,
  FISH_KIND_19,
  FISH_KIND_20, // 企鹅
  FISH_KIND_21, // 李逵
  FISH_KIND_22, // 定屏炸弹
  FISH_KIND_23, // 局部炸弹
  FISH_KIND_24, // 超级炸弹
  FISH_KIND_25, // 大三元1
  FISH_KIND_26, // 大三元2
  FISH_KIND_27, // 大三元3
  FISH_KIND_28, // 大四喜1
  FISH_KIND_29, // 大四喜2
  FISH_KIND_30, // 大四喜3
  FISH_KIND_31, // 鱼王1
  FISH_KIND_32, // 鱼王2
  FISH_KIND_33, // 鱼王3
  FISH_KIND_34, // 鱼王4
  FISH_KIND_35, // 鱼王5
  FISH_KIND_36, // 鱼王6
  FISH_KIND_37, // 鱼王7
  FISH_KIND_38, // 鱼王8
  FISH_KIND_39, // 鱼王9
  FISH_KIND_40, // 鱼王10

  FISH_KIND_COUNT
};


enum BulletKind
{
	BULLET_KIND_1_NORMAL = 0,
	BULLET_KIND_2_NORMAL,
	BULLET_KIND_3_NORMAL,
	BULLET_KIND_4_NORMAL,
#if(DEF_GAME_PLATFORM_VERSION == DEF_GAME_PLATFORM_LONGJIANG)
	BULLET_KIND_5_NORMAL,
	BULLET_KIND_6_NORMAL,
	BULLET_KIND_7_NORMAL,
	BULLET_KIND_8_NORMAL,
	BULLET_KIND_9_NORMAL,
	BULLET_KIND_10_NORMAL,
#endif
	BULLET_KIND_1_ION,
	BULLET_KIND_2_ION,
	BULLET_KIND_3_ION,
	BULLET_KIND_4_ION,
#if(DEF_GAME_PLATFORM_VERSION == DEF_GAME_PLATFORM_LONGJIANG)
	BULLET_KIND_5_ION,
	BULLET_KIND_6_ION,
	BULLET_KIND_7_ION,
	BULLET_KIND_8_ION,
	BULLET_KIND_9_ION,
	BULLET_KIND_10_ION,
#endif

	BULLET_KIND_COUNT
};


const DWORD kBulletIonTime = 20;
const DWORD kLockTime = 10;

const int kMaxCatchFishCount = 2;

// 限制客户端的鱼种类数量
//const int kFishKindCount[FISH_KIND_COUNT] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

//////////////////////////////////////////////////////////////////////////
// 服务端命令

#define SUB_S_SCENE_END							1700
#define SUB_S_STOCK_OPERATE_RESULT              1701
#define SUB_S_SWITCH_SCENE						1702
#define SUB_S_HIT_FISH_LK						1703
#define SUB_S_CATCH_SWEEP_FISH_RESULT           1704
#define SUB_S_CATCH_SWEEP_FISH					1705
#define SUB_S_LOCK_TIMEOUT						1706
#define SUB_S_BULLET_ION_TIMEOUT				1707
#define SUB_S_CATCH_FISH						1708
#define SUB_S_USER_FIRE							1709
#define SUB_S_EXCHANGE_FISHSCORE                1710
#define SUB_S_FISH_TRACE						1711
#define SUB_S_GAME_CONFIG						1712

struct CMD_S_GameStatus {
	BYTE cbBackIndex;
	SCORE lPlayScore;
	SCORE lPlayCurScore[GAME_PLAYER];
	SCORE lPlayStartScore[GAME_PLAYER];
	int lCellScore;
	int nBulletVelocity;
	int nBulletCoolingTime;
	int nFishMultiple[26][2];
	int nMaxTipsCount;
	SCORE lBulletConsume[GAME_PLAYER];
	int lPlayFishCount[26][6];
	int nMultipleValue[GAME_PLAYER];
	int nMultipleIndex[GAME_PLAYER];
	bool bUnlimitedRebound;
	unsigned char szBrowseUrl;
  //DWORD game_version;  
  //SCORE fish_score[GAME_PLAYER];
  //SCORE exchange_fish_score[GAME_PLAYER];
  //bool null_data;
};

struct CMD_S_GameConfig {
  int exchange_ratio_userscore;
  int exchange_ratio_fishscore;
  int exchange_count;
  bool null_data;

  int min_bullet_multiple;
  int max_bullet_multiple;

  int bomb_range_width;
  int bomb_range_height;

  int fish_multiple[FISH_KIND_COUNT];
  int fish_speed[FISH_KIND_COUNT];
  int fish_bounding_box_width[FISH_KIND_COUNT];
  int fish_bounding_box_height[FISH_KIND_COUNT];
  int fish_hit_radius[FISH_KIND_COUNT];

  int bullet_speed[BULLET_KIND_COUNT];
  int net_radius[BULLET_KIND_COUNT];
};

struct CMD_S_FishTrace {  
  int init_count;  
  int fish_id;
  bool null_data;
  TraceType trace_type;
  FishKind fish_kind;
  FPoint init_pos[5];
};

struct CMD_S_ExchangeFishScore {
  WORD chair_id;
  bool null_data;
  SCORE exchange_fish_score;
  SCORE swap_fish_score;  
};

struct CMD_S_UserFire {  
  bool null_data;
  WORD chair_id;
  int bullet_id;  
  int bullet_mulriple;
  int lock_fishid;
  WORD android_chairid;
  float angle;  
  SCORE fish_score;
  BulletKind bullet_kind;
};

struct CMD_S_CatchFish {
  WORD chair_id;
  SCORE fish_score;
  int fish_id;  
  bool bullet_ion;  
  FishKind fish_kind;
  bool null_data;
};

struct CMD_S_BulletIonTimeout {
  WORD chair_id;
};

struct CMD_S_CatchSweepFish {  
  int fish_id;
  WORD chair_id;
  bool null_data;
};

struct CMD_S_CatchSweepFishResult {
  int fish_id;
  WORD chair_id;  
  SCORE fish_score;
  int catch_fish_count;
  int catch_fish_id[300];
  bool null_data;
};

struct CMD_S_HitFishLK {  
  int fish_id;
  WORD chair_id;
  int fish_mulriple;
  bool null_data;
};

struct CMD_S_SwitchScene {  
  int fish_count;
  SceneKind scene_kind;
  FishKind fish_kind[300];
  int fish_id[300];
  bool null_data;
};

struct CMD_S_StockOperateResult {
  unsigned char operate_code;
  SCORE stock_score;
  bool null_data;
};

//////////////////////////////////////////////////////////////////////////
// 客户端命令

#define SUB_C_ANDROID_BULLET_MUL            71
#define SUB_C_FISH20_CONFIG                 72
#define SUB_C_ANDROID_STAND_UP              73
#define SUB_C_USER_FILTER					74
#define SUB_C_STOCK_OPERATE                 75
#define SUB_C_HIT_FISH_I					76
#define SUB_C_CATCH_SWEEP_FISH              77
#define SUB_C_CATCH_FISH					78
#define SUB_C_USER_FIRE		                79
#define SUB_C_EXCHANGE_FISHSCORE            80

struct CMD_C_ExchangeFishScore {
  bool null_data;
  bool increase;
};

struct CMD_C_UserFire {    
  bool null_data;
  int bullet_mulriple;
  int lock_fishid;
  float angle;
  BulletKind bullet_kind;
};

struct CMD_C_CatchFish {
  WORD chair_id;
  bool null_data;
  int bullet_id;
  int fish_id;    
  int bullet_mulriple;
  BulletKind bullet_kind;
};

struct CMD_C_CatchSweepFish {  
  int fish_id;
  WORD chair_id;
  bool null_data;
  int catch_fish_count;
  int catch_fish_id[300];
};

struct CMD_C_HitFishLK {
  int fish_id;
};

struct CMD_C_StockOperate {
  unsigned char operate_code; // 0查询 1 清除 2 增加 3 查询抽水
  bool null_data;
};

struct CMD_C_UserFilter {
  unsigned char operate_code; // 0 黑名单 1 白名单 2 清除
  bool null_data;
  DWORD game_id;  
};

struct CMD_C_Fish20Config {  
  int catch_count;
  DWORD game_id;
  double catch_probability;
};

struct CMD_C_AndroidBulletMul {
  WORD chair_id;
  int bullet_mulriple;
  int bullet_id;  
  BulletKind bullet_kind;
  bool null_data;
};

#pragma pack()

#endif // CMD_FISH_H_