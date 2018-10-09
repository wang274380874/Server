#include "StdAfx.h"
#include "MersenneTwister.h"
#include "Marshal.h"
#include "table_frame_sink.h"
#include "tinyxml/tinyxml.h"

// 测试标志 如果定义此标志 库存和概率无效
//#define TEST

const DWORD kBulletIonTimer = 1;
const DWORD kBulletIonTimerEnd = 8;
const DWORD kLockTimer = 9;
const DWORD kClearTraceTimer = 10;
const DWORD kBuildSmallFishTraceTimer = 11;
const DWORD kBuildMediumFishTraceTimer = 12;
const DWORD kBuildFish18TraceTimer = 13;
const DWORD kBuildFish19TraceTimer = 14;
const DWORD kBuildFish20TraceTimer = 15;
const DWORD kBuildFishLKTraceTimer = 16;
const DWORD kBuildFishBombTraceTimer = 17;
const DWORD kBuildFishSuperBombTraceTimer = 18;
const DWORD kBuildFishLockBombTraceTimer = 19;
const DWORD kBuildFishSanTraceTimer = 20;
const DWORD kBuildFishSiTraceTimer = 21;
const DWORD kBuildFishKingTraceTimer = 22;
const DWORD kSwitchSceneTimer = 23;
const DWORD kSceneEndTimer = 24;
const DWORD kLKScoreTimer = 25;
const DWORD kStockOperateTimer = 26;

const DWORD kClearTraceElasped = 60;
const DWORD kBuildSmallFishTraceElasped = 2;
const DWORD kBuildMediumFishTraceElasped = 3;
const DWORD kBuildFish18TraceElasped = 20;
const DWORD kBuildFish19TraceElasped = 30;
const DWORD kBuildFish20TraceElasped = 40;
const DWORD kBuildFishLKTraceElasped = 50;
const DWORD kBuildFishBombTraceElasped = 60;
const DWORD kBuildFishSuperBombTraceElasped = 70;
const DWORD kBuildFishLockBombTraceElasped = 60 + 10;
const DWORD kBuildFishSanTraceElasped = 80 + 20;
const DWORD kBuildFishSiTraceElasped = 90 + 10;
const DWORD kBuildFishKingTraceElasped = 34;
const DWORD kSwitchSceneElasped = 600;
const DWORD kSceneEndElasped = 63;
const DWORD kLKScoreElasped = 1;

const DWORD kRepeatTimer = (DWORD)0xFFFFFFFF;
const DWORD kFishAliveTime = 180000;

static int g_nTableCounts = 0;

// XXX: static SCORE g_stock_score_ = 0;
static SCORE g_revenue_score = 0;
static SCORE g_android_stock = 0;

static std::vector<DWORD> g_balck_list_;
static std::vector<DWORD> g_white_list_;

const SCORE g_cpBonusLevelsA[30] = {5000000,4500000,4000000,3500000,3000000,2500000,2000000,1500000,1000000,500000,4500000,4000000,3500000,3000000,2500000,2000000,1500000,1000000,500000,3500000,3000000,2500000,2000000,1500000,1000000,500000,2000000,1500000,1000000,500000};
const SCORE g_cpBonusLevelsB[30] = {30000000,20000000,15000000,10000000,8000000,5000000,3000000,1000000,800000,500000,20000000,15000000,10000000,8000000,5000000,3000000,1000000,800000,500000,10000000,8000000,5000000,3000000,1000000,800000,500000,3000000,1000000,800000,500000};
const int g_cpTimeLevels[11] = {20, 10, 5, 2, 1, 10, 5, 2, 1, 2, 1};

struct Fish20Config {
	DWORD game_id;
	int catch_count;
	double catch_probability;
};
static std::vector<Fish20Config> g_fish20_config_;

void AddUserFilter(DWORD game_id, unsigned char operate_code) {
	std::vector<DWORD>::iterator iter;
	if (operate_code == 0) {
		for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter) {
			if ((*iter) == game_id) return;
		}
		g_balck_list_.push_back(game_id);
	} else if (operate_code == 1) {
		for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter) {
			if ((*iter) == game_id) return;
		}
		g_white_list_.push_back(game_id);
	} else {
		for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter) {
			if ((*iter) == game_id) {
				iter = g_balck_list_.erase(iter);
				break;
			}
		}
		for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter) {
			if ((*iter) == game_id) {
				iter = g_white_list_.erase(iter);
				break;
			}
		}
	}
}
// 返回值 0：黑名单  1 白名单 -1 正常
int CheckUserFilter(DWORD game_id) {
	std::vector<DWORD>::iterator iter;
	for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter) {
		if ((*iter) == game_id) return 0;
	}

	for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter) {
		if ((*iter) == game_id) return 1;
	}

	return -1;
}

void AddFish20Config(DWORD game_id, int catch_count, double catch_probability) {
	std::vector<Fish20Config>::iterator iter;
	for (iter = g_fish20_config_.begin(); iter != g_fish20_config_.end(); ++iter) {
		Fish20Config& config = *iter;
		if (game_id == config.game_id) {
			if (catch_count == 0) {
				g_fish20_config_.erase(iter);
			} else {
				config.catch_count = catch_count;
				config.catch_probability = catch_probability;
			}
			return;
		}
	}

	Fish20Config config;
	config.game_id = game_id;
	config.catch_count = catch_count;
	config.catch_probability = catch_probability;
	g_fish20_config_.push_back(config);
}

