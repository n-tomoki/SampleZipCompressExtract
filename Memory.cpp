

#include "pch.h"
//#include "StdAfx.h"
#include "Memory.h"


/// <summary>コンストラクタ</summary>
CMemory::CMemory()
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;
}


/// <summary>コンストラクタ</summary>
/// <param name="nSize">確保する容量</param>
CMemory::CMemory(const int nSize)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Init(nSize);
}


/// <summary>コンストラクタ</summary>
/// <param name="str">文字列</param>
CMemory::CMemory(const char *str)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Copy(str);
}


/// <summary>コンストラクタ</summary>
/// <param name="str">文字列</param>
/// <param name="nSize">データ数</param>
CMemory::CMemory(const char *str, const int nSize)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Copy(str, nSize);
}


/// <summary>コンストラクタ</summary>
/// <param name="s">初期値</param>
CMemory::CMemory(const CMemory &s)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Copy(s);
}


/// <summary>デストラクタ</summary>
CMemory::~CMemory()
{
	if (m_pBuf != NULL) { delete []m_pBuf; m_pBuf = NULL; }
}


//-------------------------------------------------------------------------------
// 初期化
//-------------------------------------------------------------------------------

/// <summary>初期化</summary>
void  CMemory::Clear()
{
	if (m_pBuf != NULL) { *m_pBuf = '\0'; }

	m_nSize = 0;
	m_nPos  = 0;
}


/// <summary>メモリー確保</summary>
/// <param name="nSize">確保する容量</param>
/// <returns>NULL:確保失敗/NULL以外:確保した先頭のポインタ</returns>
char *CMemory::Init(const int nSize)
{
	ASSERT(nSize <= MEMORY_SIZE_MAX);

	if (nSize > MEMORY_SIZE_MAX) { return NULL; }

	Clear();

	if (m_pBuf == NULL) {
		m_nSizeMax = CAL_BLOCK(nSize);
		m_pBuf     = new char[m_nSizeMax + 1];
		*m_pBuf    = '\0';
		m_nSize    = nSize;
	} else {
		if (nSize > m_nSizeMax) {
			delete []m_pBuf;
			m_pBuf = NULL;
			Init(nSize);
		} else {
			m_nSize = nSize;
		}
	}

	return m_pBuf;
}


//-------------------------------------------------------------------------------
// Copy
//-------------------------------------------------------------------------------

/// <summary>文字列を保存する</summary>
/// <param name="s">文字列</param>
/// <returns>NULL:保存失敗/NULL以外:保存した先頭のポインタ</returns>
const char *CMemory::Copy(const char *s)
{
	if (s  == NULL) { return NULL; }
	if (*s == '\0') { return NULL; }

	return Copy(s, (int)strlen(s));
}


/// <summary>データを保存する</summary>
/// <param name="s">データの先頭ポインタ</param>
/// <param name="nSize">データ数</param>
/// <returns>NULL:保存失敗/NULL以外:保存した先頭のポインタ</returns>
const char *CMemory::Copy(const char *s, const int nSize)
{
	if (s == NULL  ) { return NULL; }
	if (s == m_pBuf) { return NULL; }
	if (0 >= nSize ) { return NULL; }

	if (Init(nSize) != NULL) {
		memcpy_s((void *)m_pBuf, nSize, s, nSize);
		*(m_pBuf + nSize) = '\0';
	}

	return m_pBuf;
}


/// <summary>データを保存する</summary>
/// <param name="s">CMemoryクラス</param>
/// <returns>NULL:保存失敗/NULL以外:保存した先頭のポインタ</returns>
const char *CMemory::Copy(const CMemory &s)
{
	return Copy(s, s.GetSize());
}


//-------------------------------------------------------------------------------
// Add
//-------------------------------------------------------------------------------

/// <summary>文字列を追加する</summary>
/// <param name="s">文字列</param>
/// <returns>NULL:追加失敗/NULL以外:追加された先頭のポインタ</returns>
const char *CMemory::Add(const char *s)
{
	if (s  == NULL) { return NULL; }
	if (*s == '\0') { return NULL; }

	return Add(s, (int)strlen(s));
}


