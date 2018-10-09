// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// Windows 头文件:
#include <windows.h>
#include <Mmsystem.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>

#ifndef ASSERT
#define ASSERT(x) assert(x) // for mfc ASSERT
#endif
#include <WTypes.h> // for DOUBLE

#ifdef _DEBUG
//#include <vld.h>
#endif

//#define TEST  // 定义这个显示包围盒

// ArraySizeHelper是一个返回类型为char[N]的函数,其形参类型为 T[N].
// 函数没必要实现, 因为sizeof只需要类型.
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(arr)返回array数组的元素个数. 该表达式是编译时常量,
// 可以用于定义新的数组. 如果传递一个指针会报编译时错误.
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define SAFE_DELETE(x) { if (NULL != (x)) { delete (x); (x) = NULL; } }

#include "..\command\CMD_Fish.h"

//编译环境
#include "..\..\..\SDKCondition.h"

//////////////////////////////////////////////////////////////////////////////////
#ifdef SDK_CONDITION

#include "..\..\..\开发库\Include\Macro.h"
#include "..\..\..\开发库\Include\Define.h"
#include "..\..\..\开发库\Include\Struct.h"
#include "..\..\..\开发库\Include\Packet.h"
#include "..\..\..\开发库\Include\Property.h"
#include "..\..\..\开发库\Include\PacketAide.h"
#include "..\..\..\开发库\Include\RightDefine.h"
#include "..\..\..\开发库\Include\CMD_Commom.h"
#include "..\..\..\开发库\Include\CMD_GameServer.h"
#include "..\..\..\开发库\Include\IPC_GameFrame.h"

#else

#include "..\..\..\..\系统模块\全局定义\Macro.h"
#include "..\..\..\..\系统模块\全局定义\Define.h"
#include "..\..\..\..\系统模块\全局定义\Struct.h"
#include "..\..\..\..\系统模块\全局定义\Packet.h"
#include "..\..\..\..\系统模块\全局定义\Property.h"
#include "..\..\..\..\系统模块\全局定义\PacketAide.h"
#include "..\..\..\..\系统模块\全局定义\RightDefine.h"
#include "..\..\..\..\系统模块\消息定义\CMD_Commom.h"
#include "..\..\..\..\系统模块\消息定义\CMD_GameServer.h"
#include "..\..\..\..\系统模块\消息定义\IPC_GameFrame.h"

#endif

//#define VERSION_FRAME   PROCESS_VERSION(11,0,3)   // 框架版本
#define VERSION_FRAME   101384195       // 这个数字为PROCESS_VERSION(11,0,3)的值,自己转换一下