bool CheckFish20Config(DWORD game_id, int* catch_count, double* catch_probability) {
	std::vector<Fish20Config>::iterator iter;
	for (iter = g_fish20_config_.begin(); iter != g_fish20_config_.end(); ++iter) {
		Fish20Config& config = *iter;
		if (game_id == config.game_id) {
			if (config.catch_count <= 0) {
				g_fish20_config_.erase(iter);
				return false;
			}
			*catch_count = config.catch_count;
			*catch_probability = config.catch_probability;
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

TableFrameSink::TableFrameSink() : table_frame_(NULL), game_service_attrib_(NULL), game_service_option_(NULL), base_score_(1),
exchange_ratio_userscore_(1), exchange_ratio_fishscore_(1), exchange_count_(1), next_scene_kind_(SCENE_KIND_1), special_scene_(false),
current_fish_lk_multiple_(50), android_chairid_(INVALID_CHAIR) {
	m_nTableID = g_nTableCounts++;
	for (WORD i = 0; i < GAME_PLAYER; ++i) {
		fish_score_[i] = 0;
		exchange_fish_score_[i] = 0;
		bullet_id_[i] = 0;
	}
	bomb_range_width_ = 0;
	bomb_range_height_ = 0;
	memset(stock_crucial_score_, 0, sizeof(stock_crucial_score_));
	memset(stock_increase_probability_, 0, sizeof(stock_increase_probability_));
	memset(bullet_energy,-1,sizeof(bullet_energy));
	memset(m_pChairsStock, 0, sizeof(m_pChairsStock)); // 座位库存
	m_nTableRandom = g_cpTimeLevels[mt_rand(0, 10)];
	for(int i = 0; i < GAME_PLAYER; i++) {
		m_pStockRandom[i] = mt_rand(10, 120);
	}
	m_lBonusStock = 0; // 彩金
	m_lBonusRandom = g_cpBonusLevelsA[mt_rand(0, 29)]; // 发放彩金随机数
	m_lTempBonus = 0;
	memset(m_pGunsScore, 1000, sizeof(m_pGunsScore)); // 子弹分数记录
	m_nBonusRevenue = mt_rand(5, 35);
	m_nBonusElapsed = 0;
	m_nBonusReCalc = mt_rand(2, 8) * 3600;
	m_nCurrentPlan = 0;
	m_nNextPlan = 0;
	m_nDeployBonusTime = 0;
	m_nDeployBonusRandom = mt_rand(2, 60);
	m_nTableStockPlan = mt_rand(0, 2);
	m_isFirstTimeDistribute = true;
	ZeroMemory(m_lpSortedIndexes, sizeof(m_lpSortedIndexes));
	stock_crucial_count_ = 0;
}

TableFrameSink::~TableFrameSink() {
	TCHAR save_file[128];
	memset(save_file, 0, sizeof(save_file));
	_stprintf(save_file, TEXT("lkpy_%d_%d.save"), game_service_option_->wServerID, m_nTableID);

	if (!_taccess(save_file, 0)) { // if save data file exist
		_tremove(save_file); //delete that mother fxxker
	}
	// save to save data file
	FILE * file = _tfopen(save_file, TEXT("w"));
	Marshal* marshal = new Marshal(file);
	marshal->dump(m_nTableRandom);
	marshal->dump(m_lBonusStock);
	marshal->dump(m_lBonusRandom);
	marshal->dump(m_lTempBonus);
	marshal->dump(m_nCurrentPlan);
	marshal->dump(m_nNextPlan);
	marshal->dump(m_nDeployBonusTime);
	marshal->dump(m_nDeployBonusRandom);
	for(int i = 0; i < GAME_PLAYER; i++) {
		marshal->dump(m_pChairsStock[i]);
		marshal->dump(m_pStockRandom[i]);
	}
	delete marshal;
	fflush(file);
	fclose(file);


	FishTraceInfoVecor::iterator iter;
	for (iter = storage_fish_trace_vector_.begin(); iter != storage_fish_trace_vector_.end(); ++iter) {
		delete (*iter);
	}
	storage_fish_trace_vector_.clear();
	for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
		delete (*iter);
	}
	active_fish_trace_vector_.clear();

	ServerBulletInfoVector::iterator it;
	for (it = storage_bullet_info_vector_.begin(); it != storage_bullet_info_vector_.end(); ++it) {
		delete (*it);
	}
	storage_bullet_info_vector_.clear();
	for (WORD i = 0; i < GAME_PLAYER; ++i) {
		for (it = server_bullet_info_vector_[i].begin(); it != server_bullet_info_vector_[i].end(); ++it) {
			delete (*it);
		}
		server_bullet_info_vector_[i].clear();
	}

	if (g_revenue_score > 0) {
		CString str;
		str.Format(L"吃水%I64d", g_revenue_score);
		AfxMessageBox(str);
		g_revenue_score = 0;
	}
}

void* TableFrameSink::QueryInterface(REFGUID guid, DWORD query_ver) {
	QUERYINTERFACE(ITableFrameSink, guid, query_ver);
	QUERYINTERFACE(ITableUserAction, guid, query_ver);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, guid, query_ver);
	return NULL;
}

bool TableFrameSink::Initialization(IUnknownEx* unknownex) {
	table_frame_ = QUERY_OBJECT_PTR_INTERFACE(unknownex, ITableFrame);
	if (!table_frame_) return false;

	table_frame_->SetStartMode(START_MODE_TIME_CONTROL);
	game_service_attrib_ = table_frame_->GetGameServiceAttrib();
	game_service_option_ = table_frame_->GetGameServiceOption();
	base_score_ = max(1, game_service_option_->lCellScore);

	if (!LoadConfig()) {
		AfxMessageBox(TEXT("配置资源解析失败，请检查"));
		return false;
	}

	TCHAR save_file[128];
	memset(save_file, 0, sizeof(save_file));
	_stprintf(save_file, TEXT("lkpy_%d_%d.save"), game_service_option_->wServerID, m_nTableID);

	if (!_taccess(save_file, 0)) { // if save data file exist
		// load from save data file
		FILE * file = _tfopen(save_file, TEXT("r"));
		Marshal* marshal = new Marshal(file);
		m_nTableRandom = marshal->loadInt();
		m_lBonusStock = marshal->loadLong();
		m_lBonusRandom = marshal->loadLong();
		m_lTempBonus = marshal->loadLong();
		m_nCurrentPlan = marshal->loadInt();
		m_nNextPlan = marshal->loadInt();
		m_nDeployBonusTime = marshal->loadInt();
		m_nDeployBonusRandom = marshal->loadInt();
		for(int i = 0; i < GAME_PLAYER; i++) {
			m_pChairsStock[i] = marshal->loadLong();
			m_pStockRandom[i] = marshal->loadInt();
		}
		delete marshal;
		fclose(file);
	}
	else {
		m_nCurrentPlan = 0; // first time is plan A
		m_nNextPlan = 0; // second time is plan A too
		for(int i = 0; i < GAME_PLAYER; i++) {
			m_pChairsStock[i] = 1000000;
		}
	}

	current_fish_lk_multiple_ = fish_multiple_[FISH_KIND_21];

	return true;
}

void TableFrameSink::RepositionSink() {
}

bool TableFrameSink::IsUserPlaying(WORD chair_id) {
	return true;
}

bool TableFrameSink::OnEventGameStart() {
	return true;
}

bool TableFrameSink::OnEventGameConclude(WORD chair_id, IServerUserItem* server_user_item, BYTE reason) {
	if (reason == GER_DISMISS) {
		for (WORD i = 0; i < GAME_PLAYER; ++i) {
			IServerUserItem* user_item = table_frame_->GetTableUserItem(i);
			if (user_item == NULL) continue;
			CalcScore(user_item);
		}
		table_frame_->ConcludeGame(GAME_STATUS_FREE);
		KillAllGameTimer();
		table_frame_->KillGameTimer(kSwitchSceneTimer);
		table_frame_->KillGameTimer(kClearTraceTimer);
		ClearFishTrace(true);
		next_scene_kind_ = SCENE_KIND_1;
		special_scene_ = false;
		android_chairid_ = INVALID_CHAIR;
	} else if (chair_id < GAME_PLAYER && server_user_item != NULL) {
		CalcScore(server_user_item);
	}
	return true;
}

bool TableFrameSink::OnEventSendGameScene(WORD chair_id, IServerUserItem* server_user_item, BYTE game_status, bool send_secret) {
	switch (game_status) {
	case GAME_STATUS_FREE:
	case GAME_STATUS_PLAY:
		SendGameConfig(server_user_item);

		CMD_S_GameStatus gamestatus;
		gamestatus.game_version = GAME_VERSION;
		memcpy(gamestatus.fish_score, fish_score_, sizeof(gamestatus.fish_score));
		memcpy(gamestatus.exchange_fish_score, exchange_fish_score_, sizeof(gamestatus.exchange_fish_score));

		table_frame_->SendGameScene(server_user_item, &gamestatus, sizeof(gamestatus));

		if (server_user_item->IsMobileUser())
		{
		}
		else
		{
			table_frame_->SendGameMessage(server_user_item, TEXT("键盘↑↓键加减炮，→←键上下分，空格键或鼠标左键发射子弹，F4关闭声音!"), SMT_CHAT);
		}

		return true;
	}
	return false;
}

bool TableFrameSink::OnTimerMessage(DWORD timer_id, WPARAM bind_param) {
	switch (timer_id) {
	case kBuildSmallFishTraceTimer:
		return OnTimerBuildSmallFishTrace(bind_param);
	case kBuildMediumFishTraceTimer:
		return OnTimerBuildMediumFishTrace(bind_param);
	case kBuildFish18TraceTimer:
		return OnTimerBuildFish18Trace(bind_param);
	case kBuildFish19TraceTimer:
		return OnTimerBuildFish19Trace(bind_param);
	case kBuildFish20TraceTimer:
		return OnTimerBuildFish20Trace(bind_param);
	case kBuildFishLKTraceTimer:
		return OnTimerBuildFishLKTrace(bind_param);
	case kBuildFishBombTraceTimer:
		return OnTimerBuildFishBombTrace(bind_param);
	case kBuildFishSuperBombTraceTimer:
		return OnTimerBuildFishSuperBombTrace(bind_param);
	case kBuildFishLockBombTraceTimer:
		return OnTimerBuildFishLockBombTrace(bind_param);
	case kBuildFishSanTraceTimer:
		return OnTimerBuildFishSanTrace(bind_param);
	case kBuildFishSiTraceTimer:
		return OnTimerBuildFishSiTrace(bind_param);
	case kBuildFishKingTraceTimer:
		return OnTimerBuildFishKingTrace(bind_param);
	case kClearTraceTimer:
		return OnTimerClearTrace(bind_param);
	case kBulletIonTimer:
	case kBulletIonTimer + 1:
	case kBulletIonTimer + 2:
	case kBulletIonTimer + 3:
	case kBulletIonTimer + 4:
	case kBulletIonTimer + 5:
	case kBulletIonTimer + 6:
	case kBulletIonTimer + 7:
		return OnTimerBulletIonTimeout(WPARAM(timer_id - kBulletIonTimer));
	case kLockTimer:
		return OnTimerLockTimeout(bind_param);
	case kSwitchSceneTimer:
		return OnTimerSwitchScene(bind_param);
	case kSceneEndTimer:
		return OnTimerSceneEnd(bind_param);
	case kLKScoreTimer:
		return OnTimerLKScore(bind_param);
	case kStockOperateTimer: // Stock operate timer by Tony Fox
		return OnTimerStockOperate(bind_param);
	default:
		ASSERT(FALSE);
	}
	return false;
}

bool TableFrameSink::OnGameMessage(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* server_user_item) {
	switch (sub_cmdid) 
	{
	case SUB_C_EXCHANGE_FISHSCORE: 
		{
			assert(data_size == sizeof(CMD_C_ExchangeFishScore));
			if (data_size != sizeof(CMD_C_ExchangeFishScore)) return false;
			CMD_C_ExchangeFishScore* exchange_fishscore = static_cast<CMD_C_ExchangeFishScore*>(data);
			if (server_user_item->GetUserStatus() == US_LOOKON) return true;
			return OnSubExchangeFishScore(server_user_item, exchange_fishscore->increase);
		}
	case SUB_C_USER_FIRE: {
		assert(data_size == sizeof(CMD_C_UserFire));
		if (data_size != sizeof(CMD_C_UserFire)) return false;
		CMD_C_UserFire* user_fire = static_cast<CMD_C_UserFire*>(data);
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		return OnSubUserFire(server_user_item, user_fire->bullet_kind, user_fire->angle, user_fire->bullet_mulriple, user_fire->lock_fishid);
						  }
	case SUB_C_CATCH_FISH: {
		assert(data_size == sizeof(CMD_C_CatchFish));
		if (data_size != sizeof(CMD_C_CatchFish)) return false;
		CMD_C_CatchFish* hit_fish = static_cast<CMD_C_CatchFish*>(data);
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		IServerUserItem* user_item = table_frame_->GetTableUserItem(hit_fish->chair_id);
		if (user_item == NULL) return true;
		return OnSubCatchFish(user_item, hit_fish->fish_id, hit_fish->bullet_kind, hit_fish->bullet_id, hit_fish->bullet_mulriple);
						   }
	case SUB_C_CATCH_SWEEP_FISH: {
		assert(data_size == sizeof(CMD_C_CatchSweepFish));
		if (data_size != sizeof(CMD_C_CatchSweepFish)) return false;
		CMD_C_CatchSweepFish* catch_sweep = static_cast<CMD_C_CatchSweepFish*>(data);
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		IServerUserItem* user_item = table_frame_->GetTableUserItem(catch_sweep->chair_id);
		if (user_item == NULL) return true;
		return OnSubCatchSweepFish(user_item, catch_sweep->fish_id, catch_sweep->catch_fish_id, catch_sweep->catch_fish_count);
								 }
	case SUB_C_HIT_FISH_I: {
		assert(data_size == sizeof(CMD_C_HitFishLK));
		if (data_size != sizeof(CMD_C_HitFishLK)) return false;
		CMD_C_HitFishLK* hit_fish = static_cast<CMD_C_HitFishLK*>(data);
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		return OnSubHitFishLK(server_user_item, hit_fish->fish_id);
						   }
	case SUB_C_STOCK_OPERATE: {
		assert(data_size == sizeof(CMD_C_StockOperate));
		if (data_size != sizeof(CMD_C_StockOperate)) return false;
		CMD_C_StockOperate* stock_operate = static_cast<CMD_C_StockOperate*>(data);
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		return OnSubStockOperate(server_user_item, stock_operate->operate_code);
							  }
	case SUB_C_USER_FILTER: {
		assert(data_size == sizeof(CMD_C_UserFilter));
		if (data_size != sizeof(CMD_C_UserFilter)) return false;
		CMD_C_UserFilter* user_filter = static_cast<CMD_C_UserFilter*>(data);
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		return OnSubUserFilter(server_user_item, user_filter->game_id, user_filter->operate_code);
							}
	case SUB_C_ANDROID_STAND_UP: {
		if (!server_user_item->IsAndroidUser()) return true;
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		table_frame_->PerformStandUpAction(server_user_item);
		return true;
								 }
	case SUB_C_FISH20_CONFIG: {
		assert(data_size == sizeof(CMD_C_Fish20Config));
		if (data_size != sizeof(CMD_C_Fish20Config)) return false;
		CMD_C_Fish20Config* fish20_config = static_cast<CMD_C_Fish20Config*>(data);
		if (server_user_item->GetUserStatus() == US_LOOKON) return true;
		return OnSubFish20Config(server_user_item, fish20_config->game_id, fish20_config->catch_count, fish20_config->catch_probability);
							  }
	case SUB_C_ANDROID_BULLET_MUL: {
		assert(data_size == sizeof(CMD_C_AndroidBulletMul));
		if (data_size != sizeof(CMD_C_AndroidBulletMul)) return false;
		CMD_C_AndroidBulletMul* android_bullet_mul = static_cast<CMD_C_AndroidBulletMul*>(data);
		IServerUserItem* user_item = table_frame_->GetTableUserItem(android_bullet_mul->chair_id);
		if (!user_item->IsAndroidUser()) return true;

		ServerBulletInfo* bullet_info = GetBulletInfo(android_bullet_mul->chair_id, android_bullet_mul->bullet_id);
		if (bullet_info == NULL) return true;
		if (fish_score_[android_bullet_mul->chair_id] + bullet_info->bullet_mulriple < android_bullet_mul->bullet_mulriple) {
			table_frame_->PerformStandUpAction(user_item);
			return true;
		}

		CMD_S_UserFire user_fire;
		user_fire.bullet_kind = bullet_info->bullet_kind;
		user_fire.bullet_id = bullet_info->bullet_id;
		user_fire.angle = 0.f;
		user_fire.chair_id = user_item->GetChairID();
		user_fire.android_chairid = android_chairid_;
		user_fire.bullet_mulriple = bullet_info->bullet_mulriple;
		user_fire.fish_score = bullet_info->bullet_mulriple - android_bullet_mul->bullet_mulriple;
		user_fire.lock_fishid = 0;
		table_frame_->SendTableData(user_item->GetChairID(), SUB_S_USER_FIRE, &user_fire, sizeof(user_fire));

		fish_score_[android_bullet_mul->chair_id] += bullet_info->bullet_mulriple;
		fish_score_[android_bullet_mul->chair_id] -= android_bullet_mul->bullet_mulriple;

		bullet_info->bullet_kind = android_bullet_mul->bullet_kind;
		bullet_info->bullet_mulriple = android_bullet_mul->bullet_mulriple;

		return true;
		}
	}
	return false;
}

bool TableFrameSink::OnFrameMessage(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* server_user_item) {
	return false;
}

// 玩家坐下
bool TableFrameSink::OnActionUserSitDown(WORD chair_id, IServerUserItem* server_user_item, bool lookon_user) {
	if (!lookon_user) {
		exchange_fish_score_[chair_id] = 0;
		fish_score_[chair_id] = 0;

		if (android_chairid_ == INVALID_CHAIR && !server_user_item->IsAndroidUser()) {
			android_chairid_ = chair_id;
		}

		if (table_frame_->GetGameStatus() == GAME_STATUS_FREE) {
			table_frame_->StartGame();
			table_frame_->SetGameStatus(GAME_STATUS_PLAY);
			StartAllGameTimer();
			table_frame_->SetGameTimer(kSwitchSceneTimer, kSwitchSceneElasped * 1000, kRepeatTimer, 0);
			table_frame_->SetGameTimer(kClearTraceTimer, kClearTraceElasped * 1000, kRepeatTimer, 0);
		}
	}

	return true;
}

// 玩家站起
bool TableFrameSink::OnActionUserStandUp(WORD chair_id, IServerUserItem* server_user_item, bool lookon_user) {
	if (lookon_user) return true;

	exchange_fish_score_[chair_id] = 0;
	fish_score_[chair_id] = 0;
	bullet_energy[chair_id]=-1;

	WORD user_count = 0;
	WORD player_count = 0;
	WORD android_chair_id[GAME_PLAYER];
	for (WORD i = 0; i < GAME_PLAYER; ++i) {
		if (i == chair_id) continue;
		IServerUserItem* user_item = table_frame_->GetTableUserItem(i);
		if (user_item) {
			if (!user_item->IsAndroidUser()) android_chair_id[player_count++] = i;
			++user_count;
		}
	}

	if (player_count == 0) {
		android_chairid_ = INVALID_CHAIR;
	} else {
		if (!server_user_item->IsAndroidUser() && chair_id == android_chairid_) {
			android_chairid_ = android_chair_id[0];
		}
	}

	if (user_count == 0) {
		table_frame_->ConcludeGame(GAME_STATUS_FREE);
		KillAllGameTimer();
		table_frame_->KillGameTimer(kSwitchSceneTimer);
		table_frame_->KillGameTimer(kClearTraceTimer);
		ClearFishTrace(true);
		next_scene_kind_ = SCENE_KIND_1;
		special_scene_ = false;
	}

	return true;
}

FishTraceInfo* TableFrameSink::ActiveFishTrace() {
	FishTraceInfo* fish_trace_info = NULL;
	if (storage_fish_trace_vector_.size() > 0) {
		fish_trace_info = storage_fish_trace_vector_.back();
		storage_fish_trace_vector_.pop_back();
		active_fish_trace_vector_.push_back(fish_trace_info);
	}

	if (fish_trace_info == NULL) {
		fish_trace_info = new FishTraceInfo;
		active_fish_trace_vector_.push_back(fish_trace_info);
	}

	return fish_trace_info;
}

bool TableFrameSink::FreeFishTrace(FishTraceInfo* fish_trace_info) {
	FishTraceInfoVecor::iterator iter;
	for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
		if (fish_trace_info == *iter) {
			active_fish_trace_vector_.erase(iter);
			storage_fish_trace_vector_.push_back(fish_trace_info);
			return true;
		}
	}

	assert(!"FreeFishTrace Failed");
	return false;
}

