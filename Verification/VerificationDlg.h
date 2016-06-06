
// VerificationDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <vector>
#include <utility>
#include <unordered_map>
#include <map>
#include "CTabPage.h"

enum FILE_TYPE { ORG = 0, REC = 1, RES = 2, PRE = 3, FILE_NUM = 4 };
enum COMPONENT { C0 = 0, C1 = 1, C2 = 2, C_NUM = 3 };

#define DISP_WIDTH 1280
#define DISP_HEIGHT 720

struct PredInfo 
{
	std::vector<int> m_vTemplate[C_NUM];
	std::vector<int> m_vPTemplate[C_NUM];
	std::pair<int, int> m_pPredFrom[C_NUM];
	int m_predValue[C_NUM];
	int m_resiValue[C_NUM];
};

// CVerificationDlg �Ի���
class CVerificationDlg : public CDialogEx
{
// ����
public:
	CVerificationDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CVerificationDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERIFICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	BOOL PreTranslateMessage(MSG*   pMsg);
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
	void SetDispData(FILE_TYPE, COMPONENT);
	void SelectPixel(int, int);
	void ShowPicture();
	void ZoomIn();
	void ZoomOut();


	void initCoordinateMap(int uiSourceWidth, int uiSourceHeight);
	void loadBlock(int number);
	PredInfo getPredInfo(int x, int y);

	//

protected:
	// �ļ���
	CString m_sFileName[FILE_NUM];

	int m_iWidth, m_iHeight;

	FILE_TYPE m_curType;
	int m_curCid;

	// predinfo
	std::vector<std::vector<PredInfo>> m_aPredInfo;
	std::map<std::pair<int, int>, PredInfo> m_mapPredInfo;

	std::vector<int> g_auiOrgToRsmpld[2];
	std::vector<int> g_auiRsmpldToOrg[2];
	// ͼƬָ��
	CImage m_Image[FILE_NUM - 1][C_NUM+1];
	CImage m_DispBuffer;
	CImage m_NullDispBuffer;
	CImage m_FullBuffer;

	// �ɼ�����
	CRect m_visibleRect;
	CRect m_displayRect;
	CRect m_dlgRect;

	// ͼƬ��ק
	BOOL m_bDrag;
	CPoint m_lastPos;
	float m_zoomFactor;



	// tab
public:

	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonLocate();
	afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonLocate2();
	afx_msg void OnBnClickedButtonLocate3();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
