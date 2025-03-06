

#pragma once

#include "Memory.h"

//////////////////////////////////////////////////////////////
// エラーメッセージを強制的に英語にする(自動変更)
// “MsgEnglish(TRUE)”
//////////////////////////////////////////////////////////////

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


#define FILE_ACCESS_LOCK 0x100

#define FILE_ERR_NONE                CFileException::none               //  0:エラーなし
#define FILE_ERR_GENERIC             CFileException::genericException   //  1:未定義エラー
#define FILE_ERR_FILE_NOT_FOUND      CFileException::fileNotFound       //  2:ファイル未定義
#define FILE_ERR_BAD_PATH            CFileException::badPath            //  3:不正パス
#define FILE_ERR_TOO_MANY_OPEN_FILES CFileException::tooManyOpenFiles   //  4:オープン ファイル数超過
#define FILE_ERR_ACCESS_DENIED       CFileException::accessDenied       //  5:ファイル アクセス禁止
#define FILE_ERR_INVALID_FILE        CFileException::invalidFile        //  6:不正ファイル ハンドル使用
#define FILE_ERR_REMOVE_CURRENT_DIR  CFileException::removeCurrentDir   //  7:作業中のカレント ディレクトリの削除不可
#define FILE_ERR_DIRCTORY_FULL       CFileException::directoryFull      //  8:ディレクトリ フル
#define FILE_ERR_BAD_SEEK            CFileException::badSeek            //  9:ファイル ポインタ設定エラー
#define FILE_ERR_HARD_IO             CFileException::hardIO             // 10:ハードウェア エラー
#define FILE_ERR_SHRING_VIOLALATION  CFileException::sharingViolation   // 11:SHARE.EXE 未ロードまたは共有領域ロック済み
#define FILE_ERR_LOCK_VIOLATION      CFileException::lockViolation      // 12:ロック済み領域のロック要求
#define FILE_ERR_DISK_FULL           CFileException::diskFull           // 13:ディスク フル
#define FILE_ERR_END_OF_FILE         CFileException::endOfFile          // 14:ファイルの終わりに到達 

#ifndef FILE_GETS_BUFFER_SIZE
#define FILE_GETS_BUFFER_SIZE 512
#endif

class File {
protected:
	static BOOL m_bFirst;
	static BOOL m_bEnglish;

	BOOL m_bOpen;
	UINT m_uErrCode;
	CFile m_File;
	const WCHAR *m_pMsg;

	// キャラクターモード
public:
	enum CharMode {_PENDING = 0, _SHIFT_JIS, _UTF8};
private:
	CharMode m_enCharCode;


public:
	enum OpenMode {_READ      = 1                   , _WRITE     , _APPEND     , _RDWR,
	               _READ_LOCK = FILE_ACCESS_LOCK + 1, _WRITE_LOCK, _APPEND_LOCK, _RDWR_LOCK};

	File();
	virtual ~File();

	BOOL Open(const WCHAR *pszFileName, const OpenMode enMode, const CharMode enCharCode = _PENDING);
	BOOL Close();

private:
	unsigned int Read();
public:
	int  Read(void *pBuf, const int nLen);
	BOOL Gets(CString &str);
	BOOL Gets(CMemory &mem);

	void Write(const char *pszBuf);
	void Write(const char *pszBuf, const int nLen);
	void Write(const WCHAR *pszBuf);
	void Puts(const WCHAR *pszBuf);
	void Putc(const WCHAR c);

	// 変換
private:
	void UnicodeToUTF8(const WCHAR *pszUnicode, CMemory &memUtf8);
	void UTF8ToUnicode(CMemory &memUtf8, CString &str);
	
	void UnicodeToShiftJis(const WCHAR *pszUnicode, CMemory &memShiftJis);
	void ShiftJisToUnicode(CMemory &memShiftJis, CString &str);
	// キャラクターコードの判断	
	void AnalyzeCharCode(CMemory &memBuf);

public:
	HANDLE GetHandle() { return m_File.m_hFile; }

private:
	DWORD64 SeekEndAction();
public:
	DWORD64 SeekEnd();
	DWORD64 SeekSet(const DWORD64 lPos);
	DWORD64 SeekOff(const DWORD64 lPos);
	DWORD64 SeekTop();
	DWORD64 GetPosition();

	time_t  GetStatusTimet();
	DWORD64 GetStatusSize();
	BYTE    GetStatusAttr();

	UINT         GetErrorCode();
	UINT         GetErrorString(WCHAR *pstr, const int nLen);
	const WCHAR *GetErrorString();

	static time_t  GetStatusTimet(const WCHAR *pszFileName);
	static DWORD64 GetStatusSize(const WCHAR *pszFileName);
	static BYTE    GetStatusAttr(const WCHAR *pszFileName);

	static BOOL IsExistFile(const WCHAR *pszFileName);
	static BOOL IsExistFolder(const WCHAR *pszFilePath);
	static BOOL Rename(const WCHAR *pszOldName, const WCHAR *pszNewName);
	static BOOL CreateEmptyFile(const WCHAR *pszFileName);
	static BOOL CreateFolder(const WCHAR *pszPath);


	static BOOL Unlink(const WCHAR *pszFileName, const BOOL bCheck = FALSE);
	static BOOL UnlinkFile(const WCHAR *pszFileName, const BOOL bCheck = FALSE);
	static BOOL UnlinkFolder(const WCHAR *pszPath);
protected:
	static BOOL UnlinkFolderMain(const WCHAR *pszPath);

public:
	// エラー表示の切り替え
	static BOOL MsgEnglish(const BOOL bEnglish);
	static BOOL MsgEnglish();
};