void TableFrameSink::FreeAllFishTrace() {
	std::copy(active_fish_trace_vector_.begin(), active_fish_trace_vector_.end(), std::back_inserter(storage_fish_trace_vector_));
	active_fish_trace_vector_.clear();
}

FishTraceInfo* TableFrameSink::GetFishTraceInfo(int fish_id) {
	FishTraceInfoVecor::iterator iter;
	FishTraceInfo* fish_trace_info = NULL;
	for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
		fish_trace_info = *iter;
		if (fish_trace_info->fish_id == fish_id) return fish_trace_info;
	}
	//assert(!"GetFishTraceInfo:not found fish");
	return NULL;
}

void TableFrameSink::SaveSweepFish(FishKind fish_kind, int fish_id, BulletKind bullet_kind, int bullet_mulriple) {
	SweepFishInfo sweep_fish;
	sweep_fish.fish_kind = fish_kind;
	sweep_fish.fish_id = fish_id;
	sweep_fish.bullet_kind = bullet_kind;
	sweep_fish.bullet_mulriple = bullet_mulriple;
	sweep_fish_info_vector_.push_back(sweep_fish);
}

bool TableFrameSink::FreeSweepFish(int fish_id) {
	std::vector<SweepFishInfo>::iterator iter;
	for (iter = sweep_fish_info_vector_.begin(); iter != sweep_fish_info_vector_.end(); ++iter) {
		if ((*iter).fish_id == fish_id) {
			sweep_fish_info_vector_.erase(iter);
			return true;
		}
	}

	assert(!"FreeSweepFish Failed");
	return false;
}

SweepFishInfo* TableFrameSink::GetSweepFish(int fish_id) {
	std::vector<SweepFishInfo>::iterator iter;
	for (iter = sweep_fish_info_vector_.begin(); iter != sweep_fish_info_vector_.end(); ++iter) {
		if ((*iter).fish_id == fish_id) {
			return &(*iter);
		}
	}
	return NULL;
}

ServerBulletInfo* TableFrameSink::ActiveBulletInfo(WORD chairid) {
	ServerBulletInfo* bullet_info = NULL;
	if (storage_bullet_info_vector_.size() > 0) {
		bullet_info = storage_bullet_info_vector_.back();
		storage_bullet_info_vector_.pop_back();
		server_bullet_info_vector_[chairid].push_back(bullet_info);
	}

	if (bullet_info == NULL) {
		bullet_info = new ServerBulletInfo();
		server_bullet_info_vector_[chairid].push_back(bullet_info);
	}

	return bullet_info;
}

bool TableFrameSink::FreeBulletInfo(WORD chairid, ServerBulletInfo* bullet_info) {
	ServerBulletInfoVector::iterator iter;
	for (iter = server_bullet_info_vector_[chairid].begin(); iter != server_bullet_info_vector_[chairid].end(); ++iter) {
		if (bullet_info == *iter) {
			server_bullet_info_vector_[chairid].erase(iter);
			storage_bullet_info_vector_.push_back(bullet_info);
			return true;
		}
	}

	assert(!"FreeBulletInfo Failed");
	return false;
}

void TableFrameSink::FreeAllBulletInfo(WORD chairid) {
	std::copy(server_bullet_info_vector_[chairid].begin(), server_bullet_info_vector_[chairid].end(), std::back_inserter(storage_bullet_info_vector_));
	server_bullet_info_vector_[chairid].clear();
}

