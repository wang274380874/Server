
#ifndef FISH_ANDROID_MANAGER_H_
#define FISH_ANDROID_MANAGER_H_
#pragma once

class CGameServiceManager : public IGameServiceManager {
 public:
  CGameServiceManager();
  virtual ~CGameServiceManager();

 public:
  virtual void Release() { return; }
  virtual void* QueryInterface(REFGUID guid, DWORD query_ver);

  // 创建接口
 public:
  // 创建桌子
  virtual void* CreateTableFrameSink(REFGUID guid, DWORD query_ver);
  // 创建机器
  virtual void* CreateAndroidUserItemSink(REFGUID guid, DWORD query_ver);
  // 创建数据
  virtual void* CreateGameDataBaseEngineSink(REFGUID guid, DWORD query_ver);

  // 参数接口
 public:
  // 组件属性
  virtual bool GetServiceAttrib(tagGameServiceAttrib& game_service_attrib);
  // 调整参数
  virtual bool RectifyParameter(tagGameServiceOption& game_service_option);
};

#endif  // FISH_ANDROID_MANAGER_H_
