
// VerificationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Verification.h"
#include "VerificationDlg.h"
#include "afxdialogex.h"
#include "data.h"
#include <sstream>

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

}

void CVerificationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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
	ON_BN_CLICKED(IDC_BUTTON_LOCATE2, &CVerificationDlg::OnBnClickedButtonLocate2)
	ON_BN_CLICKED(IDC_BUTTON_LOCATE3, &CVerificationDlg::OnBnClickedButtonLocate3)
	ON_WM_HELPINFO()
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
	m_displayRect.SetRect(500, 50, DISP_WIDTH + 500, DISP_HEIGHT + 50);
	m_dlgRect.SetRect(0, 0, DISP_WIDTH + 400, DISP_HEIGHT + 100);

	m_curType = ORG;
	m_curCid = 3;
	SetDispData(m_curType, COMPONENT(m_curCid));
	m_bDrag = FALSE;
	m_zoomFactor = 1.0;

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
		int pos = sSize.FindOneOf(" ");
		m_iWidth = _ttoi(sSize.Left(pos));
		m_iHeight = _ttoi(sSize.Right(sSize.GetLength() - pos - 1));
		// 读取文件名
		for (int i = 0; i < FILE_NUM; i++)
		{
			fConfig.ReadString(m_sFileName[i]);
		}
		fConfig.Close();

		initCoordinateMap(m_iWidth, m_iHeight);
		loadOrgPic();
		loadRecoPic();
		loadResiPic();
		//loadPredInfo();
	}
	else
	{

		MessageBox("配置文件打开失败！");
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
		MessageBox("原始图像加载失败！");
	}
}

void CVerificationDlg::loadRecoPic()
{
	CFile file;

	if (file.Open(m_sFileName[REC], CFile::modeRead))
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

			if (m_Image[REC][c].IsNull())
				m_Image[REC][c].Create(m_iWidth, -m_iHeight, 24);
			pImageBits[c] = (BYTE*)m_Image[REC][c].GetBits();
		}
		// 赋值
		int pitch = m_Image[REC][0].GetPitch();
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
		MessageBox("重建图像加载失败！");
	}
}

void CVerificationDlg::loadResiPic()
{
	CFile file;
	if (file.Open(m_sFileName[RES], CFile::modeRead))
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
			if (m_Image[RES][c].IsNull())
				m_Image[RES][c].Create(m_iWidth, -m_iHeight, 24);
			pImageBits[c] = (BYTE*)m_Image[RES][c].GetBits();
		}
		// 赋值
		int pitch = m_Image[RES][0].GetPitch();
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
		MessageBox("残差图像加载失败！");
	}
}