ServerBulletInfo* TableFrameSink::GetBulletInfo(WORD chairid, int bullet_id) {
	ServerBulletInfoVector::iterator iter;
	ServerBulletInfo* bullet_info = NULL;
	for (iter = server_bullet_info_vector_[chairid].begin(); iter != server_bullet_info_vector_[chairid].end(); ++iter) {
		bullet_info = *iter;
		if (bullet_info->bullet_id == bullet_id) return bullet_info;
	}
	//assert(!"GetBulletInfo:not found");
	return NULL;
}

void TableFrameSink::StartAllGameTimer() {
	table_frame_->SetGameTimer(kBuildSmallFishTraceTimer, kBuildSmallFishTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildMediumFishTraceTimer, kBuildMediumFishTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFish18TraceTimer, kBuildFish18TraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFish19TraceTimer, kBuildFish19TraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFish20TraceTimer, kBuildFish20TraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFishLKTraceTimer, kBuildFishLKTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFishBombTraceTimer, kBuildFishBombTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFishLockBombTraceTimer, kBuildFishLockBombTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFishSuperBombTraceTimer, kBuildFishSuperBombTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFishSanTraceTimer, kBuildFishSanTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFishSiTraceTimer, kBuildFishSiTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kBuildFishKingTraceTimer, kBuildFishKingTraceElasped * 1000, kRepeatTimer, 0);
	table_frame_->SetGameTimer(kStockOperateTimer, 1000, kRepeatTimer, 0);
}

void TableFrameSink::KillAllGameTimer() {
	table_frame_->KillGameTimer(kBuildSmallFishTraceTimer);
	table_frame_->KillGameTimer(kBuildMediumFishTraceTimer);
	table_frame_->KillGameTimer(kBuildFish18TraceTimer);
	table_frame_->KillGameTimer(kBuildFish19TraceTimer);
	table_frame_->KillGameTimer(kBuildFish20TraceTimer);
	table_frame_->KillGameTimer(kBuildFishLKTraceTimer);
	table_frame_->KillGameTimer(kBuildFishBombTraceTimer);
	table_frame_->KillGameTimer(kBuildFishLockBombTraceTimer);
	table_frame_->KillGameTimer(kBuildFishSuperBombTraceTimer);
	table_frame_->KillGameTimer(kBuildFishSanTraceTimer);
	table_frame_->KillGameTimer(kBuildFishSiTraceTimer);
	table_frame_->KillGameTimer(kBuildFishKingTraceTimer);
	table_frame_->KillGameTimer(kStockOperateTimer); // Stock operate timer by Tony Fox
}

bool TableFrameSink::LoadConfig() 
{
	TCHAR file_name[MAX_PATH] = { 0 };
	_sntprintf(file_name, CountArray(file_name), TEXT("lkpy_config_%d.xml"), game_service_option_->wServerID);

	TiXmlDocument xml_doc;
	if (!xml_doc.LoadFile(CT2A(file_name), TIXML_ENCODING_UTF8)) return false;

	const TiXmlElement* xml_element = xml_doc.FirstChildElement("Config");
	if (xml_element == NULL) return false;

	const TiXmlElement* xml_child = NULL;
	int fish_count = 0, bullet_kind_count = 0;
	for (xml_child = xml_element->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement()) 
	{
		if (!strcmp(xml_child->Value(), "Stock")) 
		{
			for (const TiXmlElement* xml_stock = xml_child->FirstChildElement(); xml_stock; xml_stock = xml_stock->NextSiblingElement()) 
			{
				xml_stock->Attribute("stockScore", &stock_crucial_score_[stock_crucial_count_]);
				xml_stock->Attribute("increaseProbability", &stock_increase_probability_[stock_crucial_count_]);
				++stock_crucial_count_;
				if (stock_crucial_count_ >= 10) break;
			}
		} 
		else if (!strcmp(xml_child->Value(), "ScoreExchange")) 
		{
			const char* attri = xml_child->Attribute("exchangeRatio");
			char* temp = NULL;
			exchange_ratio_userscore_ = strtol(attri, &temp, 10);
			exchange_ratio_fishscore_ = strtol(temp + 1, &temp, 10);
			xml_child->Attribute("exchangeCount", &exchange_count_);
		} 
		else if (!strcmp(xml_child->Value(), "Cannon")) 
		{
			const char* attri = xml_child->Attribute("cannonMultiple");
			char* temp = NULL;
			min_bullet_multiple_ = strtol(attri, &temp, 10);
			max_bullet_multiple_ = strtol(temp + 1, &temp, 10);
		} 
		else if (!strcmp(xml_child->Value(), "Bomb")) 
		{
			const char* attri = xml_child->Attribute("BombProbability");
			char* temp = NULL;
			bomb_stock_ = strtol(attri, &temp, 10);
			super_bomb_stock_ = strtol(temp + 1, &temp, 10);
		} 
		else if (!strcmp(xml_child->Value(), "Fish")) 
		{
			int fish_kind;
			xml_child->Attribute("kind", &fish_kind);
			if (fish_kind >= FISH_KIND_COUNT || fish_kind < 0) return false;

			xml_child->Attribute("speed", &fish_speed_[fish_kind]);
			if (fish_kind == FISH_KIND_18) 
			{
				const char* attri = xml_child->Attribute("multiple");
				char* temp = NULL;
				fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				fish18_max_multiple_ = strtol(temp + 1, &temp, 10);
			} 
			else if (fish_kind == FISH_KIND_19) 
			{
				const char* attri = xml_child->Attribute("multiple");
				char* temp = NULL;
				fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				fish19_max_multiple_ = strtol(temp + 1, &temp, 10);
			} 
			else if (fish_kind == FISH_KIND_21) 
			{
				const char* attri = xml_child->Attribute("multiple");
				char* temp = NULL;
				fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				fishLK_max_multiple_ = strtol(temp + 1, &temp, 10);
			} 
			else 
			{
				xml_child->Attribute("multiple", &fish_multiple_[fish_kind]);
			}
			const char* attri = xml_child->Attribute("BoundingBox");
			char* temp = NULL;
			fish_bounding_box_width_[fish_kind] = strtol(attri, &temp, 10);
			fish_bounding_box_height_[fish_kind] = strtol(temp + 1, &temp, 10);
			if (fish_kind == FISH_KIND_23) 
			{
				bomb_range_width_ = strtol(temp + 1, &temp, 10);
				bomb_range_height_ = strtol(temp + 1, &temp, 10);
			}
			xml_child->Attribute("hitRadius", &fish_hit_radius_[fish_kind]);
			xml_child->Attribute("captureProbability", &fish_capture_probability_[fish_kind]);
			++fish_count;
		} 
		else if (!strcmp(xml_child->Value(), "Bullet")) 
		{
			int bullet_kind;
			xml_child->Attribute("kind", &bullet_kind);
			if (bullet_kind >= BULLET_KIND_COUNT || bullet_kind < 0) return false;

			xml_child->Attribute("speed", &bullet_speed_[bullet_kind]);
			xml_child->Attribute("netRadius", &net_radius_[bullet_kind]);
			++bullet_kind_count;
		}
	}
	if (fish_count != FISH_KIND_COUNT) return false;
	if (bullet_kind_count != BULLET_KIND_COUNT) return false;

	return true;
}

bool TableFrameSink::SendGameConfig(IServerUserItem* server_user_item) {
	CMD_S_GameConfig game_config;
	game_config.exchange_ratio_userscore = exchange_ratio_userscore_;
	game_config.exchange_ratio_fishscore = exchange_ratio_fishscore_;
	game_config.exchange_count = exchange_count_;
	game_config.min_bullet_multiple = min_bullet_multiple_;
	game_config.max_bullet_multiple = max_bullet_multiple_;
	game_config.bomb_range_width = bomb_range_width_;
	game_config.bomb_range_height = bomb_range_height_;
	for (int i = 0; i < FISH_KIND_COUNT; ++i) {
		game_config.fish_multiple[i] = fish_multiple_[i];
		game_config.fish_speed[i] = fish_speed_[i];
		game_config.fish_bounding_box_width[i] = fish_bounding_box_width_[i];
		game_config.fish_bounding_box_height[i] = fish_bounding_box_height_[i];
		game_config.fish_hit_radius[i] = fish_hit_radius_[i];
	}

	for (int i = 0; i < BULLET_KIND_COUNT; ++i) {
		game_config.bullet_speed[i] = bullet_speed_[i];
		game_config.net_radius[i] = net_radius_[i];
	}

	return table_frame_->SendUserItemData(server_user_item, SUB_S_GAME_CONFIG, &game_config, sizeof(game_config));
}

int TableFrameSink::GetNewFishID() {
	++fish_id_;
	if (fish_id_ <= 0) fish_id_ = 1;
	return fish_id_;
}

int TableFrameSink::GetBulletID(WORD chairid) {
	ASSERT(chairid < GAME_PLAYER);
	++bullet_id_[chairid];
	if (bullet_id_[chairid] <= 0) bullet_id_[chairid] = 1;
	return bullet_id_[chairid];
}

bool TableFrameSink::OnTimerBuildSmallFishTrace(WPARAM bind_param) {
	if (CServerRule::IsForfendGameEnter(game_service_option_->dwServerRule)) {
		OnEventGameConclude(GAME_PLAYER, NULL, GER_DISMISS);
		return true;
	}
	BuildFishTrace(4 + rand() % 8, FISH_KIND_1, FISH_KIND_10);
	return true;
}

bool TableFrameSink::OnTimerBuildMediumFishTrace(WPARAM bind_param) {
	BuildFishTrace(1 + rand() % 2, FISH_KIND_11, FISH_KIND_17);
	return true;
}

bool TableFrameSink::OnTimerBuildFish18Trace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_18, FISH_KIND_18);
	return true;
}

bool TableFrameSink::OnTimerBuildFish19Trace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_19, FISH_KIND_19);
	return true;
}

