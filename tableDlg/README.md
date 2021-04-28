## DynamicDialog

如果你不想重复为简单的几个输入控件创建新的窗口资源，那么，或许这个类能帮到你



代码解释没有，该有的都在cpp里了

## 使用说明

创建好窗口并在domodal之后，可以使用createDynamicUnits()直接创建动态控件

其他情况，需要把需要创建的控件信息存入waitToCreate链表，并等待在对话框的OnInitDialog()中创建相应的控件

* style参数不明的话，可以使用定义的对应宏，或自行查找 [mfc 使用的样式]([MFC 使用的样式 | Microsoft Docs](https://docs.microsoft.com/zh-cn/cpp/mfc/reference/styles-used-by-mfc?view=msvc-160))
* Tmsg 是消息的类型 Tmsg=decltype(WM_ACTIVATE)

### 使用示例

在pch.h 或者DynamicDialog.h 文件 的头加入 

 ```c++
 #define dlgTEST
 ```

并运行 以观察测试效果

示例代码如下

```c++
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
				//非msg_handler_TRUE，以触发按钮默认的松开效果,否则...
				return msg_handler_TRUE+1;//msg_handler_TRUE;
		}),
			make_pair<Tmsg,msg_handler>(WM_MOUSELEAVE,[](msg_hanlder_paras)->int {
			//cdd->SetFocus();
			//ASSERT(GetFocus() != pThis);
			return msg_handler_TRUE+1;
		}),
			})
	};
	createDynamicUnits(us);
```

## 帮助

### 如何导入对话框资源

请使用vs的资源管理器进行导入，莫要自己手动导(雪的教训)

1. 在您的解决方案里添加已有工程，即本工程
2. 打开资源管理器，选中本工程的对话框资源里的唯一对话框，右击复制(ctrl V)当我没说
3. 选择目标工程的对话框资源，右键粘贴