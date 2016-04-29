#pragma once


// CCTabPage 对话框

class CCTabPage : public CDialogEx
{
	DECLARE_DYNAMIC(CCTabPage)

public:
	CCTabPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCTabPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
