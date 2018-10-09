#ifndef DLG_LOGON_INSURE_HEAD_FILE
#define DLG_LOGON_INSURE_HEAD_FILE

#pragma once

#include "MissionManager.h"
#include "PasswordControl.h"
#include "ShareControlHead.h"

//////////////////////////////////////////////////////////////////////////////////

//消息窗口
class SHARE_CONTROL_CLASS CDlgLogonInsure : public CDialog, public CMissionItem
{
  //变量定义
protected:
  bool                        m_bVerifyInsurePass;               //校验密码
  TCHAR             m_szInsurePass[LEN_MD5];      //银行密码

  //控件变量
protected:
  CSkinButton           m_btOK;               //确定按钮
  CSkinButton           m_btCancel;             //取消按钮
  CSkinButton           m_btClose;              //关闭按钮
  CSkinEdit           m_edInsurePass;           //密码控件
  CPasswordKeyboardEx       m_PasswordKeyboard;         //软键盘

  CSkinLayered          m_SkinLayered;            //分层窗口

  //组件变量
protected:
  CMissionManager         m_MissionManager;         //任务管理

  //函数定义
public:
  //构造函数
  CDlgLogonInsure();
  //析构函数
  virtual ~CDlgLogonInsure();

  //重载函数
protected:
  //控件绑定
  virtual VOID DoDataExchange(CDataExchange * pDX);
  //消息解释
  virtual BOOL PreTranslateMessage(MSG * pMsg);
  //初始化消息
  virtual BOOL OnInitDialog();
  //确定消息
  virtual VOID OnOK();
  //取消消息
  virtual VOID OnCancel();

  //事件处理
public:
  //开始任务
  VOID OnMissionStart();
  //终止任务
  VOID OnMissionConclude();

  //网络事件
private:
  //连接事件
  virtual bool OnEventMissionLink(INT nErrorCode);
  //关闭事件
  virtual bool OnEventMissionShut(BYTE cbShutReason);
  //读取事件
  virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

  //消息函数
protected:
  //绘画背景
  BOOL OnEraseBkgnd(CDC * pDC);
  //显示消息
  VOID OnShowWindow(BOOL bShow, UINT nStatus);
  //鼠标消息
  VOID OnLButtonDown(UINT nFlags, CPoint Point);
  //位置改变
  VOID OnWindowPosChanged(WINDOWPOS * lpWndPos);
  //关闭消息
  VOID OnBnClickedLogonClose();

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////
#endif