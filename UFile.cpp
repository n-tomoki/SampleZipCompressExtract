

//#include "stdafx.h"
#include "pch.h"

#include "ufile.h"

//////////////////////////////////////////////////////////////
// エラーメッセージを強制的に英語にする(自動変更)
// “MsgEnglish(TRUE)”
//////////////////////////////////////////////////////////////

//============================================================
// エラーメッセージの切り替え
//============================================================
BOOL File::m_bFirst;
BOOL File::m_bEnglish;

BOOL File::MsgEnglish(const BOOL bEnglish)
{
	m_bFirst   = TRUE;
	m_bEnglish = bEnglish;

	return m_bEnglish;
}

BOOL File::MsgEnglish()
{
	return m_bEnglish;
}


//============================================================
// 開始処理＆終了処理
//============================================================

/// <summary>コンストラクタ</summary>
File::File()
{
	m_bOpen      = FALSE;
	m_uErrCode   = FILE_ERR_NONE;
	m_enCharCode = _PENDING;
	
	m_pMsg = NULL;

	if (!m_bFirst) {
		m_bFirst = TRUE;

		WCHAR sz[25];

		if (GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTCODEPAGE, sz, 20)) {
			if (932 != _wtoi(sz)) {
				m_bEnglish = TRUE;
			}
		}
	}
}

/// <summary>デストラクタ</summary>
File::~File()
{
	if (m_bOpen) {
		Close();
	}
}


//============================================================
// ファイルのオープン
//============================================================

/// <summary>
/// ファイルオープン
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <param name="nMode">開くモード</param>
/// <param name="enCharCode">キャラクターモード</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::Open(const WCHAR *pszFileName, const OpenMode enMode, const CharMode enCharCode)
{
	BOOL bCode = FALSE;
	UINT uOpenFlag = 0;
	int  nRetry = 5;
	int  nOpenMode = (enMode & ~FILE_ACCESS_LOCK);
	CFileException Excep;

	if (enMode & FILE_ACCESS_LOCK) { uOpenFlag = CFile::shareExclusive; }
	else                           { uOpenFlag = CFile::shareDenyNone;  }

	if (m_bOpen) { Close(); }

	while (1) {
		if (nOpenMode == _READ) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeRead, &Excep);
		} else if (nOpenMode == _WRITE) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeCreate | CFile::modeWrite, &Excep);
		} else if (nOpenMode == _APPEND) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeReadWrite, &Excep);
			if (!bCode) {
				bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeCreate | CFile::modeWrite, &Excep);
			}
			if (bCode) {
				SeekEndAction();	// ファイルの最後に移動
			}
		} else if (nOpenMode == _RDWR) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeReadWrite, &Excep); 
			if (!bCode) {
				bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeCreate | CFile::modeWrite, &Excep);
			}
		}

		if (bCode)                                   { break; }
		if (Excep.m_cause != Excep.sharingViolation) { break; }
		if (--nRetry == 0)                           { break; }
		Sleep(100);
	}

	if (bCode) {
		m_bOpen      = TRUE;
		m_uErrCode   = FILE_ERR_NONE;
		m_enCharCode = enCharCode;
	} else {
		m_uErrCode = Excep.m_cause;
	}
	return m_bOpen;
}


//============================================================
// ファイルのクローズ
//============================================================

/// <summary>
/// ファイルのクローズ
/// </summary>
/// <returns>TRUE:失敗/FALSE:成功</returns>
BOOL File::Close()
{
	if (!m_bOpen) { return FALSE; }

	BOOL bRet = FALSE;

	m_uErrCode = FILE_ERR_NONE;
	try {
		m_File.Close();
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
		bRet = TRUE;
	}
	m_bOpen      = FALSE;
	m_enCharCode = _PENDING;

	return bRet;
}


//============================================================
// ファイルからの読み込み
//============================================================

/// <summary>
/// 1バイト読み込む
/// </summary>
/// <returns>成功:0x00～0xff/失敗:_NO_DATA</returns>
unsigned int File::Read()
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return _NO_DATA; }

	char c;

	try {
		if (m_File.Read(&c, 1)) {
			return (unsigned int)c;
		}
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return _NO_DATA;
}


