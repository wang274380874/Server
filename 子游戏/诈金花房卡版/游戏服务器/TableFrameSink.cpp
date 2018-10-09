#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
//const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_ALL_READY;	//开始模式
LONGLONG			CTableFrameSink::m_lStockScore=0;						//机器库存

//定时器 0~30
#define IDI_GAME_COMPAREEND					1									//结束定时器
#define IDI_GAME_OPENEND					2									//结束定时器
#define IDI_ADD_SCORE						3									//结束定时器

#define TIME_GAME_COMPAREEND				6000								//结束定时器
#define TIME_GAME_OPENEND					6000								//结束定时器
#define TIME_ADD_SCORE						1000								//结束定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
	m_bOperaCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	m_bGameEnd=false;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));
	m_wPlayerCount = GAME_PLAYER;
	//用户状态
	ZeroMemory(&m_StGameEnd,sizeof(m_StGameEnd));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbGiveUpUser,sizeof(m_cbGiveUpUser));
	for(int i=0;i<m_wPlayerCount;i++)
	{
		m_bMingZhu[i]=false;
		m_wCompardUser[i].RemoveAll();
	}
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=0L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//控制变量
	m_bUpdataStorage=false;
	m_lStorageMax = 0;
	m_nStorageMul = 0;
	m_lStockScore = 0;
	m_lStorageDeduct = 0;

	//服务控制
	m_hControlInst = NULL;
	m_pServerContro = NULL;
	m_hControlInst = LoadLibrary(TEXT("ZaJinHuaServerControl.dll"));
	if ( m_hControlInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hControlInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//释放对象
VOID CTableFrameSink::Release()
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}

	delete this;
}

//接口查询--检测相关信息版本
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\ZaJinHuaConfig.ini"),szPath);

	//开始存储数目
	TCHAR szServerName[MAX_PATH]=TEXT("");
	_sntprintf(szServerName,CountArray(szServerName),TEXT("%s"),m_pGameServiceOption->szServerName);
	m_lStockScore=GetPrivateProfileInt(szServerName,TEXT("StorageStart"),0,szFileName);
	m_lStorageDeduct=GetPrivateProfileInt(szServerName,TEXT("StorageDeduct"),0,szFileName);
	m_lStorageMax = GetPrivateProfileInt(szServerName, TEXT("StorageMax"), 50000000, szFileName);
	m_nStorageMul = GetPrivateProfileInt(szServerName, TEXT("StorageMul"), 50, szFileName);

	if (m_lStorageDeduct < 0 || m_lStorageDeduct > 1000) m_lStorageDeduct = 0;
	if (m_nStorageMul < 0 || m_nStorageMul > 100) m_nStorageMul = 50;
	if (m_lStorageDeduct > 1000) m_lStorageDeduct = 1000;

	return true;
}

//复位桌子
void  CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_bOperaCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));

	//用户状态
	ZeroMemory(&m_StGameEnd,sizeof(m_StGameEnd));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbGiveUpUser,sizeof(m_cbGiveUpUser));
	for(int i=0;i<m_wPlayerCount;i++)
	{
		m_bMingZhu[i]=false;	
		m_wCompardUser[i].RemoveAll();
	}
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxCellScore=0L;						
	m_lCellScore=0L;						
	m_lCurrentTimes=0L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	return;
}