bool TableFrameSink::OnTimerBuildFish20Trace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_20, FISH_KIND_20);
	return true;
}

bool TableFrameSink::OnTimerBuildFishLKTrace(WPARAM bind_param) {
	//BuildFishTrace(1, FISH_KIND_21, FISH_KIND_21);

	CMD_S_FishTrace fish_trace;

	DWORD build_tick = GetTickCount();
	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_21;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();

	fish_trace.fish_id = fish_trace_info->fish_id;
	fish_trace.fish_kind = fish_trace_info->fish_kind;
	fish_trace.init_count = 3;
	fish_trace.trace_type = TRACE_BEZIER;
	BuildInitTrace(fish_trace.init_pos, fish_trace.init_count, fish_trace.fish_kind, fish_trace.trace_type);

	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_FISH_TRACE, &fish_trace, sizeof(fish_trace));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_FISH_TRACE, &fish_trace, sizeof(fish_trace));

	current_fish_lk_multiple_ = fish_multiple_[FISH_KIND_21];
	table_frame_->SetGameTimer(kLKScoreTimer, kLKScoreElasped * 1000, fishLK_max_multiple_ - fish_multiple_[FISH_KIND_21], fish_trace_info->fish_id);

	return true;
}

bool TableFrameSink::OnTimerBuildFishBombTrace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_23, FISH_KIND_23);
	return true;
}

bool TableFrameSink::OnTimerBuildFishLockBombTrace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_22, FISH_KIND_22);
	return true;
}

bool TableFrameSink::OnTimerBuildFishSuperBombTrace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_24, FISH_KIND_24);
	return true;
}

bool TableFrameSink::OnTimerBuildFishSanTrace(WPARAM bind_param) {
	BuildFishTrace(2, FISH_KIND_25, FISH_KIND_27);
	return true;
}

bool TableFrameSink::OnTimerBuildFishSiTrace(WPARAM bind_param) {
	BuildFishTrace(2, FISH_KIND_28, FISH_KIND_30);
	return true;
}

bool TableFrameSink::OnTimerBuildFishKingTrace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_31, FISH_KIND_40);
	return true;
}

bool TableFrameSink::OnTimerClearTrace(WPARAM bind_param) {
	ClearFishTrace();
	return true;
}

bool TableFrameSink::OnTimerBulletIonTimeout(WPARAM bind_param) {
	WORD chair_id = static_cast<WORD>(bind_param);
	CMD_S_BulletIonTimeout bullet_timeout;
	bullet_timeout.chair_id = chair_id;
	bullet_energy[chair_id]=bullet_id_[chair_id];
	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_BULLET_ION_TIMEOUT, &bullet_timeout, sizeof(bullet_timeout));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_BULLET_ION_TIMEOUT, &bullet_timeout, sizeof(bullet_timeout));

	return true;
}

bool TableFrameSink::OnTimerLockTimeout(WPARAM bind_param) {
	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_LOCK_TIMEOUT);
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_LOCK_TIMEOUT);
	StartAllGameTimer();

	return true;
}

bool TableFrameSink::OnTimerSwitchScene(WPARAM bind_param) {
	KillAllGameTimer();
	ClearFishTrace(true);
	special_scene_ = true;
	table_frame_->SetGameTimer(kSceneEndTimer, kSceneEndElasped * 1000, 1, 0);

	if (next_scene_kind_ == SCENE_KIND_1) {
		BuildSceneKind1();
	} else if (next_scene_kind_ == SCENE_KIND_2) {
		BuildSceneKind2();
	} else if (next_scene_kind_ == SCENE_KIND_3) {
		BuildSceneKind3();
	} else if (next_scene_kind_ == SCENE_KIND_4) {
		BuildSceneKind4();
	} else if (next_scene_kind_ == SCENE_KIND_5) {
		BuildSceneKind5();
	}

	next_scene_kind_ = static_cast<SceneKind>((next_scene_kind_ + 1) % SCENE_KIND_COUNT);

	return true;
}

bool TableFrameSink::OnTimerSceneEnd(WPARAM bind_param) {
	special_scene_ = false;
	StartAllGameTimer();
	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_SCENE_END);
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_SCENE_END);
	return true;
}

