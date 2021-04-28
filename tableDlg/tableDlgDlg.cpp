
// tableDlgDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "tableDlg.h"
#include "tableDlgDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//default_msg_handler==nullptr is true
msg_handler default_msg_handler  ;

// CtableDlgDlg 对话框



CDynamicDialog::CDynamicDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TABLEDLG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDynamicDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDynamicDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CtableDlgDlg 消息处理程序

BOOL CDynamicDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
#if defined(dlgTEST)/*|defined(_DEBUG)*/
	vector<unitAttr> us = {
		make_UnitAttr("sta0", unitAttr::unitTypeCol::C_STATIC_TEXT, L"起点&&中点 int X4", SS_CENTER, { 100,100,200,150 }, "",{}),
		make_UnitAttr("sta1", unitAttr::unitTypeCol::C_STATIC_TEXT, L"线型", SS_CENTER, { 100,200,200,250 }, "",{}),
		make_UnitAttr("ed1_specify", unitAttr::unitTypeCol::C_STATIC_TEXT, L" multiLine like \n###\n # \n# #", SS_CENTER, { 400,200,600,300 }, "",{}),
		make_UnitAttr("ed0",unitAttr::unitTypeCol::CEDIT,L"",ES_CENTER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN  ,{ 0.3,0.2,0.6,0.4},"",{}),
		make_UnitAttr("bt2",unitAttr::unitTypeCol::CBUTTON,L"bt2",0,{500,400,650,450},"",{}),
		make_UnitAttr("bt_test",unitAttr::unitTypeCol::CBUTTON,L"append#",0,{400,300,550,350},"",{
			make_pair<Tmsg,msg_handler>(WM_LBUTTONUP,[](msg_hanlder_paras)->int {
				CEdit* ed0 = (CEdit*)cdd->unitsRec[cdd->stringTable["ed0"]];
				CString con;
				ed0->GetWindowTextW(con);
				ed0->SetWindowTextW(con + L"#");
				//非true，以触发按钮默认的松开效果,否则会有严重的后果，参阅msg_hanlder的说明
				return 1;//msg_handler_TRUE;
		}),
			make_pair<Tmsg,msg_handler>(WM_MOUSELEAVE,[](msg_hanlder_paras)->int {
			//cdd->SetFocus();
			//ASSERT(GetFocus() != pThis);
			return 1;
		}),
			})
	};
	createDynamicUnits(us);
#endif
	for (auto& wus : waitToCreate)
		createDynamicUnits(wus);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDynamicDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		//::DefWindowProc(this->GetSafeHwnd(), WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDynamicDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CDynamicDialog::PreTranslateMessage(MSG* pMsg)
{
	
/*
* 在HWND集中查找有无消息中的句柄
*	如有 
*		再查有无对应处理函数
*			如有 调用之
*				如返回msg_handler_TRUE，则返回true
*	其他 则抛给父类
*			
*/
	WM_ACTIVATE;
	auto p = HWNDsRec.find( pMsg->hwnd);
	if ( p!= HWNDsRec.end())
	{
		auto handler = _get_hanlder(pMsg->message, p->second);
		if (handler != nullptr)
		{
			auto ret=handler(unitsRec[ p->second], this, pMsg);
			if (ret == msg_handler_TRUE)//0
				return true;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
CDynamicDialog::~CDynamicDialog()
{
/*
* 释放iid资源
* 释放控件资源
*/
	for (auto p : stringTable)
	{
		releaseIID(p.second);
	};
	for (auto p : unitsRec)
	{
		delete p.second;
	};
}
unitAttr CDynamicDialog::make_UnitAttr(string unitName, unitAttr::unitTypeCol unitType, LPCTSTR lpszCaption, DWORD dwStyle, m_Rect rect, string pParentUnitName, decltype(unitAttr::msg_handlers) msg_handlers)
{
	return unitAttr(unitName,unitType,lpszCaption,dwStyle,rect,pParentUnitName,msg_handlers);
}

CWnd* CDynamicDialog::dynamicCreateUnit(unitAttr ua, _m_UID iid)
{
/*
* 若ua.rect为[0,1)，修改使之映射到整个客户区
* 根据所传参数,创建控件，并返回句柄
*/
	CRect cr;
	GetClientRect(cr);
	ua.rect.modify(cr);
	switch (ua.unitType)
	{
	case decltype(ua)::unitTypeCol::CBUTTON :
	{
		CButton* ret = new CButton;
		ret->Create(ua.lpszCaption, ua.dwStyle/*|WS_EX_TRANSPARENT  |WS_EX_LAYERED*/  , RECT(ua.rect), this, UINT( iid));
		return ret;
	}
	case decltype(ua)::unitTypeCol::CEDIT:
	{
		CEdit* ret = new CEdit;
		ret->Create(ua.dwStyle, RECT(ua.rect), this, iid);
		return ret;
	}
	case decltype(ua)::unitTypeCol::C_STATIC_TEXT: 
	{
		CStatic* ret = new CStatic;
		ret->Create(ua.lpszCaption, ua.dwStyle, RECT(ua.rect), this, iid);
		return ret;
	}
	default:
		break;
	}
	return nullptr;
}
void CDynamicDialog::createDynamicUnits(const vector<unitAttr>& units)
{

	/*
	创建unit名到IID的映射
	创建所有单个unit
	录入 子控件集，
	创建 子控件hwnd到iid的映射
	录入 消息处理函数
	*/
	for (auto& unit : units)
	{
		_m_UID newIID = getNewIID();
		auto &ret= stringTable.emplace(make_pair(unit.unitName,newIID));
		if (ret.second == false)
		{
			//it's already exist
			// send  a message active it
			::SendMessageW((HWND)unitsRec[stringTable[unit.unitName]], WM_ACTIVATE, 0, 0);
			releaseIID(newIID);
		}
		CWnd*unitCWnd= dynamicCreateUnit(unit, newIID);
		unitsRec.emplace(std::make_pair(newIID, unitCWnd));
		HWNDsRec.emplace(std::make_pair(unitCWnd->GetSafeHwnd(), newIID));
		attrRec.emplace(make_pair(newIID, unit));
		for (auto pCF : unit.msg_handlers)
			msg_map[pCF.first].emplace(std::make_pair(newIID, pCF.second));
	}
	//设置父窗口
	for (auto& unit : units)
	{
		if (unit.pParentUnitName != "")
			unitsRec[stringTable[unit.unitName]]->SetParent(unitsRec[stringTable[unit.pParentUnitName]]);
		unitsRec[stringTable[unit.unitName]]->ShowWindow(SW_NORMAL);
	}
}

msg_handler& CDynamicDialog::_get_hanlder(Tmsg msg, WID wid)
{
/*
* 凭借消息和窗口iid在消息地图中查找处理函数，
	如有 返回之
	否则 返回default_msg_handler,其满足 default_msg_handler==nullptr
*/
	auto wm = msg_map.find(msg);
	if (wm == msg_map.end())
		return default_msg_handler;
	auto ret = wm->second.find(wid);
	if (ret == wm->second.end())
		return default_msg_handler;
	return ret->second;
}