//开始模式
BYTE  CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount);
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	//库存
	if(m_lStockScore>0 && NeedDeductStorage()) m_lStockScore = m_lStockScore-m_lStockScore*m_lStorageDeduct/1000;

	//发送库存消息
	for (WORD i=0;i< m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if ( pIServerUserItem == NULL )
			continue;

		if( CUserRight::IsGameCheatUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("当前库存：%I64d"), m_lStockScore);

			m_pITableFrame->SendGameMessage(pIServerUserItem,strInfo,SMT_CHAT);
		}	
	}

	//写如库存日志
	CString strInfo;
	strInfo.Format(TEXT("当前库存：%I64d 【%d】桌"), m_lStockScore, m_pITableFrame->GetTableID() + 1);
	NcaTextOut(strInfo, m_pGameServiceOption->szServerName);

	//删除定时器
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

	//设置状态
	m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

	m_bGameEnd=false;

	//最大下注
	LONGLONG lTimes=6L;
	BYTE cbAiCount = 0;
	BYTE cbPlayCount = 0;
	
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		const LONGLONG lUserScore=pIServerUserItem->GetUserScore();

		//设置变量
		m_cbPlayStatus[i]=TRUE;
		m_lUserMaxScore[i]=lUserScore;
		cbPlayCount++;
		if (pIServerUserItem->IsAndroidUser() == true) 
		{
			cbAiCount++;
			m_cbAndroidStatus[i] = TRUE;
		}
		else
		{
			m_cbRealPlayer[i] = TRUE;
		}

		//判断单注
		LONGLONG Temp=lTimes;
		if(m_lUserMaxScore[i]<10001)Temp=1L;
		else if(m_lUserMaxScore[i]<100001)Temp=2L;
		else if(m_lUserMaxScore[i]<1000001)Temp=3L;
		else if(m_lUserMaxScore[i]<10000001)Temp=4L;
		else if(m_lUserMaxScore[i]<100000001)Temp=5L;
		if(lTimes>Temp)lTimes=Temp;
	}

	m_bUpdataStorage = (cbAiCount != cbPlayCount);
	//下注变量
	m_lCellScore = 1;
	//m_lCellScore=m_pGameServiceOption->lCellScore;
	if(m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
	{
		while ((lTimes--) > 0) m_lCellScore *= 10;//add by yang yang 2018年9月30日 17:25:03
	}


	//基础分数
	for(BYTE i=0;i< m_wPlayerCount;i++)
	{
		if(m_lUserMaxScore[i] != 0)
			m_lCellScore=__min(m_lUserMaxScore[i],m_lCellScore);
	}

	m_lCurrentTimes=1;
	m_lMaxCellScore=m_lCellScore*10;

	//最大下注
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)
		{
			m_lUserMaxScore[i] = __min(m_lUserMaxScore[i],m_lMaxCellScore*101);
			if(m_pGameServiceOption->lRestrictScore != 0)
				m_lUserMaxScore[i]=__min(m_lUserMaxScore[i],m_pGameServiceOption->lRestrictScore);
		}
	}

	//分发扑克
	m_GameLogic.RandCardList(m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));

	//游戏开始
	AnalyseStartCard();

	//机器人消息
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//机器人数据
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL) continue;
		if (pServerUserItem->IsAndroidUser() == true)
		{
			//机器人数据
			CMD_S_AndroidCard AndroidCard ;
			ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

			//设置变量
			AndroidCard.lStockScore = m_lStockScore;
			CopyMemory(AndroidCard.cbRealPlayer, m_cbRealPlayer, sizeof(m_cbRealPlayer));
			CopyMemory(AndroidCard.cbAndroidStatus, m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
			CopyMemory(AndroidCard.cbAllHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
			m_pITableFrame->SendTableData(i, SUB_S_ANDROID_CARD, &AndroidCard, sizeof(AndroidCard));
		}
	}


	//设置庄家
	if(m_wBankerUser==INVALID_CHAIR) m_wBankerUser=rand()%m_wPlayerCount;

	//庄家离开
	if(m_wBankerUser<m_wPlayerCount && m_cbPlayStatus[m_wBankerUser]==FALSE) m_wBankerUser=rand()%m_wPlayerCount;

	//确定庄家
	while(m_cbPlayStatus[m_wBankerUser]==FALSE)
	{
		m_wBankerUser = (m_wBankerUser+1)%m_wPlayerCount;
	}

	//用户设置
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			m_lTableScore[i]=m_lCellScore;
		}
	}

	//当前用户
	m_wCurrentUser=(m_wBankerUser+1)%m_wPlayerCount;
	while(m_cbPlayStatus[m_wCurrentUser]==FALSE)
	{
		m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	}

	//构造数据
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.lMaxScore=m_lMaxCellScore;
	GameStart.lCellScore=m_lCellScore;
	GameStart.lCurrentTimes=m_lCurrentTimes;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.wBankerUser=m_wBankerUser;
	CopyMemory(GameStart.cbPlayStatus, m_cbPlayStatus, sizeof(BYTE)*GAME_PLAYER);

	//发送数据
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			GameStart.lUserMaxScore=m_lUserMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//服务控制
	if (m_pServerContro && m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
	{
		//服务信息
		BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT];
		CopyMemory(cbHandCardData, m_cbHandCardData, sizeof(cbHandCardData) );

		//排列扑克
		for (WORD i= 0;i < m_wPlayerCount; i++)
			m_GameLogic.SortCardList(cbHandCardData[i], MAX_COUNT);

		//信息结算
		m_pServerContro->ServerControl(cbHandCardData, m_pITableFrame);
	}

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_COMPARECARD:	//比牌结束
	case GER_NO_PLAYER:		//没有玩家
		{
			if(m_bGameEnd)return true;
			m_bGameEnd=true;

			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<4;j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//唯一玩家
			WORD wWinner,wUserCount=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{	
				if(m_cbPlayStatus[i]==TRUE)
				{
					wUserCount++;
					wWinner=i;
					if(GER_COMPARECARD==cbReason)ASSERT(m_wBankerUser==i);
					m_wBankerUser=i;
				}
			}

			//胜利者强退
			if (wUserCount==0)
			{
				wWinner=m_wBankerUser;
			}

			//延迟开始
			bool bDelayOverGame = false;

			//计算总注
			LONGLONG lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
				GameEnd.lGameScore[i]=-m_lTableScore[i];
				lWinnerScore+=m_lTableScore[i];
			}

			//处理税收
			GameEnd.lGameTax=m_pITableFrame->CalculateRevenue(wWinner,lWinnerScore);
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//扑克数据
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				WORD wCount=0;
				while(m_wCompardUser[i].GetCount()>0)
				{
					GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
					m_wCompardUser[i].RemoveAt(0);
				}
			}

			GameEnd.wEndState=0;

			//胜利者强退
			if (wUserCount==1)
			{
				//修改积分
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if(m_cbPlayStatus[i]==FALSE)continue;
					ScoreInfoArray[i].cbType =(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
					ScoreInfoArray[i].lRevenue=GameEnd.lGameTax;                                  
					ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];

					if (ScoreInfoArray[i].cbType == SCORE_TYPE_LOSE && bDelayOverGame == false)
					{
						IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pUserItem!=NULL && (pUserItem->GetUserScore()+GameEnd.lGameScore[i])<m_pGameServiceOption->lMinTableScore)
						{
							bDelayOverGame = true;
						}
					}
				}

				m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);
			}

			//库存统计
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//获取用户
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;
				if(!m_cbPlayStatus[i]) continue;

				//库存累计
				if(!pIServerUserIte->IsAndroidUser())
					UpdateStorage(-GameEnd.lGameScore[i]);//m_lStockScore -= GameEnd.lGameScore[i];

			}
			//房间类型
			GameEnd.wServerType = m_pGameServiceOption->wServerType;
			//延迟开始
			if(bDelayOverGame)
			{
				GameEnd.bDelayOverGame = true;
			}

			if(wChairID==GAME_PLAYER)
			{
				//结束数据
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SetGameTimer(IDI_GAME_COMPAREEND,TIME_GAME_COMPAREEND,1,0);
			}
			else
			{
				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:		//网络中断
		{
			if(m_bGameEnd || m_cbPlayStatus[wChairID]==FALSE) return true;

			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//强退处理
			return OnUserGiveUp(wChairID,true);
		}
	case GER_OPENCARD:		//开牌结束   
		{
			if(m_bGameEnd)return true;
			m_bGameEnd = true;

			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<4;j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//胜利玩家
			WORD wWinner=m_wBankerUser;

			//计算分数
			LONGLONG lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
				lWinnerScore+=m_lTableScore[i];
				GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//处理税收
			GameEnd.lGameTax=m_pITableFrame->CalculateRevenue(wWinner,lWinnerScore);
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			//开牌结束
			GameEnd.wEndState=1;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//扑克数据
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				WORD wCount=0;
				while(m_wCompardUser[i].GetCount()>0)
				{
					GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
					m_wCompardUser[i].RemoveAt(0);
				}
			}

			//修改积分
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
			for (WORD i=0; i<m_wPlayerCount; i++) 
			{				
				if(i==wWinner || m_cbPlayStatus[i]==FALSE)continue;
				ScoreInfo[i].lScore=GameEnd.lGameScore[i];
				ScoreInfo[i].cbType=SCORE_TYPE_LOSE;
			}
			ScoreInfo[wWinner].lScore=GameEnd.lGameScore[wWinner];
			ScoreInfo[wWinner].cbType=SCORE_TYPE_WIN;
			ScoreInfo[wWinner].lRevenue=GameEnd.lGameTax;
	
			m_pITableFrame->WriteTableScore(ScoreInfo,m_wPlayerCount);

			//库存统计
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//获取用户
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;
				if(!m_cbPlayStatus[i]) continue;

				//库存累计
				if(!pIServerUserIte->IsAndroidUser())
					UpdateStorage(-GameEnd.lGameScore[i]);//m_lStockScore -= GameEnd.lGameScore[i];

			}

			//房间类型
			GameEnd.wServerType = m_pGameServiceOption->wServerType;

			if(wChairID==GAME_PLAYER)
			{
				//结束数据
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SetGameTimer(IDI_GAME_OPENEND,TIME_GAME_OPENEND,1,0);
			}
			else
			{
				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	return false;
}

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=0L;

			//房间名称
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_STATUS_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//加注信息
			StatusPlay.lMaxCellScore=m_lMaxCellScore;
			StatusPlay.lCellScore=m_lCellScore;
			StatusPlay.lCurrentTimes=m_lCurrentTimes;
			StatusPlay.lUserMaxScore=m_lUserMaxScore[wChiarID];

			//设置变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			CopyMemory(StatusPlay.bMingZhu,m_bMingZhu,sizeof(m_bMingZhu));
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));

			//房间名称
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

			//当前状态
			StatusPlay.bCompareState=(m_lCompareCount<=0)?false:true;

			//设置扑克
			if(m_bMingZhu[wChiarID])
				CopyMemory(&StatusPlay.cbHandCardData,&m_cbHandCardData[wChiarID],MAX_COUNT);

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件 
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	//结束定时
	if(wTimerID==IDI_GAME_COMPAREEND || IDI_GAME_OPENEND==wTimerID)
	{
		//删除时间
		if(wTimerID==IDI_GAME_COMPAREEND)m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
		else m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));

		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
	}
	if(wTimerID==IDI_ADD_SCORE)
	{
		m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
		if (m_wCurrentUser!=INVALID_CHAIR)
		{
			OnUserGiveUp(m_wCurrentUser,false);
		}		
	}

	return false;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_GIVE_UP:			//用户放弃
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_LOOK_CARD:		//用户看牌
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserLookCard(pUserData->wChairID);
		}
	case SUB_C_COMPARE_CARD:	//用户比牌
		{
		TCHAR szOut[256];
		wsprintf(szOut, TEXT("炸金花 SUB_C_COMPARE_CARD  start "));
		OutputDebugString(szOut);
		
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CompareCard));
			if (wDataSize != sizeof(CMD_C_CompareCard))
			{
				return false;
			}
			//变量定义
			CMD_C_CompareCard * pCompareCard=(CMD_C_CompareCard *)pData;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING)
			{
				wsprintf(szOut, TEXT("炸金花 SUB_C_COMPARE_CARD  pUserData->cbUserStatus = %d"), pUserData->cbUserStatus);
				OutputDebugString(szOut);
				return true;
			}

			//参数效验
			ASSERT(pUserData->wChairID==m_wCurrentUser);
			if(pUserData->wChairID!=m_wCurrentUser)return false;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_cbPlayStatus[pCompareCard->wCompareUser]==TRUE);
			if(m_cbPlayStatus[pUserData->wChairID]==FALSE || m_cbPlayStatus[pCompareCard->wCompareUser]==FALSE)
			{ 
				wsprintf(szOut, TEXT("炸金花 SUB_C_COMPARE_CARD  m_cbPlayStatus[pUserData->wChairID] = %d,pUserData->wChairID = %d,m_cbPlayStatus[pCompareCard->wCompareUser] = %d,pCompareCard->wCompareUser = %d"),
					m_cbPlayStatus[pUserData->wChairID], pUserData->wChairID,m_cbPlayStatus[pCompareCard->wCompareUser], pCompareCard->wCompareUser);
				OutputDebugString(szOut);
				return false;
			}
				

			//消息处理
			return OnUserCompareCard(pUserData->wChairID,pCompareCard->wCompareUser);
		}
	case SUB_C_ADD_SCORE:		//用户加注
		{
			//CTraceService::TraceString(TEXT("zzz onSubAddScore"),TraceLevel_Normal);
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) 
			{
				//构造提示
			//TCHAR szString[512]=TEXT("");
			//_sntprintf(szString,CountArray(szString),TEXT("zzz [ %d ]!=sizeof CMD_C_AddScore [ %d ]"),wDataSize,sizeof(CMD_C_AddScore));

			//提示消息
			//CTraceService::TraceString(szString,TraceLevel_Normal);
				return false;
			}

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_lCompareCount==0);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE || m_lCompareCount> 0)
			{
				//CTraceService::TraceString(TEXT("zzz m_cbPlayStatus[pUserData->wChairID]==FALSE || m_lCompareCount> 0"),TraceLevel_Normal);
				return false;
			}

			//当前状态
			if(pAddScore->wState>0)m_lCompareCount=pAddScore->lScore;

			//消息处理
			bool isOK = OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false,((pAddScore->wState>0)?true:false));
			//if(isOK)
			///{
				//CTraceService::TraceString(TEXT("zzz OK"),TraceLevel_Normal);
			//}
			//else
			//{
			//	CTraceService::TraceString(TEXT("zzz NO"),TraceLevel_Normal);
			//}
			return isOK;
		}
	case SUB_C_WAIT_COMPARE:	//等待比牌
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//参数效验
			ASSERT(pUserData->wChairID==m_wCurrentUser);
			if(pUserData->wChairID!=m_wCurrentUser)return false;

			//消息处理
			CMD_S_WaitCompare WaitCompare;
			WaitCompare.wCompareUser = pUserData->wChairID;
			if(m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) &&
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) )

			return true;
		}
	case SUB_C_FINISH_FLASH:	//完成动画 
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_wFlashUser[pUserData->wChairID]==TRUE);
			if (m_wFlashUser[pUserData->wChairID]==FALSE) return true;

			//设置变量
			m_wFlashUser[pUserData->wChairID] = FALSE;

			//结束游戏
			WORD i = 0;
			for(i=0;i< m_wPlayerCount;i++)
			{
				if(m_wFlashUser[i] != FALSE)break;
			}
			if(i== m_wPlayerCount)
			{
				//删除时间
				m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
				m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			return true;
		}
	case SUB_C_ADD_SCORE_TIME:
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE*30,1,0);
			return true;
		}
	}

	return false;
}

