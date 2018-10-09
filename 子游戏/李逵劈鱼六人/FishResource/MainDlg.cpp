// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"

#include "ResUtility.h"

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	CButton radio = GetDlgItem(IDC_RADIO1);
	radio.SetCheck(BST_CHECKED);
	radio = GetDlgItem(IDC_RADIO3);
	radio.SetCheck(BST_CHECKED);
	radio = GetDlgItem(IDC_CHECK1);
	radio.SetCheck(BST_CHECKED);
	radio = GetDlgItem(IDC_CHECK2);
	radio.SetCheck(BST_CHECKED);

	return TRUE;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnBnClickedBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!DoDataExchange(TRUE)) return 0;
	if (m_bSourceIsFile)
	{
		CFileDialog fileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST,
					  m_bDecrypt ? _T("fish files (*.fish)\0*.fish\0\0") : _T("All files (*.*)\0*.*\0\0"));
		int nRet = fileDlg.DoModal();
		if (nRet == IDOK)
		{
			m_strSource = fileDlg.m_szFileName;
		}
	}
	else
	{
		BROWSEINFO bInfo;
		LPITEMIDLIST pidl;
		CString strPath;
		ZeroMemory ((PVOID)&bInfo, sizeof(BROWSEINFO));

		bInfo.hwndOwner = m_hWnd;
		bInfo.pszDisplayName = strPath.GetBuffer(MAX_PATH);
		bInfo.lpszTitle = _T("Browse");
		bInfo.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bInfo.lpfn = NULL;
		bInfo.lParam = (LPARAM)this;

		if ((pidl = ::SHBrowseForFolder(&bInfo) ) == NULL)
		{
			return 0;
		}

		if (::SHGetPathFromIDList(pidl, strPath.GetBuffer(MAX_PATH)) == FALSE)
		{
			return 0;
		}

		m_strSource = bInfo.pszDisplayName;
	}

	if (!DoDataExchange(FALSE)) return 0;

	return 0;
}

LRESULT CMainDlg::OnBnClickedStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!DoDataExchange(TRUE)) return 0;

	if (!m_bDecrypt)
	{
		if (m_bSourceIsFile)
		{
			EncryptFishFile(m_strSource, m_bDeleteFile);
		}
		else
		{
			EncryptFishFileFoder(m_strSource, m_bSubFoder, m_bDeleteFile);
		}
	}
	else
	{
		if (m_bSourceIsFile)
		{
			DecryptFishFile(m_strSource, m_bDeleteFile);
		}
		else
		{
			DecryptFishFileFoder(m_strSource, m_bSubFoder, m_bDeleteFile);
		}
	}

	MessageBox(_T("转化完成"));

	return 0;
}

bool CMainDlg::EncryptFishFile(const CString& strFile, BOOL bDeleteSourceFile)
{
	FILE* file = _tfopen( strFile, _T("rb"));
	if (file == NULL) {
		MessageBox(_T("文件打开失败"));
		return false;
	}

	int pos = strFile.ReverseFind(_T('.'));
	if (pos < 0)
	{
		MessageBox(_T("文件名错误"));
		return false;
	}
	CString strDstFile = strFile.Left(pos);
	strDstFile += _T(".fish");

	fseek(file, 0, SEEK_END);
	long file_len = ftell(file);

	fseek(file, 0, SEEK_SET);
	BYTE* file_data = new BYTE[file_len];
	fread(file_data, file_len, 1, file);
	for (long i = 0; i < file_len; ++i)
	{
		file_data[i] = g_EncryptByteMap[file_data[i]];
	}

	// 加密数据
	DWORD dwXorKey = g_dwKey;
	WORD* pwSeed = (WORD*)file_data;
	DWORD* pdwXor = (DWORD*)file_data;
	WORD wEncrypCount = (WORD)(file_len / sizeof(DWORD));
	for (WORD i = 0; i < wEncrypCount; i++)
	{
		*pdwXor++ ^= dwXorKey;
		dwXorKey = SeedRandMap(*pwSeed++);
    dwXorKey |= ((DWORD)SeedRandMap(*pwSeed++)) << 16;
		dwXorKey ^= g_dwKey;
	}

	FILE* wfile = _tfopen(strDstFile, _T("wb"));
	fwrite(file_data, file_len, 1, wfile);

	delete[] file_data;
	fclose(file);
	fclose(wfile);

	if (bDeleteSourceFile)
	{
		DeleteFile(strFile);
	}

	return true;
}

