// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainDlg : public CDialogImpl<CMainDlg>, public CWinDataExchange<CMainDlg>
{
protected:
	BOOL m_bDecrypt;
	BOOL m_bSourceIsFile;
	CString m_strSource;
	BOOL m_bDeleteFile;
	BOOL m_bSubFoder;

public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_DDX_MAP(CMainDlg)
		DDX_RADIO(IDC_RADIO1, m_bDecrypt)
		DDX_RADIO(IDC_RADIO3, m_bSourceIsFile)
		DDX_TEXT(IDC_EDIT1, m_strSource)
		DDX_CHECK(IDC_CHECK1, m_bDeleteFile)
		DDX_CHECK(IDC_CHECK2, m_bSubFoder)
	END_DDX_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedBrowse)
		COMMAND_HANDLER(IDC_BUTTON2, BN_CLICKED, OnBnClickedStart)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	bool EncryptFishFile(const CString& strFile, BOOL bDeleteSourceFile);
	bool DecryptFishFile(const CString& strFile, BOOL bDeleteSourceFile);
	bool EncryptFishFileFoder(const CString& strFileFoder, BOOL bSubFoder, BOOL bDeleteSourceFile);
	bool DecryptFishFileFoder(const CString& strFileFoder, BOOL bSubFoder, BOOL bDeleteSourceFile);
};
