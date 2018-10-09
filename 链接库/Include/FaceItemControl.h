#ifndef FACE_ITEM_CONTROL_HEAD_FILE
#define FACE_ITEM_CONTROL_HEAD_FILE

#pragma once

#include "AvatarControlHead.h"

//////////////////////////////////////////////////////////////////////////////////

//头像控件
class AVATAR_CONTROL_CLASS CFaceItemControl : public IFaceItemControl
{
  //变量定义
protected:
  WORD              m_wLineCount;           //头像数目
  WORD              m_wFullCount;           //头像数目
  CPngImage           m_ImageUserFace;          //用户头像
  CPngImage           m_ImageGenderFaceB;         //性别头像
  CPngImage           m_ImageGenderFaceS;         //性别头像
  CPngImage           m_ImageScoreFace;         //分数头像

  //头像框架
protected:
  CSize             m_SizeItemFrame;          //框架大小
  CPngImage           m_ImageItemFrame;         //头像框架

  //对象接口
protected:
  static CFaceItemControl *   m_pFaceItemControl;         //头像接口
  static CGoldAvatarArray     m_GoldAvatarStorage;        //存储数组
  static INT            m_nInstanceCount;         //引用计数

  //函数定义
public:
  //构造函数
  CFaceItemControl();
  //析构函数
  virtual ~CFaceItemControl();

  //基础接口
public:
  //释放对象
  virtual VOID Release()
  {
    delete this;
  }
  //接口查询
  virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //信息接口
public:
  //头像数目
  virtual WORD GetFaceCount();
  //获取尺寸
  virtual VOID GetFaceItemSize(CSize & SizeFace);

  //资源接口
public:
  //获取资源
  virtual VOID GetAvatarResource(tagAvatarResource & AvatarResource);
  //获取资源
  virtual VOID GetScoreAvatarResource(tagAvatarResource & AvatarResource);
  //绘画框架
  virtual VOID DrawFaceItemFrame(CDC * pDC, INT nXPos, INT nYPos, BYTE cbFrameKind);

  //系统头像
public:
  //绘画头像
  virtual VOID DrawFaceNormal(CDC * pDC, INT nXPos, INT nYPos, WORD wFaceID);
  //绘画头像
  virtual VOID DrawFaceOffLine(CDC * pDC, INT nXPos, INT nYPos, WORD wFaceID);

  //自定头像
public:
  //绘画头像
  virtual VOID DrawFaceNormal(CDC * pDC, INT nXPos, INT nYPos, DWORD dwCustomFace[FACE_CX*FACE_CY]);
  //绘画头像
  virtual VOID DrawFaceOffLine(CDC * pDC, INT nXPos, INT nYPos, DWORD dwCustomFace[FACE_CX*FACE_CY]);

  //系统头像
public:
  //绘画头像
  virtual VOID DrawFaceNormal(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, WORD wFaceID);
  //绘画头像
  virtual VOID DrawFaceOffLine(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, WORD wFaceID);

  //自定头像
public:
  //绘画头像
  virtual VOID DrawFaceNormal(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, DWORD dwCustomFace[FACE_CX*FACE_CY]);
  //绘画头像
  virtual VOID DrawFaceOffLine(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, DWORD dwCustomFace[FACE_CX*FACE_CY]);

  //其他头像
public:
  //获取索引
  virtual VOID GetScoreFaceIndex(BYTE cbGender, bool bOffline, SCORE lScore, INT &nXIndex, INT &nYIndex);
  //绘画头像
  virtual VOID DrawGenderFaceNormal(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, BYTE cbGender);
  //绘画头像
  virtual VOID DrawGenderFaceOffline(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, BYTE cbGender);
  //绘画头像
  virtual VOID DrawScoreFaceNormal(CDC * pDC, INT nXPos, INT nYPos, BYTE cbGender, SCORE lScore);
  //绘画头像
  virtual VOID DrawScoreFaceOffline(CDC * pDC, INT nXPos, INT nYPos, BYTE cbGender, SCORE lScore);

  //静态函数
public:
  //获取对象
  static CFaceItemControl * GetInstance()
  {
    return m_pFaceItemControl;
  }
  //加载配置
  static bool LoadGoldAvatarConfig();
};

//////////////////////////////////////////////////////////////////////////////////

#endif