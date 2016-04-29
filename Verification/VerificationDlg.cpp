
// VerificationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Verification.h"
#include "VerificationDlg.h"
#include "afxdialogex.h"
#include "data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVerificationDlg 对话框



CVerificationDlg::CVerificationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VERIFICATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CVerificationDlg::~CVerificationDlg()
{
	//for (int t = 0; t < FILE_NUM - 1; t++)
	//{
	//	for (int c = 0; c <= C_NUM; c++)
	//	{
	//		if (m_pImage[t][c] != NULL)
	//		{
	//			m_pImage[t][c]->Destroy();
	//			LocalFree(m_pImage[t][c]);
	//			m_pImage[t][c] = NULL;
	//		}
	//	}
	//}

	//if (m_pDispBuffer != NULL)
	//{
	//	m_pDispBuffer->Destroy();
	//	LocalFree(m_pDispBuffer);
	//	m_pDispBuffer = NULL;
	//}
	//if (m_pNullDispBuffer != NULL)
	//{
	//	m_pNullDispBuffer->Destroy();
	//	LocalFree(m_pNullDispBuffer);
	//	m_pNullDispBuffer = NULL;
	//}
	//if (m_pFullBuffer != NULL)
	//{
	//	m_pFullBuffer->Destroy();
	//	LocalFree(m_pFullBuffer);
	//	m_pFullBuffer = NULL;
	//}
}

void CVerificationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CVerificationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_LOCATE, &CVerificationDlg::OnBnClickedButtonLocate)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CVerificationDlg::OnSelchangeTab)
END_MESSAGE_MAP()


// CVerificationDlg 消息处理程序

BOOL CVerificationDlg::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CVerificationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 加载配置文件 加载图像数据
	loadConfig();


	m_DispBuffer.Create(DISP_WIDTH, -DISP_HEIGHT, 24);
	m_NullDispBuffer.Create(DISP_WIDTH, -DISP_HEIGHT, 24);
	m_FullBuffer.Create(m_iWidth, -m_iHeight, 24);

	m_visibleRect.SetRect(0, 0, DISP_WIDTH, DISP_HEIGHT);
	m_displayRect.SetRect(400, 100, DISP_WIDTH + 400, DISP_HEIGHT + 100);
	m_dlgRect.SetRect(0, 0, DISP_WIDTH + 400, DISP_HEIGHT + 100);

	m_curType = ORG;
	m_curCid = 3;
	SetDispData(m_curType, COMPONENT(m_curCid));
	m_bDrag = FALSE;
	m_zoomFactor = 1.0;
	m_curSelCur = -1;

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
		ShowPicture();
		CDialog::UpdateWindow();
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
	CStdioFile fConfig;

	if (fConfig.Open(g_sConfigFile, CFile::modeRead))
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

		// 读取文件信息
		BYTE* buffer[C_NUM];
		for (int c = 0; c < C_NUM; c++)
		{
			buffer[c] = (BYTE*)LocalAlloc(LPTR, m_iWidth* m_iHeight);
			file.Read(buffer[c], m_iWidth*m_iHeight);
		}

		// 创建图像
		BYTE* pImageBits[C_NUM + 1];
		for (int c = 0; c <= C_NUM; c++)
		{
			//if (m_pImage[ORG][c] == NULL)
			//{
			//	m_pImage[ORG][c] = (CImage*)LocalAlloc(LPTR, sizeof(CImage*));
			//	m_pImage[ORG][c]->Create(m_iWidth, -m_iHeight, 24);
			//}
			if(m_Image[ORG][c].IsNull())
				m_Image[ORG][c].Create(m_iWidth, -m_iHeight, 24);
			pImageBits[c] = (BYTE*)m_Image[ORG][c].GetBits();
		}
		// 赋值
		int pitch = m_Image[ORG][0].GetPitch();
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

void CVerificationDlg::SetDispData(FILE_TYPE fileType, COMPONENT cid)
{
	if (!m_Image[fileType][cid].IsNull())
	{
		m_Image[fileType][cid].BitBlt(m_FullBuffer.GetDC(), 0, 0);
		m_FullBuffer.ReleaseDC();
	}
}

void CVerificationDlg::SelectPixel(int x, int y)
{
	if (x >= 0 && x < m_iWidth&&y >= 0 && y < m_iHeight)
	{
		SetDispData(m_curType, COMPONENT(m_curCid));
		m_FullBuffer.SetPixelRGB(x, y, 255, 0, 0);
		addTab(m_curType, COMPONENT(m_curCid), x, y);
	}

}

void CVerificationDlg::ShowPicture()
{
	if (!m_FullBuffer.IsNull())
	{
		CRect destRect(0, 0, DISP_WIDTH, DISP_HEIGHT);

		HDC hdc = m_DispBuffer.GetDC();
		m_NullDispBuffer.BitBlt(hdc, CPoint(0, 0));
		m_FullBuffer.StretchBlt(hdc, destRect, m_visibleRect);
		//m_pFullBuffer->StretchBlt(hdc, destRect, m_visibleRect);
		m_DispBuffer.ReleaseDC();

		CStatic* picCtrl = (CStatic*)GetDlgItem(IDC_STATIC_DISPLAY);
		picCtrl->MoveWindow(m_displayRect);

		CDC* pDC = picCtrl->GetDC();
		CRect rect;
		picCtrl->GetClientRect(rect);
		m_DispBuffer.Draw(pDC->m_hDC, rect);
		ReleaseDC(pDC);
	}
}