/// <summary>
/// nバイト読み込む 
/// </summary>
/// <param name="pBuf">読み込む場所</param>
/// <param name="nLen">読み込むバイト数</param>
/// <returns>読み込んだバイト数</returns>
int File::Read(void *pBuf, const int nBuffSize)
{
	if (nBuffSize <=  0) { return 0; }
	if (!m_bOpen)        { *(char *)pBuf = '\0'; return 0; }

	int n = 0;

	try {
		n = m_File.Read(pBuf, nBuffSize);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}
	
	return n;
}


/// <summary>
/// １行読み込む(Unicodeで返ってくる)
/// </summary>
/// <param name="str">CStringを渡す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::Gets(CString &str)
{
	str.Empty();
	if (!m_bOpen) { return FALSE; }

	CMemory tmp;

	BOOL bRet = Gets(tmp);

	// 読み込めたらUnicodeに変換する
	if (bRet) {
		if (m_enCharCode == _PENDING) { AnalyzeCharCode(tmp); }

		switch (m_enCharCode) {
		default:
		case _SHIFT_JIS: ShiftJisToUnicode(tmp, str); break;
		case _UTF8:      UTF8ToUnicode(tmp, str); break;
		}
	}

	return bRet;
}


/// <summary>
/// １行読み込む
/// </summary>
/// <param name="mem">CMemoryを渡す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::Gets(CMemory &mem)
{
	mem.Clear();
	if (!m_bOpen) { return FALSE; }

	int  n;
	int  nCnt = 0;
	BOOL bRet = FALSE;
	BOOL bRun = TRUE;
	char szLine[FILE_GETS_BUFFER_SIZE + 5];

	while (bRun) {
		nCnt = 0;
		while (nCnt < FILE_GETS_BUFFER_SIZE) {
			n = Read();
			if (n == _NO_DATA) { // 最後まで読んだら終わる
				bRun = FALSE;
				break;
			} else if (n == 0) { // 文字列の読み込みで「0」はありえない
				bRun = FALSE;
				bRet = FALSE;
				break;
			}
			bRet = TRUE;
			if (n == C_LF || n == C_CR) {
				unsigned int n2 = Read();	// dumy
				if (n2 == _NO_DATA) {
				} else if (n == C_LF && n2 == C_CR) {
				} else if (n == C_CR && n2 == C_LF) {
				} else {
					SeekOff(-1);
				}
				bRun = FALSE;
				break;
			}
			szLine[nCnt++] = n;
		}
		szLine[nCnt] = '\0';
		mem += szLine;
	}

	return bRet;
}

/// <summary>
/// キャラクターコードの判断を頑張る
/// </summary>
/// <param name="memBuf">読み込んだ文字列</param>
void File::AnalyzeCharCode(CMemory &memBuf)
{
	if (m_enCharCode != _PENDING) { return; }

	const BYTE *cp = (const BYTE *)memBuf;
	int nSize      = memBuf.GetSize();

	while (nSize-- && m_enCharCode == _PENDING) {
		// １バイト目
		if      (0x81 <= *cp && *cp <= 0x9f) { m_enCharCode = _SHIFT_JIS; }
		else if (0xc0 <= *cp && *cp <= 0xdf) { m_enCharCode = _UTF8;      }
		else if (0xf0 <= *cp && *cp <= 0xf7) { m_enCharCode = _UTF8;      }
		else if (0xa0 <= *cp && *cp <= 0xc1) { m_enCharCode = _SHIFT_JIS; }
		else if (0xe0 <= *cp && *cp <= 0xef) {
			// ２バイト目
			cp++;
			if (0x40 <= *cp && *cp <= 0x7e) { m_enCharCode = _SHIFT_JIS; }
			else if (0x80 <= *cp) {
				if (*cp <= 0xbf)  { // 0x80-0xbf
					// ３バイト目
					cp++;
					if (0x80 <= *cp && *cp <= 0xbf) { m_enCharCode = _UTF8;      }
					else                            { m_enCharCode = _SHIFT_JIS; }
				} else if (*cp <= 0xef) { // 0xc0-0xef
					m_enCharCode = _SHIFT_JIS;
				}
			}
		}
		cp++;
	}
}




//============================================================
// ファイルの書き込み
//============================================================

/// <summary>
/// nバイト書き込む
/// </summary>
/// <param name="pBuf">書き込むデータ</param>
/// <param name="nLen">書き込むバイト数</param>
void File::Write(const char *pszBuf, const int nLen)
{
	if (!m_bOpen) { return; }

	m_uErrCode = FILE_ERR_NONE;
	try {
		m_File.Write(pszBuf, nLen);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}
}