bool TableFrameSink::OnTimerLKScore(WPARAM bind_param) {
	if (current_fish_lk_multiple_ >= fishLK_max_multiple_) {
		table_frame_->KillGameTimer(kLKScoreTimer);
		return true;
	}
	CMD_S_HitFishLK hit_fish;
	hit_fish.chair_id = 3;
	hit_fish.fish_id = (int)bind_param;
	hit_fish.fish_mulriple = ++current_fish_lk_multiple_;
	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));
	if (current_fish_lk_multiple_ == fishLK_max_multiple_) {
		table_frame_->KillGameTimer(kLKScoreTimer);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////
// Table score to chairs by Tony Fox

void TableFrameSink::DistributePlanC(SCORE lTableStock, SCORE pOutput[], int pGunsScore[]) {
	SCORE half = lTableStock / 2;
	int rnd1 = mt_rand(0, 5);
	int rnd2 = 0;
	do {
		rnd2 = mt_rand(0, 5);
	}
	while(rnd2 == rnd1);
	pOutput[rnd1] = half;
	pOutput[rnd2] = half;
}
void TableFrameSink::DistributePlanB(SCORE lTableStock, SCORE pOutput[], int pGunsScore[]) {
	SCORE eighty = lTableStock * 80 / 100;
	SCORE twenty = lTableStock * 20 / 100;
	int rnd1 = mt_rand(0, 5);
	int rnd2 = 0;
	do {
		rnd2 = mt_rand(0, 5);
	}
	while(rnd2 == rnd1);
	pOutput[rnd1] = eighty;
	pOutput[rnd2] = twenty;
}
void TableFrameSink::DistributePlanA(SCORE lTableStock, SCORE pOutput[], int pGunsScore[]) {
	int rnd = mt_rand(0, 5);
	pOutput[rnd] = lTableStock;
}
bool TableFrameSink::OnTimerStockOperate(WPARAM bind_param) {
	int i = 0;
	m_nBonusElapsed++;
	// 重新计算彩金抽水比例
	if (m_nBonusElapsed >= m_nBonusReCalc) {
		m_nBonusReCalc = mt_rand(2, 8) * 3600;
		m_nBonusElapsed = 0;
		m_nBonusRevenue = mt_rand(2, 5);
	}
	// 判断是否触及彩金发放条件
	if (m_lBonusStock >= m_lBonusRandom) {
		m_lTempBonus += m_lBonusStock;
		m_lBonusStock = 0;
		// 生成新的彩金最大值
		m_nCurrentPlan = m_nNextPlan;
		m_nNextPlan = (mt_rand(1, 100) > 60 ? 1 : 0);
		//if (m_nCurrentPlan == 1) {
		//	m_lBonusRandom = g_cpBonusLevelsB[mt_rand(0, 29)];
		//}
		//else {
			m_lBonusRandom = g_cpBonusLevelsA[mt_rand(0, 29)];
		//}
	}
	// 统计小奖池的总分
	m_nTimeSum++;
	if (m_nTimeSum >= m_nTableRandom * 60) {
		LONGLONG sum = m_pChairsStock[0];
		sum += m_pChairsStock[1];
		sum += m_pChairsStock[2];
		sum += m_pChairsStock[3];
		sum += m_pChairsStock[4];
		sum += m_pChairsStock[5];
		// 彩金抽水
		SCORE revenue = sum * m_nBonusRevenue / 100;
		m_lBonusStock += revenue;
		sum = sum - revenue;
		if (m_lTempBonus > 0) {
			m_lTempBonus = 0;
			sum += m_lTempBonus;
		}
		ZeroMemory(m_pChairsStock, sizeof(m_pChairsStock));
		m_nResortRnd = mt_rand(0, 99);
		if (m_nResortRnd < 10 || m_isFirstTimeDistribute) {
			m_nLastDestributePlan = mt_rand(1, 5);
		}
		switch(m_nLastDestributePlan) {
		case 1:
			DistributePlanA(sum, m_pChairsStock, m_pGunsScore);
			break;
		case 2:
			DistributePlanB(sum, m_pChairsStock, m_pGunsScore);
			break;
		case 3:
			DistributePlanC(sum, m_pChairsStock, m_pGunsScore);
			break;
		}
		m_nTableRandom = g_cpTimeLevels[mt_rand(0, 10)];
		m_nTableStockPlan = mt_rand(0, 2);
		m_nTimeSum = 0;
	}
	return true;
}
bool ArrayContains(int length, int array[], int needle) {
	for(int i = 0; i < length; i++) {
		if (array[i] == needle)
			return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////

bool TableFrameSink::SendTableData(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* exclude_user_item) {
	if (exclude_user_item == NULL) {
		table_frame_->SendTableData(INVALID_CHAIR, sub_cmdid, data, data_size);
	} else {
		IServerUserItem* send_user_item = NULL;
		for (WORD i = 0; i < GAME_PLAYER; ++i) {
			send_user_item = table_frame_->GetTableUserItem(i);
			if (send_user_item == NULL) continue;
			if (send_user_item == exclude_user_item) continue;
			table_frame_->SendTableData(send_user_item->GetChairID(), sub_cmdid, data, data_size);
		}
	}
	table_frame_->SendLookonData(INVALID_CHAIR, sub_cmdid, data, data_size);
	return true;
}

bool TableFrameSink::OnSubExchangeFishScore(IServerUserItem* server_user_item, bool increase) 
{
	WORD chair_id = server_user_item->GetChairID();

	CMD_S_ExchangeFishScore exchange_fish_score;
	exchange_fish_score.chair_id = chair_id;

	SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
	SCORE user_leave_score = server_user_item->GetUserScore() - exchange_fish_score_[chair_id] * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
	SCORE exchange_count=user_leave_score*exchange_ratio_fishscore_/exchange_ratio_userscore_;
	if (increase) 
	{
		if (exchange_count<0) 
		{
			if (server_user_item->IsAndroidUser()) 
			{
				table_frame_->PerformStandUpAction(server_user_item);
				return true;
			} 
			else 
			{
				return false;
			}
		}
		fish_score_[chair_id] += exchange_count;
		exchange_fish_score_[chair_id] += exchange_count;
		exchange_fish_score.swap_fish_score = exchange_count;
		exchange_fish_score.exchange_fish_score = exchange_fish_score_[chair_id];
		// 		if (need_user_score > user_leave_score) 
		// 		{
		// 			if (server_user_item->IsAndroidUser()) 
		// 			{
		// 				table_frame_->PerformStandUpAction(server_user_item);
		// 				return true;
		// 			} 
		// 			else 
		// 			{
		// 				return false;
		// 			}
		// 		}
		// 		fish_score_[chair_id] += exchange_count_;
		// 		exchange_fish_score_[chair_id] += exchange_count_;
		// 		exchange_fish_score.swap_fish_score = exchange_count_;
	} 
	else 
	{
		if (fish_score_[chair_id] <= 0) return true;
		/*if (fish_score_[chair_id] > 0 && fish_score_[chair_id] < exchange_count_) {
		exchange_fish_score_[chair_id] -= fish_score_[chair_id];
		exchange_fish_score.swap_fish_score = -fish_score_[chair_id];
		fish_score_[chair_id] = 0;
		} else if (fish_score_[chair_id] >= exchange_count_) {
		fish_score_[chair_id] -= exchange_count_;
		exchange_fish_score_[chair_id] -= exchange_count_;
		exchange_fish_score.swap_fish_score = -exchange_count_;
		}*/
		exchange_fish_score_[chair_id] -= fish_score_[chair_id];
		exchange_fish_score.swap_fish_score = -fish_score_[chair_id];
		fish_score_[chair_id] = 0;
	}

	exchange_fish_score.exchange_fish_score = exchange_fish_score_[chair_id];
	SendTableData(SUB_S_EXCHANGE_FISHSCORE, &exchange_fish_score, sizeof(exchange_fish_score), server_user_item->IsAndroidUser() ? NULL : server_user_item);

	return true;
}

// 玩家发炮
bool TableFrameSink::OnSubUserFire(IServerUserItem * server_user_item, BulletKind bullet_kind, float angle, int bullet_mul, int lock_fishid) {
	// 没真实玩家机器人不打炮
	if (android_chairid_ == INVALID_CHAIR) return true;
	if (bullet_mul < min_bullet_multiple_ || bullet_mul > max_bullet_multiple_) return false;// 非法数据
	WORD chair_id = server_user_item->GetChairID(); // 椅子ID
	assert(fish_score_[chair_id] >= bullet_mul); // 断言玩家桌上的分数大于等于子弹分数
	if (fish_score_[chair_id] < bullet_mul) { // 如果玩家桌上的分数小于子弹分数
		if (server_user_item->IsAndroidUser()) { // 如果是机器人则跳过
			table_frame_->PerformStandUpAction(server_user_item);
			return true;
		} else { // 否则则为非法数据，踢出游戏
			return false;
		}
	}
	////////////////////////////////////////////////////////
	m_pGunsScore[chair_id] = bullet_mul; // 记录玩家最后一发炮弹的分数
	///////////////////////////////////////////////////////
	fish_score_[chair_id] -= bullet_mul; // 鱼的分数减去炮的分数
	if (!server_user_item->IsAndroidUser()) { // 如果玩家不是机器人
		int revenue = (int)ceil(game_service_option_->wRevenueRatio * (double)bullet_mul / 1000.0); // 计算抽水，当炮的倍数小于等于100时抽取1分的水
		//g_stock_score_ += bullet_mul - revenue; // 增加库存值并抽水 - 已废弃
		m_pChairsStock[chair_id] += bullet_mul - revenue; // 增加座位库存并抽水
		g_revenue_score += revenue; // 记录总抽水数
	}
	else
	{
		g_android_stock += bullet_mul;
	}

	if (lock_fishid > 0 && GetFishTraceInfo(lock_fishid) == NULL) {
		lock_fishid = 0;
	}
	CMD_S_UserFire user_fire;
	user_fire.bullet_kind = bullet_kind;
	user_fire.bullet_id = GetBulletID(chair_id);
	user_fire.angle = angle;
	user_fire.chair_id = server_user_item->GetChairID();
	user_fire.android_chairid = server_user_item->IsAndroidUser() ? android_chairid_ : INVALID_CHAIR;
	user_fire.bullet_mulriple = bullet_mul;
	user_fire.fish_score = -bullet_mul;
	user_fire.lock_fishid = lock_fishid;
	SendTableData(SUB_S_USER_FIRE, &user_fire, sizeof(user_fire), NULL);
	ServerBulletInfo* bullet_info = ActiveBulletInfo(chair_id);
	bullet_info->bullet_id = user_fire.bullet_id;
	bullet_info->bullet_kind = user_fire.bullet_kind;
	bullet_info->bullet_mulriple = user_fire.bullet_mulriple;
	return true;
}
// 玩家抓到鱼
bool TableFrameSink::OnSubCatchFish(IServerUserItem* server_user_item, int fish_id, BulletKind bullet_kind, int bullet_id, int bullet_mul) {
	if (bullet_mul < min_bullet_multiple_ || bullet_mul > max_bullet_multiple_) return true;
	FishTraceInfo* fish_trace_info = GetFishTraceInfo(fish_id);
	if (fish_trace_info == NULL) return true;
	if (fish_trace_info->fish_kind >= FISH_KIND_COUNT) return true;
	ServerBulletInfo* bullet_info = GetBulletInfo(server_user_item->GetChairID(), bullet_id);
	if (bullet_info == NULL) return true;
	ASSERT(bullet_info->bullet_mulriple == bullet_mul && bullet_info->bullet_kind == bullet_kind);
	if (!(bullet_info->bullet_mulriple == bullet_mul && bullet_info->bullet_kind == bullet_kind)) return false;

#ifndef TEST
	if (!server_user_item->IsAndroidUser())
	{
		if(m_pChairsStock[server_user_item->GetChairID()] < 0) return true;
	}
	else
	{
		if(g_android_stock < 0) return true;
	}

#endif

	int fish_multiple = fish_multiple_[fish_trace_info->fish_kind];
	SCORE fish_score = fish_multiple_[fish_trace_info->fish_kind] * bullet_mul;
	SCORE less_score = fish_multiple_[fish_trace_info->fish_kind] * max_bullet_multiple_;
	if (fish_trace_info->fish_kind == FISH_KIND_18) {
		int fish18_mul = fish_multiple_[fish_trace_info->fish_kind] + rand() % (fish18_max_multiple_ - fish_multiple_[fish_trace_info->fish_kind] + 1);
		fish_multiple = fish18_mul;
		fish_score = fish18_mul * bullet_mul;
		less_score = fish18_mul * max_bullet_multiple_;
	} else if (fish_trace_info->fish_kind == FISH_KIND_19) {
		int fish19_mul = fish_multiple_[fish_trace_info->fish_kind] + rand() % (fish19_max_multiple_ - fish_multiple_[fish_trace_info->fish_kind] + 1);
		fish_score = fish19_mul * bullet_mul;
		fish_multiple = fish19_mul;
		less_score = fish19_mul * max_bullet_multiple_;
	} else if (fish_trace_info->fish_kind == FISH_KIND_21) {
		fish_score = current_fish_lk_multiple_ * bullet_mul;
		less_score = current_fish_lk_multiple_ * max_bullet_multiple_;
		fish_multiple = current_fish_lk_multiple_;
	}
	if (bullet_kind >= BULLET_KIND_1_ION) 
	{
		if(bullet_energy[server_user_item->GetChairID()]==-1)return true;
		if(bullet_energy[server_user_item->GetChairID()]!=0 && bullet_id>bullet_energy[server_user_item->GetChairID()]) return true;
		fish_score *= 2;
		less_score *= 2;
	}
#ifndef TEST
	if (!server_user_item->IsAndroidUser() && m_pChairsStock[server_user_item->GetChairID()] - less_score < 0) return true;
	if (server_user_item->IsAndroidUser() && g_android_stock - less_score < 0) return true;

	int change_probability = -1;
	if (!server_user_item->IsAndroidUser()) change_probability = CheckUserFilter(server_user_item->GetGameID());
	double probability = static_cast<double>((rand() % 1000 + 1)) / 1000;
	int stock_crucial_count = stock_crucial_count_;
	double fish_probability = fish_capture_probability_[fish_trace_info->fish_kind];
	int fish20_catch_count = 0;
	double fish20_catch_probability = 0.0;
	bool fish20_config = false;
	if (fish_trace_info->fish_kind == FISH_KIND_20/* && !server_user_item->IsAndroidUser()*/) {
		fish20_config = CheckFish20Config(server_user_item->GetGameID(), &fish20_catch_count, &fish20_catch_probability);
		if (fish20_config) {
			fish_probability = fish20_catch_probability;
		}
	}
	// 机器人能打中企鹅
	if (fish_trace_info->fish_kind == FISH_KIND_20 && server_user_item->IsAndroidUser()) fish_probability = 0.001;

	if (change_probability == 0) {
		fish_probability *= 0.2;
	} else if (change_probability == 1) {
		fish_probability *= 1.3;
	}
	if (special_scene_ && (fish_trace_info->fish_kind == FISH_KIND_1 || fish_trace_info->fish_kind == FISH_KIND_2)) fish_probability *= 0.7;

	// 炸弹库根据库存调整概率
	if (!server_user_item->IsAndroidUser())
	{
		if(fish_trace_info->fish_kind == FISH_KIND_23 && m_pChairsStock[server_user_item->GetChairID()] < bomb_stock_)
			fish_probability = 0;
		if (fish_trace_info->fish_kind == FISH_KIND_24 && m_pChairsStock[server_user_item->GetChairID()] < super_bomb_stock_)
			fish_probability = 0;
	}
	else
	{
		if(fish_trace_info->fish_kind == FISH_KIND_23 && g_android_stock < bomb_stock_)
			fish_probability = 0;
		if (fish_trace_info->fish_kind == FISH_KIND_24 && g_android_stock < super_bomb_stock_)
			fish_probability = 0;
	}

	//////////////////////////////////////////////////////新加对3-9号鱼的难度处理
	//if (!server_user_item->IsAndroidUser())
	{
		static int nFish=0;	//捕中过,10次经过逻辑一次
		static bool bRunFish=true;	//控制周期内是否捕中
		static double m_dRand[7]={0.2,0.3,0.5,0.6,0.8,0.4,1.0};//几率随机



		if (bRunFish==false)
		{
			if(fish_trace_info->fish_kind == FISH_KIND_3 || fish_trace_info->fish_kind == FISH_KIND_4 || fish_trace_info->fish_kind == FISH_KIND_5 || fish_trace_info->fish_kind == FISH_KIND_6 || fish_trace_info->fish_kind == FISH_KIND_7 || fish_trace_info->fish_kind == FISH_KIND_8 || fish_trace_info->fish_kind == FISH_KIND_9)
			{
				nFish++;

				if(nFish>=10)
				{
					nFish=0;
					bRunFish = true;//10次后走正常逻辑，不在走概率随机
				}
				else
				{  
					fish_probability *= m_dRand[rand()%7];
				}
			}
		}

		if(bRunFish)
		{
			if(fish_trace_info->fish_kind == FISH_KIND_3 || fish_trace_info->fish_kind == FISH_KIND_4 || fish_trace_info->fish_kind == FISH_KIND_5 || fish_trace_info->fish_kind == FISH_KIND_6 || fish_trace_info->fish_kind == FISH_KIND_7 || fish_trace_info->fish_kind == FISH_KIND_8 || fish_trace_info->fish_kind == FISH_KIND_9)
			{
				bRunFish = false;
			}
		}
	}
	//////////////////////////////////////////////////////

	// 机器人打中几率增加
	// if (server_user_item->IsAndroidUser()) fish_probability *= 1.3;

	while ((--stock_crucial_count) >= 0)
	{
		if (m_pChairsStock[server_user_item->GetChairID()] >= stock_crucial_score_[stock_crucial_count]) 
		{
			if (probability > (fish_probability * (stock_increase_probability_[stock_crucial_count] + 1))) 
			{
				return true;
			}
			else 
			{
				break;
			}
		}
	}
	if (fish20_config) AddFish20Config(server_user_item->GetGameID(), fish20_catch_count - 1, fish20_catch_probability);
#endif

	WORD chair_id = server_user_item->GetChairID();

	if (fish_trace_info->fish_kind == FISH_KIND_23 || fish_trace_info->fish_kind == FISH_KIND_24 || (fish_trace_info->fish_kind >= FISH_KIND_31 && fish_trace_info->fish_kind <= FISH_KIND_40)) {
		SaveSweepFish(fish_trace_info->fish_kind, fish_id, bullet_kind, bullet_mul);
		CMD_S_CatchSweepFish catch_sweep_fish;
		catch_sweep_fish.chair_id = chair_id;
		catch_sweep_fish.fish_id = fish_id;
		table_frame_->SendTableData(server_user_item->IsAndroidUser() ? android_chairid_ : chair_id, SUB_S_CATCH_SWEEP_FISH, &catch_sweep_fish, sizeof(catch_sweep_fish));
		table_frame_->SendLookonData(server_user_item->IsAndroidUser() ? android_chairid_ : chair_id, SUB_S_CATCH_SWEEP_FISH, &catch_sweep_fish, sizeof(catch_sweep_fish));
	} else {
		fish_score_[chair_id] += fish_score;
		if (!server_user_item->IsAndroidUser()) 
			m_pChairsStock[server_user_item->GetChairID()] -= fish_score;
		else
			g_android_stock -= fish_score;

		CMD_S_CatchFish catch_fish;
		catch_fish.bullet_ion = fish_multiple >= 15 && (rand() % 100 < 10);
		catch_fish.chair_id = server_user_item->GetChairID();
		catch_fish.fish_id = fish_id;
		catch_fish.fish_kind = fish_trace_info->fish_kind;
		catch_fish.fish_score = fish_score;
		if (fish_trace_info->fish_kind == FISH_KIND_22) {
			table_frame_->SetGameTimer(kLockTimer, kLockTime * 1000, 1, 0);
			KillAllGameTimer();
		}

		if (catch_fish.bullet_ion) {
			table_frame_->SetGameTimer(kBulletIonTimer + chair_id, kBulletIonTime * 1000, 1, 0);
			bullet_energy[chair_id]=0;
		}

		table_frame_->SendTableData(INVALID_CHAIR, SUB_S_CATCH_FISH, &catch_fish, sizeof(catch_fish));
		table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_CATCH_FISH, &catch_fish, sizeof(catch_fish));

		if (fish_trace_info->fish_kind == FISH_KIND_21) {
			current_fish_lk_multiple_ = fish_multiple_[FISH_KIND_21];
		} else if (fish_trace_info->fish_kind == FISH_KIND_20) {
			TCHAR tips_msg[1024] = { 0 };
			_sntprintf(tips_msg, CountArray(tips_msg), TEXT("%s %d桌的企鹅王被大侠%s打死了，获得%I64d鱼币奖励!"),
				game_service_option_->szServerName, table_frame_->GetTableID() + 1, server_user_item->GetNickName(), fish_score);
			//table_frame_->SendGlobalMessage(tips_msg, SMT_CHAT | SMT_GLOBAL);
		}
	}

	FreeFishTrace(fish_trace_info);
	FreeBulletInfo(chair_id, bullet_info);

	return true;
}

// 玩家抓到炸弹
bool TableFrameSink::OnSubCatchSweepFish(IServerUserItem* server_user_item, int fish_id, int* catch_fish_id, int catch_fish_count) {
	SweepFishInfo* sweep_fish_info = GetSweepFish(fish_id);
	if (sweep_fish_info == NULL) return true;
	assert(sweep_fish_info->fish_kind == FISH_KIND_23 || sweep_fish_info->fish_kind == FISH_KIND_24 || (sweep_fish_info->fish_kind >= FISH_KIND_31 && sweep_fish_info->fish_kind <= FISH_KIND_40));
	if (!(sweep_fish_info->fish_kind == FISH_KIND_23 || sweep_fish_info->fish_kind == FISH_KIND_24 || (sweep_fish_info->fish_kind >= FISH_KIND_31 && sweep_fish_info->fish_kind <= FISH_KIND_40))) return false;

	WORD chair_id = server_user_item->GetChairID();

	SCORE fish_score = fish_multiple_[sweep_fish_info->fish_kind] * sweep_fish_info->bullet_mulriple;
	if (sweep_fish_info->fish_kind == FISH_KIND_18) {
		int fish18_mul = fish_multiple_[sweep_fish_info->fish_kind] + rand() % (fish18_max_multiple_ - fish_multiple_[sweep_fish_info->fish_kind] + 1);
		fish_score = fish18_mul * sweep_fish_info->bullet_mulriple;
	} else if (sweep_fish_info->fish_kind == FISH_KIND_19) {
		int fish19_mul = fish_multiple_[sweep_fish_info->fish_kind] + rand() % (fish19_max_multiple_ - fish_multiple_[sweep_fish_info->fish_kind] + 1);
		fish_score = fish19_mul * sweep_fish_info->bullet_mulriple;
	}
	FishTraceInfoVecor::iterator iter;
	FishTraceInfo* fish_trace_info = NULL;
	for (int i = 0; i < catch_fish_count; ++i) {
		for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
			fish_trace_info = *iter;
			if (fish_trace_info->fish_id == catch_fish_id[i]) {
				fish_score += fish_multiple_[fish_trace_info->fish_kind] * sweep_fish_info->bullet_mulriple;
				active_fish_trace_vector_.erase(iter);
				storage_fish_trace_vector_.push_back(fish_trace_info);
				break;
			}
		}
	}
	if (sweep_fish_info->bullet_kind >= BULLET_KIND_1_ION) fish_score *= 2;

	if (!server_user_item->IsAndroidUser()) 
	{
		if(m_pChairsStock[server_user_item->GetChairID()] - fish_score < 0) 
		{		  
			return true;
		}
		m_pChairsStock[server_user_item->GetChairID()] -= fish_score;
	}
	else
	{
		if(g_android_stock - fish_score < 0) return true;
		g_android_stock -= fish_score;
	}
	fish_score_[chair_id] += fish_score;
	CMD_S_CatchSweepFishResult catch_sweep_result;
	memset(&catch_sweep_result, 0, sizeof(catch_sweep_result));
	catch_sweep_result.fish_id = fish_id;
	catch_sweep_result.chair_id = chair_id;
	catch_sweep_result.fish_score = fish_score;
	catch_sweep_result.catch_fish_count = catch_fish_count;
	memcpy(catch_sweep_result.catch_fish_id, catch_fish_id, catch_fish_count * sizeof(int));
	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_CATCH_SWEEP_FISH_RESULT, &catch_sweep_result, sizeof(catch_sweep_result));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_CATCH_SWEEP_FISH_RESULT, &catch_sweep_result, sizeof(catch_sweep_result));

	FreeSweepFish(fish_id);

	return true;
}

