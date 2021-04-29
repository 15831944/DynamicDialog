
// tableDlgDlg.h: 头文件
//define dlgTEST 使得窗口生成测试控件

#pragma once
#include<unordered_map>
#include<string>
#include<functional>
#include<vector>
#include<list>
#include"IIDpool.h"
using namespace std;
class CDynamicDialog;
/*
* 返回int 意为传递处理结果
* 注意,像按钮的buttonup消息处理，不可以返回msg_handler_TRUE，否则会导致控件一直保留焦点，会使得该控件接受所有点击类消息或是其他
*/
using msg_handler = std::function<int(CWnd*pThis,CDynamicDialog*cdd,MSG*msg)>;
using Tmsg = decltype(WM_ACTIVATE);
using WID = UINT;
#define CBUUTTON_DEFUALT_STYLE (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON)
#define CEDIT_DEFAULT_STYLE (ES_MULTILINE |/*ES_AUTOVSCROLL |*/ WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER)
#define CSTATIC_TEXT_STYLE (SS_CENTER)
#define msg_hanlder_paras CWnd*pThis,CDynamicDialog*cdd,MSG*msg
#define msg_handler_TRUE int(0)
/*
* 辅助类，用以使构造控件时传入区域参数不需要在意对话框实际大小，而按比例
*/
struct m_Rect {
	double l, t, r, b;//左上右下
	template<typename T>
	m_Rect(std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, T> l, T t, T r, T b) :l(l), t(t), r(r), b(b) {}
	void modify(CRect rect) {
#pragma push(mac)
#define mac(l) l>=0.0&&l<1.0
		if (mac(l) && mac(t) && mac(r) && mac(b))
#undef mac
#pragma pop(mac)
		{
			this->l *= rect.Width();
			this->r *= rect.Width();
			this->t *= rect.Height();
			this->b *= rect.Height();
		};
	}
	bool operator==(const m_Rect& ot) { return l == ot.l && t == ot.t && r == ot.r && b == ot.b; };
	explicit operator CRect() { return CRect(l, t, r, b); }
	explicit operator RECT() { return CRect( l, t, r, b ); }
};
/*
* 控件参数类，封装了各类的create函数所需的参数
*/
struct unitAttr {
	string unitName;
	enum class unitTypeCol{ CBUTTON,CEDIT,C_STATIC_TEXT} unitType;
	//以下是create函数的参数
	LPCTSTR lpszCaption; 
	DWORD dwStyle;
	m_Rect rect; 
	string pParentUnitName; //CWnd* pParentWnd; 用unit名替换父窗口指针
							/*UINT nID;    会传入 IID*/
							//			消息名	消息处理函数
	vector<pair<Tmsg, msg_handler> > msg_handlers;
	unitAttr(string unitName, unitTypeCol unitType, LPCTSTR lpszCaption, DWORD dwStyle, m_Rect rect, string pParentUnitName,decltype(msg_handlers) msg_handlers) :
		unitName(unitName), unitType(unitType), lpszCaption(lpszCaption), dwStyle(dwStyle), rect(rect), pParentUnitName(pParentUnitName),msg_handlers(msg_handlers) {};
	unitAttr() = delete;
	unitAttr(const unitAttr& ot) = default;

};
// CtableDlgDlg 对话框
class CDynamicDialog : public CDialogEx
{
// 构造
public:
	CDynamicDialog(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TABLEDLG_DIALOG };
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
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//释放资源
	~CDynamicDialog();
	//自定义内容
	using baseClass = CDialogEx;
/*
* 分别是
*	stringTable		控件名到iid的映射
*	unitsRec		iid到控件句柄的映射
*	HWNDsRec		安全句柄到iid的映射		在preTranslate里使用以快速检查是否有符合HWND的相应控件
*	attrRec			保留的 控件创建时的信息
*	msg_map			消息地图
*/
	unordered_map<string, WID> stringTable;
	unordered_map<WID, CWnd*> unitsRec;
	unordered_map<HWND, WID> HWNDsRec;
	unordered_map<WID, unitAttr>attrRec;
	unordered_map<Tmsg, unordered_map<WID, msg_handler>> msg_map;
	CWnd* dynamicCreateUnit(unitAttr ua,_m_UID iid);
	void createDynamicUnits(const vector<unitAttr>& units);
	//这个参数应该在domodal之前更改，并在oninitialDlg里使用并清空
	list<vector<unitAttr>> waitToCreate;
protected:
	msg_handler& _get_hanlder(Tmsg msg, WID wid);
};
unitAttr make_UnitAttr(string unitName, unitAttr:: unitTypeCol unitType, LPCTSTR lpszCaption, DWORD dwStyle, m_Rect rect, string pParentUnitName,decltype(unitAttr:: msg_handlers) msg_handlers);
// default==nullptr
extern msg_handler default_msg_handler;
