
// ZipCompressExtract.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "ZipCompressExtract.h"
#include "DlgMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
END_MESSAGE_MAP()


// CApp の構築

CApp::CApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

CApp::~CApp()
{
}


// 唯一の CApp オブジェクト

CApp App;
HANDLE hAppMutex;


// CApp の初期化

BOOL CApp::InitInstance()
{
	// Windows XP では InitCommonControlsEx() が必要です (以下の場合: アプリケーション
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
//	SetRegistryKey(L"Application");

	InitPath();

	{ //二重起動チェック
		WCHAR szMutex[MAX_PATH];

		wcscpy_s(szMutex, m_pszAppName);

		HANDLE hMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szMutex);
		if (hMutex) {
			CloseHandle(hMutex);
			return FALSE;
		} else {
			hAppMutex = CreateMutexW(NULL, FALSE, szMutex);
		}
	}

	CDlgMain dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

void CApp::InitPath()
{
	CString str;

	//make m_strWorkPath
	GetCurrentDirectoryW(MAX_PATH, str.GetBuffer(MAX_PATH));
	str.ReleaseBuffer();
	if (!AnalyzePathFolder(str)) { str += '\\'; }

	m_strWorkPath = str /*+ m_lpCmdLine*/;
//	if (!AnalyzePathFolder(strWorkPath)) { strWorkPath += '\\'; }

	// make profile name
	if (m_pszRegistryKey == NULL) {
		str = m_strWorkPath + m_pszProfileName;
		free((void *)m_pszProfileName);
		m_pszProfileName = _wcsdup((const WCHAR *)str);
	}

	//make parfile name
	m_strParamFileName = m_strWorkPath + m_pszAppName + L".par";
}

// パスの最後が'\'で終わってたら'TRUE'を返す
BOOL CApp::AnalyzePathFolder(const WCHAR *pszPath)
{
	BOOL bVal = TRUE;
	const WCHAR *cp = pszPath;

	while (*cp) {
		if (_ismbblead(*cp) && _ismbbtrail(*(cp + 1))) {
			cp++;
			bVal = FALSE;
		} else if (*cp == '\\') {
			bVal = TRUE;
		} else {
			bVal = FALSE;
		}
		cp++;
	}

	return bVal;
}


/// <summary>
/// メッセージ処理
/// </summary>
BOOL CApp::DoBackground()
{
	MSG msg;

	while (::PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		switch (msg.message) {
		case WM_QUIT:
		case WM_CLOSE:
		case WM_DESTROY:
			return FALSE;
//		case WM_TIMER:
//			::PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE);
//			return TRUE;
		}
		if (!PumpMessage()) {
//			::PostQuitMessage(0);
			return FALSE;
		}
	}
	LONG n = 0;
	while (OnIdle(n++)) {
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
#define PROFILE_BUFFER_SIZE 4096
#define PROFILE_VALUSE_SIZE 32

CString CApp::GetParamFileString(const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszDefault, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);
	if (pszDefault == NULL) {
		pszDefault  = L"";
	}
	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	WCHAR sz[PROFILE_BUFFER_SIZE];

	DWORD dw = ::GetPrivateProfileStringW(pszSection, pszEntry, pszDefault, sz, PROFILE_BUFFER_SIZE, pszName);
	ASSERT(dw < PROFILE_BUFFER_SIZE - 1);
	return sz;
}

int CApp::GetParamFileInt(const WCHAR *pszSection, const WCHAR *pszEntry, const int nDefault, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	return ::GetPrivateProfileIntW(pszSection, pszEntry, nDefault, pszName);
}

BOOL CApp::GetParamFileBOOL(const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bDefault, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	return ((::GetPrivateProfileIntW(pszSection, pszEntry, bDefault, pszName) != 0) ? TRUE : FALSE);
}

DWORD CApp::GetParamFileHex(const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD dwDefault, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	WCHAR szDef[PROFILE_VALUSE_SIZE];
	WCHAR szVal[PROFILE_VALUSE_SIZE];

	swprintf_s(szDef, L"%X", dwDefault);
	DWORD dw = ::GetPrivateProfileStringW(pszSection, pszEntry, szDef, szVal, PROFILE_VALUSE_SIZE, pszName);
	ASSERT(dw < PROFILE_VALUSE_SIZE - 1);

	DWORD dwVal = 0;
	WCHAR c;
	WCHAR *cp = szVal;

	while (*cp) {
		c = toupper(*cp++);
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
			dwVal *= 16;
			c -= '0';
			if (c > 9) {
				c -= 7;
			}
			dwVal += c;
		} else {
			break;
		}
	}

	return dwVal;
}