/// <summary>データを追加する</summary>
/// <param name="s">データ</param>
/// <param name="nSize">データ数</param>
/// <returns>NULL:追加失敗/NULL以外:追加された先頭のポインタ</returns>
const char *CMemory::Add(const char *s, const int nSize)
{
	if (s == NULL  ) { return NULL; }
	if (s == m_pBuf) { return NULL; }
	if (0 >= nSize ) { return NULL; }

	if (m_nSizeMax < m_nSize + nSize) {
		int nOldSize  = m_nSize;
		char *pOldBuf = m_pBuf; m_pBuf = NULL;

		if (Init(nOldSize + nSize) != NULL) {
			memcpy_s((void *)m_pBuf             , nOldSize, pOldBuf, nOldSize);
			memcpy_s((void *)(m_pBuf + nOldSize), nSize   , s      , nSize   );
			*(m_pBuf + m_nSize) = '\0';

			delete []pOldBuf;
		}
	} else {
		memcpy_s((void *)(m_pBuf + m_nSize), nSize, s, nSize);
		m_nSize  += nSize;
		*(m_pBuf +  m_nSize) = '\0';
	}

	return m_pBuf;
}


/// <summary>データを追加する</summary>
/// <param name="s">CMemoryクラス</param>
/// <returns>NULL:追加失敗/NULL以外:追加された先頭のポインタ</returns>
const char *CMemory::Add(const CMemory &s)
{
	return Add(s, s.GetSize());
}


/// <summary>データの追加する</summary>
/// <param name="s">追加する文字列</param>
void CMemory::AddW(const WCHAR *s)
{
	UnicodeToShiftJis(s);
}


//-------------------------------------------------------------------------------
// Read
//-------------------------------------------------------------------------------

/// <summary>
/// 保存データの読み込み
/// </summary>
/// <param name="s">読み込むポインタ</param>
/// <param name="nSize">読み込み数</param>
/// <returns>読み込んだ数</returns>
int CMemory::Read(char *s, const int nSize)
{
	// 途中まで読み込んでいた場合は、読み込み済みのデータを読み飛ばす
	if (m_nPos > 0) {
		CMemory tmp;
		int n  = m_nPos;
		m_nPos = 0;

		Read(tmp, n);
	}

	int nLen = nSize;

	if (0    >= nSize  ) { return 0; }
	if (NULL == m_pBuf ) { return 0; }
	if (nLen  > m_nSize) {
		nLen  = m_nSize;
	}

	memcpy_s((void *)s     , nLen   , m_pBuf         , nLen          );
	memcpy_s((void *)m_pBuf, m_nSize, (m_pBuf + nLen), m_nSize - nLen);

	m_nSize -= nLen;
	m_pBuf[m_nSize] = '\0';

	return nLen;
}


/// <summary>
/// 保存データの読み込み
/// </summary>
/// <param name="s">CMemory</param>
/// <param name="nSize">読み込み数</param>
/// <returns>読み込んだ数</returns>
int CMemory::Read(CMemory &s, const int nSize)
{
	// 途中まで読み込んでいた場合は、読み込み済みのデータを読み飛ばす
	if (m_nPos > 0) {
		CMemory tmp;
		int n  = m_nPos;
		m_nPos = 0;

		Read(tmp, n);
	}

	if (0    >= nSize ) { return 0; }
	if (NULL == m_pBuf) { return 0; }

	char *p  = new char[nSize + 1];
	int nLen = Read(p, nSize);
	s.Copy(p, nLen);
	delete []p;

	return nLen;
}



/// <summary>
/// 保存データの読み込み
/// </summary>
/// <returns>無い場合は[_NO_DATA]</returns>
int CMemory::Read()
{
	if (NULL   == m_pBuf ) { return _NO_DATA; }
	if (0      == m_nSize) { return _NO_DATA; }
	if (m_nPos >= m_nSize) { return _NO_DATA; }

	return m_pBuf[m_nPos++];
}


//-------------------------------------------------------------------------------
// Write&Puts
//-------------------------------------------------------------------------------

/// <summary>
/// データを追加する
/// </summary>
/// <param name="s">データ</param>
/// <returns>データ数</returns>
int CMemory::Write(const char *s)
{
	if (s  == NULL) { return 0; }
	if (*s == '\0') { return 0; }

	int n = (int)strlen(s);

	Add(s, n);

	return n;
}


/// <summary>
/// データを追加する
/// </summary>
/// <param name="s">データ</param>
/// <param name="nSize">データ数</param>
/// <returns>データ数</returns>
int CMemory::Write(const char *s, const int nSize)
{
	if (s == NULL  ) { return 0; }
	if (s == m_pBuf) { return 0; }
	if (0 >= nSize ) { return 0; }

	Add(s, nSize);

	return nSize;
}


