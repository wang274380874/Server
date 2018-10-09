
#ifndef TABLE_FRAME_SINK_H_
#define TABLE_FRAME_SINK_H_
#pragma once

#include <vector>

struct FishTraceInfo {  
  int fish_id;
  FishKind fish_kind;
  DWORD build_tick;
};

struct SweepFishInfo {  
  int fish_id;
  int bullet_mulriple;
  FishKind fish_kind;
  BulletKind bullet_kind;  
};

struct ServerBulletInfo {  
  int bullet_id;
  int bullet_mulriple;
  BulletKind bullet_kind;
};

class TableFrameSink : public ITableFrameSink, public ITableUserAction {
 public:
  TableFrameSink();
  virtual ~TableFrameSink();

  virtual void Release() { delete this; }
  virtual void* QueryInterface(REFGUID guid, DWORD query_ver);

  virtual bool Initialization(IUnknownEx* unknownex);
  virtual void RepositionSink();

  virtual SCORE QueryConsumeQuota(IServerUserItem* server_user_item) { return 0; }
  virtual SCORE QueryLessEnterScore(WORD chair_id, IServerUserItem* server_user_item) { return 0; }
  virtual bool QueryBuckleServiceCharge(WORD chair_id) { return true; }
  virtual void SetGameBaseScore(LONG base_score) {}
  virtual bool OnDataBaseMessage(WORD request_id, void* data, WORD data_size) { return false; }
  virtual bool OnUserScroeNotify(WORD chair_id, IServerUserItem* server_user_item, BYTE reason) { return false; }

  virtual bool IsUserPlaying(WORD chair_id);

  virtual bool OnEventGameStart();
  virtual bool OnEventGameConclude(WORD chair_id, IServerUserItem* server_user_item, BYTE reason);
  virtual bool OnEventSendGameScene(WORD chair_id, IServerUserItem* server_user_item, BYTE game_status, bool send_secret);

  virtual bool OnTimerMessage(DWORD timer_id, WPARAM bind_param);
  virtual bool OnGameMessage(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* server_user_item);
  virtual bool OnFrameMessage(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* server_user_item);

  virtual bool OnActionUserSitDown(WORD chair_id, IServerUserItem* server_user_item, bool lookon_user);
  virtual bool OnActionUserStandUp(WORD chair_id, IServerUserItem* server_user_item, bool lookon_user);
  virtual bool OnActionUserOnReady(WORD chair_id, IServerUserItem * server_user_item, void* data, WORD data_size) { return true; }

 private:
  FishTraceInfo* ActiveFishTrace();
  bool FreeFishTrace(FishTraceInfo* fish_trace_info);
  void FreeAllFishTrace();
  FishTraceInfo* GetFishTraceInfo(int fish_id);

  void SaveSweepFish(FishKind fish_kind, int fish_id, BulletKind bullet_kind, int bullet_mulriple);
  bool FreeSweepFish(int fish_id);
  SweepFishInfo* GetSweepFish(int fish_id);

  ServerBulletInfo* ActiveBulletInfo(WORD chairid);
  bool FreeBulletInfo(WORD chairid, ServerBulletInfo* bullet_info);
  void FreeAllBulletInfo(WORD chairid);
  ServerBulletInfo* GetBulletInfo(WORD chairid, int bullet_id);

 private:
  void StartAllGameTimer();
  void KillAllGameTimer();
  bool LoadConfig();
  bool SendGameConfig(IServerUserItem* server_user_item);
  int GetNewFishID();
  int GetBulletID(WORD chairid);

  bool OnTimerBuildSmallFishTrace(WPARAM bind_param);
  bool OnTimerBuildMediumFishTrace(WPARAM bind_param);
  bool OnTimerBuildFish18Trace(WPARAM bind_param);
  bool OnTimerBuildFish19Trace(WPARAM bind_param);
  bool OnTimerBuildFish20Trace(WPARAM bind_param);
  bool OnTimerBuildFishLKTrace(WPARAM bind_param);
  bool OnTimerBuildFishBombTrace(WPARAM bind_param);
  bool OnTimerBuildFishSuperBombTrace(WPARAM bind_param);
  bool OnTimerBuildFishLockBombTrace(WPARAM bind_param);
  bool OnTimerBuildFishSanTrace(WPARAM bind_param);
  bool OnTimerBuildFishSiTrace(WPARAM bind_param);
  bool OnTimerBuildFishKingTrace(WPARAM bind_param);
  bool OnTimerClearTrace(WPARAM bind_param);
  bool OnTimerBulletIonTimeout(WPARAM bind_param);
  bool OnTimerLockTimeout(WPARAM bind_param);
  bool OnTimerSwitchScene(WPARAM bind_param);
  bool OnTimerSceneEnd(WPARAM bind_param);
  bool OnTimerLKScore(WPARAM bind_param);

  /////////////////////////////////////////////////////////
  // stock operate timer by Tony Fox
  bool OnTimerStockOperate(WPARAM bind_param);
  /////////////////////////////////////////////////////////

