
// DlgMain.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "ZipCompressExtract.h"
#include "DlgMain.h"
#include "afxdialogex.h"
#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
	public:
	CAboutDlg();

	// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	// 実装
	protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDlgMain ダイアログ



CDlgMain::CDlgMain(CWnd* pParent /*=nullptr*/)
: CDialogEx(IDD_ZIPCOMPRESSEXTRACT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bEnding = FALSE;
}

CDlgMain::~CDlgMain()
{
	CoUninitialize();
}

void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgMain, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK, &CDlgMain::OnBnClickedOk)
ON_BN_CLICKED(IDCANCEL, &CDlgMain::OnBnClickedCancel)
ON_BN_CLICKED(IDC_BUTTON_QUIT, &CDlgMain::OnBnClickedButtonQuit)
ON_BN_CLICKED(IDC_BUTTON_EXTRACT, &CDlgMain::OnBnClickedButtonExtract)
ON_BN_CLICKED(IDC_BUTTON_COMPRESS, &CDlgMain::OnBnClickedButtonCompress)
END_MESSAGE_MAP()


// CDlgMain メッセージ ハンドラー

BOOL CDlgMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadStringW(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenuW(MF_SEPARATOR);
			pSysMenu->AppendMenuW(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	Init();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CDlgMain::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CDlgMain::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessageW(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CDlgMain::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDlgMain::OnBnClickedOk()
{
	CWnd *pWnd = GetFocus();

	if      (pWnd == GetDlgItem(IDC_BUTTON_QUIT))     { PostMessageW(WM_COMMAND, IDC_BUTTON_QUIT); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_COMPRESS)) { PostMessageW(WM_COMMAND, IDC_BUTTON_COMPRESS); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_EXTRACT))  { PostMessageW(WM_COMMAND, IDC_BUTTON_EXTRACT); }
	else {
		NextDlgCtrl();
	}
}

void CDlgMain::OnBnClickedCancel()
{
	PostMessageW(WM_COMMAND, IDC_BUTTON_QUIT);
}


/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 開始＆終了
//===========================================================================
void CDlgMain::Init()
{
	CoInitialize(NULL);
}


void CDlgMain::OnBnClickedButtonQuit()
{
	End();
}


void CDlgMain::End(const int nEndCode)
{
	if (m_bEnding) { return; }
	m_bEnding = TRUE;


	EndDialog(nEndCode ? IDCANCEL : IDOK);
}


//===========================================================================
//===========================================================================

/// <summary>
/// 解凍ボタンが押された
/// </summary>
void CDlgMain::OnBnClickedButtonExtract()
{
	ExtractFromZip(
		L"D:\\Developmentsoft\\VsWork\\VC22\\Test\\ZipCompressExtract\\Work\\Settings.zip",
		L"D:\\Developmentsoft\\VsWork\\VC22\\Test\\ZipCompressExtract\\Work\\展開後"
	);
}

/// <summary>
/// Zipファイルを指定フォルダに解凍する
/// </summary>
/// <param name="pszFilePath">ZIPファイル</param>
/// <param name="pszFolerPath">解凍先フォルダ</param>
/// <returns>FALSE:失敗/TRUE:成功</returns>
BOOL CDlgMain::ExtractFromZip(const WCHAR *pszFilePath, const WCHAR *pszFolerPath)
{
	BOOL    bVal = FALSE;
	HRESULT hr   = FALSE;

	IShellDispatch *pISD = NULL;
	Folder *pZipFile     = NULL;
	Folder *pDestination = NULL;

	long FilesCount  = 0;
	IDispatch   *pItem        = NULL;
	FolderItems *pFilesInside = NULL;

	VARIANT vOptions, vOutFolder, vInZipFile, vItem;

	VariantInit(&vItem);
	VariantInit(&vOptions);
	VariantInit(&vOutFolder);
	VariantInit(&vInZipFile);

	hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD);
	if (FAILED(hr)) { goto ERR_END; }

	vInZipFile.vt = VT_BSTR;
	vInZipFile.bstrVal = SysAllocString(pszFilePath);
	hr = pISD->NameSpace(vInZipFile, &pZipFile);
	if (FAILED(hr)) { goto ERR_END; }

	vOutFolder.vt = VT_BSTR;
	vOutFolder.bstrVal = SysAllocString(pszFolerPath);
	hr = pISD->NameSpace(vOutFolder, &pDestination);
	if (FAILED(hr)) { goto ERR_END; }

	hr = pZipFile->Items(&pFilesInside);
	if (FAILED(hr)) { goto ERR_END; }

	hr = pFilesInside->get_Count(&FilesCount);
	if (FAILED(hr) || FilesCount < 1) { goto ERR_END; }

	hr = pFilesInside->QueryInterface(IID_IDispatch, (void**)&pItem);
	if (FAILED(hr)) { goto ERR_END; }

	vItem.vt       = VT_DISPATCH;
	vItem.pdispVal = pItem;

	vOptions.vt   = VT_I4;
	vOptions.lVal = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR |FOF_NOERRORUI | FOF_NOCOPYSECURITYATTRIBS;

	//	(0x0004)FOF_SILENT                進行状況ダイアログ ボックスを表示しません。
	//	(0x0008)FOF_RENAMEONCOLLISION     ターゲット名を持つファイルが既に存在する場合は、移動、コピー、または名前変更操作で、新しい名前で操作されているファイルを指定します。
	//	(0x0010)FOF_NOCONFIRMATION        表示されるダイアログ ボックスに対して、"すべてにはい" と応答します
	//	(0x0040)FOF_ALLOWUNDO             可能な場合は、元に戻す情報を保持します。
	//	(0x0080)FOF_FILESONLY             ワイルドカード ファイル名 (*.*) が指定されている場合にのみ、ファイルに対して操作を実行します。
	//	(0x0100)FOF_SIMPLEPROGRESS        進行状況ダイアログ ボックスを表示しますが、ファイル名は表示しません。
	//	(0x0200)FOF_NOCONFIRMMKDIR        操作で作成する必要がある場合は、新しいディレクトリの作成を確認しないでください
	//	(0x0400)FOF_NOERRORUI             エラーが発生した場合は、ユーザー インターフェイスを表示しないでください
	//	(0x0800)FOF_NOCOPYSECURITYATTRIBS ファイルのセキュリティ属性をコピーしないでください。(Ver 4.71)
	//	(0x1000)FOF_NORECURSION           ローカル ディレクトリでのみ動作します。 サブディレクトリに再帰的に操作しないでください。
	//	(0x2000)FOF_NO_CONNECTED_ELEMENTS 接続されているファイルをグループとしてコピーしないでください。 指定したファイルのみをコピーします。(Ver 5.0)

	hr = pDestination->CopyHere(vItem, vOptions);
	if (FAILED(hr)) { goto ERR_END; }

	bVal = TRUE;
	WaitForCopyToComplete();


ERR_END:
	VariantClear(&vItem);
	VariantClear(&vOptions);
	VariantClear(&vOutFolder);
	VariantClear(&vInZipFile);

	if (pItem        != NULL) { pItem       ->Release(); }
	if (pFilesInside != NULL) { pFilesInside->Release(); }
	if (pDestination != NULL) { pDestination->Release(); }
	if (pZipFile     != NULL) { pZipFile    ->Release(); }
	if (pISD         != NULL) { pISD        ->Release(); }

	return bVal;
}