//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}



//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID,bool bExit)
{
	//设置数据
	m_cbGiveUpUser[wChairID] = TRUE;
	m_cbPlayStatus[wChairID] = FALSE;

	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//修改积分
	tagScoreInfo ScoreInfoArray[GAME_PLAYER];
	ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
	ScoreInfoArray[wChairID].cbType =(bExit)?SCORE_TYPE_FLEE:SCORE_TYPE_LOSE;
	ScoreInfoArray[wChairID].lRevenue=0;  
	ScoreInfoArray[wChairID].lScore=-m_lTableScore[wChairID];

	m_pITableFrame->WriteTableScore(ScoreInfoArray,m_wPlayerCount);

	//获取用户
	IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);

	//库存统计
	if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
		UpdateStorage(m_lTableScore[wChairID]);//m_lStockScore += m_lTableScore[wChairID];

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//判断结束
	if (wPlayerCount>=2)
	{
		if (m_wCurrentUser==wChairID) OnUserAddScore(wChairID,0L,true,false);
	}
	else OnEventGameConclude(INVALID_CHAIR,NULL,GER_NO_PLAYER);

	return true;
}

//看牌事件
bool CTableFrameSink::OnUserLookCard(WORD wChairID)
{
	//状态效验
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return false;

	//参数效验
	ASSERT(!m_bMingZhu[wChairID]);
	if (m_bMingZhu[wChairID]) return true;

	//设置参数
	m_bMingZhu[wChairID]=true;
	//构造数据
	CMD_S_LookCard LookCard;
	ZeroMemory(&LookCard,sizeof(LookCard));
	LookCard.wLookCardUser=wChairID;

	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LOOK_CARD,&LookCard,sizeof(LookCard));

	for(WORD i = 0;i< m_wPlayerCount;i++)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem == NULL ) continue;
		if(i == wChairID)
		{
			CopyMemory(LookCard.cbCardData,m_cbHandCardData[wChairID],sizeof(m_cbHandCardData[0]));
		}
		else
		{
			ZeroMemory(LookCard.cbCardData,sizeof(LookCard.cbCardData));
		}
		m_pITableFrame->SendTableData(i,SUB_S_LOOK_CARD,&LookCard,sizeof(LookCard));
	}

	return true;
}