BOOL CApp::WriteParamFileString(const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszValue, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);
	ASSERT(wcslen(pszValue) < PROFILE_BUFFER_SIZE - 1); // can't read in bigger

	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	return ::WritePrivateProfileStringW(pszSection, pszEntry, pszValue, pszName);
}

BOOL CApp::WriteParamFileInt(const WCHAR *pszSection, const WCHAR *pszEntry, const int nValue, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	WCHAR sz[16];
	swprintf_s(sz, L"%d", nValue);
	return ::WritePrivateProfileStringW(pszSection, pszEntry, sz, pszName);
}

BOOL CApp::WriteParamFileBOOL(const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bValue, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	WCHAR sz[16];
	swprintf_s(sz, L"%d", (bValue ? 1 : 0));
	return ::WritePrivateProfileStringW(pszSection, pszEntry, sz, pszName);
}

BOOL CApp::WriteParamFileHex(const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD dwValue, const WCHAR *pszFileName)
{
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	const WCHAR *pszName = (const WCHAR *)m_strParamFileName;
	if (pszFileName != NULL && wcslen(pszFileName) > 0) { pszName = pszFileName; }

	WCHAR sz[16];
	swprintf_s(sz, L"%X", dwValue);
	return ::WritePrivateProfileStringW(pszSection, pszEntry, sz, pszName);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CString CApp::RegGetParamFileString(const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszDefault)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);
	if (pszDefault == NULL) {
		pszDefault = L"";
	}

	return GetProfileStringW(pszSection, pszEntry, pszDefault);
}

int CApp::RegGetParamFileInt(const WCHAR *pszSection, const WCHAR *pszEntry, const int nDefault)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	return GetProfileIntW(pszSection, pszEntry, nDefault);
}

BOOL CApp::RegGetParamFileBOOL(const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bDefault)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	return ((GetProfileIntW(pszSection, pszEntry, bDefault) != 0) ? TRUE : FALSE);
}

DWORD CApp::RegGetParamFileHex(const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD dwDefault)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	WCHAR szDef[32];
	CString str;

	swprintf_s(szDef, L"%X", dwDefault);
	str = GetProfileStringW(pszSection, pszEntry, szDef);

	DWORD dwVal = 0;
	WCHAR c;
	const WCHAR *cp = (const WCHAR *)str;

	while (*cp) {
		c = toupper(*cp++);
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
			dwVal *= 16;
			c -= '0';
			if (c > 9) {
				c -= 7;
			}
			dwVal += c;
		} else {
			break;
		}
	}

	return dwVal;
}


BOOL CApp::RegWriteParamFileString(const WCHAR *pszSection, const WCHAR *pszEntry, const WCHAR *pszValue)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	return WriteProfileStringW(pszSection, pszEntry, pszValue);
}

BOOL CApp::RegWriteParamFileInt(const WCHAR *pszSection, const WCHAR *pszEntry, const int nValue)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	return WriteProfileInt(pszSection, pszEntry, nValue);
}

BOOL CApp::RegWriteParamFileBOOL(const WCHAR *pszSection, const WCHAR *pszEntry, const BOOL bValue)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	return WriteProfileInt(pszSection, pszEntry, (bValue ? 1 : 0));
}

BOOL CApp::RegWriteParamFileHex(const WCHAR *pszSection, const WCHAR *pszEntry, const DWORD dwValue)
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(pszSection != NULL);
	ASSERT(pszEntry != NULL);

	WCHAR sz[16];
	swprintf_s(sz, L"%X", dwValue);
	return WriteProfileStringW(pszSection, pszEntry, sz);
}
/////////////////////////////////////////////////////////////////////////////
