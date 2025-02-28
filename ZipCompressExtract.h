
// ZipCompressExtract.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CApp:
// このクラスの実装については、ZipCompressExtract.cpp を参照してください
//

class CApp : public CWinApp
{
public:
	CApp();
	virtual ~CApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装
private:
	CString m_strParamFileName;

	void InitPath();

public:
	CString m_strWorkPath;

	// パラメータファイル
	CString GetParamFileString  (const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszDefault = L"", const WCHAR *pszFileName = L"");
	int     GetParamFileInt     (const WCHAR *pszSection, const WCHAR *pszEntry, const int nDefault = 0       , const WCHAR *pszFileName = L"");
	BOOL    GetParamFileBOOL    (const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bDefault = FALSE  , const WCHAR *pszFileName = L"");
	DWORD   GetParamFileHex     (const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD dwDefault = 0    , const WCHAR *pszFileName = L"");
	BOOL    WriteParamFileString(const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszValue        , const WCHAR *pszFileName = L"");
	BOOL    WriteParamFileInt   (const WCHAR *pszSection, const WCHAR *pszEntry, const int nValue             , const WCHAR *pszFileName = L"");
	BOOL    WriteParamFileBOOL  (const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bValue            , const WCHAR *pszFileName = L"");
	BOOL    WriteParamFileHex   (const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD nValue           , const WCHAR *pszFileName = L"");
	// レジストリー
	CString RegGetParamFileString  (const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszDefault = L"");
	int     RegGetParamFileInt     (const WCHAR *pszSection, const WCHAR *pszEntry, const int nDefault = 0       );
	BOOL    RegGetParamFileBOOL    (const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bDefault = FALSE  );
	DWORD   RegGetParamFileHex     (const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD dwDefault = 0    );
	BOOL    RegWriteParamFileString(const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszValue        );
	BOOL    RegWriteParamFileInt   (const WCHAR *pszSection, const WCHAR *pszEntry, const int nValue             );
	BOOL    RegWriteParamFileBOOL  (const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bValue            );
	BOOL    RegWriteParamFileHex   (const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD nValue           );
	// パスの最後が'\'で終わってたら'TRUE'を返す
	BOOL    AnalyzePathFolder(const WCHAR *pszPath);
	// メッセージ処理
	BOOL    DoBackground();


public:
	DECLARE_MESSAGE_MAP()
};

extern CApp App;
