

//#include "stdafx.h"
#include "pch.h"

#include "ufile.h"

//////////////////////////////////////////////////////////////
// �G���[���b�Z�[�W�������I�ɉp��ɂ���(�����ύX)
// �gMsgEnglish(TRUE)�h
//////////////////////////////////////////////////////////////

//============================================================
// �G���[���b�Z�[�W�̐؂�ւ�
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
// �J�n�������I������
//============================================================

/// <summary>�R���X�g���N�^</summary>
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

/// <summary>�f�X�g���N�^</summary>
File::~File()
{
	if (m_bOpen) {
		Close();
	}
}


//============================================================
// �t�@�C���̃I�[�v��
//============================================================

/// <summary>
/// �t�@�C���I�[�v��
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <param name="nMode">�J�����[�h</param>
/// <param name="enCharCode">�L�����N�^�[���[�h</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
				SeekEndAction();	// �t�@�C���̍Ō�Ɉړ�
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
// �t�@�C���̃N���[�Y
//============================================================

/// <summary>
/// �t�@�C���̃N���[�Y
/// </summary>
/// <returns>TRUE:���s/FALSE:����</returns>
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
// �t�@�C������̓ǂݍ���
//============================================================

/// <summary>
/// 1�o�C�g�ǂݍ���
/// </summary>
/// <returns>����:0x00�`0xff/���s:_NO_DATA</returns>
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
/// n�o�C�g�ǂݍ��� 
/// </summary>
/// <param name="pBuf">�ǂݍ��ޏꏊ</param>
/// <param name="nLen">�ǂݍ��ރo�C�g��</param>
/// <returns>�ǂݍ��񂾃o�C�g��</returns>
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
/// �P�s�ǂݍ���(Unicode�ŕԂ��Ă���)
/// </summary>
/// <param name="str">CString��n��</param>
/// <returns>TRUE:����/FALSE:���s</returns>
BOOL File::Gets(CString &str)
{
	str.Empty();
	if (!m_bOpen) { return FALSE; }

	CMemory tmp;

	BOOL bRet = Gets(tmp);

	// �ǂݍ��߂���Unicode�ɕϊ�����
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
/// �P�s�ǂݍ���
/// </summary>
/// <param name="mem">CMemory��n��</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
			if (n == _NO_DATA) { // �Ō�܂œǂ񂾂�I���
				bRun = FALSE;
				break;
			} else if (n == 0) { // ������̓ǂݍ��݂Łu0�v�͂��肦�Ȃ�
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
/// �L�����N�^�[�R�[�h�̔��f���撣��
/// </summary>
/// <param name="memBuf">�ǂݍ��񂾕�����</param>
void File::AnalyzeCharCode(CMemory &memBuf)
{
	if (m_enCharCode != _PENDING) { return; }

	const BYTE *cp = (const BYTE *)memBuf;
	int nSize      = memBuf.GetSize();

	while (nSize-- && m_enCharCode == _PENDING) {
		// �P�o�C�g��
		if      (0x81 <= *cp && *cp <= 0x9f) { m_enCharCode = _SHIFT_JIS; }
		else if (0xc0 <= *cp && *cp <= 0xdf) { m_enCharCode = _UTF8;      }
		else if (0xf0 <= *cp && *cp <= 0xf7) { m_enCharCode = _UTF8;      }
		else if (0xa0 <= *cp && *cp <= 0xc1) { m_enCharCode = _SHIFT_JIS; }
		else if (0xe0 <= *cp && *cp <= 0xef) {
			// �Q�o�C�g��
			cp++;
			if (0x40 <= *cp && *cp <= 0x7e) { m_enCharCode = _SHIFT_JIS; }
			else if (0x80 <= *cp) {
				if (*cp <= 0xbf)  { // 0x80-0xbf
					// �R�o�C�g��
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
// �t�@�C���̏�������
//============================================================

/// <summary>
/// n�o�C�g��������
/// </summary>
/// <param name="pBuf">�������ރf�[�^</param>
/// <param name="nLen">�������ރo�C�g��</param>
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
/// '\0'�R�[�h�܂ŏ�������
/// </summary>
/// <param name="pszBuf">�������ރf�[�^</param>
void File::Write(const char *pszBuf)
{
	if (!m_bOpen) { return; }

	Write(pszBuf, (int)strlen(pszBuf));
}


/// <summary>
/// ���������������
/// </summary>
/// <param name="pszBuf">�������ޕ�����</param>
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
/// 1�s��������
/// </summary>
/// <param name="pszBuf">�������ޕ�����</param>
void File::Puts(const WCHAR *pszBuf)
{
	if (!m_bOpen) { return; }

	Write(pszBuf);
	Write("\r\n");
}


/// <summary>
/// �P������������
/// </summary>
/// <param name="c">�������ޕ���</param>
void File::Putc(const WCHAR c)
{
	if (!m_bOpen) { return; }

	WCHAR sz[5];

	swprintf_s(sz, L"%c", c);
	Write(sz);
}


//============================================================
// �t�@�C���̃|�C���g����
//============================================================

/// <summary>
/// �t�@�C���|�C���^���ړ�����
/// </summary>
/// <param name="lPos">�ړ�����ʒu</param>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^���ړ�����
/// </summary>
/// <param name="lPos">�ړ�����I�t�Z�b�g</param>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^��擪�Ɉړ�����
/// </summary>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^���Ō�Ɉړ�����
/// </summary>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
DWORD64 File::SeekEnd()
{
	if (!m_bOpen) { return - 1; }

	return SeekEndAction();
}


/// <summary>
/// �t�@�C���|�C���^���Ō�Ɉړ�����
/// </summary>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^���ʒu��Ԃ�
/// </summary>
/// <returns>�t�@�C���|�C���^�̈ʒu</returns>
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
// �t�@�C���̍폜
//============================================================

/// <summary>
/// �t�@�C���̍폜
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <param name="bCheck">���������H�m�F����</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL File::Unlink(const WCHAR *pszFileName, const BOOL bCheck)
{
	BOOL bErr = FALSE;

	try {
		if (IsExistFile(pszFileName)) {
			CFile::Remove(pszFileName);

			if (bCheck == TRUE) {
				// �ő��Q�b�܂ő҂�
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
/// �t�@�C���̍폜
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <param name="bCheck">���������H�m�F����</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL File::UnlinkFile(const WCHAR *pszFileName, const BOOL bCheck)
{
	return Unlink(pszFileName, bCheck);
}


/// <summary>
/// �w��t�H���_�ȉ����폜����
/// </summary>
/// <param name="pszPath">�t�H���_</param>
/// <returns>TRUE</returns>
BOOL File::UnlinkFolder(const WCHAR *pszPath)
{
	UnlinkFolderMain(pszPath);
	RemoveDirectoryW(pszPath);

	return TRUE;
}


/// <summary>
/// �w��t�H���_�ȉ����폜����(���s��)
/// </summary>
/// <param name="pszPath">�t�H���_</param>
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
// �X�e�C�^�X�֌W
//============================================================

/// <summary>
/// �t�@�C���̍X�V�b��Ԃ�
/// </summary>
/// <returns>�X�V�b</returns>
time_t File::GetStatusTimet()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_mtime.GetTime();
}

/// <summary>
/// �t�@�C���̍X�V�b��Ԃ�
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>�X�V�b</returns>
time_t File::GetStatusTimet(const WCHAR *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_mtime.GetTime();
}

/// <summary>
/// �t�@�C���T�C�Y��Ԃ�
/// </summary>
/// <returns>�t�@�C���T�C�Y</returns>
DWORD64 File::GetStatusSize()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_size;
}

/// <summary>
/// �t�@�C���T�C�Y��Ԃ�
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>�t�@�C���T�C�Y</returns>
DWORD64 File::GetStatusSize(const WCHAR *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_size;
}

/// <summary>
/// �t�@�C���̃A�g���r���[�g��Ԃ�
/// </summary>
/// <returns>�t�@�C���̃A�g���r���[�g</returns>
BYTE  File::GetStatusAttr()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return (BYTE)st.m_attribute; 
}

/// <summary>
/// �t�@�C���̃A�g���r���[�g��Ԃ�
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>�t�@�C���̃A�g���r���[�g</returns>
BYTE  File::GetStatusAttr(const WCHAR *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return (BYTE)st.m_attribute; 
}


//============================================================
// �t�@�C���̖��O�ύX
//============================================================

/// <summary>
/// �t�@�C�����̕ύX
/// </summary>
/// <param name="pszOldName">���t�@�C����</param>
/// <param name="pszNewName">�V�t�@�C����</param>
/// <returns>FALSE:����/TRUE:���s</returns>
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
// ��t�@�C���̍쐬
//============================================================

/// <summary>
/// ��t�@�C���̍쐬
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
/// �t�H���_�̍쐬
/// </summary>
/// <param name="pszPath">�t�H���_��</param>
/// <returns>TRUE:����/FALSE:���s(���Ƀt�H���_���L��)</returns>
BOOL File::CreateFolder(const WCHAR  *pszPath)
{
	return CreateDirectoryW(pszPath, NULL);
}



//=============================================================
// UTF-8�ϊ�
//=============================================================


/// <summary>
/// Unicode��UTF-8�ɕϊ�����
/// </summary>
/// <param name="pszUnicode">�ϊ����̕�����</param>
/// <param name="memUtf8">�ϊ���̕�����</param>
void File::UnicodeToUTF8(const WCHAR *pszUnicode, CMemory &memUtf8)
{
	int nLenUnicode = (int)wcslen(pszUnicode);

	// �ϊ����镶���񂪂Ȃ��ꍇ�͏������Ȃ�
	if (nLenUnicode == 0) {
		memUtf8.Clear();
		return;
	}

	// UTF-8�ɕϊ���̃T�C�Y���v�Z����
	int nLenUtf8 = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// UTF-8�ɕϊ�����
	char *pBufUTF8 = new char[nLenUtf8 + 1];
	WideCharToMultiByte(CP_UTF8, 0, pszUnicode, nLenUnicode, pBufUTF8, nLenUtf8, NULL, NULL);

	memUtf8.Copy(pBufUTF8, nLenUtf8);

	delete []pBufUTF8;
}


/// <summary>
/// UTF-8��Unicode�ɕϊ�����
/// </summary>
/// <param name="memUtf8">�ϊ����̕�����</param>
/// <param name="str">�ϊ���̕�����</param>
void File::UTF8ToUnicode(CMemory &memUtf8, CString &str)
{
	int nLenUtf8 = memUtf8.GetSize();

	// �ϊ����镶���񂪂Ȃ��ꍇ�͏������Ȃ�
	if (nLenUtf8 == 0) {
		str.Empty();
		return;
	}

	// Unicode�ɕϊ���̃T�C�Y���v�Z����
	int nLenUnicode = MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8 + 1, NULL, 0);

	// UniCode�ɕϊ�����
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8  + 1, pBufUnicode, nLenUnicode);

	str = pBufUnicode;

	delete []pBufUnicode;
}


/// <summary>
/// Unicode��ShiftJis�ɕϊ�����
/// </summary>
/// <param name="pszUnicode">�ϊ����̕�����</param>
/// <param name="memShiftJis">�ϊ���̕�����</param>
void File::UnicodeToShiftJis(const WCHAR *pszUnicode, CMemory &memShiftJis)
{
	int nLenUnicode = (int)wcslen(pszUnicode);

	// �ϊ����镶���񂪂Ȃ��ꍇ�͏������Ȃ�
	if (nLenUnicode == 0) {
		memShiftJis.Clear();
		return;
	}

	// ShiftJis�ɕϊ���̃T�C�Y���v�Z����
	int nLenShiftJis = WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// ShiftJis�ɕϊ�����
	char *pBufShiftJis = new char[nLenShiftJis + 1];
	WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, pBufShiftJis, nLenShiftJis, NULL, NULL);
	*(pBufShiftJis + nLenShiftJis) = '\0';

	memShiftJis = pBufShiftJis;

	delete []pBufShiftJis;
}


/// <summary>
/// ShiftJis��Unicode�ɕϊ�����
/// </summary>
/// <param name="memShiftJis"></param>
/// <param name="str"></param>
void File::ShiftJisToUnicode(CMemory &memShiftJis, CString &str)
{
	int nLneShiftJis = memShiftJis.GetSize();

	// �ϊ����镶���񂪂Ȃ��ꍇ�͏������Ȃ�
	if (nLneShiftJis == 0) {
		str.Empty();
		return;
	}

	// Uniocde�ɕϊ���̃T�C�Y���v�Z����
	int nLenUnicode = MultiByteToWideChar(CP_ACP, 0, memShiftJis, nLneShiftJis, NULL, 0);

	// Uniocde�ɕϊ�����
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_ACP, 0, memShiftJis, nLneShiftJis, pBufUnicode, nLenUnicode);
	*(pBufUnicode + nLenUnicode) = '\0';

	str = pBufUnicode;

	delete []pBufUnicode;
}



//============================================================
// ���̑�
//============================================================

/// <summary>
/// �t�@�C�������邩�H
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>TRUE:����/FALSE:�Ȃ�</returns>
BOOL File::IsExistFile(const WCHAR *pszFileName)
{
	BYTE bAttr = GetStatusAttr(pszFileName);

	if (bAttr == 0) { return FALSE; }

	return (bAttr & ~FILE_ATTRIBUTE_DIRECTORY) != 0 ? TRUE : FALSE;
}

/// <summary>
/// �t�H���_�����邩�H
/// </summary>
/// <param name="pszFilePath">�t�H���_��</param>
/// <returns>TRUE:����/FALSE:�Ȃ�</returns>
BOOL File::IsExistFolder(const WCHAR *pszFilePath)
{
	BYTE bAttr = GetStatusAttr(pszFilePath);

	if (bAttr == 0) { return FALSE; }

	return (bAttr & FILE_ATTRIBUTE_DIRECTORY) != 0 ? TRUE : FALSE;
}

/// <summary>
/// �Ō�̃G���[�R�[�h��Ԃ�
/// </summary>
/// <returns>�G���[�R�[�h</returns>
UINT File::GetErrorCode()
{
	return m_uErrCode;
}

/// <summary>
/// �Ō�̃G���[���e��Ԃ�
/// </summary>
/// <param name="pstr">�󂯎��ʒu</param>
/// <param name="nMaxLen">�󂯎��ő啶����</param>
/// <returns>�G���[�R�[�h</returns>
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
/// �Ō�̃G���[���e
/// </summary>
/// <returns>�G���[���e</returns>
const WCHAR *File::GetErrorString()
{
	m_pMsg = NULL;

	switch (GetErrorCode()) {
	case FILE_ERR_NONE:
		if (m_bEnglish) { m_pMsg = L"No error occurred."; }
		else            { m_pMsg = L"�G���[�͂���܂���"; }
		break;

	case FILE_ERR_GENERIC:
	default:
		if (m_bEnglish) { m_pMsg = L"An unspecified error occurred."; }
		else            { m_pMsg = L"�s���ȃG���[���������܂���"; }
		break;
		
	case FILE_ERR_FILE_NOT_FOUND:
		if (m_bEnglish) { m_pMsg = L"The file could not be located."; }
		else            { m_pMsg = L"�t�@�C��������܂���"; }
		break;
		
	case FILE_ERR_BAD_PATH:
		if (m_bEnglish) { m_pMsg = L"All or part of the path is invalid."; }
		else            { m_pMsg = L"�p�X���s���ł�"; }
		break;
		
	case FILE_ERR_TOO_MANY_OPEN_FILES:
		if (m_bEnglish) { m_pMsg = L"The permitted number of open files was exceeded."; }
		else            { m_pMsg = L"����ȏ�t�@�C�����I�[�v���ł��܂���"; }
		break;
		
	case FILE_ERR_ACCESS_DENIED:
		if (m_bEnglish) { m_pMsg = L"The file could not be accessed."; }
		else            { m_pMsg = L"�t�@�C���̃A�N�Z�X���֎~����Ă��܂�"; }
		break;
		
	case FILE_ERR_INVALID_FILE:
		if (m_bEnglish) { m_pMsg = L"There was an attempt to use an invalid file handle."; }
		else            { m_pMsg = L"�t�@�C���n���h�����s���ł�"; }
		break;
		
	case FILE_ERR_REMOVE_CURRENT_DIR:
		if (m_bEnglish) { m_pMsg = L"The current working directory cannot be removed."; }
		else            { m_pMsg = L"�J�����g�E�t�H���_�[�̍폜�͂ł��܂���"; }
		break;

	case FILE_ERR_DIRCTORY_FULL:
		if (m_bEnglish) { m_pMsg = L"There are no more directory entries."; }
		else            { m_pMsg = L"�t�H���_�[����t�ł�"; }
		break;
		
	case FILE_ERR_BAD_SEEK:
		if (m_bEnglish) { m_pMsg = L"There was an error trying to set the file pointer."; }
		else            { m_pMsg = L"�t�@�C���E�V�[�N�Ɏ��s���܂���"; }
		break;

	case FILE_ERR_HARD_IO:
		if (m_bEnglish) { m_pMsg = L"There was a hardware error."; }
		else            { m_pMsg = L"�n�[�h�E�F�A�E�G���[����"; }
		break;

	case FILE_ERR_SHRING_VIOLALATION:
		if (m_bEnglish) { m_pMsg = L"SHARE.EXE was not loaded, or a shared region was locked."; }
		else            { m_pMsg = L"���̃A�v���P�[�V�������t�@�C�������b�N���Ă��܂�"; }
		break;
		
	case FILE_ERR_LOCK_VIOLATION:
		if (m_bEnglish) { m_pMsg = L"There was an attempt to lock a region that was already locked."; }
		else            { m_pMsg = L"���b�N�ςݗ̈�̃��b�N�v��"; }
		break;

	case FILE_ERR_DISK_FULL:
		if (m_bEnglish) { m_pMsg = L"The disk is full."; }
		else            { m_pMsg = L"�f�B�X�N����t�ł�"; }
		break;

	case FILE_ERR_END_OF_FILE:
		if (m_bEnglish) { m_pMsg = L"The end of file was reached."; }
		else            { m_pMsg = L"END OF FILE"; }
		break;
	}

	return m_pMsg;
}

