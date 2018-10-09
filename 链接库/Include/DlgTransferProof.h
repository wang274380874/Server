#ifndef DLG_TRANSFER_PROOF_HEAD_FILE
#define DLG_TRANSFER_PROOF_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////////////
struct tagTansferProofInfo
{
  DWORD           dwSrcUserID;              //玩家ID
  DWORD           dwDesUserID;              //目标ID
  DWORD           dwRecordIndex;              //记录编号
  SCORE           lTransferScore;             //赠送金额
  CTime           tRecvTime;                //接收时间
  TCHAR           szSrcNickName[LEN_NICKNAME];      //玩家昵称
  TCHAR           szDesNickName[LEN_NICKNAME];      //目标昵称
  TCHAR           szScoreCHZ[128];            //金额大写
};
//////////////////////////////////////////////////////////////////////////////////

//消息窗口
class SHARE_CONTROL_CLASS CDlgTransferProof : public CDialog
{
  //信息变量
protected:
  tagTansferProofInfo *     m_pTansferProofInfo;          //凭证内容

  //控件变量
protected:
  CSkinButton           m_btOK;               //确定按钮

  CSkinLayered          m_SkinLayered;            //分层窗口

  //函数定义
public:
  //构造函数
  CDlgTransferProof();
  //析构函数
  virtual ~CDlgTransferProof();

  //重载函数
protected:
  //控件绑定
  virtual VOID DoDataExchange(CDataExchange * pDX);
  //消息解释
  virtual BOOL PreTranslateMessage(MSG * pMsg);
  //初始化消息
  virtual BOOL OnInitDialog();
  //确定函数
  virtual VOID OnOK();

  //功能函数
public:
  //设置信息
  VOID SetTransferProofInfo(tagTansferProofInfo *pTansferProofInfo)
  {
    m_pTansferProofInfo=pTansferProofInfo;
  }

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

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////
#endif