  bool SendTableData(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* exclude_user_item);
  bool OnSubExchangeFishScore(IServerUserItem* server_user_item, bool increase);
  bool OnSubUserFire(IServerUserItem* server_user_item, BulletKind bullet_kind, float angle, int bullet_mul, int lock_fishid);
  bool OnSubCatchFish(IServerUserItem* server_user_item, int fish_id, BulletKind bullet_kind, int bullet_id, int bullet_mul);
  bool OnSubCatchSweepFish(IServerUserItem* server_user_item, int fish_id, int* catch_fish_id, int catch_fish_count);
  bool OnSubHitFishLK(IServerUserItem* server_user_item, int fish_id);
  bool OnSubStockOperate(IServerUserItem* server_user_item, unsigned char operate_code);
  bool OnSubUserFilter(IServerUserItem* server_user_item, DWORD game_id, unsigned char operate_code);
  bool OnSubFish20Config(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability);

  void BuildInitTrace(FPoint init_pos[5], int init_count, FishKind fish_kind, TraceType trace_type);
  void BuildFishTrace(int fish_count, FishKind fish_kind_start, FishKind fish_kind_end);
  void BuildSceneKind1();
  void BuildSceneKind2();
  void BuildSceneKind3();
  void BuildSceneKind4();
  void BuildSceneKind5();
  void ClearFishTrace(bool force = false);

  void CalcScore(IServerUserItem* server_user_item);

 private:
  ITableFrame* table_frame_;
  tagGameServiceOption* game_service_option_;
  tagGameServiceAttrib* game_service_attrib_;
  SCORE base_score_;

  int stock_crucial_score_[10];
  double stock_increase_probability_[10];
  int stock_crucial_count_;

  int exchange_ratio_userscore_;
  int exchange_ratio_fishscore_;
  int exchange_count_;

  int min_bullet_multiple_;
  int max_bullet_multiple_;
  int bomb_stock_;
  int super_bomb_stock_;
  int fish_speed_[FISH_KIND_COUNT];
  int fish_multiple_[FISH_KIND_COUNT];
  int fish18_max_multiple_;
  int fish19_max_multiple_;
  int fishLK_max_multiple_;
  int fish_bounding_box_width_[FISH_KIND_COUNT];
  int fish_bounding_box_height_[FISH_KIND_COUNT];
  int fish_hit_radius_[FISH_KIND_COUNT];
  double fish_capture_probability_[FISH_KIND_COUNT];
  int bomb_range_width_;
  int bomb_range_height_;

  int bullet_speed_[BULLET_KIND_COUNT];
  int net_radius_[BULLET_KIND_COUNT];
  int net_bounding_box_width_[BULLET_KIND_COUNT];
  int net_bounding_box_height_[BULLET_KIND_COUNT];

  typedef std::vector<FishTraceInfo*> FishTraceInfoVecor;
  FishTraceInfoVecor active_fish_trace_vector_;
  FishTraceInfoVecor storage_fish_trace_vector_;

  std::vector<SweepFishInfo> sweep_fish_info_vector_;

  typedef std::vector<ServerBulletInfo*> ServerBulletInfoVector;
  ServerBulletInfoVector server_bullet_info_vector_[GAME_PLAYER];
  ServerBulletInfoVector storage_bullet_info_vector_;

  int fish_id_;
  SCORE exchange_fish_score_[GAME_PLAYER];
  SCORE fish_score_[GAME_PLAYER];

  int bullet_id_[GAME_PLAYER];
  int bullet_energy[GAME_PLAYER];

  SceneKind next_scene_kind_;
  bool special_scene_;
  int current_fish_lk_multiple_;

  WORD android_chairid_;

  ///////////////////////////////////////////////////////////////////////////////////
  // 新库存系统 by Tony Fox
  int m_nTableID;
  SCORE m_pChairsStock[GAME_PLAYER]; // 座位库存
  int m_nTableRandom; // 桌台库存上限
  int m_nTimeSum; // 计时器
  int m_pStockRandom[GAME_PLAYER]; // 归档随机时间(秒)
  SCORE m_lBonusStock; // 彩金
  SCORE m_lBonusRandom; // 发放彩金随机数
  SCORE m_lTempBonus; // 临时彩金池
  int m_pGunsScore[GAME_PLAYER]; // 子弹分数记录
  int m_nBonusRevenue; // 彩金抽水百分比
  int m_nBonusElapsed; // 距上一次重设抽水百分比经过的时间
  int m_nBonusReCalc; // 重新计算彩金抽水
  int m_nCurrentPlan; // 方案，方案A记作0，方案B记作1
  int m_nNextPlan; // 下一次方案，在第一次进入系统时默认为0，如果为-1则进行1D100，超过60使用方案B（1），否则使用方案A（0）
  int m_nDeployBonusTime; // 发放奖金的随机时间，单位：秒钟
  int m_nDeployBonusRandom; // 随机发放彩金时间，单位：分钟（2-30分钟）
  int m_nTableStockPlan;
  int m_nLastDestributePlan;
  int m_lpSortedIndexes[GAME_PLAYER]; // 排序后的索引，如果 1d100 大于 90 则更换索引
  bool m_isFirstTimeDistribute; // 是否为第一次发放库存
  int m_nResortRnd; // 重新排序的随机数
  void DistributePlanA(SCORE, SCORE[], int[]);
  void DistributePlanB(SCORE, SCORE[], int[]);
  void DistributePlanC(SCORE, SCORE[], int[]);
  ///////////////////////////////////////////////////////////////////////////////////
};

#endif // TABLE_FRAME_SINK_H_