/// <summary>
/// '\0'コードまで書き込む
/// </summary>
/// <param name="pszBuf">書き込むデータ</param>
void File::Write(const char *pszBuf)
{
	if (!m_bOpen) { return; }

	Write(pszBuf, (int)strlen(pszBuf));
}


/// <summary>
/// 文字列を書き込む
/// </summary>
/// <param name="pszBuf">書き込む文字列</param>
void File::Write(const WCHAR *pszBuf)
{
	if (!m_bOpen) { return; }

	CMemory tmp;

	switch (m_enCharCode) {
	default:
	case _PENDING:
		m_enCharCode = _UTF8;
	case _UTF8:
		UnicodeToUTF8(pszBuf, tmp);
		break;
	case _SHIFT_JIS:
		UnicodeToShiftJis(pszBuf, tmp);
		break;
	}

	Write(tmp, tmp.GetSize());
}


/// <summary>
/// 1行書き込む
/// </summary>
/// <param name="pszBuf">書き込む文字列</param>
void File::Puts(const WCHAR *pszBuf)
{
	if (!m_bOpen) { return; }

	Write(pszBuf);
	Write("\r\n");
}


/// <summary>
/// １文字書き込む
/// </summary>
/// <param name="c">書き込む文字</param>
void File::Putc(const WCHAR c)
{
	if (!m_bOpen) { return; }

	WCHAR sz[5];

	swprintf_s(sz, L"%c", c);
	Write(sz);
}


//============================================================
// ファイルのポイント操作
//============================================================

/// <summary>
/// ファイルポインタを移動する
/// </summary>
/// <param name="lPos">移動する位置</param>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekSet(const DWORD64 lPos)
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return - 1; }

	DWORD64 dw64Val = -1;

	try {
		dw64Val = m_File.Seek(lPos, CFile::begin);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを移動する
/// </summary>
/// <param name="lPos">移動するオフセット</param>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekOff(const DWORD64 lPos)
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return - 1; }

	DWORD64 dw64Val = -1;

	try {
		dw64Val = m_File.Seek(lPos, CFile::current);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを先頭に移動する
/// </summary>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekTop()
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return - 1; }

	DWORD64 dw64Val = -1;

	try {
		m_File.Seek(0, CFile::begin);
		dw64Val = 0;
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを最後に移動する
/// </summary>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekEnd()
{
	if (!m_bOpen) { return - 1; }

	return SeekEndAction();
}


/// <summary>
/// ファイルポインタを最後に移動する
/// </summary>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekEndAction()
{
	DWORD64 dw64Val = -1;

	m_uErrCode = FILE_ERR_NONE;
	try {
		dw64Val = m_File.Seek(0, CFile::end);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを位置を返す
/// </summary>
/// <returns>ファイルポインタの位置</returns>
DWORD64 File::GetPosition()
{
	DWORD64 dw64Pos = 0;

	if (!m_bOpen) { return 0; }

	m_uErrCode = FILE_ERR_NONE;
	try {
		dw64Pos = m_File.GetPosition();
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}
	return dw64Pos;
}
	

//============================================================
// ファイルの削除
//============================================================

/// <summary>
/// ファイルの削除
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <param name="bCheck">消えたか？確認する</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL File::Unlink(const WCHAR *pszFileName, const BOOL bCheck)
{
	BOOL bErr = FALSE;

	try {
		if (IsExistFile(pszFileName)) {
			CFile::Remove(pszFileName);

			if (bCheck == TRUE) {
				// 最大約２秒まで待つ
				int nLoop = 200;
				do {
					Sleep(10);
					if (!IsExistFile(pszFileName)) { nLoop = 0; }
				} while (nLoop--);
			}
		}
	} catch (CFileException *e) {
		bErr = TRUE;
		e->Delete();
	}

	return bErr;
}


/// <summary>
/// ファイルの削除
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <param name="bCheck">消えたか？確認する</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL File::UnlinkFile(const WCHAR *pszFileName, const BOOL bCheck)
{
	return Unlink(pszFileName, bCheck);
}


/// <summary>
/// 指定フォルダ以下を削除する
/// </summary>
/// <param name="pszPath">フォルダ</param>
/// <returns>TRUE</returns>
BOOL File::UnlinkFolder(const WCHAR *pszPath)
{
	UnlinkFolderMain(pszPath);
	RemoveDirectoryW(pszPath);

	return TRUE;
}


/// <summary>
/// 指定フォルダ以下を削除する(実行部)
/// </summary>
/// <param name="pszPath">フォルダ</param>
/// <returns>TRUE</returns>
BOOL File::UnlinkFolderMain(const WCHAR *pszPath)
{
	int nPathLen    = (int)wcslen(pszPath) + 10;
	WCHAR *pSrcFile = new WCHAR[nPathLen + 1];

	wcscpy_s(pSrcFile, nPathLen, pszPath);
	wcscat_s(pSrcFile, nPathLen, L"\\*.*");

	CFileFind Find;
	BOOL bFind = Find.FindFile(pSrcFile);
	while (bFind) {
		bFind = Find.FindNextFileW();
		if (Find.IsDots()) continue;

		if (Find.IsDirectory()) {
			int nLen = Find.GetFilePath().GetLength() + 2;
			WCHAR *p  = new WCHAR[nLen + 1];

			wcscpy_s(p, nLen, (const WCHAR *)Find.GetFilePath());
			UnlinkFolderMain(p);
			RemoveDirectoryW(p);

			delete []p;
			continue;
		}

		Unlink(Find.GetFilePath());
	}

	delete []pSrcFile;

	return TRUE;
}

//============================================================
// ステイタス関係
//============================================================

/// <summary>
/// ファイルの更新秒を返す
/// </summary>
/// <returns>更新秒</returns>
time_t File::GetStatusTimet()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_mtime.GetTime();
}

/// <summary>
/// ファイルの更新秒を返す
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>更新秒</returns>
time_t File::GetStatusTimet(const WCHAR *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_mtime.GetTime();
}

/// <summary>
/// ファイルサイズを返す
/// </summary>
/// <returns>ファイルサイズ</returns>
DWORD64 File::GetStatusSize()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_size;
}

/// <summary>
/// ファイルサイズを返す
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>ファイルサイズ</returns>
DWORD64 File::GetStatusSize(const WCHAR *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_size;
}

/// <summary>
/// ファイルのアトリビュートを返す
/// </summary>
/// <returns>ファイルのアトリビュート</returns>
BYTE  File::GetStatusAttr()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return (BYTE)st.m_attribute; 
}

