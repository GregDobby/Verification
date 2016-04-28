
// VerificationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Verification.h"
#include "VerificationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVerificationDlg 对话框



CVerificationDlg::CVerificationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VERIFICATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVerificationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVerificationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &CVerificationDlg::OnBnClickedButtonConfig)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGBF, &CVerificationDlg::OnBnClickedButtonConfigbf)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CVerificationDlg 消息处理程序

BOOL CVerificationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	for (int t = 0; t < FILE_NUM - 1; t++)
	{
		for (int c = 0; c <= C_NUM; c++)
		{
			m_pImage[t][c] = NULL;
		}
	}


	m_pDispBuffer = (CImage*)LocalAlloc(LPTR, sizeof(CImage));
	m_pDispBuffer->Create(DISP_WIDTH, -DISP_HEIGHT, 24);

	m_pNullDispBuffer = (CImage*)LocalAlloc(LPTR, sizeof(CImage));
	m_pNullDispBuffer->Create(DISP_WIDTH, -DISP_HEIGHT, 24);

	m_pFullBuffer = NULL;

	m_visibleRect.SetRect(0, 0, DISP_WIDTH, DISP_HEIGHT);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVerificationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//GetDlgItem(IDC_STATIC_DISPLAY)->Invalidate(TRUE);
		showPicture();
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVerificationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVerificationDlg::loadConfig()
{
	CEdit* editConfig = (CEdit*)GetDlgItem(IDC_EDIT_CONFIG);
	CString sConfigPath; 
	editConfig->GetWindowTextW(sConfigPath);
	CStdioFile fConfig;

	if (fConfig.Open(sConfigPath, CFile::modeRead))
	{
		// 读取图像尺寸
		CString sSize;
		fConfig.ReadString(sSize);
		int pos = sSize.FindOneOf(L" ");
		m_iWidth = _ttoi(sSize.Left(pos));
		m_iHeight = _ttoi(sSize.Right(sSize.GetLength() - pos - 1));
		// 读取文件名
		for (int i = 0; i < FILE_NUM; i++)
		{
			fConfig.ReadString(m_sFileName[i]);
		}
		fConfig.Close();
		loadOrgPic();
		loadRecoPic();
		loadResiPic();
		loadPredInfo();
		m_curType = ORG;
		m_curCid = 3;
		setDispData(m_curType, COMPONENT(0));

		this->Invalidate(TRUE);
	}
	else
	{

		MessageBox(L"配置文件打开失败！");
	}
}

void CVerificationDlg::loadOrgPic()
{

	CFile file;
	if (file.Open(m_sFileName[ORG], CFile::modeRead))
	{
		// 创建图像
		BYTE* pImageBits[C_NUM + 1];
		for (int c = 0; c <= C_NUM; c++)
		{
			if (m_pImage[ORG][c] == NULL)
			{
				m_pImage[ORG][c] = (CImage*)LocalAlloc(LPTR, sizeof(CImage));
				m_pImage[ORG][c]->Create(m_iWidth, -m_iHeight, 24);
			}
			pImageBits[c] = (BYTE*)m_pImage[ORG][c]->GetBits();
		}
		// 读取文件信息
		BYTE* buffer[C_NUM];
		for (int c = 0; c < C_NUM; c++)
		{
			buffer[c] = (BYTE*)LocalAlloc(LPTR, m_iWidth* m_iHeight);
			file.Read(buffer[c], m_iWidth*m_iHeight);
		}
		// 赋值
		int pitch = m_pImage[ORG][0]->GetPitch();
		for (int y = 0; y < m_iHeight; y++)
		{
			for (int x = 0; x < m_iWidth; x++)
			{
				BYTE r, g, b;
				int idx = y*m_iWidth + x;
				r = buffer[C0][idx];
				g = buffer[C1][idx];
				b = buffer[C2][idx];

				// c0
				pImageBits[C0][3 * x] = pImageBits[C0][3 * x + 1] = pImageBits[C0][3 * x + 2] = r;

				// c1
				pImageBits[C1][3 * x] = pImageBits[C1][3 * x + 1] = pImageBits[C1][3 * x + 2] = g;

				// c2
				pImageBits[C2][3 * x] = pImageBits[C2][3 * x + 1] = pImageBits[C2][3 * x + 2] = b;

				// all
				pImageBits[C_NUM][3 * x] = b;
				pImageBits[C_NUM][3 * x + 1] = g;
				pImageBits[C_NUM][3 * x + 2] = r;
			}
			pImageBits[C0] += pitch;
			pImageBits[C1] += pitch;
			pImageBits[C2] += pitch;
			pImageBits[C_NUM] += pitch;
		}

		// free
		for (int c = 0; c < C_NUM; c++)
		{
			LocalFree(buffer[c]);
			buffer[c] = NULL;
		}

	}
	else
	{
		MessageBox(L"原始图像加载失败！");
	}

}

