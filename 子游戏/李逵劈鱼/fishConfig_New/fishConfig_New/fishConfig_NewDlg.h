
// fishConfig_NewDlg.h : 头文件
//

#pragma once


// CfishConfig_NewDlg 对话框
class CfishConfig_NewDlg : public CDialogEx
{
// 构造
public:
	CfishConfig_NewDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_FISH_CONFIG_DIALOG
};
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