/// <summary>
/// ファイルのアトリビュートを返す
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>ファイルのアトリビュート</returns>
BYTE  File::GetStatusAttr(const WCHAR *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return (BYTE)st.m_attribute; 
}


//============================================================
// ファイルの名前変更
//============================================================

/// <summary>
/// ファイル名の変更
/// </summary>
/// <param name="pszOldName">旧ファイル名</param>
/// <param name="pszNewName">新ファイル名</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL File::Rename(const WCHAR *pszOldName, const WCHAR *pszNewName)
{
	BOOL bErr = FALSE;

	try {
		if (IsExistFile(pszOldName)) {
			CFile::Rename(pszOldName, pszNewName);
		} else {
			bErr = TRUE;
		}
	} catch(CFileException *p) {
		bErr = TRUE;
		p->Delete();
	}

	return bErr;
}

//============================================================
// 空ファイルの作成
//============================================================

/// <summary>
/// 空ファイルの作成
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::CreateEmptyFile(const WCHAR *pszFileName)
{
	BOOL  bOpen = FALSE;
	CFile File;

	if (TRUE == (bOpen = File.Open(pszFileName, CFile::modeCreate | CFile::modeWrite))) {
		File.Close();
	}

	return bOpen;
}


/// <summary>
/// フォルダの作成
/// </summary>
/// <param name="pszPath">フォルダ名</param>
/// <returns>TRUE:成功/FALSE:失敗(既にフォルダが有る)</returns>
BOOL File::CreateFolder(const WCHAR  *pszPath)
{
	return CreateDirectoryW(pszPath, NULL);
}



//=============================================================
// UTF-8変換
//=============================================================