bool TableFrameSink::OnSubHitFishLK(IServerUserItem* server_user_item, int fish_id) {
	FishTraceInfo* fish_trace_info = GetFishTraceInfo(fish_id);
	if (fish_trace_info == NULL) return true;
	if (fish_trace_info->fish_kind != FISH_KIND_21) return true;
	if (current_fish_lk_multiple_ >= fishLK_max_multiple_) return true;

	++current_fish_lk_multiple_;
	CMD_S_HitFishLK hit_fish;
	hit_fish.chair_id = server_user_item->GetChairID();
	hit_fish.fish_id = fish_id;
	hit_fish.fish_mulriple = current_fish_lk_multiple_;
	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));

	return true;
}

bool TableFrameSink::OnSubStockOperate(IServerUserItem* server_user_item, unsigned char operate_code) {
	if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;

	SCORE sum = m_pChairsStock[0];
	sum += m_pChairsStock[1];
	sum += m_pChairsStock[2];
	sum += m_pChairsStock[3];
	sum += m_pChairsStock[4];
	sum += m_pChairsStock[5];
	CMD_S_StockOperateResult stock_operate_result;
	stock_operate_result.operate_code = operate_code;
	if (operate_code == 0) {
		stock_operate_result.stock_score = sum;
	} else if (operate_code == 1) {
		CString str;
		str.Format(TEXT("【%s】清空了库存%I64d。"), server_user_item->GetNickName(), sum);
		stock_operate_result.stock_score = m_pChairsStock[0] = m_pChairsStock[1] = m_pChairsStock[2] = m_pChairsStock[3] = m_pChairsStock[4] = m_pChairsStock[5] = 0;
		CTraceService::TraceString(str, TraceLevel_Info);
	} else if (operate_code == 2) {
		int plan = mt_rand(1, 5);
		switch(plan) {
		case 1:
			DistributePlanA(game_service_option_->lCellScore, m_pChairsStock, m_pGunsScore);
			break;
		case 2:
			DistributePlanB(game_service_option_->lCellScore, m_pChairsStock, m_pGunsScore);
			break;
		case 3:
			DistributePlanC(game_service_option_->lCellScore, m_pChairsStock, m_pGunsScore);
			break;
		}
		stock_operate_result.stock_score = sum;
		CString str;
		str.Format(TEXT("【%s】添加了库存%d。"), server_user_item->GetNickName(),game_service_option_->lCellScore);
		CTraceService::TraceString(str, TraceLevel_Info);	
	} else if (operate_code == 3) {
		stock_operate_result.stock_score = g_revenue_score;
	}

	table_frame_->SendUserItemData(server_user_item, SUB_S_STOCK_OPERATE_RESULT, &stock_operate_result, sizeof(stock_operate_result));

	return true;
}