/// <summary>
/// データを追加する
/// </summary>
/// <param name="s">データ</param>
/// <returns>データ数(\r\nを含む)</returns>
int CMemory::Puts(const char *s)
{
	int n = 0;

	if (s != NULL && *s != '\0') {
		n = (int)strlen(s);

		Add(s, n);
	}
	Add("\r\n", 2);

	return n + 2;
}


//-------------------------------------------------------------------------------
// Get
//-------------------------------------------------------------------------------

/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="pBuf">読み込む場所</param>
/// <param name="nMax">読み込む最大バイト数</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL CMemory::Gets(char *s, const int nSize)
{
	*s = '\0';

	if (0    >= nSize ) { return FALSE; }
	if (NULL == m_pBuf) { return FALSE; }

	int nLen;
	BOOL bRet = FALSE;
	unsigned int n;
	char *cp = s;

	nLen = nSize - 2;
	while (--nLen) {
		n = Read();
		if (n == _NO_DATA) { break; }
		// if (n == C_EOF)    { break; }
		bRet = TRUE;
		if (n == C_LF || n == C_CR) {
			unsigned int n2 = Read();	// dumy
			if (n2 == _NO_DATA) {}
			else if (n == C_LF && n2 == C_CR) {}
			else if (n == C_CR && n2 == C_LF) {}
			else {
				SeekOff(-1);
			}
			break;
		}
		*cp = (char)n;
		cp++;
	}
	*cp = '\0';

	return bRet;
}


/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="str">CStringを渡す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL CMemory::Gets(CString &str)
{
	CMemory s;

	BOOL bRet = Gets(s);

	str = (const char *)s;

	return bRet;
}



/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="str">CMemoryを渡す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL CMemory::Gets(CMemory &s)
{
	s.Clear();

	if (NULL == m_pBuf) { return FALSE; }

	int n;
	int nCnt;
	BOOL bRet = FALSE;
	BOOL bRun = TRUE;
	char sz[FILE_GETS_BUFFER_SIZE+5];

	while (bRun) {
		nCnt = 0;
		while (nCnt < FILE_GETS_BUFFER_SIZE) {
			n = Read();
			if (n == _NO_DATA) { bRun = FALSE; break; }
			// if (n == C_EOF)    { bRun = FALSE; break; }
			bRet = TRUE;
			if (n == C_LF || n == C_CR) {
				unsigned int n2 = Read();	// dumy
				if (n2 == _NO_DATA) {}
				else if (n == C_LF && n2 == C_CR) {}
				else if (n == C_CR && n2 == C_LF) {}
				else {
					SeekOff(-1);
				}
				bRun = FALSE;
				break;
			}
			sz[nCnt++] = n;
		}
		sz[nCnt] = '\0';
		s += sz;
	}

	return bRet;
}



//-------------------------------------------------------------------------------
// [m_nPos]を変更する
//-------------------------------------------------------------------------------

/// <summary>
/// 読み込み位置を先頭にする
/// </summary>
/// <returns>読み込み位置</returns>
int CMemory::SeekTop()
{
	m_nPos = 0;

	return m_nPos;
}


/// <summary>
/// 読み込み位置を設定する
/// </summary>
/// <param name="nPos">読み込み位置</param>
/// <returns>読み込み位置</returns>
int CMemory::SeekSet(const int nPos)
{
	if (nPos < 0 || m_nSize == 0) {
		m_nPos = 0;
	} else if (nPos >= 0 && nPos < m_nSize) { 
		m_nPos = nPos;
	} else {
		m_nPos = m_nSize - 1;
	}

	return m_nPos;
}


/// <summary>
/// 読み込み位置を移動する
/// </summary>
/// <param name="nOff">オフセット</param>
/// <returns>読み込み位置</returns>
int CMemory::SeekOff(const int nOff)
{
	return SeekSet(m_nPos + nOff);
}


/// <summary>
/// 現在の読み込み位置を返す
/// </summary>
/// <returns>読み込み位置</returns>
int CMemory::SeekGet() const
{
	return m_nPos;
}


//-------------------------------------------------------------------------------
// 保存している文字へのアクセス
//-------------------------------------------------------------------------------