/// <summary>
/// UnicodeをUTF-8に変換する
/// </summary>
/// <param name="pszUnicode">変換元の文字列</param>
/// <param name="memUtf8">変換後の文字列</param>
void File::UnicodeToUTF8(const WCHAR *pszUnicode, CMemory &memUtf8)
{
	int nLenUnicode = (int)wcslen(pszUnicode);

	// 変換する文字列がない場合は処理しない
	if (nLenUnicode == 0) {
		memUtf8.Clear();
		return;
	}

	// UTF-8に変換後のサイズを計算する
	int nLenUtf8 = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// UTF-8に変換する
	char *pBufUTF8 = new char[nLenUtf8 + 1];
	WideCharToMultiByte(CP_UTF8, 0, pszUnicode, nLenUnicode, pBufUTF8, nLenUtf8, NULL, NULL);

	memUtf8.Copy(pBufUTF8, nLenUtf8);

	delete []pBufUTF8;
}


/// <summary>
/// UTF-8をUnicodeに変換する
/// </summary>
/// <param name="memUtf8">変換元の文字列</param>
/// <param name="str">変換後の文字列</param>
void File::UTF8ToUnicode(CMemory &memUtf8, CString &str)
{
	int nLenUtf8 = memUtf8.GetSize();

	// 変換する文字列がない場合は処理しない
	if (nLenUtf8 == 0) {
		str.Empty();
		return;
	}

	// Unicodeに変換後のサイズを計算する
	int nLenUnicode = MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8 + 1, NULL, 0);

	// UniCodeに変換する
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8  + 1, pBufUnicode, nLenUnicode);

	str = pBufUnicode;

	delete []pBufUnicode;
}


/// <summary>
/// UnicodeをShiftJisに変換する
/// </summary>
/// <param name="pszUnicode">変換元の文字列</param>
/// <param name="memShiftJis">変換後の文字列</param>
void File::UnicodeToShiftJis(const WCHAR *pszUnicode, CMemory &memShiftJis)
{
	int nLenUnicode = (int)wcslen(pszUnicode);

	// 変換する文字列がない場合は処理しない
	if (nLenUnicode == 0) {
		memShiftJis.Clear();
		return;
	}

	// ShiftJisに変換後のサイズを計算する
	int nLenShiftJis = WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// ShiftJisに変換する
	char *pBufShiftJis = new char[nLenShiftJis + 1];
	WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, pBufShiftJis, nLenShiftJis, NULL, NULL);
	*(pBufShiftJis + nLenShiftJis) = '\0';

	memShiftJis = pBufShiftJis;

	delete []pBufShiftJis;
}


/// <summary>
/// ShiftJisをUnicodeに変換する
/// </summary>
/// <param name="memShiftJis"></param>
/// <param name="str"></param>
void File::ShiftJisToUnicode(CMemory &memShiftJis, CString &str)
{
	int nLneShiftJis = memShiftJis.GetSize();

	// 変換する文字列がない場合は処理しない
	if (nLneShiftJis == 0) {
		str.Empty();
		return;
	}

	// Uniocdeに変換後のサイズを計算する
	int nLenUnicode = MultiByteToWideChar(CP_ACP, 0, memShiftJis, nLneShiftJis, NULL, 0);

	// Uniocdeに変換する
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_ACP, 0, memShiftJis, nLneShiftJis, pBufUnicode, nLenUnicode);
	*(pBufUnicode + nLenUnicode) = '\0';

	str = pBufUnicode;

	delete []pBufUnicode;
}



//============================================================
// その他
//============================================================

/// <summary>
/// ファイルがあるか？
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>TRUE:ある/FALSE:ない</returns>
BOOL File::IsExistFile(const WCHAR *pszFileName)
{
	BYTE bAttr = GetStatusAttr(pszFileName);

	if (bAttr == 0) { return FALSE; }

	return (bAttr & ~FILE_ATTRIBUTE_DIRECTORY) != 0 ? TRUE : FALSE;
}

/// <summary>
/// フォルダがあるか？
/// </summary>
/// <param name="pszFilePath">フォルダ名</param>
/// <returns>TRUE:ある/FALSE:ない</returns>
BOOL File::IsExistFolder(const WCHAR *pszFilePath)
{
	BYTE bAttr = GetStatusAttr(pszFilePath);

	if (bAttr == 0) { return FALSE; }

	return (bAttr & FILE_ATTRIBUTE_DIRECTORY) != 0 ? TRUE : FALSE;
}

/// <summary>
/// 最後のエラーコードを返す
/// </summary>
/// <returns>エラーコード</returns>
UINT File::GetErrorCode()
{
	return m_uErrCode;
}