void CVerificationDlg::loadRecoPic()
{
}

void CVerificationDlg::loadResiPic()
{
}

void CVerificationDlg::loadPredInfo()
{
}

void CVerificationDlg::setDispData(FILE_TYPE fileType, COMPONENT cid)
{
	if (m_pFullBuffer == NULL)
	{
		m_pFullBuffer = (CImage*)LocalAlloc(LPTR, sizeof(CImage*));
		m_pFullBuffer->Create(m_iWidth, -m_iHeight, 24);
	}
	//m_pImage[fileType][cid]->BitBlt(m_pFullBuffer->GetDC(),0,0);
	//m_pFullBuffer->ReleaseDC();

	// copy
	BYTE* pDest = (BYTE*)m_pFullBuffer->GetBits();
	BYTE* pSrc = (BYTE*)m_pImage[fileType][cid]->GetBits();
	int pitch = m_pFullBuffer->GetPitch();
	for (int y = 0; y < m_iHeight; y++)
	{
		for (int x = 0; x < m_iWidth; x++)
		{
			pDest[3 * x] = pSrc[3 * x];
			pDest[3 * x+1] = pSrc[3 * x+1];
			pDest[3 * x+2] = pSrc[3 * x+2];
		}
		pDest += pitch;
		pSrc += pitch;
	}
}

void CVerificationDlg::showPicture()
{
	if (m_pFullBuffer != NULL)
	{
		CRect destRect(0, 0, DISP_WIDTH, DISP_HEIGHT);

		HDC hdc = m_pDispBuffer->GetDC();
		m_pNullDispBuffer->BitBlt(hdc, CPoint(0, 0));
		m_pFullBuffer->StretchBlt(hdc, destRect, m_visibleRect);
		m_pDispBuffer->ReleaseDC();

		CRect picRect(200, 200, 200 + DISP_WIDTH, 200 + DISP_HEIGHT);
		CStatic* picCtrl = (CStatic*)GetDlgItem(IDC_STATIC_DISPLAY);
		picCtrl->MoveWindow(&picRect);

		HBITMAP bitmap = m_pDispBuffer->Detach();
		picCtrl->SetBitmap(bitmap);
		m_pDispBuffer->Attach(bitmap);
	}
}


void CVerificationDlg::OnBnClickedButtonConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	loadConfig();
}


void CVerificationDlg::OnBnClickedButtonConfigbf()
{
	// TODO: 在此添加控件通知处理程序代码
	CString filter = L"文件 (*.*)|*.*||";	//文件过虑的类型
	CFileDialog openFileDlg(TRUE, NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter,this);
	INT_PTR nResponse = openFileDlg.DoModal();
	if (nResponse == IDOK) {
		CEdit* editConfig = (CEdit*)GetDlgItem(IDC_EDIT_CONFIG);
		editConfig->SetWindowTextW(openFileDlg.GetPathName());
	}
}


void CVerificationDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for (int t = 0; t < FILE_NUM - 1; t++)
	{
		for (int c = 0; c <= C_NUM; c++)
		{
			if (m_pImage[t][c] != NULL)
			{
				LocalFree(m_pImage[t][c]);
				m_pImage[t][c] = NULL;
			}
		}
	}

	if (m_pDispBuffer != NULL)
	{
		LocalFree(m_pDispBuffer);
		m_pDispBuffer = NULL;
	}
	if (m_pNullDispBuffer != NULL)
	{
		LocalFree(m_pNullDispBuffer);
		m_pNullDispBuffer = NULL;
	}
	if (m_pFullBuffer != NULL)
	{
		LocalFree(m_pFullBuffer);
		m_pFullBuffer = NULL;
	}
	CDialogEx::OnClose();
}
