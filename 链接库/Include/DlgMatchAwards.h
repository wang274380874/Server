#ifndef DLG_MATCH_AWARDS_HEAD_FILE
#define DLG_MATCH_AWARDS_HEAD_FILE

#pragma once

#include "afxwin.h"
#include "GameFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//比赛奖状
class GAME_FRAME_CLASS CDlgMatchAwards : public CFGuiDialog
{
  //控件变量
public:
  CMD_GR_MatchResult        *m_pMatchResult;          //比赛结果

  //界面控件
protected:
  CButtonUI *           m_pbtClose;             //关闭按钮

  //界面控件
protected:
  CSkinLayered          m_SkinLayered;            //分层窗口

  //函数定义
public:
  //构造函数
  CDlgMatchAwards();
  //析构函数
  virtual ~CDlgMatchAwards();

  //重载函数
protected:
  //确定函数
  virtual VOID OnCancel();
  //初始化窗口
  virtual BOOL OnInitDialog();

  //接口函数
public:
  //初始控件
  virtual VOID InitControlUI();
  //消息提醒
  virtual VOID Notify(TNotifyUI &  msg);

  //绘画接口
public:
  //开始绘画
  virtual VOID OnBeginPaintWindow(HDC hDC);

  //功能函数
public:
  //设置参数
  VOID SetMatchResult(CMD_GR_MatchResult *pMatchResult);
  //绘制多行文本
  VOID DrawMultLineText(CDC *pDC, CString strText, CRect rect, int nRowDis, UINT nFromat);
};

#endif