/// <summary>
/// 最後のエラー内容を返す
/// </summary>
/// <param name="pstr">受け取る位置</param>
/// <param name="nMaxLen">受け取る最大文字数</param>
/// <returns>エラーコード</returns>
UINT File::GetErrorString(WCHAR *pstr, const int nMaxLen)
{
	const WCHAR *pMsg = GetErrorString();
	int nLen         = (int)wcslen(pMsg);

	if (nLen < nMaxLen - 1) {
		wcscpy_s(pstr, nMaxLen, pMsg);
	} else {
		*pstr = '\0';
	}

	return m_uErrCode;
}

/// <summary>
/// 最後のエラー内容
/// </summary>
/// <returns>エラー内容</returns>
const WCHAR *File::GetErrorString()
{
	m_pMsg = NULL;

	switch (GetErrorCode()) {
	case FILE_ERR_NONE:
		if (m_bEnglish) { m_pMsg = L"No error occurred."; }
		else            { m_pMsg = L"エラーはありません"; }
		break;

	case FILE_ERR_GENERIC:
	default:
		if (m_bEnglish) { m_pMsg = L"An unspecified error occurred."; }
		else            { m_pMsg = L"不明なエラーが発生しました"; }
		break;
		
	case FILE_ERR_FILE_NOT_FOUND:
		if (m_bEnglish) { m_pMsg = L"The file could not be located."; }
		else            { m_pMsg = L"ファイルがありません"; }
		break;
		
	case FILE_ERR_BAD_PATH:
		if (m_bEnglish) { m_pMsg = L"All or part of the path is invalid."; }
		else            { m_pMsg = L"パスが不正です"; }
		break;
		
	case FILE_ERR_TOO_MANY_OPEN_FILES:
		if (m_bEnglish) { m_pMsg = L"The permitted number of open files was exceeded."; }
		else            { m_pMsg = L"これ以上ファイルがオープンできません"; }
		break;
		
	case FILE_ERR_ACCESS_DENIED:
		if (m_bEnglish) { m_pMsg = L"The file could not be accessed."; }
		else            { m_pMsg = L"ファイルのアクセスが禁止されています"; }
		break;
		
	case FILE_ERR_INVALID_FILE:
		if (m_bEnglish) { m_pMsg = L"There was an attempt to use an invalid file handle."; }
		else            { m_pMsg = L"ファイルハンドルが不正です"; }
		break;
		
	case FILE_ERR_REMOVE_CURRENT_DIR:
		if (m_bEnglish) { m_pMsg = L"The current working directory cannot be removed."; }
		else            { m_pMsg = L"カレント・フォルダーの削除はできません"; }
		break;

	case FILE_ERR_DIRCTORY_FULL:
		if (m_bEnglish) { m_pMsg = L"There are no more directory entries."; }
		else            { m_pMsg = L"フォルダーが一杯です"; }
		break;
		
	case FILE_ERR_BAD_SEEK:
		if (m_bEnglish) { m_pMsg = L"There was an error trying to set the file pointer."; }
		else            { m_pMsg = L"ファイル・シークに失敗しました"; }
		break;

	case FILE_ERR_HARD_IO:
		if (m_bEnglish) { m_pMsg = L"There was a hardware error."; }
		else            { m_pMsg = L"ハードウェア・エラー発生"; }
		break;

	case FILE_ERR_SHRING_VIOLALATION:
		if (m_bEnglish) { m_pMsg = L"SHARE.EXE was not loaded, or a shared region was locked."; }
		else            { m_pMsg = L"他のアプリケーションがファイルをロックしています"; }
		break;
		
	case FILE_ERR_LOCK_VIOLATION:
		if (m_bEnglish) { m_pMsg = L"There was an attempt to lock a region that was already locked."; }
		else            { m_pMsg = L"ロック済み領域のロック要求"; }
		break;

	case FILE_ERR_DISK_FULL:
		if (m_bEnglish) { m_pMsg = L"The disk is full."; }
		else            { m_pMsg = L"ディスクが一杯です"; }
		break;

	case FILE_ERR_END_OF_FILE:
		if (m_bEnglish) { m_pMsg = L"The end of file was reached."; }
		else            { m_pMsg = L"END OF FILE"; }
		break;
	}

	return m_pMsg;
}

