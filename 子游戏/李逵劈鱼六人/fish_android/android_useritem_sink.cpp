#include "Stdafx.h"
#include "android_useritem_sink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

// 定时器标识
#define IDI_SWITCH_SCENE				  1									// 切换场景不能打炮
#define IDI_EXCHANGE_FISHSCORE    2                 // 交换子弹
#define IDI_FIRE                  3
#define IDI_STAND_UP              4

// 时间标识
#define TIME_SWITCH_SCENE				  7									// 切换场景不能打炮 大概7秒钟 客户端有修改 这个应该也要修改
#define TIME_EXCHANGE_FISHSCORE   2
#define TIME_FIRE                 2											// 这里不能改成1 会造成死循环, 如果想要加快子弹发射速度只能改平台.

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
//	OutputDebugString(_T("CAndroidUserItemSink::CAndroidUserItemSink()"));
  exchange_ratio_userscore_ = 1;
  exchange_ratio_fishscore_ = 1;
  exchange_count_ = 1;
  exchange_times_ = 1;
  allow_fire_ = false;
  current_bullet_kind_ = BULLET_KIND_2_NORMAL;
  current_bullet_mulriple_ = 10;
  exchange_fish_score_ = 0;
  fish_score_ = 0;
  last_fire_angle_ = 0.f;
  min_bullet_multiple_ = 1;
  max_bullet_multiple_ = 9900;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
//	OutputDebugString(_T("CAndroidUserItemSink::Initialization()"));
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

  //srand((UINT)time(NULL));
//	OutputDebugString(_T("CAndroidUserItemSink::Initialization() end"));
	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	TCHAR szOut[128];
	sprintf_yy(szOut, _T("CAndroidUserItemSink::OnEventTimer  nTimerID = %d"), nTimerID);
//	OutputDebugString(szOut);
//	OutputDebugString(_T("CAndroidUserItemSink::OnEventTimer()"));
	switch (nTimerID) {
    case IDI_SWITCH_SCENE:
      allow_fire_ = true;
      return true;
    case IDI_EXCHANGE_FISHSCORE:
      ExchangeFishScore();
      return true;
    case IDI_FIRE:
      Fire();
      return true;
    case IDI_STAND_UP:
      m_pIAndroidUserItem->SendSocketData(SUB_C_ANDROID_STAND_UP);
      return true;
	}