//比牌事件
bool CTableFrameSink::OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID)
{
	//效验参数
	ASSERT(m_lCompareCount>0);
	if(!(m_lCompareCount>0))return false;
	//LONGLONG lTemp=(m_bMingZhu[m_wCurrentUser])?6:5;
	//ASSERT((m_lUserMaxScore[m_wCurrentUser]-m_lTableScore[m_wCurrentUser]+m_lCompareCount) >= (m_lMaxCellScore*lTemp));
	//if((m_lUserMaxScore[m_wCurrentUser]-m_lTableScore[m_wCurrentUser]+m_lCompareCount) < (m_lMaxCellScore*lTemp))return false;
	//原来>=2*m_lCellScore，如果第一轮都的积分都没能达到加注跟注的条件，那么只能开牌，而底注就是m_lCellScore，
	//那么比牌就一定会掉线，此时m_lTableScore[m_wCurrentUser]为底注+m_lCompareCount。
	//至于为什么要大于等于2*m_lCellScore，原因不明
	ASSERT(m_wCurrentUser==m_wBankerUser || m_lTableScore[m_wCurrentUser]-m_lCompareCount>=m_lCellScore);	
	if (!(m_wCurrentUser == m_wBankerUser || m_lTableScore[m_wCurrentUser] - m_lCompareCount >= m_lCellScore))
	{
		TCHAR szOut[256];
		wsprintf(szOut, TEXT("炸金花 OnUserCompareCard m_wCurrentUser = %d,m_wBankerUser = %d,m_lTableScore[m_wCurrentUser] = %64d,m_lCompareCount = %64d,m_lCellScore = %64d"),
			m_wCurrentUser, m_wBankerUser, m_lTableScore[m_wCurrentUser], m_lCompareCount, m_lCellScore);
		OutputDebugString(szOut);
		return false;
	}
	//删除定时器
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

	//比较大小
	BYTE cbWin=m_GameLogic.CompareCard(m_cbHandCardData[wFirstChairID],m_cbHandCardData[wNextChairID],MAX_COUNT);

	//状态设置
	m_lCompareCount=0;

	//胜利用户
	WORD wLostUser,wWinUser;
	if(cbWin==TRUE) 
	{
		wWinUser=wFirstChairID;
		wLostUser=wNextChairID;
	}
	else
	{
		wWinUser=wNextChairID;
		wLostUser=wFirstChairID;
	}

	//设置数据
	m_wCompardUser[wLostUser].Add(wWinUser);
	m_wCompardUser[wWinUser].Add(wLostUser);
	m_cbPlayStatus[wLostUser]=FALSE;

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//继续游戏
	if(wPlayerCount>=2)
	{
		//用户切换
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//设置变量
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//继续判断
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		//设置用户
		m_wCurrentUser=wNextPlayer;
	}
	else m_wCurrentUser=INVALID_CHAIR;

	//构造数据
	CMD_S_CompareCard CompareCard;
	CompareCard.wCurrentUser = m_wCurrentUser;
	CompareCard.wLostUser = wLostUser;
	CompareCard.wCompareUser[0] = wFirstChairID;
	CompareCard.wCompareUser[1] = wNextChairID;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CompareCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

	//修改积分
	tagScoreInfo ScoreInfoArray[GAME_PLAYER];
	ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
	ScoreInfoArray[wLostUser].cbType =SCORE_TYPE_LOSE;
	ScoreInfoArray[wLostUser].lRevenue=0;
	ScoreInfoArray[wLostUser].lScore=-m_lTableScore[wLostUser];

	m_pITableFrame->WriteTableScore(ScoreInfoArray, m_wPlayerCount);
	
	//库存统计
	IServerUserItem * pIServreUserItemLost=m_pITableFrame->GetTableUserItem(wLostUser);
	if ((pIServreUserItemLost!=NULL)&&(!pIServreUserItemLost->IsAndroidUser())) 
		UpdateStorage(m_lTableScore[wLostUser]);//m_lStockScore += m_lTableScore[wLostUser];

	//结束游戏
	if (wPlayerCount<2)
	{
		m_wBankerUser=wWinUser;
		m_wFlashUser[wNextChairID] = TRUE;
		m_wFlashUser[wFirstChairID] = TRUE;
		OnEventGameConclude(m_wPlayerCount,NULL,GER_COMPARECARD);
	}

	return true;
}