/// <summary>
/// 指定位置の文字を返す
/// </summary>
/// <param name="n">位置</param>
/// <param name="pErr">エラー判断を返すポインタ(TRUE:エラー)</param>
/// <returns>指定位置の文字</returns>
char CMemory::GetAt(const int n, BOOL *pErr)
{
	if (n < 0 || n >= m_nSize) {
		*pErr = *pErr != NULL ? TRUE : NULL;
		return '\0';
	}

	*pErr = *pErr != NULL ? FALSE : NULL;

	return m_pBuf[n];
}


/// <summary>
/// 指定位置の文字を置き換える
/// </summary>
/// <param name="n">位置</param>
/// <param name="c">文字</param>
/// <param name="pErr">エラー判断を返すポインタ(TRUE:エラー)</param>
/// <returns>文字/</returns>
char CMemory::SetAt(const int n, const char c, BOOL *pErr)
{
	if (n < 0 || n >= m_nSize) {
		*pErr = *pErr != NULL ? TRUE : NULL;
	} else {
		*pErr = *pErr != NULL ? FALSE : NULL;
		m_pBuf[n] = c;
	}

	return c;
}


//-------------------------------------------------------------------------------
// Size
//-------------------------------------------------------------------------------

/// <summary>保存しているデータ数を返す</summary>
/// <returns>保存しているデータ数</returns>
int CMemory::GetSize() const
{
	return m_nSize;
}


//-------------------------------------------------------------------------------
// いろいろ
//-------------------------------------------------------------------------------

/// <summary>
/// データを保持している？
/// </summary>
/// <returns>TRUE:保持して無い/FALSE:保持している</returns>
BOOL CMemory::IsEmpty() const
{
	return m_nSize == 0 ? TRUE : FALSE;
}


/// <summary>
/// メモリーを確保する
/// </summary>
/// <param name="n">確保する容量(Max-100MB)</param>
/// <returns>確保したメモリーのポインタ(失敗はNULLが返る)</returns>
char *CMemory::Reserve(const int n)
{
	return Init(n);
}



//-------------------------------------------------------------------------------
// operator
//-------------------------------------------------------------------------------

const char *CMemory::operator  = (const char *s)
{
	return Copy(s);
}


const char *CMemory::operator  = (const CMemory &s)
{
	return Copy(s);
}


const char *CMemory::operator += (const char *s)
{
	return Add(s);
}


const char *CMemory::operator += (const CMemory &s)
{
	return Add(s);
}


BOOL  CMemory::operator == (const char *s) const
{
	return strcmp(m_pBuf, s) == 0;
}


BOOL  CMemory::operator == (char * s)
{
	return strcmp(m_pBuf, s) == 0;
}


BOOL  CMemory::operator == (CMemory &s)
{
	if (s.GetSize() != m_nSize) { return FALSE; }	// 違う
	if (0           == m_nSize) { return TRUE;  }	// 同じ

	BOOL bVal       = TRUE;
	int  nSize      = m_nSize;
	const char *cp1 = m_pBuf;
	const char *cp2 = s;

	while (nSize-- && TRUE == bVal) {
		if (*cp1++ != *cp2++) {
			bVal = FALSE;
		}
	}

	return bVal;
}


BOOL  CMemory::operator != (const char *s) const
{
	return strcmp(m_pBuf, s) != 0;
}


BOOL  CMemory::operator != (char *s)
{
	return strcmp(m_pBuf, s) != 0;
}


BOOL  CMemory::operator != (CMemory &s)
{
	if (s.GetSize() != m_nSize) { return TRUE;  }	// 違う
	if (0           == m_nSize) { return FALSE; }	// 同じ

	BOOL bVal       = FALSE;
	int  nSize      = m_nSize;
	const char *cp1 = m_pBuf;
	const char *cp2 = s;

	while (nSize-- && FALSE == bVal) {
		if (*cp1++ != *cp2++) {
			bVal = TRUE;
		}
	}

	return bVal;
}


char  CMemory::operator[] (int n) const 
{
	if (n < 0 || n >= m_nSize) { return '\0'; }

	return m_pBuf[n];
}


char &CMemory::operator[] (int n)
{
	if (n < 0 || n >= m_nSize) { return (char &)""; }

	return m_pBuf[n];
}


CMemory::operator const char* () const
{
	return m_pBuf;
}


CMemory::operator char* ()
{
	return m_pBuf;
}


CMemory::operator const BYTE* () const
{
	return (BYTE *)m_pBuf;
}