//	OutputDebugString(_T("CAndroidUserItemSink::OnEventTimer() end"));
	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD sub_cmdid, void * data, WORD data_size)
{
//	OutputDebugString(_T("CAndroidUserItemSink::OnEventGameMessage()"));
  switch (sub_cmdid) {
    case SUB_S_GAME_CONFIG:
      return OnSubGameConfig(data, data_size);
    case SUB_S_EXCHANGE_FISHSCORE:
      return OnSubExchangeFishScore(data, data_size);
    case SUB_S_USER_FIRE:
      return OnSubUserFire(data, data_size);
    case SUB_S_CATCH_FISH:
      return OnSubCatchFish(data, data_size);
    case SUB_S_BULLET_ION_TIMEOUT:
      return OnSubBulletIonTimeout(data, data_size);
    case SUB_S_CATCH_SWEEP_FISH_RESULT:
      return OnSubCatSweepFishResult(data, data_size);
    case SUB_S_SWITCH_SCENE:
      return OnSubSwitchScene(data, data_size);
  }
//  OutputDebugString(_T("CAndroidUserItemSink::OnEventGameMessage() end"));
	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
//	OutputDebugString(_T("CAndroidUserItemSink::OnEventFrameMessage()"));
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * data, WORD data_size)
{
//	OutputDebugString(_T("CAndroidUserItemSink::OnEventSceneMessage()"));
  switch (cbGameStatus) {
    case GAME_STATUS_FREE:
    case GAME_STATUS_PLAY:
      ASSERT(data_size == sizeof(CMD_S_GameStatus));
      if (data_size != sizeof(CMD_S_GameStatus)) return false;
      CMD_S_GameStatus* gamestatus = static_cast<CMD_S_GameStatus*>(data);
      if (gamestatus->game_version != GAME_VERSION) return false;

      fish_score_ = 0;
      exchange_fish_score_ = 0;

      srand((DWORD)time(NULL)+m_pIAndroidUserItem->GetTableID()*100+m_pIAndroidUserItem->GetChairID());
      if (max_bullet_multiple_ >= 100 && max_bullet_multiple_ <= 1000) {
        int n = max_bullet_multiple_ / 100;
        int nRand = rand() % (n + 1);
        if (nRand == 0) {
          current_bullet_mulriple_ = min_bullet_multiple_;
        } else {
          current_bullet_mulriple_ = nRand * 100;
          if (current_bullet_mulriple_ > max_bullet_multiple_) current_bullet_mulriple_ = max_bullet_multiple_;
        }
      } else if (max_bullet_multiple_ > 1000) {
        int n = max_bullet_multiple_ / 1000;
        int nRand = rand() % (n + 2);
        if (nRand == 0) {
          current_bullet_mulriple_ = min_bullet_multiple_;
        } else {
          current_bullet_mulriple_ = nRand * 1000;
          if (current_bullet_mulriple_ > max_bullet_multiple_) current_bullet_mulriple_ = max_bullet_multiple_;
        }
      } else {
        current_bullet_mulriple_ = min_bullet_multiple_;
      }
	  if(max_bullet_multiple_==min_bullet_multiple_)
		current_bullet_mulriple_ = min_bullet_multiple_;

      if (current_bullet_mulriple_ < 100) {
        current_bullet_kind_ = BULLET_KIND_1_NORMAL;
      } else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) {
        current_bullet_kind_ = BULLET_KIND_2_NORMAL;
      } else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) {
        current_bullet_kind_ = BULLET_KIND_3_NORMAL;
      } else {
        current_bullet_kind_ = BULLET_KIND_4_NORMAL;
      }

      allow_fire_ = true;

      SCORE android_score = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
      SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
      SCORE user_leave_score = android_score - exchange_fish_score_ * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
      int times = rand() % 5 + 5;
      exchange_times_ = max(1, min(times, (int)(user_leave_score / need_user_score)));

      DWORD play_time = 10 * 60 + (rand() % 21) * 60;
      //play_time = 30;
      m_pIAndroidUserItem->SetGameTimer(IDI_STAND_UP, play_time);
      m_pIAndroidUserItem->SetGameTimer(IDI_EXCHANGE_FISHSCORE, TIME_EXCHANGE_FISHSCORE);

      return true;
  }
  //OutputDebugString(_T("CAndroidUserItemSink::OnSubGameConfig() end"));
	return true;
}

bool CAndroidUserItemSink::OnSubGameConfig(void* data, WORD data_size) 
{
//	OutputDebugString(_T("CAndroidUserItemSink::OnSubGameConfig()"));
  ASSERT(data_size == sizeof(CMD_S_GameConfig));
  if (data_size != sizeof(CMD_S_GameConfig)) return false;
  CMD_S_GameConfig* game_config = static_cast<CMD_S_GameConfig*>(data);

  exchange_ratio_userscore_ = game_config->exchange_ratio_userscore;
  exchange_ratio_fishscore_ = game_config->exchange_ratio_fishscore;
  exchange_count_ = game_config->exchange_count;

  min_bullet_multiple_ = game_config->min_bullet_multiple;
  max_bullet_multiple_ = game_config->max_bullet_multiple;
  //OutputDebugString(_T("CAndroidUserItemSink::OnSubGameConfig() end"));

  return true;
}

bool CAndroidUserItemSink::OnSubExchangeFishScore(void* data, WORD data_size) 
{
	//OutputDebugString(_T("CAndroidUserItemSink::OnSubExchangeFishScore()"));
  ASSERT(data_size == sizeof(CMD_S_ExchangeFishScore));
  if (data_size != sizeof(CMD_S_ExchangeFishScore)) return false;
  CMD_S_ExchangeFishScore* exchange_fishscore = static_cast<CMD_S_ExchangeFishScore*>(data);

  if (exchange_fishscore->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += exchange_fishscore->swap_fish_score;
    exchange_fish_score_ = exchange_fishscore->exchange_fish_score;
    --exchange_times_;
    if (exchange_times_ <= 0) {
      m_pIAndroidUserItem->SetGameTimer(IDI_FIRE, 2);
    } else {
      m_pIAndroidUserItem->SetGameTimer(IDI_EXCHANGE_FISHSCORE, TIME_EXCHANGE_FISHSCORE);
    }
  }

  return true;
}