void CVerificationDlg::ZoomIn()
{
	int width = m_visibleRect.Width();
	int height = m_visibleRect.Height();
	if (width > 64 && height > 64)
	{
		m_zoomFactor *= 2;
		int centerX = m_visibleRect.CenterPoint().x;
		int centerY = m_visibleRect.CenterPoint().y;
		width /= 4;
		height /= 4;
		m_visibleRect.SetRect(centerX - width, centerY - height, centerX + width, centerY + height);
	}
}

void CVerificationDlg::ZoomOut()
{
	int width = m_visibleRect.Width();
	int height = m_visibleRect.Height();
	if (width <m_iWidth  ||  height < m_iHeight)
	{
		m_zoomFactor /= 2;
		int centerX = m_visibleRect.CenterPoint().x;
		int centerY = m_visibleRect.CenterPoint().y;
		m_visibleRect.SetRect(centerX - width, centerY - height, centerX + width, centerY + height);
	}
}

void CVerificationDlg::addTab(FILE_TYPE, COMPONENT, int, int)
{
	CCTabPage newPage;
	newPage.Create(IDD_DIALOG_TAB, &m_tabCtrl);
	CRect rect;
	m_tabCtrl.GetClientRect(&rect);
	rect.top -= 20;
	newPage.MoveWindow(&rect);
	if (m_curSelCur != -1)
		m_tabPage[m_curSelCur]->ShowWindow(SW_HIDE);
	newPage.ShowWindow(SW_SHOW);
	m_tabPage.push_back(&newPage);
	m_curSelCur = m_tabPage.size() - 1;
	m_tabCtrl.InsertItem(m_curSelCur, L"lala");
}





void CVerificationDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
}


void CVerificationDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nChar)
	{
	case VK_F1:
		SetDispData(ORG, COMPONENT(m_curCid));
		break;
	case VK_F2:
		SetDispData(REC, COMPONENT(m_curCid));
		break;
	case VK_F3:
		SetDispData(RES, COMPONENT(m_curCid));
		break;
	case 'n':
	case 'N':
		if (GetAsyncKeyState(VK_CONTROL))
		{
			m_curCid = (m_curCid + 1) % 4;
			SetDispData(m_curType, COMPONENT(m_curCid));
		}
		break;
	case 'i':
	case 'I':
		if (GetAsyncKeyState(VK_CONTROL))
		{
			ZoomIn();
		}
		break;
	case 'o':
	case 'O':
		if (GetAsyncKeyState(VK_CONTROL))
		{
			ZoomOut();
		}
		break;
	default:
		break;
	}
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
	InvalidateRect(m_displayRect);
}


void CVerificationDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CStatic* pPicCtrl = (CStatic*)GetDlgItem(IDC_STATIC_DISPLAY);
	CRect rect;
	pPicCtrl->GetWindowRect(rect);
	m_lastPos = point;
	ClientToScreen(&point);
	if (rect.PtInRect(point))
	{
		if (GetAsyncKeyState(VK_CONTROL))
		{
			if (m_zoomFactor < 1.0f)
			{
				MessageBox(L"无法准确拾取像素");
			}
			else
			{
				int x, y;
				x = (point.x - rect.left)*1.0f/rect.Width() * m_visibleRect.Width() + m_visibleRect.left;
				y = (point.y - rect.top)*1.0f / rect.Height() * m_visibleRect.Height() + m_visibleRect.top;
				SelectPixel(x, y);
				InvalidateRect(m_displayRect);
			}
		}
		else
		{
			m_bDrag = TRUE;
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CVerificationDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bDrag = FALSE;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CVerificationDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bDrag)
	{
		int offsetX = point.x - m_lastPos.x;
		int offsetY = point.y - m_lastPos.y;
		m_visibleRect.OffsetRect(-offsetX, -offsetY);
		m_lastPos = point;
		InvalidateRect(m_displayRect);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CVerificationDlg::OnBnClickedButtonLocate()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sX, sY;
	GetDlgItem(IDC_EDIT_X)->GetWindowTextW(sX);
	GetDlgItem(IDC_EDIT_Y)->GetWindowTextW(sY);
	if (!sX.IsEmpty() && !sY.IsEmpty())
	{
		int x = _ttoi(sX);
		int y = _ttoi(sY);
		if (x >= 0 && x < m_iWidth && y >= 0 && m_iHeight)
		{
			int width = m_visibleRect.Width() / 2;
			int height = m_visibleRect.Height() / 2;
			m_visibleRect.SetRect(x - width, y - height, x + width, y + height);
			InvalidateRect(m_displayRect);
		}
	}

}


void CVerificationDlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