CMemory::operator BYTE* ()
{
	return (BYTE *)m_pBuf;
}


//-------------------------------------------------------------------------------
// ワイド文字(Unicode)の処理
//-------------------------------------------------------------------------------

void CMemory::operator = (const WCHAR *s)
{
	UnicodeToShiftJis(s);
}


/// <summary>
/// UnicodeをShiftJisに変換する
/// </summary>
/// <param name="pszUnicode">変換元の文字列</param>
/// <param name="memShiftJis">変換後の文字列</param>
void CMemory::UnicodeToShiftJis(const WCHAR *pszUnicode)
{
	int nLenUnicode = (int)wcslen(pszUnicode);

	// 変換する文字列がない場合は処理しない
	if (nLenUnicode == 0) { return; }

	// ShiftJisに変換後のサイズを計算する
	int nLenShiftJis = WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// ShiftJisに変換する
	char *pBufShiftJis = new char[nLenShiftJis + 1];
	WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, pBufShiftJis, nLenShiftJis, NULL, NULL);
	*(pBufShiftJis + nLenShiftJis) = '\0';

	Add(pBufShiftJis);

	delete []pBufShiftJis;
}


/// <summary>
/// ShiftJisをUnicodeに変換する
/// </summary>
/// <param name="memShiftJis"></param>
/// <param name="str"></param>
//void CMemory::ShiftJisToUnicode(CMemory &memShiftJis, CString &str)
//{
//	int nLneShiftJis = memShiftJis.GetSize();
//
//	// 変換する文字列がない場合は処理しない
//	if (nLneShiftJis == 0) {
//		str.Empty();
//		return;
//	}
//
//	// Uniocdeに変換後のサイズを計算する
//	int nLenUnicode = MultiByteToWideChar(CP_ACP, 0, memShiftJis, nLneShiftJis, NULL, 0);
//
//	// Uniocdeに変換する
//	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
//	MultiByteToWideChar(CP_ACP, 0, memShiftJis, nLneShiftJis, pBufUnicode, nLenUnicode);
//	*(pBufUnicode + nLenUnicode) = '\0';
//
//	str = pBufUnicode;
//
//	delete []pBufUnicode;
//}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#ifdef _EXCLUSIVE_MEMORY_

/// <summary>コンストラクタ</summary>
CExclusiveMemory::CExclusiveMemory()
{
}

/// <summary>デストラクタ</summary>
CExclusiveMemory::~CExclusiveMemory()
{
}


/// <summary>
/// 記録領域の処理
/// </summary>
/// <param name="pMem">ワークエリア</param>
/// <param name="pNum">ワークサイズ</param>
/// <param name="nMode">実行する処理</param>
/// <returns>TRUEを返す</returns>
BOOL CExclusiveMemory::Analyze(CMemory *pMem, int *pNum, const ACTION_MODE enMode)
{
	Lock();
	{
		switch (enMode) {
		case _MEM_ADD:
			ASSERT(pMem != NULL);
			m_Memory.Add(*pMem, pMem->GetSize());

		case _MEM_REFER:
			ASSERT(pMem != NULL);
			pMem->Copy(m_Memory, m_Memory.GetSize());
			break;

		case _MEM_GET:
			ASSERT(pMem != NULL);
			pMem->Copy(m_Memory, m_Memory.GetSize());

		case _MEM_CLEAR:
			m_Memory.Clear();
			break;

		case _MEM_SIZE:
			ASSERT(pNum != NULL);
			*pNum = m_Memory.GetSize();
			break;

		case _MEM_COPY:
			ASSERT(pMem != NULL);
			m_Memory.Copy(*pMem, pMem->GetSize());
			break;

		default:
			break;
		}
	}
	Unlock();

	return TRUE;
}


//-------------------------------------------------------------------------------
// Clear
//-------------------------------------------------------------------------------

/// <summary>初期化</summary>
void CExclusiveMemory::Clear()
{
	Analyze(NULL, NULL, _MEM_CLEAR);
}


//-------------------------------------------------------------------------------
// add
//-------------------------------------------------------------------------------

/// <summary>
/// データの追加
/// </summary>
/// <param name="pMem">追加するデータ</param>
/// <returns>TRUE</returns>
CMemory CExclusiveMemory::Add(CMemory *pMem)
{
	Analyze(pMem, NULL, _MEM_ADD);

	return *pMem;
}