void CVerificationDlg::loadPredInfo()
{
	CStdioFile file;
	if (file.Open(m_sFileName[PRE], CFile::modeRead))
	{
		m_aPredInfo.resize(m_iHeight, std::vector<PredInfo>(m_iWidth));
		// 读取文件信息
		CString line;
		while (file.ReadString(line))
		{
			std::string sLine = line;
			std::stringstream ss(sLine);
			int cid, x, y;
			ss >> cid >> x >> y;
			m_aPredInfo[y][x].m_vTemplate[cid].resize(21, 0);
			for (int i = 0; i < 21; i++)
				ss >> m_aPredInfo[y][x].m_vTemplate[cid][i];


			m_aPredInfo[y][x].m_pPredFrom[cid] = std::make_pair(-1, -1);
			m_aPredInfo[y][x].m_vPTemplate[cid].resize(21, 0);
			if (ss.good())
			{
				int matchX, matchY;
				ss >> matchX >> matchY;
				m_aPredInfo[y][x].m_pPredFrom[cid].first = matchX;
				m_aPredInfo[y][x].m_pPredFrom[cid].second = matchY;
				for (int i = 0; i < 21; i++)
					ss >> m_aPredInfo[y][x].m_vPTemplate[cid][i];
			}
		}
	}
	else
	{
		MessageBox("预测信息加载失败！");
	}
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
		PredInfo predInfo = getPredInfo(x, y);
		
		// 原始值 重建值
		COLORREF orgColor =  m_Image[ORG][C_NUM].GetPixel(x, y);
		COLORREF recColor = m_Image[REC][C_NUM].GetPixel(x, y);
		COLORREF resColor = m_Image[RES][C_NUM].GetPixel(x, y);

		int orgRGB[3], recRGB[3], resRGB[3];
		orgRGB[0] = GetRValue(orgColor);
		orgRGB[1] = GetGValue(orgColor);
		orgRGB[2] = GetBValue(orgColor);

		recRGB[0] = GetRValue(recColor);
		recRGB[1] = GetGValue(recColor);
		recRGB[2] = GetBValue(recColor);

		resRGB[0] = GetRValue(resColor);
		resRGB[1] = GetGValue(resColor);
		resRGB[2] = GetBValue(resColor);

		CString info;
		info = "****************\r\n";
		CString tmp;
		tmp.Format("坐标: ( %d , %d )    ( %d , %d )\r\n", x, y,g_auiOrgToRsmpld[0][x], g_auiOrgToRsmpld[1][y]);
		info.Append(tmp);
		for (int i = 0; i < 3; i++)
		{
			tmp.Format(">>>>通道: %d\r\n", i);
			info.Append(tmp);
			int predX = predInfo.m_pPredFrom[i].first;
			int predY = predInfo.m_pPredFrom[i].second;
			int rsmpldX = predX == -1 ? -1 : g_auiOrgToRsmpld[0][predX];
			int rsmpldY = predY == -1 ? -1 : g_auiOrgToRsmpld[1][predY];

			tmp.Format("预测坐标: ( %d , %d )    ( %d , %d )\r\n", predX, predY, rsmpldX, rsmpldY);
			info.Append(tmp);
			tmp.Format("原始值\\重建值\\预测值\\残差: %d \\ %d \\ %d \\ %d\r\n", orgRGB[i], recRGB[i], predInfo.m_predValue[i], predInfo.m_resiValue[i]);
			info.Append(tmp);

			int pred = 0;
			if (predX != -1)
			{
				recColor = m_Image[REC][C_NUM].GetPixel(predX,predY);
				if (i == 0)
					pred = GetRValue(recColor);
				else if (i == 1)
					pred = GetGValue(recColor);
				else
					pred = GetBValue(recColor);
			}

			tmp.Format("预测值\\残差值: %d \\ %d\r\n", pred,resRGB[i]);
			info.Append(tmp);

			tmp.Format("模板P: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\r\n",
				predInfo.m_vPTemplate[i][0], predInfo.m_vPTemplate[i][1], predInfo.m_vPTemplate[i][2], predInfo.m_vPTemplate[i][3],
				predInfo.m_vPTemplate[i][4], predInfo.m_vPTemplate[i][5], predInfo.m_vPTemplate[i][6], 
				predInfo.m_vPTemplate[i][7], predInfo.m_vPTemplate[i][8], predInfo.m_vPTemplate[i][9], 
				predInfo.m_vPTemplate[i][10], predInfo.m_vPTemplate[i][11], predInfo.m_vPTemplate[i][12], 
				predInfo.m_vPTemplate[i][13], predInfo.m_vPTemplate[i][14], predInfo.m_vPTemplate[i][15], 
				predInfo.m_vPTemplate[i][16], predInfo.m_vPTemplate[i][17], predInfo.m_vPTemplate[i][18], 
				predInfo.m_vPTemplate[i][19], predInfo.m_vPTemplate[i][20]);
			info.Append(tmp);
			tmp.Format("模板C: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\r\n",
				predInfo.m_vTemplate[i][0],  predInfo.m_vTemplate[i][1], predInfo.m_vTemplate[i][2], predInfo.m_vTemplate[i][3],
				predInfo.m_vTemplate[i][4], predInfo.m_vTemplate[i][5], predInfo.m_vTemplate[i][6],
				predInfo.m_vTemplate[i][7], predInfo.m_vTemplate[i][8], predInfo.m_vTemplate[i][9],
				predInfo.m_vTemplate[i][10], predInfo.m_vTemplate[i][11], predInfo.m_vTemplate[i][12],
				predInfo.m_vTemplate[i][13], predInfo.m_vTemplate[i][14], predInfo.m_vTemplate[i][15],
				predInfo.m_vTemplate[i][16], predInfo.m_vTemplate[i][17], predInfo.m_vTemplate[i][18],
				predInfo.m_vTemplate[i][19], predInfo.m_vTemplate[i][20]);
			info.Append(tmp);
		}

		CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_INFO);
		edit->GetWindowTextA(tmp);
		tmp.Append(info);
		edit->SetWindowTextA(tmp);
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