//===========================================================================
//===========================================================================


/// <summary>
/// 圧縮ボタンが押された
/// </summary>
void CDlgMain::OnBnClickedButtonCompress()
{
	CompressToZip(
		L"D:\\Developmentsoft\\VsWork\\VC22\\Test\\ZipCompressExtract\\Work\\Compress.zip",
		L"D:\\Developmentsoft\\VsWork\\VC22\\Test\\ZipCompressExtract\\Work\\圧縮するファイル"
	);
}


/// <summary>
/// 指定フォルダ内のファイルをZIPファイルに圧縮する
/// </summary>
/// <param name="pszFilePath">Zipファイル</param>
/// <param name="pszFolerPath">指定フォルダ</param>
/// <returns>FALSE:失敗/TRUE:成功</returns>
BOOL CDlgMain::CompressToZip(const WCHAR *pszFilePath, const WCHAR *pszFolerPath)
{
	BOOL bVal = FALSE;
	HRESULT hr;
	IShellDispatch *pISD = NULL;
	
	VARIANT vInFolder;
	VARIANT vOutZipFile;
	VARIANT vItem, vOptions;
	Folder  *pSource  = NULL;
	Folder  *pZipFile = NULL;

	VariantInit(&vInFolder);
	VariantInit(&vOutZipFile);
	VariantInit(&vItem);
	VariantInit(&vOptions);

	hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pISD));
	if (FAILED(hr)) { goto ERR_END; }

	// 追加Folderオブジェクトを作成
	vInFolder.vt      = VT_BSTR;
	vInFolder.bstrVal = SysAllocString(pszFolerPath);
	hr = pISD->NameSpace(vInFolder, &pSource);
	if (FAILED(hr)) {
		MessageBoxW(L"追加フォルダーが見つかりませんでした。", L"エラー", MB_ICONWARNING);
		goto ERR_END;
	}

	// ZIPファイルを削除
	File::UnlinkFile(pszFilePath);

	// ZIPファイルの元を作成
	{
		File fp;
		BYTE bin[] = { 80, 75, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		if (fp.Open(pszFilePath, File::_WRITE)) {
			fp.Write((const char*)bin, sizeof(bin));
			fp.Close();
		} else {
			MessageBoxW(L"ZIPファイルが作成できませんでした。", L"エラー", MB_ICONWARNING);
			goto ERR_END;
		}
	}

	// ZIPファイルのFolderオブジェクトを作成
	vOutZipFile.vt      = VT_BSTR;
	vOutZipFile.bstrVal = SysAllocString(pszFilePath);
	hr = pISD->NameSpace(vOutZipFile, &pZipFile);
	if (FAILED(hr)) {
		MessageBoxW(L"ZIPファイルが見つかりませんでした。", L"エラー", MB_ICONWARNING);
		goto ERR_END;
	}

	// ZIPへフォルダーをコピー
	vItem.vt       = VT_DISPATCH;
	vItem.pdispVal = pSource;

	vOptions.vt   = VT_I4;
	vOptions.lVal = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR |FOF_NOERRORUI | FOF_NOCOPYSECURITYATTRIBS;

	//	(0x0004)FOF_SILENT                進行状況ダイアログ ボックスを表示しません。
	//	(0x0008)FOF_RENAMEONCOLLISION     ターゲット名を持つファイルが既に存在する場合は、移動、コピー、または名前変更操作で、新しい名前で操作されているファイルを指定します。
	//	(0x0010)FOF_NOCONFIRMATION        表示されるダイアログ ボックスに対して、"すべてにはい" と応答します
	//	(0x0040)FOF_ALLOWUNDO             可能な場合は、元に戻す情報を保持します。
	//	(0x0080)FOF_FILESONLY             ワイルドカード ファイル名 (*.*) が指定されている場合にのみ、ファイルに対して操作を実行します。
	//	(0x0100)FOF_SIMPLEPROGRESS        進行状況ダイアログ ボックスを表示しますが、ファイル名は表示しません。
	//	(0x0200)FOF_NOCONFIRMMKDIR        操作で作成する必要がある場合は、新しいディレクトリの作成を確認しないでください
	//	(0x0400)FOF_NOERRORUI             エラーが発生した場合は、ユーザー インターフェイスを表示しないでください
	//	(0x0800)FOF_NOCOPYSECURITYATTRIBS ファイルのセキュリティ属性をコピーしないでください。(Ver 4.71)
	//	(0x1000)FOF_NORECURSION           ローカル ディレクトリでのみ動作します。 サブディレクトリに再帰的に操作しないでください。
	//	(0x2000)FOF_NO_CONNECTED_ELEMENTS 接続されているファイルをグループとしてコピーしないでください。 指定したファイルのみをコピーします。(Ver 5.0)

	hr = pZipFile->CopyHere(vItem, vOptions);
	if (FAILED(hr)) {
		MessageBoxW(L"ZIPファイルにファイル追加失敗", L"エラー", MB_ICONWARNING);
		goto ERR_END;
	}
	bVal = TRUE;

	// CopyHere の終了を待機
	WaitForCopyToComplete();

ERR_END:
	VariantClear(&vInFolder);
	VariantClear(&vOutZipFile);
	VariantClear(&vItem);
	VariantClear(&vOptions);

	if (pISD     != NULL) { pISD->Release(); }
	if (pSource  != NULL) { pSource->Release(); }
	if (pZipFile != NULL) { pZipFile->Release(); }

	return bVal;
}