/// <summary>
/// データの追加
/// </summary>
/// <param name="pMem">追加するデータ</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Add(CExclusiveMemory *pMem)
{
	CMemory tmp(pMem->Get());

	Analyze(&tmp, NULL, _MEM_ADD);

	return tmp;
}

/// <summary>
/// 文字列の追加
/// </summary>
/// <param name="pstr">追加する文字列</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Add(const char *pstr)
{
	CMemory tmp(pstr);

	Analyze(&tmp, NULL, _MEM_ADD);

	return tmp;
}

/// <summary>
/// 文字列の追加
/// </summary>
/// <param name="pstr">追加する文字列</param>
/// <param name="nSize">文字列の長さ</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Add(const char *pstr, const int nSize)
{
	CMemory tmp(pstr, nSize);

	Analyze(&tmp, NULL, _MEM_ADD);

	return tmp;
}


//-------------------------------------------------------------------------------
// Copy
//-------------------------------------------------------------------------------

/// <summary>
/// データを保存する
/// </summary>
/// <param name="pMem">保存するデータ</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Copy(CMemory *pMem)
{
	Analyze(pMem, NULL, _MEM_COPY);

	return *pMem;
}

/// <summary>
/// データを保存する
/// </summary>
/// <param name="pMem">保存するデータ</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Copy(CExclusiveMemory *pMem)
{
	CMemory tmp(pMem->Get());

	Analyze(&tmp, NULL, _MEM_COPY);

	return tmp;
}

/// <summary>
/// 文字列を保存する
/// </summary>
/// <param name="pstr">保存する文字列</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Copy(const char *pstr)
{
	CMemory tmp(pstr);

	Analyze(&tmp, NULL, _MEM_COPY);

	return Analyze(&tmp, NULL, _MEM_COPY);
}

/// <summary>
/// 文字列を保存する
/// </summary>
/// <param name="pstr">保存する文字列</param>
/// <param name="nSize">文字列の長さ</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Copy(const char *pstr, const int nSize)
{
	CMemory tmp(pstr, nSize);

	Analyze(&tmp, NULL, _MEM_COPY);

	return tmp;
}


//-------------------------------------------------------------------------------
// Get
//-------------------------------------------------------------------------------

/// <summary>
/// 保存しているデータを取得する
/// </summary>
/// <param name="pMem">取得するデータを受け取る</param>
/// <param name="bDelete">保存しているデータを削除する(初期値:TRUE)</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Get(const BOOL bDelete)
{
	ACTION_MODE enMode = _MEM_GET;

	if (!bDelete) { enMode = _MEM_REFER; }

	CMemory tmp;

	Analyze(&tmp, NULL, enMode);

	return tmp;
}

/// <summary>
/// 保存しているデータのサイズを返す
/// </summary>
/// <returns>保存しているデータサイズ</returns>
int CExclusiveMemory::GetSize()
{
	int nNum = 0;

	Analyze(NULL, &nNum, _MEM_SIZE);

	return nNum;
}


//-------------------------------------------------------------------------------
// Read
//-------------------------------------------------------------------------------

/// <summary>
/// 保存しているデータの1バイト目を返す
/// </summary>
/// <returns>1バイト目</returns>
char CExclusiveMemory::Read()
{
	char c = '\0';
	CMemory mem;

	Analyze(&mem, NULL, _MEM_REFER);
	mem.Read(&c, 1);
	Analyze(&mem, NULL, _MEM_COPY);

	return c;
}


/// <summary>
/// 保存しているデータの指定サイズ分を返す
/// </summary>
/// <param name="nSize">サイズ</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Read(const int nSize)
{
	CMemory memTmp;
	CMemory memVal;

	Analyze(&memTmp, NULL, _MEM_REFER);
	memTmp.Read(memVal, nSize);
	Analyze(&memTmp, NULL, _MEM_COPY);

	return memVal;
}


//-------------------------------------------------------------------------------
// operator
//-------------------------------------------------------------------------------

CMemory CExclusiveMemory::operator  = (const char *s)
{
	return (*this) += s;
}


CMemory CExclusiveMemory::operator  = (CMemory s)
{
	return (*this) += s;
}

CMemory CExclusiveMemory::operator  += (const char *s)
{
	return Add(s);
}


CMemory CExclusiveMemory::operator  += (CMemory s)
{
	return Add(s);
}


CExclusiveMemory::operator CMemory ()
{
	return Get(FALSE);
}

