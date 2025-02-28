
// DlgMain.h : ヘッダー ファイル
//

#pragma once


// CDlgMain ダイアログ
class CDlgMain : public CDialogEx
{
// コンストラクション
public:
	CDlgMain(CWnd* pParent = nullptr);	// 標準コンストラクター
	virtual ~CDlgMain();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ZIPCOMPRESSEXTRACT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;
	BOOL  m_bEnding;

	void Init();
	void End(const int nEndCode = 0);

protected:
	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnBnClickedButtonExtract();
	afx_msg void OnBnClickedButtonCompress();
};