//开牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wUserID)
{
	//效验参数
	ASSERT(m_lCompareCount>0);
	if(!(m_lCompareCount>0))return false;
	ASSERT(m_wCurrentUser==wUserID);
	if(m_wCurrentUser!=wUserID)return false;
	//LONGLONG lTemp=(m_bMingZhu[wUserID])?6:5;
	//ASSERT((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) < (m_lMaxCellScore*lTemp));
	//if((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) >= (m_lMaxCellScore*lTemp))return false;

	//清理数据
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount = 0;

	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//比牌玩家
	WORD wWinner=wUserID;

	//查找最大玩家
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		WORD w=(wUserID+i)%m_wPlayerCount;

		//用户过滤
		if (m_cbPlayStatus[w]==FALSE) continue;

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[w],cbUserCardData[wWinner],MAX_COUNT)>=TRUE) 
		{
			wWinner=w;
		}
	}
	ASSERT(m_cbPlayStatus[wWinner]==TRUE);
	if(m_cbPlayStatus[wWinner]==FALSE)return false;

	//胜利玩家
	m_wBankerUser = wWinner;

	//构造数据
	CMD_S_OpenCard OpenCard;
	OpenCard.wWinner=wWinner;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));

	//结束游戏
	for(WORD i=0;i< m_wPlayerCount;i++)if(m_cbPlayStatus[i]==TRUE)m_wFlashUser[i] = TRUE;
	OnEventGameConclude(m_wPlayerCount,NULL,GER_OPENCARD);

	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore, bool bGiveUp, bool bCompareCard)
{
	//CTraceService::TraceString(TEXT("zzz 00"),TraceLevel_Normal);
	if (bGiveUp==false)				//设置数据
	{
		//状态效验
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) 
		{
			//提示消息
			//CTraceService::TraceString(TEXT("zzz 11"),TraceLevel_Normal);
			return false;
		}
			
		if (m_pITableFrame->GetGameServiceOption()->wServerType != GAME_GENRE_PERSONAL)
		{


			//金币效验
			ASSERT(lScore >= 0 && lScore%m_lCellScore == 0);
			ASSERT((lScore + m_lTableScore[wChairID]) <= m_lUserMaxScore[wChairID]);
			if (lScore < 0 || lScore % m_lCellScore != 0)
			{
				//提示消息
				//CTraceService::TraceString(TEXT("zzz 22"),TraceLevel_Normal);
				return false;
			}
			if ((lScore + m_lTableScore[wChairID]) > m_lUserMaxScore[wChairID])
			{
				//提示消息
				//CTraceService::TraceString(TEXT("zzz 33"),TraceLevel_Normal);
				return false;
			}

			IServerUserItem *pIserberUser = m_pITableFrame->GetTableUserItem(wChairID);
			if (pIserberUser->GetUserScore() < m_lTableScore[wChairID] + lScore)
			{
				//提示消息
				//CTraceService::TraceString(TEXT("zzz 44"),TraceLevel_Normal);
				return false;
			}
		}
		//当前倍数
		LONGLONG lTimes=(m_bMingZhu[wChairID] || bCompareCard)?2:1;
		if(m_bMingZhu[wChairID] && bCompareCard)lTimes=4;
		LONGLONG lTemp=lScore/m_lCellScore/lTimes;
		ASSERT(m_lCurrentTimes<=lTemp && m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore);
		if(!(m_lCurrentTimes<=lTemp && m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore))
		{
			//提示消息
			//CTraceService::TraceString(TEXT("zzz 55"),TraceLevel_Normal);
			return false;
		}
		m_lCurrentTimes = lTemp;

		//用户注金
		m_lTableScore[wChairID]+=lScore;
	}

	//设置用户
	if(!bCompareCard)
	{
		//用户切换
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//设置变量
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//继续判断
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		m_wCurrentUser=wNextPlayer;
	}

	//构造数据
	CMD_S_AddScore AddScore;
	AddScore.lCurrentTimes=m_lCurrentTimes;
	AddScore.wCurrentUser=m_wCurrentUser;
	AddScore.wAddScoreUser=wChairID;
	AddScore.lAddScoreCount=lScore;

	//判断状态
	AddScore.wCompareState=( bCompareCard )?TRUE:FALSE;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	//设置定时器
	m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE*30,1,0);

	return true;
}