/// <summary>
/// CopyHere()の終了を待つ
/// </summary>
void CDlgMain::WaitForCopyToComplete()
{
	DWORD ThreadOldCount = 0;
	DWORD ThreadNewCount = 0;
	HANDLE hSnapshot     = NULL;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te;
		te.dwSize = sizeof(THREADENTRY32);
		if (Thread32First(hSnapshot, &te)) {
			do {
				if (te.th32OwnerProcessID == GetCurrentProcessId() && te.th32ThreadID != GetCurrentThreadId()) {
					ThreadOldCount++;
				}
			} while (Thread32Next(hSnapshot, &te));
		}
		CloseHandle(hSnapshot);
	}

	do {
		Sleep(100);
		ThreadNewCount = 0;
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hSnapshot != INVALID_HANDLE_VALUE) {
			THREADENTRY32 te;
			te.dwSize = sizeof(THREADENTRY32);
			if (Thread32First(hSnapshot, &te)) {
				do {
					if (te.th32OwnerProcessID == GetCurrentProcessId() && te.th32ThreadID != GetCurrentThreadId()) {
						ThreadNewCount++;
					}
				} while (Thread32Next(hSnapshot, &te));
			}
			CloseHandle(hSnapshot);
		}
	} while (ThreadNewCount > ThreadOldCount);
}