#endif


/////////////////////////////////////////////////////////////////////////////////
// 文字列のテーブルを扱う
/////////////////////////////////////////////////////////////////////////////////

/// <summary>コンストラクタ</summary>
CStringTable::CStringTable()
{
	m_nIndex = -1;
}


/// <summary>コンストラクタ</summary>
CStringTable::CStringTable(CStringTable &p)
{
	(*this) = p;
}


/// <summary>コンストラクタ</summary>
CStringTable::CStringTable(const TCHAR *s)
{
	(*this) = s;
}


/// <summary>デストラクタ</summary>
CStringTable::~CStringTable()
{
	Clear();
}


/// <summary>初期化</summary>
void CStringTable::Clear()
{
	int nSize = GetSize();

	if (nSize) {
		for (int i = 0; i < nSize; i++)
		{
			TCHAR *p = (TCHAR*)m_arrItem.GetAt(0);
			delete []p;
			m_arrItem.RemoveAt(0);
		}
	}
	m_nIndex = -1;
}


/// <summary>初期化</summary>
void CStringTable::Init()
{
	Clear();
}


/// <summary>インデックスを先頭に移動</summary>
/// <returns>現在位置(失敗:-1)</returns>
int CStringTable::SetIndexTop()
{
	int n = GetSize();

	m_nIndex = n > 0 ? 0 : -1;

	return m_nIndex;
}


/// <summary>インデックスを指定位置に移動</summary>
/// <param name="nNum">指定位置</param>
/// <returns>現在位置(失敗:-1)</returns>
int CStringTable::SetIndexPos(const int nNum)
{
	int nSize = GetSize();

	if (nNum <      0) { return -1; }
	if (0    <= nSize) { return -1; }
	if (nNum >= nSize) { return -1; }

	m_nIndex = nNum;

	return m_nIndex;
}


/// <summary>インデックスを最後に移動</summary>
/// <returns>現在位置(失敗:-1)</returns>
int CStringTable::SetIndexEnd()
{
	int nVal  = -1;
	int nSize = GetSize();

	if (nSize > 0) {
		m_nIndex = nSize - 1;
		nVal     = m_nIndex;
	}

	return nVal;
}


/// <summary>現在のインデックス位置を返す</summary>
/// <returns>現在位置(データ無し:-1)</returns>
int CStringTable::GetIndex()
{
	int nSize = GetSize();

	if (nSize    <= 0)     { return -1; }
	if (m_nIndex == -1)    { m_nIndex = 0; }
	if (m_nIndex >= nSize) { m_nIndex = nSize - 1; }

	return m_nIndex;
}


/// <summary>保存しているデータ数を返す</summary>
/// <returns>保存しているデータ数</returns>
int CStringTable::GetSize() const
{
	return (int)m_arrItem.GetSize();
}


//-------------------------------------------------------------------------------
// operator
//-------------------------------------------------------------------------------

void CStringTable::operator  = (const TCHAR *pstr)
{
	Clear();

	(*this) += pstr;
}


void CStringTable::operator  = (CStringTable &p)
{
	Clear();

	(*this) += p;
}


void CStringTable::operator += (const TCHAR *pstr)
{
#ifdef _UNICODE
	int nLen = (int)wcslen(pstr) + 2;
	WCHAR *p = new WCHAR[nLen];

	wcscpy_s(p, nLen, pstr);
#else 
	int nLen = (int)strlen(pstr) + 2;
	char *p = new char[nLen];

	strcpy_s(p, nLen, pstr);
#endif

	m_arrItem.Add((void*)p);
}


void CStringTable::operator += (CStringTable &p)
{
	int nSize = p.GetSize();
	p.SetIndexTop();

	for (int i = 0; i < nSize; i++)
	{
		(*this) += (const TCHAR *)p;
	}
}


const TCHAR* CStringTable::operator[] (int nNum) const
{
	int nSize = GetSize();

	if (nNum < nSize && nNum >= 0) {
		return (const TCHAR *)m_arrItem.GetAt(nNum);
	}

	return NULL;
}


CStringTable::operator const TCHAR* ()
{
	int nSize = GetSize();

	if (nSize <= 0)     { return NULL;  }
	if (m_nIndex == -1) { m_nIndex = 0; }

	if (m_nIndex < nSize) {
		return (const TCHAR *)m_arrItem.GetAt(m_nIndex++);
	}

	return NULL;
}

