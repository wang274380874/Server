#ifndef WH_SOCKET_HEAD_FILE
#define WH_SOCKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台文件
//#include "Template.h"
//#include "Constant.h"
//#include "GlobalDef.h"
#include "Platform.h"
#include "ServiceCoreHead.h"
//////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef WH_SOCKET_CLASS
#ifdef  WH_SOCKET_DLL
#define WH_SOCKET_CLASS _declspec(dllexport)
#else
#define WH_SOCKET_CLASS _declspec(dllimport)
#endif
#endif

//模块定义
#ifndef _DEBUG
#define WH_SOCKET_DLL_NAME  TEXT("WHSocket.dll")    //组件 DLL 名字
#else
#define WH_SOCKET_DLL_NAME  TEXT("WHSocketD.dll")   //组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//公共定义

//关闭原因
#define SHUT_REASON_INSIDE      0                 //内部原因
#define SHUT_REASON_NORMAL      1                 //正常关闭
#define SHUT_REASON_REMOTE      2                 //远程关闭
#define SHUT_REASON_TIME_OUT    3                 //网络超时
#define SHUT_REASON_EXCEPTION   4                 //异常关闭

//////////////////////////////////////////////////////////////////////////
//公共定义

//网络状态
#define SOCKET_STATUS_IDLE      0                 //空闲状态
#define SOCKET_STATUS_WAIT      1                 //等待状态
#define SOCKET_STATUS_CONNECT   2                 //连接状态

//连接错误
#define CONNECT_SUCCESS       0                 //连接成功
#define CONNECT_FAILURE       1                 //连接失败
#define CONNECT_EXCEPTION     2                 //参数异常
#define CONNECT_PROXY_FAILURE   3                 //连接成功
#define CONNECT_PROXY_USER_INVALID  4                 //用户无效

//////////////////////////////////////////////////////////////////////////
//枚举定义

////代理类型
//enum enProxyServerType
//{
//  PROXY_NONE,         //没有代理
//  PROXY_HTTP,         //HTTP 代理
//  PROXY_SOCKS4,       //SOCKS4 代理
//  PROXY_SOCKS5,       //SOCKS5 代理
//};
//代理类型
#define PROXY_NONE          0                 //没有代理
#define PROXY_HTTP          1                 //代理类型
#define PROXY_SOCKS4        2                 //代理类型
#define PROXY_SOCKS5        3                 //代理类型

//////////////////////////////////////////////////////////////////////////
//结构定义

//代理描述
struct tagProxyDescribe
{
  BYTE              cbProxyType;            //代理类型
  TCHAR             szDescribe[32];           //描述字符
};

//代理信息
struct tagProxyServer
{
  WORD              wProxyPort;             //代理端口
  TCHAR             szUserName[32];           //代理用户
  TCHAR             szPassword[32];           //代理密码
  TCHAR             szProxyServer[32];          //代理地址
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITCPSocket INTERFACE_VERSION(1,1)
static const GUID IID_ITCPSocket= {0xa870d592,0x447a,0x40bd,0x00ae,0x4e,0x5e,0x64,0xde,0xce,0x22,0xed};
#else
#define VER_ITCPSocket INTERFACE_VERSION(1,1)
static const GUID IID_ITCPSocket= {0x8dfd434a,0xfb21,0x4178,0x00b0,0x3b,0xe6,0x32,0x9f,0x0b,0x5b,0x2e};
#endif

//网络接口
interface ITCPSocket :
public IUnknownEx
{
  //标识接口
public:

  //设置标识
  virtual VOID  SetSocketID(WORD wSocketID)=NULL;

  //设置接口
  virtual bool  SetTCPSocketSink(IUnknownEx * pIUnknownEx)=NULL;


  //信息接口
public:
  //网络状态
  virtual BYTE  GetSocketStatus()=NULL;
  //发送间隔
  virtual DWORD  GetLastSendTick()=NULL;
  //接收间隔
  virtual DWORD  GetLastRecvTick()=NULL;

  //代理接口
public:
  //代理测试
  virtual BYTE  ProxyServerTesting()=NULL;
  //设置代理
  virtual bool  SetProxyServerInfo(BYTE cbProxyType, const tagProxyServer & ProxyServer)=NULL;

  //操作接口
public:
  //关闭连接
  virtual VOID CloseSocket()=NULL;
  //连接操作
  virtual BYTE Connect(DWORD dwServerIP, WORD wPort)=NULL;
  //连接操作
  virtual BYTE Connect(LPCTSTR pszServerIP, WORD wPort)=NULL;
  //发送函数
  virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
  //发送函数
  virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITCPSocketSink INTERFACE_VERSION(1,1)
static const GUID IID_ITCPSocketSink= {0xdb0f09dd,0x65e4,0x441f,0x0094,0xb1,0xfe,0x80,0x8c,0x1d,0xf2,0x65};
#else
#define VER_ITCPSocketSink INTERFACE_VERSION(1,1)
static const GUID IID_ITCPSocketSink= {0x231416e8,0x74b0,0x472d,0x009e,0xc3,0x4a,0x22,0xe5,0x21,0x09,0x37};
#endif

//网络接口
interface ITCPSocketSink :
public IUnknownEx
{
  //网络事件
public:
  //连接事件
  virtual bool OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)=NULL;
  //关闭事件
  virtual bool OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)=NULL;
  //读取事件
  virtual bool OnEventTCPSocketRead(WORD wSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IWHNetwork INTERFACE_VERSION(1,1)
static const GUID IID_IWHNetwork= {0xe500997b,0xe45e,0x474e,0x8a,0xd4,0xbc,0xf3,0x4d,0x89,0x45,0x41};
#else
#define VER_IWHNetwork INTERFACE_VERSION(1,1)
static const GUID IID_IWHNetwork= {0x32198885,0x0c13,0x4ea8,0x8a,0x36,0x27,0x56,0x51,0x36,0xac,0xc1};
#endif

//网络管理接口
interface IWHNetwork :
public IUnknownEx
{
  //枚举代理
  virtual bool EnumProxyServerType(WORD wIndex, tagProxyDescribe & ProxyDescribe)=NULL;
  //代理测试
  virtual BYTE ProxyServerTesting(BYTE cbProxyType, const tagProxyServer & ProxyServer)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(TCPSocket,WH_SOCKET_DLL_NAME,"CreateTCPSocket")
DECLARE_MODULE_HELPER(WHNetwork,WH_SOCKET_DLL_NAME,"CreateWHNetwork")

//////////////////////////////////////////////////////////////////////////

#endif