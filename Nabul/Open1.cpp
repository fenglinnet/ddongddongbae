// Open1.cpp : implementation file
//

#include "stdafx.h"
#include "Nabul.h"
#include "Open1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpen1 dialog


COpen1::COpen1(CWnd* pParent /*=NULL*/)
	: CDialog(COpen1::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpen1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COpen1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpen1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpen1, CDialog)
	//{{AFX_MSG_MAP(COpen1)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpen1 message handlers