bool CAndroidUserItemSink::OnSubUserFire(void* data, WORD data_size)
{
	//OutputDebugString(_T("CAndroidUserItemSink::OnSubUserFire()"));
  ASSERT(data_size == sizeof(CMD_S_UserFire));
  if (data_size != sizeof(CMD_S_UserFire)) return false;
  CMD_S_UserFire* user_fire = static_cast<CMD_S_UserFire*>(data);

  if (user_fire->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += user_fire->fish_score;
  }
  //OutputDebugString(_T("CAndroidUserItemSink::OnSubUserFire() end"));
  return true;
}

bool CAndroidUserItemSink::OnSubCatchFish(void* data, WORD data_size)
{
	//OutputDebugString(_T("CAndroidUserItemSink::OnSubCatchFish()"));
  ASSERT(data_size == sizeof(CMD_S_CatchFish));
  if (data_size != sizeof(CMD_S_CatchFish)) return false;
  CMD_S_CatchFish* catch_fish = static_cast<CMD_S_CatchFish*>(data);

  if (catch_fish->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += catch_fish->fish_score;
    if (catch_fish->bullet_ion) {
      if (current_bullet_mulriple_ < 100) {
        current_bullet_kind_ = BULLET_KIND_1_ION;
      } else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) {
        current_bullet_kind_ = BULLET_KIND_2_ION;
      } else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) {
        current_bullet_kind_ = BULLET_KIND_3_ION;
      } else {
        current_bullet_kind_ = BULLET_KIND_4_ION;
      }
    }
  }
  //OutputDebugString(_T("CAndroidUserItemSink::OnSubCatchFish() end"));
  return true;
}

bool CAndroidUserItemSink::OnSubBulletIonTimeout(void* data, WORD data_size) {
  ASSERT(data_size == sizeof(CMD_S_BulletIonTimeout));
  if (data_size != sizeof(CMD_S_BulletIonTimeout)) return false;
  CMD_S_BulletIonTimeout* bullet_timeout = static_cast<CMD_S_BulletIonTimeout*>(data);
  if (bullet_timeout->chair_id == m_pIAndroidUserItem->GetChairID()) {
    if (current_bullet_mulriple_ < 100) {
      current_bullet_kind_ = BULLET_KIND_1_NORMAL;
    } else if (current_bullet_mulriple_ >= 100 && current_bullet_mulriple_ < 1000) {
      current_bullet_kind_ = BULLET_KIND_2_NORMAL;
    } else if (current_bullet_mulriple_ >= 1000 && current_bullet_mulriple_ < 5000) {
      current_bullet_kind_ = BULLET_KIND_3_NORMAL;
    } else {
      current_bullet_kind_ = BULLET_KIND_4_NORMAL;
    }
  }

  return true;
}

bool CAndroidUserItemSink::OnSubCatSweepFishResult(void* data, WORD data_size) {
  ASSERT(data_size == sizeof(CMD_S_CatchSweepFishResult));
  if (data_size != sizeof(CMD_S_CatchSweepFishResult)) return false;
  CMD_S_CatchSweepFishResult* catch_sweep_result = static_cast<CMD_S_CatchSweepFishResult*>(data);

  if (catch_sweep_result->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += catch_sweep_result->fish_score;
  }

  return true;
}

bool CAndroidUserItemSink::OnSubSwitchScene(void* data, WORD data_size) {
  ASSERT(data_size == sizeof(CMD_S_SwitchScene));
  if (data_size != sizeof(CMD_S_SwitchScene)) return false;
  CMD_S_SwitchScene* switch_scene = static_cast<CMD_S_SwitchScene*>(data);

  allow_fire_ = false;
  m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE, TIME_SWITCH_SCENE);

  return true;
}

void CAndroidUserItemSink::ExchangeFishScore() {
  CMD_C_ExchangeFishScore exchange_fishscore;
  exchange_fishscore.increase = true;

  m_pIAndroidUserItem->SendSocketData(SUB_C_EXCHANGE_FISHSCORE, &exchange_fishscore, sizeof(exchange_fishscore));
}