bool TableFrameSink::OnSubFish20Config(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability) {
	if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;

	AddFish20Config(game_id, catch_count, catch_probability);

	return true;
}

bool TableFrameSink::OnSubUserFilter(IServerUserItem* server_user_item, DWORD game_id, unsigned char operate_code) {
	if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;

	AddUserFilter(game_id, operate_code);

	return true;
}

void TableFrameSink::BuildInitTrace(FPoint init_pos[5], int init_count, FishKind fish_kind, TraceType trace_type) {
	assert(init_count >= 2 && init_count <= 3);
	srand(GetTickCount() + rand() % kResolutionWidth);
	WORD chair_id = rand() % GAME_PLAYER;
	int center_x = kResolutionWidth / 2;
	int center_y = kResolutionHeight / 2;
	int factor = rand() % 2 == 0 ? 1 : -1;
	switch (chair_id) {
	case 0:
	case 1:
	case 2:
		init_pos[0].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[0].y = 0.f - static_cast<float>(fish_bounding_box_height_[fish_kind]) * 2;
		init_pos[1].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y + (rand() % center_y));
		init_pos[2].x = static_cast<float>(center_x - factor * (rand() % center_x));
		init_pos[2].y = static_cast<float>(kResolutionHeight + fish_bounding_box_height_[fish_kind] * 2);
		break;
	case 3:
		init_pos[0].x = static_cast<float>(kResolutionWidth + fish_bounding_box_width_[fish_kind] * 2);
		init_pos[0].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[1].x = static_cast<float>(center_x - (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[2].x = -static_cast<float>(fish_bounding_box_width_[fish_kind] * 2);
		init_pos[2].y = static_cast<float>(center_y - factor* (rand() % center_y));
		break;
	case 5:
	case 6:
	case 4:
		init_pos[0].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[0].y = kResolutionHeight + static_cast<float>(fish_bounding_box_height_[fish_kind] * 2);
		init_pos[1].x = static_cast<float>(center_x + factor * (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y - (rand() % center_y));
		init_pos[2].x = static_cast<float>(center_x - factor * (rand() % center_x));
		init_pos[2].y = static_cast<float>(-fish_bounding_box_height_[fish_kind] * 2);
		break;
	case 7:
		init_pos[0].x = static_cast<float>(-fish_bounding_box_width_[fish_kind] * 2);
		init_pos[0].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[1].x = static_cast<float>(center_x + (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[2].x = static_cast<float>(kResolutionWidth + fish_bounding_box_width_[fish_kind] * 2);
		init_pos[2].y = static_cast<float>(center_y - factor* (rand() % center_y));
		break;
	}

	if (trace_type == TRACE_LINEAR && init_count == 2) {
		init_pos[1].x = init_pos[2].x;
		init_pos[1].y = init_pos[2].y;
	}
}

void TableFrameSink::BuildFishTrace(int fish_count, FishKind fish_kind_start, FishKind fish_kind_end) {
	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	WORD send_size = 0;
	CMD_S_FishTrace* fish_trace = reinterpret_cast<CMD_S_FishTrace*>(tcp_buffer);

	DWORD build_tick = GetTickCount();
	srand(build_tick + fish_count * 123321);
	for (int i = 0; i < fish_count; ++i) {
		if (send_size + sizeof(CMD_S_FishTrace) > sizeof(tcp_buffer)) {
			table_frame_->SendTableData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
			table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
			send_size = 0;
		}

		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = static_cast<FishKind>(fish_kind_start + (rand() + i) % (fish_kind_end - fish_kind_start + 1));
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		fish_trace->fish_id = fish_trace_info->fish_id;
		fish_trace->fish_kind = fish_trace_info->fish_kind;
		if (fish_trace_info->fish_kind == FISH_KIND_1 || fish_trace_info->fish_kind == FISH_KIND_2) {
			fish_trace->init_count = 2;
			fish_trace->trace_type = TRACE_LINEAR;
		} else {
			fish_trace->init_count = 3;
			fish_trace->trace_type = TRACE_BEZIER;
		}
		BuildInitTrace(fish_trace->init_pos, fish_trace->init_count, fish_trace->fish_kind, fish_trace->trace_type);

		send_size += sizeof(CMD_S_FishTrace);
		++fish_trace;
	}

	if (send_size > 0) {
		table_frame_->SendTableData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
		table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
	}
}

void TableFrameSink::BuildSceneKind1() {
	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 100; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 100;
	for (int i = 0; i < 17; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_3;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 17;
	for (int i = 0; i < 17; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_5;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 17;
	for (int i = 0; i < 30; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_2;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 30;
	for (int i = 0; i < 30; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_4;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 30;
	for (int i = 0; i < 15; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_6;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 15;

	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_20;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();

	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void TableFrameSink::BuildSceneKind2() {
	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 200; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 200;
	for (int i = 0; i < 14; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = static_cast<FishKind>(FISH_KIND_12 + i % 7);
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 14;

	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void TableFrameSink::BuildSceneKind3() {
	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 50; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 50;

	for (int i = 0; i < 40; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_3;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;

	for (int i = 0; i < 30; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_4;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 30;

	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_16;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();

	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	for (int i = 0; i < 50; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 50;

	for (int i = 0; i < 40; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_2;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;

	for (int i = 0; i < 30; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_5;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 30;

	fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_17;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();

	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void TableFrameSink::BuildSceneKind4() {
	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_11;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_12;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_13;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_14;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_15;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_16;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_17;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;
	for (int i = 0; i < 8; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_18;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 8;

	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void TableFrameSink::BuildSceneKind5() {
	BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
	CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
	switch_scene->scene_kind = next_scene_kind_;
	DWORD build_tick = GetTickCount();
	switch_scene->fish_count = 0;
	for (int i = 0; i < 40; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_1;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 40; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_2;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 40; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_5;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 40; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_3;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 40;
	for (int i = 0; i < 24; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_4;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 24;
	for (int i = 0; i < 24; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_6;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 24;
	for (int i = 0; i < 13; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_7;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 13;
	for (int i = 0; i < 13; ++i) {
		FishTraceInfo* fish_trace_info = ActiveFishTrace();
		fish_trace_info->fish_kind = FISH_KIND_6;
		fish_trace_info->build_tick = build_tick;
		fish_trace_info->fish_id = GetNewFishID();

		switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
		switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
	}
	switch_scene->fish_count += 13;

	FishTraceInfo* fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_18;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();
	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	fish_trace_info = ActiveFishTrace();
	fish_trace_info->fish_kind = FISH_KIND_17;
	fish_trace_info->build_tick = build_tick;
	fish_trace_info->fish_id = GetNewFishID();
	switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
	switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
	switch_scene->fish_count += 1;

	table_frame_->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
	table_frame_->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void TableFrameSink::ClearFishTrace(bool force) {
	if (force) {
		std::copy(active_fish_trace_vector_.begin(), active_fish_trace_vector_.end(), std::back_inserter(storage_fish_trace_vector_));
		active_fish_trace_vector_.clear();
	} else {
		FishTraceInfoVecor::iterator iter;
		FishTraceInfo* fish_trace_info = NULL;
		DWORD now_tick = GetTickCount();
		for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end();) {
			fish_trace_info = *iter;
			if (now_tick >= (fish_trace_info->build_tick + kFishAliveTime)) {
				iter = active_fish_trace_vector_.erase(iter);
				storage_fish_trace_vector_.push_back(fish_trace_info);
			} else {
				++iter;
			}
		}
	}
}

void TableFrameSink::CalcScore(IServerUserItem* server_user_item) {
	if (server_user_item == NULL) return;
	WORD chair_id = server_user_item->GetChairID();

	tagScoreInfo score_info;
	memset(&score_info, 0, sizeof(score_info));
	score_info.cbType = SCORE_TYPE_WIN;
	score_info.lScore = (fish_score_[chair_id] - exchange_fish_score_[chair_id]) * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
	table_frame_->WriteUserScore(chair_id, score_info);

	fish_score_[chair_id] = 0;
	exchange_fish_score_[chair_id] = 0;
}