/*
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

void CompressToZip(LPCWSTR sourceFolder, LPCWSTR zipFilePath)
{
	CoInitialize(NULL);

	IShellDispatch *pISD;
	Folder *pToFolder = NULL;
	Folder *pFromFolder = NULL;
	VARIANT vDir, vFile, vOpt;

	CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD);

	VariantInit(&vDir);
	vDir.vt = VT_BSTR;
	vDir.bstrVal = SysAllocString(zipFilePath);
	pISD->NameSpace(vDir, &pToFolder);

	VariantInit(&vFile);
	vFile.vt = VT_BSTR;
	vFile.bstrVal = SysAllocString(sourceFolder);
	pISD->NameSpace(vFile, &pFromFolder);

	VariantInit(&vOpt);
	vOpt.vt = VT_I4;
	vOpt.lVal = FOF_NO_UI;

	pToFolder->CopyHere(vFile, vOpt);

	pFromFolder->Release();
	pToFolder->Release();
	pISD->Release();

	CoUninitialize();
}
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

void DecompressFromZip(LPCWSTR zipFilePath, LPCWSTR destinationFolder)
{
	CoInitialize(NULL);

	IShellDispatch *pISD;
	Folder *pToFolder = NULL;
	Folder *pFromFolder = NULL;
	VARIANT vDir, vFile, vOpt;

	CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pISD);

	VariantInit(&vDir);
	vDir.vt = VT_BSTR;
	vDir.bstrVal = SysAllocString(destinationFolder);
	pISD->NameSpace(vDir, &pToFolder);

	VariantInit(&vFile);
	vFile.vt = VT_BSTR;
	vFile.bstrVal = SysAllocString(zipFilePath);
	pISD->NameSpace(vFile, &pFromFolder);

	VariantInit(&vOpt);
	vOpt.vt = VT_I4;
	vOpt.lVal = FOF_NO_UI;

	pToFolder->CopyHere(vFile, vOpt);

	pFromFolder->Release();
	pToFolder->Release();
	pISD->Release();

	CoUninitialize();
}

*/