void CAndroidUserItemSink::Fire() {
  int need_fish_score = current_bullet_mulriple_;
  if (fish_score_ < need_fish_score) {
    SCORE android_score = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
    SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
    SCORE user_leave_score = android_score - exchange_fish_score_ * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
    int times = rand() % 5 + 5;
    exchange_times_ = max(1, min(times, (int)(user_leave_score / need_user_score)));
    ExchangeFishScore();
  } else {
    if (allow_fire_) {
      CMD_C_UserFire user_fire;
      user_fire.bullet_kind = current_bullet_kind_;
      user_fire.angle = GetAngle(m_pIAndroidUserItem->GetChairID());
      user_fire.bullet_mulriple = current_bullet_mulriple_;
	    user_fire.lock_fishid = /*-1*/(rand() % 100) > 50 ? -1 : 0;
      m_pIAndroidUserItem->SendSocketData(SUB_C_USER_FIRE, &user_fire, sizeof(user_fire));
    }

    m_pIAndroidUserItem->SetGameTimer(IDI_FIRE, 2);
  }
}

float CAndroidUserItemSink::GetAngle(WORD chair_id) {
  //chair_id 0, 1: M_PI_2, M_PI_2 + M_PI;
  //chair_id 3: M_PI, M_PI * 2;
  //chair_id 5, 4: M_PI_2 + M_PI, M_PI_2 + M_PI + M_PI;
  //chair_id 7: M_PI * 2 , M_PI * 2 + M_PI_2, M_PI_2, M_PI;
  static const float kFireAngle[] = { 1.87f, 2.17f, 2.47f, 2.77f, 3.07f, 3.14f, 3.21f, 3.51f, 3.81f, 4.11f, 4.41f, 4.61f };
  static const float kFireAngleR[] = { 5.98f, 5.68f, 5.38f, 5.08f, 4.91f, 4.71f, 4.61f, 4.51f, 4.34f, 4.04f, 3.74f, 3.44f };
  static const float kFireAngleL[] = { 6.58f, 6.88f, 7.18f, 7.40f, 7.60f, 7.80f, 1.57f, 1.77f, 2.08f, 2.38f, 2.48f, 2.78f };
  float angle;
  int idx = 0;
//   if (chair_id == 3) {
//     for (int i = 0; i < CountArray(kFireAngleR); ++i) {
//       if (last_fire_angle_ == kFireAngleR[i]) {
//         idx = i;
//         break;
//       }
//     }
// 
//     int n = rand() % 5 - 2;
//     if (idx - n < 0) {
//       idx = rand() % 5;
//     } else if (idx + n >= CountArray(kFireAngleR)) {
//       idx = CountArray(kFireAngleR) - rand() % 5 - 1;
//     } else {
//       idx = idx + n;
//     }
// 
//     last_fire_angle_ = angle = kFireAngleR[idx];
//   } else if (chair_id == 7) {
//     for (int i = 0; i < CountArray(kFireAngleL); ++i) {
//       if (last_fire_angle_ == kFireAngleL[i]) {
//         idx = i;
//         break;
//       }
//     }
// 
//     int n = rand() % 5 - 2;
//     if (idx - n < 0) {
//       idx = rand() % 5;
//     } else if (idx + n >= CountArray(kFireAngleL)) {
//       idx = CountArray(kFireAngleL) - rand() % 5 - 1;
//     } else {
//       idx = idx + n;
//     }
// 
//     last_fire_angle_ = angle = kFireAngleL[idx];
//   } else
  {
    for (int i = 0; i < CountArray(kFireAngle); ++i) {
      if (last_fire_angle_ == kFireAngle[i]) {
        idx = i;
        break;
      }
    }

    int n = rand() % 5 - 2;
    if (idx - n < 0) {
      idx = rand() % 5;
    } else if (idx + n >= CountArray(kFireAngle)) {
      idx = CountArray(kFireAngle) - rand() % 5 - 1;
    } else {
      idx = idx + n;
    }

    angle = kFireAngle[idx];
    if (chair_id == 4 || chair_id == 5 || chair_id == 3) {
      angle += M_PI;
    }

    last_fire_angle_ = angle;
  }

  return angle;
}