bool CMainDlg::DecryptFishFile(const CString& strFile, BOOL bDeleteSourceFile)
{
	FILE* file = _tfopen(strFile, _T("rb"));
	if (file == NULL) {
		MessageBox(_T("文件打开失败"));
		return false;
	}

	int pos = strFile.ReverseFind(_T('.'));
	if (pos < 0)
	{
		MessageBox(_T("文件名错误"));
		return false;
	}
	CString strDstFile = strFile.Left(pos);

	fseek(file, 0, SEEK_END);
	long file_len = ftell(file);

	fseek(file, 0, SEEK_SET);
	BYTE* file_data = new BYTE[file_len];
	fread(file_data, file_len, 1, file);

	// 解密数据
	DWORD dwXorKey = g_dwKey;
	DWORD dwDecryptKey = g_dwKey;
	WORD* pwSeed = (WORD*)file_data;
	DWORD* pdwXor = (DWORD*)file_data;
	WORD wDecrypCount = (WORD)(file_len / sizeof(DWORD));
	for (WORD i = 0; i < wDecrypCount; i++)
	{
    dwXorKey = SeedRandMap(*pwSeed++);
		dwXorKey |= ((DWORD)SeedRandMap(*pwSeed++)) << 16;
		dwXorKey ^= g_dwKey;
		*pdwXor++ ^= dwDecryptKey;
		dwDecryptKey = dwXorKey;
	}

	for (long i = 0; i < file_len; ++i)
	{
		file_data[i] = g_DecryptByteMap[file_data[i]];
	}

	strDstFile += GetFileExtension(file_data);

	FILE* wfile = _tfopen(strDstFile, _T("wb"));
	fwrite(file_data, file_len, 1, wfile);

	delete[] file_data;
	fclose(file);
	fclose(wfile);

	if (bDeleteSourceFile)
	{
		DeleteFile(strFile);
	}

	return true;
}

bool CMainDlg::EncryptFishFileFoder(const CString& strFileFoder, BOOL bSubFoder, BOOL bDeleteSourceFile)
{
	CString strSource = strFileFoder;
	if (strSource.Right(1) != _T("\\") && strSource.Right(1) != _T("/"))
		strSource += _T("\\");
	strSource += _T("*.*");

	CFindFile finder;
	if (finder.FindFile(strSource))
	{
		do
		{
			if (finder.IsDots()) continue;

			CString strFilePath = finder.GetFilePath();

			if (finder.IsDirectory())
			{
				if (bSubFoder)
					EncryptFishFileFoder(strFilePath, bSubFoder, bDeleteSourceFile);
			}
			else
			{
				EncryptFishFile(strFilePath, bDeleteSourceFile);
			}

		}
		while (finder.FindNextFile());

	}
	finder.Close();

	return true;
}

bool CMainDlg::DecryptFishFileFoder(const CString& strFileFoder, BOOL bSubFoder, BOOL bDeleteSourceFile)
{
	CString strSource = strFileFoder;
	if (strSource.Right(1) != _T("\\") && strSource.Right(1) != _T("/"))
		strSource += _T("\\");
	strSource += _T("*.*");

	CFindFile finder;
	if (finder.FindFile(strSource))
	{
		do
		{
			if (finder.IsDots()) continue;

			CString strFilePath = finder.GetFilePath();

			if (finder.IsDirectory())
			{
				if (bSubFoder)
					DecryptFishFileFoder(strFilePath, bSubFoder, bDeleteSourceFile);
			}
			else
			{
				DecryptFishFile(strFilePath, bDeleteSourceFile);
			}

		}
		while (finder.FindNextFile());

	}
	finder.Close();

	return true;
}
