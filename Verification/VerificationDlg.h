
// VerificationDlg.h : 头文件
//

#pragma once

enum FILE_TYPE { ORG = 0, REC = 1, RES = 2, PRE = 3, FILE_NUM = 4 };
enum COMPONENT { C0 = 0, C1 = 1, C2 = 2, C_NUM = 3 };

#define DISP_WIDTH 800
#define DISP_HEIGHT 600

// CVerificationDlg 对话框
class CVerificationDlg : public CDialogEx
{
// 构造
public:
	CVerificationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERIFICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	// 加载函数
	void loadConfig();
	void loadOrgPic();
	void loadRecoPic();
	void loadResiPic();
	void loadPredInfo();

	// 显示图片
	void setDispData(FILE_TYPE, COMPONENT);
	void showPicture();

protected:
	// 文件名
	CString m_sFileName[FILE_NUM];

	int m_iWidth, m_iHeight;

	FILE_TYPE m_curType;
	int m_curCid;

	// 图片指针
	CImage* m_pImage[FILE_NUM - 1][C_NUM+1];
	CImage* m_pDispBuffer;
	CImage* m_pNullDispBuffer;
	CImage* m_pFullBuffer;

	// 可见区域
	CRect m_visibleRect;

public:

	afx_msg void OnBnClickedButtonConfig();
	afx_msg void OnBnClickedButtonConfigbf();
	afx_msg void OnClose();
};