void CVerificationDlg::initCoordinateMap(int uiSourceWidth, int uiSourceHeight)
{

		int uiPicWidth = uiSourceWidth;
		int uiPicHeight = uiSourceHeight;

		// init elements in the map with value -1
		g_auiOrgToRsmpld[0].resize(uiPicWidth, -1);		// orginal to resampled x
		g_auiOrgToRsmpld[1].resize(uiPicHeight, -1);		// original to resampled y
		g_auiRsmpldToOrg[0].resize(uiPicWidth, -1);		// resampled to original x
		g_auiRsmpldToOrg[1].resize(uiPicHeight, -1);		// resampled to original y

		int uiStrideX = uiPicWidth / 64;			// sample stride in horizontal direction as well as the number of intact CUs in a row
		int uiStrideY = uiPicHeight / 64;		// sample stride in vertical direction as well as the number of intact CUs in a column

		int uiStrideXplus1 = uiStrideX + 1;
		int uiStrideYplus1 = uiStrideY + 1;

		int uiNumberUseBiggerStrideX = uiPicWidth % 64;		// number of bigger strides in x direction
		int uiNumberUseBiggerStrideY = uiPicHeight % 64;	// number of bigger strides in y direction

																		// traverse the resampled picture
		for (int uiPicRsmpldY = 0; uiPicRsmpldY < uiPicHeight; uiPicRsmpldY++)
		{
			int uiIdY = uiPicRsmpldY % 64;
			int uiPicOrgY;

			if (uiIdY < uiNumberUseBiggerStrideY)
				uiPicOrgY = uiPicRsmpldY / 64 + uiIdY * uiStrideYplus1;	// corresponding Y in the original picture
			else
				uiPicOrgY = uiPicRsmpldY / 64 + uiNumberUseBiggerStrideY * uiStrideYplus1 + (uiIdY - uiNumberUseBiggerStrideY) * uiStrideY;

			g_auiOrgToRsmpld[1][uiPicOrgY] = uiPicRsmpldY;
			g_auiRsmpldToOrg[1][uiPicRsmpldY] = uiPicOrgY;
		}

		for (int uiPicRsmpldX = 0; uiPicRsmpldX < uiPicWidth; uiPicRsmpldX++)
		{
			int uiIdX = uiPicRsmpldX % 64;
			int uiPicOrgX;
			if (uiIdX < uiNumberUseBiggerStrideX)
				uiPicOrgX = uiPicRsmpldX / 64 + uiIdX * uiStrideXplus1;	// corresponding X in the original picture
			else
				uiPicOrgX = uiPicRsmpldX / 64 + uiNumberUseBiggerStrideX * uiStrideXplus1 + (uiIdX - uiNumberUseBiggerStrideX) * uiStrideX;

			g_auiOrgToRsmpld[0][uiPicOrgX] = uiPicRsmpldX;
			g_auiRsmpldToOrg[0][uiPicRsmpldX] = uiPicOrgX;
		}
}

void CVerificationDlg::loadBlock(int number)
{
	CStdioFile file;
	CString fileName;
	fileName.Format("%spredinfo_%d.txt", m_sFileName[PRE], number);
	if (file.Open(fileName, CFile::modeRead))
	{
		// 读取文件信息
		CString line;
		while (file.ReadString(line))
		{
			std::string sLine = line;
			//sLine = sLine.substr(0, sLine.find_last_not_of(' '));
			std::stringstream ss(sLine);
			int cid, x, y;
			ss >> cid >> x >> y;
			//cid = cid ;
			PredInfo tmp;
			if (m_mapPredInfo.find(std::make_pair(x, y)) != m_mapPredInfo.end())
			{
				tmp = m_mapPredInfo[std::make_pair(x, y)];
			}
			tmp.m_vTemplate[cid].resize(21, 0);
			for (int i = 0; i < 21; i++)
				ss >> tmp.m_vTemplate[cid][i];


			tmp.m_pPredFrom[cid] = std::make_pair(-1, -1);
			tmp.m_vPTemplate[cid].resize(21, 0);

			int matchX, matchY;
			ss >> matchX;
			//if (ss.good())
			//{
				ss >> matchY;
				tmp.m_pPredFrom[cid].first = matchX;
				tmp.m_pPredFrom[cid].second = matchY;
				for (int i = 0; i < 21; i++)
					ss >> tmp.m_vPTemplate[cid][i];
			//}
				ss >> tmp.m_predValue[cid] >> tmp.m_resiValue[cid];
			m_mapPredInfo[std::make_pair(x, y)] = tmp;
		}
	}
	else
	{
		MessageBox("预测信息加载失败！");
	}
}

