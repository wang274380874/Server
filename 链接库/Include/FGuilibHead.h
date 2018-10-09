#ifndef FGUI_LIB_HEAD_FILE
#define FGUI_LIB_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//宏定义
#ifdef FGUI_LIB_DLL
#define FGUI_LIB_CLASS __declspec(dllexport)
#else
#define FGUI_LIB_CLASS __declspec(dllimport)
#endif

//模块定义
#ifndef _DEBUG
#define FGUILIB_DLL_NAME  TEXT("FGuilib.dll")     //组件名字
#else
#define FGUILIB_DLL_NAME  TEXT("FGuilibD.dll")    //组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//包含文件
#include "UIBase.h"
#include "UIManager.h"
#include "UIRender.h"
#include "UIDelegate.h"
#include "UIControl.h"
#include "UIContainer.h"
#include "UICommon.h"
//#include "UIRichEdit.h"
#include "UIList.h"
#include "UICombo.h"
#include "UIMarkup.h"
#include "UIResLoader.h"
#include "UIDlgBuilder.h"

//////////////////////////////////////////////////////////////////////////////////

#endif
