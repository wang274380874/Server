#ifndef DLG_PROPERTY_STORE_HEAD_FILE
#define DLG_PROPERTY_STORE_HEAD_FILE

#pragma once

#include "GamePropertyHead.h"
#include "GamePropertyItem.h"

//////////////////////////////////////////////////////////////////////////////////
#define CX_COUNT            2           //横向个数
#define CY_COUNT            2           //竖向个数

//////////////////////////////////////////////////////////////////////////////////
//游戏道具
class GAME_PROPERTY_CLASS CDlgPropertyStore : public CDialog
{
  //按钮变量
protected:
  CSkinButton           m_btCancel;             //取消按钮
  CSkinButton                     m_btPrev;             //左移按钮
  CSkinButton                     m_btNext;             //右移按钮
  CSkinButton           m_btBuy[CX_COUNT*CY_COUNT];     //购买按钮
  CSkinButton           m_btTransfer[CX_COUNT*CY_COUNT];  //赠送按钮
  CSkinButton           m_btPageNo[10];           //页码按钮

  //变量定义
protected:
  BYTE              m_cbCurPageNo;            //当前页数
  BYTE              m_cbPageCount;            //道具页数
  BYTE              m_cbPropertyCount;          //道具数目
  CGamePropertyItem *             m_pCurrPropertyItem;                //当前道具
  CGamePropertyItem *       m_pGamePropertyItem[MAX_PROPERTY];  //道具数组

  //接口变量
protected:
  IGamePropertySink *       m_pIGamePropertySink;       //回调接口

  CFont             m_FontBold;             //粗体字体

  //函数定义
public:
  //构造函数
  CDlgPropertyStore();
  //析构函数
  virtual ~CDlgPropertyStore();

  //重置函数
protected:
  //控件绑定
  virtual VOID DoDataExchange(CDataExchange * pDX);
  //消息解释
  virtual BOOL PreTranslateMessage(MSG * pMsg);
  //创建函数
  virtual BOOL OnInitDialog();
  //确定函数
  virtual VOID OnOK();
  //取消消息
  virtual VOID OnCancel();

  //界面函数
private:
  //调整控件
  VOID RectifyControl(INT nWidth, INT nHeight);

  //配置函数
public:
  //设置接口
  bool SetGamePropertySink(IUnknownEx * pIUnknownEx);
  //设置道具
  bool SetGamePropertyInfo(tagPropertyInfo PropertyInfo[], BYTE cbPropertyCount, WORD wIssueArea);
  //改变事件
  VOID OnSelectedUserItemChange(IClientUserItem * pIClintUserItem);

  //内部函数
protected:
  //点击判断
  WORD SwitchItemFromPoint(CPoint MousePoint);

  //消息映射
protected:
  //绘画背景
  BOOL OnEraseBkgnd(CDC * pDC);
  //左移按钮
  VOID OnBtnClickedPrev();
  //右移按钮
  VOID OnBtnClickedNext();
  //购买道具
  VOID OnBnClickedBuy(UINT nCommandID);
  //赠送道具
  VOID OnBnClickedTransfer(UINT nCommandID);
  //页码按钮
  VOID OnBnClickedPageNo(UINT nCommandID);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif