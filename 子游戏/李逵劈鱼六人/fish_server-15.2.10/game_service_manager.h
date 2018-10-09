
#ifndef GAME_SERVICE_MANAGER_H_
#define GAME_SERVICE_MANAGER_H_
#pragma once

class GameServiceManager : public IGameServiceManager {
 public:
  GameServiceManager();
  virtual ~GameServiceManager();

  virtual void Release() {}
  virtual void* QueryInterface(REFGUID guid, DWORD query_ver);

  virtual void* CreateTableFrameSink(REFGUID guid, DWORD query_ver);
  virtual void* CreateAndroidUserItemSink(REFGUID guid, DWORD query_ver);
  virtual void* CreateGameDataBaseEngineSink(REFGUID guid, DWORD query_ver);

  virtual bool GetServiceAttrib(tagGameServiceAttrib& game_service_attrib);
  virtual bool RectifyParameter(tagGameServiceOption& game_service_option);

 private:
  tagGameServiceAttrib game_service_attrib_;
  CGameServiceManagerHelper game_service_manager_helper_;
};

#endif // GAME_SERVICE_MANAGER_H_
