
#include "StdAfx.h"
#include "game_service_manager.h"
#include "table_frame_sink.h"

GameServiceManager::GameServiceManager() {
  game_service_attrib_.wKindID = KIND_ID;
  game_service_attrib_.wChairCount = GAME_PLAYER;
  game_service_attrib_.wSupporType = GAME_GENRE_GOLD | GAME_GENRE_EDUCATE;

  //功能标志
#ifndef GAME_FUNC_FLAG
  game_service_attrib_.cbDynamicJoin=TRUE;
  game_service_attrib_.cbAndroidUser=TRUE;
  game_service_attrib_.cbOffLineTrustee=FALSE;
#else 
  game_service_attrib_.wGameFuncFlag=GAME_FUNC_ANDROID_ATTEND | GAME_FUNC_DYNAMIC_JOIN;
#endif

  game_service_attrib_.dwServerVersion = VERSION_SERVER;
  game_service_attrib_.dwClientVersion = VERSION_CLIENT;
  lstrcpyn(game_service_attrib_.szGameName, GAME_NAME, CountArray(game_service_attrib_.szGameName));
  lstrcpyn(game_service_attrib_.szDataBaseName, szTreasureDB, CountArray(game_service_attrib_.szDataBaseName));
  lstrcpyn(game_service_attrib_.szClientEXEName, TEXT("lkpy6.exe"), CountArray(game_service_attrib_.szClientEXEName));
  lstrcpyn(game_service_attrib_.szServerDLLName, TEXT("lkpy6_server.dll"), CountArray(game_service_attrib_.szServerDLLName));
}

GameServiceManager::~GameServiceManager() {
}

void* GameServiceManager::QueryInterface(REFGUID guid, DWORD query_ver) {
  QUERYINTERFACE(IGameServiceManager, guid, query_ver);
  QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, guid, query_ver);
  return NULL;
}

void* GameServiceManager::CreateTableFrameSink(REFGUID guid, DWORD query_ver) {
  TableFrameSink* table_frame_sink = new TableFrameSink();
  void* ret = table_frame_sink->QueryInterface(guid, query_ver);
  if (ret == NULL) SafeDelete(table_frame_sink);
  return ret;
}

void* GameServiceManager::CreateAndroidUserItemSink(REFGUID guid, DWORD query_ver)
{
	try
	{
		if (game_service_manager_helper_.GetInterface() == NULL)
		{
			game_service_manager_helper_.SetModuleCreateInfo(TEXT("lkpy6_android.dll"), "CreateGameServiceManager");
			if (!game_service_manager_helper_.CreateInstance()) return NULL;
		}
		VOID *pAndroidObject = game_service_manager_helper_->CreateAndroidUserItemSink(guid, query_ver);
		if (pAndroidObject == NULL)
		{
			OutputDebugString(_T("创建机器人失败"));
			throw TEXT("创建机器人失败");
		}
		return pAndroidObject;
	}
	catch (...)
	{

	}
	return NULL;
}

void* GameServiceManager::CreateGameDataBaseEngineSink(REFGUID guid, DWORD query_ver) {
  return NULL;
}

bool GameServiceManager::GetServiceAttrib(tagGameServiceAttrib& game_service_attrib) {
  game_service_attrib = game_service_attrib_;
  return true;
}

bool GameServiceManager::RectifyParameter(tagGameServiceOption& game_service_option) {
  return true;
}

extern "C" __declspec(dllexport) void* CreateGameServiceManager(REFGUID guid, DWORD interface_ver) {
  static GameServiceManager game_service_manager;
  return game_service_manager.QueryInterface(guid, interface_ver);
}