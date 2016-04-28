
// VerificationDlg.h : ͷ�ļ�
//

#pragma once

enum FILE_TYPE { ORG = 0, REC = 1, RES = 2, PRE = 3, FILE_NUM = 4 };
enum COMPONENT { C0 = 0, C1 = 1, C2 = 2, C_NUM = 3 };

#define DISP_WIDTH 800
#define DISP_HEIGHT 600

// CVerificationDlg �Ի���
class CVerificationDlg : public CDialogEx
{
// ����
public:
	CVerificationDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERIFICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	// ���غ���
	void loadConfig();
	void loadOrgPic();
	void loadRecoPic();
	void loadResiPic();
	void loadPredInfo();

	// ��ʾͼƬ
	void setDispData(FILE_TYPE, COMPONENT);
	void showPicture();

protected:
	// �ļ���
	CString m_sFileName[FILE_NUM];

	int m_iWidth, m_iHeight;

	FILE_TYPE m_curType;
	int m_curCid;

	// ͼƬָ��
	CImage* m_pImage[FILE_NUM - 1][C_NUM+1];
	CImage* m_pDispBuffer;
	CImage* m_pNullDispBuffer;
	CImage* m_pFullBuffer;

	// �ɼ�����
	CRect m_visibleRect;

public:

	afx_msg void OnBnClickedButtonConfig();
	afx_msg void OnBnClickedButtonConfigbf();
	afx_msg void OnClose();
};