PredInfo CVerificationDlg::getPredInfo(int x, int y)
{
	if (m_mapPredInfo.find(std::make_pair(x, y)) == m_mapPredInfo.end())
	{
		int number = g_auiOrgToRsmpld[0][x] / 64 + (g_auiOrgToRsmpld[1][y] / 64)*(m_iWidth / 64);
		loadBlock(number);
	}
	return m_mapPredInfo[std::make_pair(x, y)];

}

//void CVerificationDlg::addTab(FILE_TYPE, COMPONENT, int, int)
//{
//	CCTabPage newPage;
//	newPage.Create(IDD_DIALOG_TAB, &m_tabCtrl);
//	CRect rect;
//	m_tabCtrl.GetClientRect(&rect);
//	rect.top -= 20;
//	newPage.MoveWindow(&rect);
//	if (m_curSelCur != -1)
//		m_tabPage[m_curSelCur]->ShowWindow(SW_HIDE);
//
//	newPage.ShowWindow(SW_SHOW);
//	m_tabPage.push_back(&newPage);
//	m_curSelCur = m_tabPage.size() - 1;
//	m_tabCtrl.InsertItem(m_curSelCur, "lala");
//
//}





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
		m_curType = ORG;
		SetDispData(m_curType, COMPONENT(m_curCid));
		break;
	case VK_F2:
		m_curType = REC;
		SetDispData(m_curType, COMPONENT(m_curCid));
		break;
	case VK_F3:
		m_curType = RES;
		SetDispData(m_curType, COMPONENT(m_curCid));
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
				MessageBox("无法准确拾取像素");
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
		m_visibleRect.OffsetRect(-offsetX/m_zoomFactor, -offsetY/m_zoomFactor);
		m_lastPos = point;
		InvalidateRect(m_displayRect);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CVerificationDlg::OnBnClickedButtonLocate()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sX, sY;
	GetDlgItem(IDC_EDIT_X)->GetWindowText(sX);
	GetDlgItem(IDC_EDIT_Y)->GetWindowText(sY);
	if (!sX.IsEmpty() && !sY.IsEmpty())
	{
		int x = _ttoi(sX);
		int y = _ttoi(sY);
		if (x >= 0 && x < m_iWidth && y >= 0 && m_iHeight)
		{
			SetDispData(m_curType, COMPONENT(m_curCid));
			m_FullBuffer.SetPixelRGB(x, y, 255, 0, 0);
			int width = m_visibleRect.Width() / 2;
			int height = m_visibleRect.Height() / 2;
			m_visibleRect.SetRect(x - width, y - height, x + width, y + height);

			COLORREF orgColor = m_Image[REC][C_NUM].GetPixel(x, y);

			int orgRGB[3];
			orgRGB[0] = GetRValue(orgColor);
			orgRGB[1] = GetGValue(orgColor);
			orgRGB[2] = GetBValue(orgColor);

			CStatic* test = (CStatic*)GetDlgItem(IDC_STATIC_TEST);
			CString s;
			s.Format("%d %d %d", orgRGB[0], orgRGB[1], orgRGB[2]);
			test->SetWindowText(s);
			InvalidateRect(m_displayRect);
		}
	}

}


void CVerificationDlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CVerificationDlg::OnBnClickedButtonLocate2()
{
	CString sX, sY;
	GetDlgItem(IDC_EDIT_X)->GetWindowText(sX);
	GetDlgItem(IDC_EDIT_Y)->GetWindowText(sY);
	if (!sX.IsEmpty() && !sY.IsEmpty())
	{
		int x = _ttoi(sX);
		int y = _ttoi(sY);
		SelectPixel(x, y);
	}
	Invalidate(TRUE);
}


void CVerificationDlg::OnBnClickedButtonLocate3()
{
	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_INFO);
	edit->SetWindowTextA("");
	//Invalidate(TRUE);
}


BOOL CVerificationDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	return CDialogEx::OnHelpInfo(pHelpInfo);
}