//扑克分析
void CTableFrameSink::AnalyseStartCard()
{
	//机器人数
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
			}
			wPlayerCount++;
		}
	}

	//全部机器
	if(wPlayerCount == wAiCount || wAiCount==0) return;

	//扑克变量
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

	//排列扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
	}

	//变量定义
	WORD wWinUser = INVALID_CHAIR;

	//查找数据
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//用户过滤
		if (m_cbPlayStatus[i] == FALSE) continue;

		//设置用户
		if (wWinUser == INVALID_CHAIR)
		{
			wWinUser=i;
			continue;
		}

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_COUNT) >= TRUE)
		{
			wWinUser = i;
		}
	}

	//库存判断
	if (m_lStockScore - m_lCellScore*10*wAiCount < 0)
	{
		//随机赢家
		WORD wBeWinAndroid = INVALID_CHAIR;

		do 
		{
			wBeWinAndroid = rand()%GAME_PLAYER;
		} while (m_pITableFrame->GetTableUserItem(wBeWinAndroid) == NULL || !m_pITableFrame->GetTableUserItem(wBeWinAndroid)->IsAndroidUser());

		if(wBeWinAndroid != wWinUser)
		{
			//交换数据
			BYTE cbTempData[MAX_COUNT];
			CopyMemory(cbTempData, m_cbHandCardData[wBeWinAndroid], sizeof(BYTE)*MAX_COUNT);
			CopyMemory(m_cbHandCardData[wBeWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
			CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
		}
	}
	//控制输分
	else if (m_lStockScore > m_lStorageMax && rand()%100 <= m_nStorageMul)
	{
		//随机赢家
		WORD wWinPlayer = INVALID_CHAIR;

		do 
		{
			wWinPlayer = rand()%GAME_PLAYER;
		} while (m_pITableFrame->GetTableUserItem(wWinPlayer) == NULL || m_pITableFrame->GetTableUserItem(wWinPlayer)->IsAndroidUser());

		if(wWinPlayer != wWinUser)
		{
			//交换数据
			BYTE cbTempData[MAX_COUNT];
			CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
			CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
			CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
		}
	}

	return;
}

//是否衰减
bool CTableFrameSink::NeedDeductStorage()
{
	//机器人数
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
			}
			wPlayerCount++;
		}
	}

	return wPlayerCount != wAiCount && wAiCount > 0;

}

