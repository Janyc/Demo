#pragma once

class CBufPool	//�̰߳�ȫ���ڴ����
{
public:
	struct BufNode
	{
		BOOL bUsed;
		PCHAR pBuffer;
	};
	struct BufAddress
	{
		union
		{
			DWORD dwAddress;
			PCHAR pAddress;
		}Address;
	};
    CBufPool(UINT _uiBufCount,UINT _uiBufSize);
    ~CBufPool(void);
    PCHAR GetFreeBuf();
    void ReleaseBuf(PCHAR pBuf);
    
private:
	void Terminate();
	void Init();
	LPVOID					m_pVirtualAddress;	//�����ڴ��ַ
	BufNode*				m_pBufNodeHead;		//��ǰ�����ڴ����
    UINT					m_uiBufCount;       //��������
	UINT					m_uiBufSize;		//���浥Ԫ��С
    CRITICAL_SECTION		m_criBufPool;       //�����ٽ���
	
};
