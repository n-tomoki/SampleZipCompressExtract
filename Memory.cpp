

#include "pch.h"
//#include "StdAfx.h"
#include "Memory.h"


/// <summary>�R���X�g���N�^</summary>
CMemory::CMemory()
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;
}


/// <summary>�R���X�g���N�^</summary>
/// <param name="nSize">�m�ۂ���e��</param>
CMemory::CMemory(const int nSize)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Init(nSize);
}


/// <summary>�R���X�g���N�^</summary>
/// <param name="str">������</param>
CMemory::CMemory(const char *str)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Copy(str);
}


/// <summary>�R���X�g���N�^</summary>
/// <param name="str">������</param>
/// <param name="nSize">�f�[�^��</param>
CMemory::CMemory(const char *str, const int nSize)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Copy(str, nSize);
}


/// <summary>�R���X�g���N�^</summary>
/// <param name="s">�����l</param>
CMemory::CMemory(const CMemory &s)
{
	m_pBuf     = NULL;
	m_nPos     = 0;
	m_nSize    = 0;
	m_nSizeMax = 0;

	Copy(s);
}


/// <summary>�f�X�g���N�^</summary>
CMemory::~CMemory()
{
	if (m_pBuf != NULL) { delete []m_pBuf; m_pBuf = NULL; }
}


//-------------------------------------------------------------------------------
// ������
//-------------------------------------------------------------------------------

/// <summary>������</summary>
void  CMemory::Clear()
{
	if (m_pBuf != NULL) { *m_pBuf = '\0'; }

	m_nSize = 0;
	m_nPos  = 0;
}


/// <summary>�������[�m��</summary>
/// <param name="nSize">�m�ۂ���e��</param>
/// <returns>NULL:�m�ێ��s/NULL�ȊO:�m�ۂ����擪�̃|�C���^</returns>
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

/// <summary>�������ۑ�����</summary>
/// <param name="s">������</param>
/// <returns>NULL:�ۑ����s/NULL�ȊO:�ۑ������擪�̃|�C���^</returns>
const char *CMemory::Copy(const char *s)
{
	if (s  == NULL) { return NULL; }
	if (*s == '\0') { return NULL; }

	return Copy(s, (int)strlen(s));
}


/// <summary>�f�[�^��ۑ�����</summary>
/// <param name="s">�f�[�^�̐擪�|�C���^</param>
/// <param name="nSize">�f�[�^��</param>
/// <returns>NULL:�ۑ����s/NULL�ȊO:�ۑ������擪�̃|�C���^</returns>
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


/// <summary>�f�[�^��ۑ�����</summary>
/// <param name="s">CMemory�N���X</param>
/// <returns>NULL:�ۑ����s/NULL�ȊO:�ۑ������擪�̃|�C���^</returns>
const char *CMemory::Copy(const CMemory &s)
{
	return Copy(s, s.GetSize());
}


//-------------------------------------------------------------------------------
// Add
//-------------------------------------------------------------------------------

/// <summary>�������ǉ�����</summary>
/// <param name="s">������</param>
/// <returns>NULL:�ǉ����s/NULL�ȊO:�ǉ����ꂽ�擪�̃|�C���^</returns>
const char *CMemory::Add(const char *s)
{
	if (s  == NULL) { return NULL; }
	if (*s == '\0') { return NULL; }

	return Add(s, (int)strlen(s));
}


/// <summary>�f�[�^��ǉ�����</summary>
/// <param name="s">�f�[�^</param>
/// <param name="nSize">�f�[�^��</param>
/// <returns>NULL:�ǉ����s/NULL�ȊO:�ǉ����ꂽ�擪�̃|�C���^</returns>
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


/// <summary>�f�[�^��ǉ�����</summary>
/// <param name="s">CMemory�N���X</param>
/// <returns>NULL:�ǉ����s/NULL�ȊO:�ǉ����ꂽ�擪�̃|�C���^</returns>
const char *CMemory::Add(const CMemory &s)
{
	return Add(s, s.GetSize());
}


/// <summary>�f�[�^�̒ǉ�����</summary>
/// <param name="s">�ǉ����镶����</param>
void CMemory::AddW(const WCHAR *s)
{
	UnicodeToShiftJis(s);
}


//-------------------------------------------------------------------------------
// Read
//-------------------------------------------------------------------------------