//修改库存
bool CTableFrameSink::UpdateStorage(LONGLONG lScore)
{
	if(!m_bUpdataStorage) return true;

//	ASSERT(lScore >= 0);
	LONGLONG lStockScore = m_lStockScore;
	m_lStockScore += lScore;

	//库存为负
	if (m_lStockScore < 0)
	{
		CString str;
		CString strTemp;
		strTemp.Format(TEXT("%d桌 最大下注%I64d 库存为负%I64d原库存%I64d"), m_pITableFrame->GetTableID() + 1, m_lMaxCellScore, m_lStockScore, lStockScore);
		str += strTemp;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_lUserMaxScore[i] > 0)
			{
				strTemp.Format(TEXT("，%d号椅子最大下注%I64d，"), i, m_lUserMaxScore[i]);
				str += strTemp;
				//获取用户
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem!=NULL)
				{
					strTemp.Format(TEXT("UserID=%ld，机器人%d，分数%I64d，下注为%I64d，看牌%d，放弃%d，扑克："), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), pIServerUserItem->GetUserScore(), m_lTableScore[i], m_bMingZhu[i], m_cbGiveUpUser[i]);
					str += strTemp;
				}
				else
				{
					strTemp.Format(TEXT("已经离开桌子，下注为%I64d 看牌%d，放弃%d，扑克："), m_lTableScore[i], m_bMingZhu[i], m_cbGiveUpUser[i]);
					str += strTemp;
				}

				for (int j = 0; j < MAX_COUNT; j++)
				{
					strTemp.Format(TEXT("%02x "), m_cbHandCardData[i][j]);
					str += strTemp;
				}
			}
		}
		NcaTextOut(str, m_pGameServiceOption->szServerName);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//私人房设置
VOID CTableFrameSink::SetPersonalGameRule()
{
	OutputDebugString(TEXT("CTableFrameSink::SetPersonalGameRule"));

	BYTE cbRulle[RULE_LEN];
	memset(cbRulle, 0, sizeof(cbRulle));
	memcpy(cbRulle, m_pITableFrame->GetGameRule(), RULE_LEN);

	//设置参与人数
	if (cbRulle[1] >= 2)
	{
		m_pITableFrame->SetTableChairCount(cbRulle[1]);
		m_wPlayerCount = cbRulle[1];
	}
	return;
}
