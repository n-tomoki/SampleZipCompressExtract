#pragma once

//////////////////////////////////////////////////////////////
// 排他的メモリーを使用する場合は以下の定義を“stdafx.h/pch.h”に明記すること
// #ifndef _EXCLUSIVE_MEMORY_
// #define _EXCLUSIVE_MEMORY_
// #endif
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
#define MEMORY_SIZE_MAX (1024*1024*100)	// 100MB
#define MEMORY_BLOCK_SIZE 10240
#define CAL_BLOCK(n) (((n / MEMORY_BLOCK_SIZE) + 1) * MEMORY_BLOCK_SIZE)

#ifndef _NO_DATA
#define _NO_DATA 0x100
#endif

#ifndef C_LF
#define C_LF  0x0a
#endif

#ifndef C_CR
#define C_CR  0x0d
#endif

#ifndef C_EOF
#define C_EOF 0x1a
#endif

#ifndef FILE_GETS_BUFFER_SIZE
#define FILE_GETS_BUFFER_SIZE 512
#endif


class CMemory
{
private:
	int   m_nPos;
	int   m_nSize;
	int   m_nSizeMax;
	char *m_pBuf;

private:
	char *Init(const int nSize);

public:
	CMemory();
	CMemory(const int nSize);
	CMemory(const char *str);
	CMemory(const char *str, const int nSize);
	CMemory(const CMemory &s);
	virtual ~CMemory();

	void Clear();
	int  Read();
	int  Read(char *s, const int nSize);
	int  Read(CMemory &s, const int nSize);
	int  Write(const char *s);
	int  Write(const char *s, const int nSize);
	int  Puts(const char *s);
	BOOL Gets(char *s, const int nSize);
	BOOL Gets(CString &str);
	BOOL Gets(CMemory &s);
	int  GetSize() const;

	BOOL  IsEmpty() const;
	char *Reserve(const int n);

	char GetAt(const int n,               BOOL *pErr = NULL);
	char SetAt(const int n, const char c, BOOL *pErr = NULL);

	const char *Copy(const char *s);
	const char *Copy(const char *s, const int nSize);
	const char *Copy(const CMemory &s);
	const char *Add (const char *s);
	void        AddW(const WCHAR *s);
	const char *Add (const char *s, const int nSize);
	const char *Add (const CMemory &s);

	const char *operator  = (const char *s);
	const char *operator  = (const CMemory &s);
	void        operator  = (const WCHAR *s);
	const char *operator += (const char *s);
	const char *operator += (const CMemory &s);
	BOOL        operator == (const char *s) const;
	BOOL        operator == (char *s);
	BOOL        operator == (CMemory &s);
	BOOL        operator != (const char *s) const;
	BOOL        operator != (char *s);
	BOOL        operator != (CMemory &s);
	char        operator[] (int n) const;
	char&       operator[] (int n);

	operator const char* () const;
	operator       char* ();

	operator const BYTE* () const;
	operator       BYTE* ();

	// [m_nPos]を変更する
	int SeekTop();
	int SeekSet(const int nPos);
	int SeekOff(const int nOff);
	int SeekGet() const;

private:
	void UnicodeToShiftJis(const WCHAR *pszUnicode);
//	void ShiftJisToUnicode(CMemory &memShiftJis, CString &str);
};

#ifdef _EXCLUSIVE_MEMORY_

#include <afxmt.h>

class CExclusiveMemory {
	CMemory m_Memory;
	enum ACTION_MODE { _MEM_ADD = 0, _MEM_GET, _MEM_SIZE, _MEM_COPY, _MEM_CLEAR, _MEM_REFER };

	CCriticalSection m_rSemaphore;
	inline void Lock()   { m_rSemaphore.Lock();   }
	inline void Unlock() { m_rSemaphore.Unlock(); }

	BOOL Analyze(CMemory *pMem, int *pNum, const ACTION_MODE enMode);

public:
	CExclusiveMemory();
	virtual ~CExclusiveMemory();

	void Clear();
	// Add
	CMemory Add(CMemory *pMem);
	CMemory Add(CExclusiveMemory *pMem);
	CMemory Add(const char *pstr);
	CMemory Add(const char *pstr, const int nSize);
	// Copy
	CMemory Copy(CMemory *pMem);
	CMemory Copy(CExclusiveMemory *pMem);
	CMemory Copy(const char *pstr);
	CMemory Copy(const char *pstr, const int nSize);
	// Get
	CMemory Get(const BOOL bDelete = TRUE);
	int     GetSize();
	// Read
	char    Read();
	CMemory Read(const int nSize);
	// operator
	CMemory operator  = (const char *s);
	CMemory operator  = (CMemory s);
	CMemory operator += (const char *s);
	CMemory operator += (CMemory s);
	operator CMemory ();
};

#endif


//////////////////////////////////////////////////////////////
// 文字列のテーブルを扱う

class CStringTable
{
public:
	CStringTable();
	CStringTable(CStringTable &p);
	CStringTable(const TCHAR *s);
	virtual ~CStringTable();

private:
	int       m_nIndex;
	CPtrArray m_arrItem;

public:
	void Clear();
	void Init();

	int  SetIndexTop();
	int  SetIndexPos(const int nNum);
	int  SetIndexEnd();

	int  GetIndex();
	int  GetSize() const;

	void operator  = (const TCHAR *pstr);
	void operator  = (CStringTable &p);
	void operator += (const TCHAR *pstr);
	void operator += (CStringTable &p);
	const TCHAR* operator[] (int n) const;
	operator const TCHAR* ();
};



