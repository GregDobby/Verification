// ConfigDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Verification.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"
#include "data.h"


// CConfigDlg �Ի���

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CONFIG, pParent)
{

}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGBF, &CConfigDlg::OnBnClickedButtonConfigbf)
END_MESSAGE_MAP()


// CConfigDlg ��Ϣ�������


void CConfigDlg::OnBnClickedButtonConfigbf()
{
	CString filter = L"�ļ� (*.*)|*.*||";	//�ļ����ǵ�����
	CFileDialog openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, this);
	INT_PTR nResponse = openFileDlg.DoModal();
	if (nResponse == IDOK) {
		CEdit* editConfig = (CEdit*)GetDlgItem(IDC_EDIT_CONFIG);
		editConfig->SetWindowTextW(openFileDlg.GetPathName());
		g_sConfigFile = openFileDlg.GetPathName();
	}
}
