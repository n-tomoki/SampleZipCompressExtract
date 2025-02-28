
// DlgMain.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "ZipCompressExtract.h"
#include "DlgMain.h"
#include "afxdialogex.h"

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
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


/// <summary>
/// 圧縮ボタンが押された
/// </summary>
void CDlgMain::OnBnClickedButtonCompress()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
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