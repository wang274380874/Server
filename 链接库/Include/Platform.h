#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//定义文件
#include "Macro.h"
#include "Define.h"

//结构文件
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//模板文件
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//////////////////////////////////////////////////////////////////////////////////

//程序版本
#define VERSION_FRAME       PROCESS_VERSION(7,0,1)        //框架版本
#define VERSION_PLAZA       PROCESS_VERSION(7,0,1)        //大厅版本
#define VERSION_MOBILE_ANDROID    PROCESS_VERSION(7,0,1)        //手机版本
#define VERSION_MOBILE_IOS      PROCESS_VERSION(7,0,1)        //手机版本

//版本定义
#define VERSION_EFFICACY      0                 //效验版本
#define VERSION_FRAME_SDK     INTERFACE_VERSION(7,1)        //框架版本

//////////////////////////////////////////////////////////////////////////////////
//发布版本

#ifndef _DEBUG

//平台常量
const TCHAR szProduct[]=TEXT("008打游戏");                //产品名字
const TCHAR szPlazaClass[]=TEXT("WHFJXYFQPGame");           //广场类名
const TCHAR szProductKey[]=TEXT("WHFJXYFQPGame");           //产品主键

//地址定义
const TCHAR szCookieUrl[]=TEXT("http://www.008d.com");          //记录地址
const TCHAR szLogonServer[]=TEXT("127.0.0.1");           //登录地址
const TCHAR szPlatformLink[]=TEXT("http://www.008d.com");       //平台网站
const TCHAR szValidateKey[]=TEXT("JDSyncLoginKey");           //验证密钥
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //验证地址

#else

//////////////////////////////////////////////////////////////////////////////////
//内测版本

//平台常量
const TCHAR szProduct[]=TEXT("网狐棋牌平台");             //产品名字
const TCHAR szPlazaClass[]=TEXT("WHJDGamePlaza");           //广场类名
const TCHAR szProductKey[]=TEXT("WHJDGamePlatform");          //产品主键

//地址定义
const TCHAR szCookieUrl[]=TEXT("http://jd.foxuc.net");          //记录地址
const TCHAR szLogonServer[]=TEXT("127.0.0.1");           //登录地址
const TCHAR szPlatformLink[]=TEXT("http://jd.foxuc.net");       //平台网站
const TCHAR szValidateKey[]=TEXT("JDSyncLoginKey");           //验证密钥
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //验证地址

#endif

//////////////////////////////////////////////////////////////////////////////////

//数据库名
const TCHAR szPlatformDB[]=TEXT("THPlatformDB");            //平台数据库
const TCHAR szAccountsDB[]=TEXT("THAccountsDB");            //用户数据库
const TCHAR szTreasureDB[]=TEXT("THTreasureDB");            //财富数据库
const TCHAR szGameMatchDB[]=TEXT("THGameMatchDB");            //比赛数据库
const TCHAR szExerciseDB[]=TEXT("THExerciseDB");            //练习数据库

//////////////////////////////////////////////////////////////////////////////////

//授权信息
const TCHAR szCompilation[]=TEXT("6F6AEE2F-53CD-4EF2-B876-35D57D81C7AC");

//////////////////////////////////////////////////////////////////////////////////

#endif