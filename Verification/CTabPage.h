#pragma once


// CCTabPage �Ի���

class CCTabPage : public CDialogEx
{
	DECLARE_DYNAMIC(CCTabPage)

public:
	CCTabPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCTabPage();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