/// <summary>
/// �ۑ��f�[�^�̓ǂݍ���
/// </summary>
/// <param name="s">�ǂݍ��ރ|�C���^</param>
/// <param name="nSize">�ǂݍ��ݐ�</param>
/// <returns>�ǂݍ��񂾐�</returns>
int CMemory::Read(char *s, const int nSize)
{
	// �r���܂œǂݍ���ł����ꍇ�́A�ǂݍ��ݍς݂̃f�[�^��ǂݔ�΂�
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
/// �ۑ��f�[�^�̓ǂݍ���
/// </summary>
/// <param name="s">CMemory</param>
/// <param name="nSize">�ǂݍ��ݐ�</param>
/// <returns>�ǂݍ��񂾐�</returns>
int CMemory::Read(CMemory &s, const int nSize)
{
	// �r���܂œǂݍ���ł����ꍇ�́A�ǂݍ��ݍς݂̃f�[�^��ǂݔ�΂�
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
/// �ۑ��f�[�^�̓ǂݍ���
/// </summary>
/// <returns>�����ꍇ��[_NO_DATA]</returns>
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
/// �f�[�^��ǉ�����
/// </summary>
/// <param name="s">�f�[�^</param>
/// <returns>�f�[�^��</returns>
int CMemory::Write(const char *s)
{
	if (s  == NULL) { return 0; }
	if (*s == '\0') { return 0; }

	int n = (int)strlen(s);

	Add(s, n);

	return n;
}


/// <summary>
/// �f�[�^��ǉ�����
/// </summary>
/// <param name="s">�f�[�^</param>
/// <param name="nSize">�f�[�^��</param>
/// <returns>�f�[�^��</returns>
int CMemory::Write(const char *s, const int nSize)
{
	if (s == NULL  ) { return 0; }
	if (s == m_pBuf) { return 0; }
	if (0 >= nSize ) { return 0; }

	Add(s, nSize);

	return nSize;
}


/// <summary>
/// �f�[�^��ǉ�����
/// </summary>
/// <param name="s">�f�[�^</param>
/// <returns>�f�[�^��(\r\n���܂�)</returns>
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
/// 1�s�ǂݍ���
/// </summary>
/// <param name="pBuf">�ǂݍ��ޏꏊ</param>
/// <param name="nMax">�ǂݍ��ލő�o�C�g��</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
/// 1�s�ǂݍ���
/// </summary>
/// <param name="str">CString��n��</param>
/// <returns>TRUE:����/FALSE:���s</returns>
BOOL CMemory::Gets(CString &str)
{
	CMemory s;

	BOOL bRet = Gets(s);

	str = (const char *)s;

	return bRet;
}



/// <summary>
/// 1�s�ǂݍ���
/// </summary>
/// <param name="str">CMemory��n��</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
// [m_nPos]��ύX����
//-------------------------------------------------------------------------------

/// <summary>
/// �ǂݍ��݈ʒu��擪�ɂ���
/// </summary>
/// <returns>�ǂݍ��݈ʒu</returns>
int CMemory::SeekTop()
{
	m_nPos = 0;

	return m_nPos;
}


/// <summary>
/// �ǂݍ��݈ʒu��ݒ肷��
/// </summary>
/// <param name="nPos">�ǂݍ��݈ʒu</param>
/// <returns>�ǂݍ��݈ʒu</returns>
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
/// �ǂݍ��݈ʒu���ړ�����
/// </summary>
/// <param name="nOff">�I�t�Z�b�g</param>
/// <returns>�ǂݍ��݈ʒu</returns>
int CMemory::SeekOff(const int nOff)
{
	return SeekSet(m_nPos + nOff);
}


/// <summary>
/// ���݂̓ǂݍ��݈ʒu��Ԃ�
/// </summary>
/// <returns>�ǂݍ��݈ʒu</returns>
int CMemory::SeekGet() const
{
	return m_nPos;
}


//-------------------------------------------------------------------------------
// �ۑ����Ă��镶���ւ̃A�N�Z�X
//-------------------------------------------------------------------------------

/// <summary>
/// �w��ʒu�̕�����Ԃ�
/// </summary>
/// <param name="n">�ʒu</param>
/// <param name="pErr">�G���[���f��Ԃ��|�C���^(TRUE:�G���[)</param>
/// <returns>�w��ʒu�̕���</returns>
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
/// �w��ʒu�̕�����u��������
/// </summary>
/// <param name="n">�ʒu</param>
/// <param name="c">����</param>
/// <param name="pErr">�G���[���f��Ԃ��|�C���^(TRUE:�G���[)</param>
/// <returns>����/</returns>
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

/// <summary>�ۑ����Ă���f�[�^����Ԃ�</summary>
/// <returns>�ۑ����Ă���f�[�^��</returns>
int CMemory::GetSize() const
{
	return m_nSize;
}


//-------------------------------------------------------------------------------
// ���낢��
//-------------------------------------------------------------------------------

/// <summary>
/// �f�[�^��ێ����Ă���H
/// </summary>
/// <returns>TRUE:�ێ����Ė���/FALSE:�ێ����Ă���</returns>
BOOL CMemory::IsEmpty() const
{
	return m_nSize == 0 ? TRUE : FALSE;
}


/// <summary>
/// �������[���m�ۂ���
/// </summary>
/// <param name="n">�m�ۂ���e��(Max-100MB)</param>
/// <returns>�m�ۂ����������[�̃|�C���^(���s��NULL���Ԃ�)</returns>
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
	if (s.GetSize() != m_nSize) { return FALSE; }	// �Ⴄ
	if (0           == m_nSize) { return TRUE;  }	// ����

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
	if (s.GetSize() != m_nSize) { return TRUE;  }	// �Ⴄ
	if (0           == m_nSize) { return FALSE; }	// ����

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
// ���C�h����(Unicode)�̏���
//-------------------------------------------------------------------------------

void CMemory::operator = (const WCHAR *s)
{
	UnicodeToShiftJis(s);
}


/// <summary>
/// Unicode��ShiftJis�ɕϊ�����
/// </summary>
/// <param name="pszUnicode">�ϊ����̕�����</param>
/// <param name="memShiftJis">�ϊ���̕�����</param>
void CMemory::UnicodeToShiftJis(const WCHAR *pszUnicode)
{
	int nLenUnicode = (int)wcslen(pszUnicode);

	// �ϊ����镶���񂪂Ȃ��ꍇ�͏������Ȃ�
	if (nLenUnicode == 0) { return; }

	// ShiftJis�ɕϊ���̃T�C�Y���v�Z����
	int nLenShiftJis = WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// ShiftJis�ɕϊ�����
	char *pBufShiftJis = new char[nLenShiftJis + 1];
	WideCharToMultiByte(CP_ACP, 0, pszUnicode, nLenUnicode, pBufShiftJis, nLenShiftJis, NULL, NULL);
	*(pBufShiftJis + nLenShiftJis) = '\0';

	Add(pBufShiftJis);

	delete []pBufShiftJis;
}


/// <summary>
/// ShiftJis��Unicode�ɕϊ�����
/// </summary>
/// <param name="memShiftJis"></param>
/// <param name="str"></param>
//void CMemory::ShiftJisToUnicode(CMemory &memShiftJis, CString &str)
//{
//	int nLneShiftJis = memShiftJis.GetSize();
//
//	// �ϊ����镶���񂪂Ȃ��ꍇ�͏������Ȃ�
//	if (nLneShiftJis == 0) {
//		str.Empty();
//		return;
//	}
//
//	// Uniocde�ɕϊ���̃T�C�Y���v�Z����
//	int nLenUnicode = MultiByteToWideChar(CP_ACP, 0, memShiftJis, nLneShiftJis, NULL, 0);
//
//	// Uniocde�ɕϊ�����
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

/// <summary>�R���X�g���N�^</summary>
CExclusiveMemory::CExclusiveMemory()
{
}

/// <summary>�f�X�g���N�^</summary>
CExclusiveMemory::~CExclusiveMemory()
{
}


/// <summary>
/// �L�^�̈�̏���
/// </summary>
/// <param name="pMem">���[�N�G���A</param>
/// <param name="pNum">���[�N�T�C�Y</param>
/// <param name="nMode">���s���鏈��</param>
/// <returns>TRUE��Ԃ�</returns>
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

/// <summary>������</summary>
void CExclusiveMemory::Clear()
{
	Analyze(NULL, NULL, _MEM_CLEAR);
}


//-------------------------------------------------------------------------------
// add
//-------------------------------------------------------------------------------

/// <summary>
/// �f�[�^�̒ǉ�
/// </summary>
/// <param name="pMem">�ǉ�����f�[�^</param>
/// <returns>TRUE</returns>
CMemory CExclusiveMemory::Add(CMemory *pMem)
{
	Analyze(pMem, NULL, _MEM_ADD);

	return *pMem;
}

/// <summary>
/// �f�[�^�̒ǉ�
/// </summary>
/// <param name="pMem">�ǉ�����f�[�^</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Add(CExclusiveMemory *pMem)
{
	CMemory tmp(pMem->Get());

	Analyze(&tmp, NULL, _MEM_ADD);

	return tmp;
}

/// <summary>
/// ������̒ǉ�
/// </summary>
/// <param name="pstr">�ǉ����镶����</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Add(const char *pstr)
{
	CMemory tmp(pstr);

	Analyze(&tmp, NULL, _MEM_ADD);

	return tmp;
}

/// <summary>
/// ������̒ǉ�
/// </summary>
/// <param name="pstr">�ǉ����镶����</param>
/// <param name="nSize">������̒���</param>
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
/// �f�[�^��ۑ�����
/// </summary>
/// <param name="pMem">�ۑ�����f�[�^</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Copy(CMemory *pMem)
{
	Analyze(pMem, NULL, _MEM_COPY);

	return *pMem;
}

/// <summary>
/// �f�[�^��ۑ�����
/// </summary>
/// <param name="pMem">�ۑ�����f�[�^</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Copy(CExclusiveMemory *pMem)
{
	CMemory tmp(pMem->Get());

	Analyze(&tmp, NULL, _MEM_COPY);

	return tmp;
}

/// <summary>
/// �������ۑ�����
/// </summary>
/// <param name="pstr">�ۑ����镶����</param>
/// <returns>CMemory</returns>
CMemory CExclusiveMemory::Copy(const char *pstr)
{
	CMemory tmp(pstr);

	Analyze(&tmp, NULL, _MEM_COPY);

	return Analyze(&tmp, NULL, _MEM_COPY);
}

/// <summary>
/// �������ۑ�����
/// </summary>
/// <param name="pstr">�ۑ����镶����</param>
/// <param name="nSize">������̒���</param>
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
/// �ۑ����Ă���f�[�^���擾����
/// </summary>
/// <param name="pMem">�擾����f�[�^���󂯎��</param>
/// <param name="bDelete">�ۑ����Ă���f�[�^���폜����(�����l:TRUE)</param>
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
/// �ۑ����Ă���f�[�^�̃T�C�Y��Ԃ�
/// </summary>
/// <returns>�ۑ����Ă���f�[�^�T�C�Y</returns>
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
/// �ۑ����Ă���f�[�^��1�o�C�g�ڂ�Ԃ�
/// </summary>
/// <returns>1�o�C�g��</returns>
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
/// �ۑ����Ă���f�[�^�̎w��T�C�Y����Ԃ�
/// </summary>
/// <param name="nSize">�T�C�Y</param>
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
// ������̃e�[�u��������
/////////////////////////////////////////////////////////////////////////////////

/// <summary>�R���X�g���N�^</summary>
CStringTable::CStringTable()
{
	m_nIndex = -1;
}


/// <summary>�R���X�g���N�^</summary>
CStringTable::CStringTable(CStringTable &p)
{
	(*this) = p;
}


/// <summary>�R���X�g���N�^</summary>
CStringTable::CStringTable(const TCHAR *s)
{
	(*this) = s;
}


/// <summary>�f�X�g���N�^</summary>
CStringTable::~CStringTable()
{
	Clear();
}


/// <summary>������</summary>
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


/// <summary>������</summary>
void CStringTable::Init()
{
	Clear();
}


/// <summary>�C���f�b�N�X��擪�Ɉړ�</summary>
/// <returns>���݈ʒu(���s:-1)</returns>
int CStringTable::SetIndexTop()
{
	int n = GetSize();

	m_nIndex = n > 0 ? 0 : -1;

	return m_nIndex;
}


/// <summary>�C���f�b�N�X���w��ʒu�Ɉړ�</summary>
/// <param name="nNum">�w��ʒu</param>
/// <returns>���݈ʒu(���s:-1)</returns>
int CStringTable::SetIndexPos(const int nNum)
{
	int nSize = GetSize();

	if (nNum <      0) { return -1; }
	if (0    <= nSize) { return -1; }
	if (nNum >= nSize) { return -1; }

	m_nIndex = nNum;

	return m_nIndex;
}


/// <summary>�C���f�b�N�X���Ō�Ɉړ�</summary>
/// <returns>���݈ʒu(���s:-1)</returns>
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


/// <summary>���݂̃C���f�b�N�X�ʒu��Ԃ�</summary>
/// <returns>���݈ʒu(�f�[�^����:-1)</returns>
int CStringTable::GetIndex()
{
	int nSize = GetSize();

	if (nSize    <= 0)     { return -1; }
	if (m_nIndex == -1)    { m_nIndex = 0; }
	if (m_nIndex >= nSize) { m_nIndex = nSize - 1; }

	return m_nIndex;
}


/// <summary>�ۑ����Ă���f�[�^����Ԃ�</summary>
/// <returns>�ۑ����Ă���f�[�^��</returns>
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

