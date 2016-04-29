// CTabPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Verification.h"
#include "CTabPage.h"
#include "afxdialogex.h"


// CCTabPage �Ի���

IMPLEMENT_DYNAMIC(CCTabPage, CDialogEx)

CCTabPage::CCTabPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TAB, pParent)
{

}

CCTabPage::~CCTabPage()
{
}

void CCTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCTabPage, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCTabPage ��Ϣ�������


void CCTabPage::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_TAB);
	if (edit != NULL)
	{
		CRect rect;
		GetClientRect(&rect);
		edit->MoveWindow(&rect);
	}
}
