#ifndef DLG_INSURE_GAME_HEAD_FILE
#define DLG_INSURE_GAME_HEAD_FILE

#pragma once

#include "GameFrameHead.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//游戏银行
class CDlgInsureGame : public CSkinDialog
{
  //标识变量
public:
  bool              m_bSwitchIng;         //转换标识
  bool              m_blCanStore;                   //存款标识
  bool                            m_blCanGetOnlyFree;             //取款标识

  //银行信息
public:
  WORD              m_wRevenueTake;         //税收比例
  WORD              m_wRevenueTransfer;       //税收比例
  SCORE             m_lUserScore;         //用户游戏币
  SCORE             m_lUserInsure;          //银行游戏币

  //界面控件
protected:
  CEditUI *           m_pEditAmount;          //数量编辑
  CEditUI *           m_pEditPassword;        //密码编辑
  CTextUI *           m_pTextForget;          //忘记密码

  //界面控件
protected:
  CButtonUI *           m_pButtonSave;          //存款按钮
  CButtonUI *           m_pButtonTake;          //取款按钮
  CButtonUI *           m_pButtonRefresh;       //刷新按钮
  CButtonUI *           m_pButtonScore[8];        //分数按钮

  //控件变量
protected:
  CPngImage           m_ImageNumber;          //数字图片

  //接口变量
protected:
  IClientUserItem *       m_pIMeUserData;         //用户信息
  IClientKernel *         m_pIClientKernel;       //内核接口

  //函数定义
public:
  //构造函数
  CDlgInsureGame(CWnd* pParent = NULL);
  //析构函数
  virtual ~CDlgInsureGame();

  //重载函数
public:
  //确认消息
  virtual VOID OnOK();
  //取消消息
  virtual VOID OnCancel();
  //初始化函数
  virtual BOOL OnInitDialog();
  //控件绑定
  virtual VOID DoDataExchange(CDataExchange* pDX);
  //命令信息
  virtual BOOL PreTranslateMessage(MSG* pMsg);

  //接口函数
public:
  //初始控件
  virtual void InitControlUI();
  //消息提醒
  virtual void Notify(TNotifyUI &  msg);

  //重载函数
public:
  //绘画消息
  virtual VOID OnDrawClientAreaEnd(CDC * pDC, INT nWidth, INT nHeight);

  //辅助函数
public:
  //调整控件
  void RectifyControl(INT nWidth, INT nHeight);

  //辅助函数
public:
  //获取游戏币
  SCORE GetUserControlScore(LPCTSTR pszScore);
  //绘画数字
  VOID DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos);
  //转换字符
  VOID SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize);

  //控件函数
protected:
  //分数变更
  VOID OnEnChangeScore();
  //存款按钮
  VOID OnBnClickedSaveScore();
  //取款按钮
  VOID OnBnClickedTakeScore();
  //刷新按钮
  VOID OnBnClickedRefreshScore();
  //忘记密码
  VOID OnBnClickedForgetInsure();
  //分数按钮
  VOID OnBnCheckedScoreButton(CControlUI* pControlUI);

  //接口函数
public:
  //显示银行
  void ShowItem();
  //设置位置
  void SetPostPoint(CPoint Point);

  //功能函数
public:
  //设置组件
  void SetSendInfo(IClientKernel *pClientKernel,IClientUserItem const*pMeUserDatas);
  //发送信息
  void SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);

  //消息函数
protected:
  //时间消息
  afx_msg void OnTimer(UINT nIDEvent);
  //大小消息
  afx_msg void OnSize(UINT nType, int cx, int cy);

  DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif