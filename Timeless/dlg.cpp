#include <stdio.h>
//#include <string.h>
//#include <tchar.h>

#include "dlg.h"
#include "Global.h"

/////////////////////////////////////////////////////
//�⺻ ��ȭ ó�� Ŭ���� �޼ҵ�
#define DLG_TILE_SIZE	10

CDlg::CDlg()
{
	//�� �ʱ�ȭ
	setting=setting2=FALSE;
	b_shadow=FALSE;
	dlgbox=NULL;
	show_dlgbox=FALSE;
	SetTextColor(0, 0, 0);
	text_font=&global_font;
	frame_x=frame_y=10;
	opacity=1.0f;
}

CDlg::~CDlg()
{
	JPictureInfo pi;
	if(jdd->GetPictureInfo(dlgbox, &pi))
	{
		jdd->DeleteSurface(dlgbox);
	}
}

//��ȭâ ������ �����ؼ� �׷��ִ� �Լ�
void CDlg::MakeDlgBox(char* dlgbox)
{
	JPictureInfo pi;
	if(jdd->GetPictureInfo(dlgbox, &pi))
	{
		jdd->DeleteSurface(dlgbox);	//�̹� �׸��� �����ϸ� �����
	}

	//���ο� ��ȭâ ����
	pi.SetWidth((frame_x * 2) + (width * 10));
	pi.SetHeight((frame_y * 2) + (line * 20));
	pi.SetColorKey(JColor(0,0,255));
	pi.SetOpacity(opacity);
	jdd->CreateSurface(dlgbox, &pi, true);

	//������ ������
	JBrush blue_brush;
	blue_brush = jdd->CreateBrush(JColor(0,0,255));
	RECT pic_rect;
	SetRect(&pic_rect, 0, 0, (frame_x * 2) + (width * DLG_TILE_SIZE), (frame_y * 2) + (line * DLG_TILE_SIZE * 2));
	jdd->DrawRect(dlgbox, blue_brush, &pic_rect);
	jdd->DeleteBrush(blue_brush);

	//��ȭâ �̸� �׸���
	RECT src_rect[3];
	
	for(int i = -1; i <= line * 2; ++i)
	{
		int tmp_y;
		//��, �Ʒ�, �߰�
		if(i == -1)tmp_y=0;
			else if(i == line * 2)tmp_y = frame_y * 2;
			else tmp_y = frame_y;

		//��, ��, ��
		for(int j = 0; j < 3; ++j)SetRect(&src_rect[j], frame_x * j, tmp_y, frame_x * (j + 1), tmp_y + frame_y);

		//�׸���
		for(int j = -1; j <= width; ++j)
		{
			if(j == -1)jdd->DrawPicture(dlgbox,"_dlgbox", (j + 1) * DLG_TILE_SIZE, (i + 1) * DLG_TILE_SIZE, &src_rect[0]);
				else if(j == width)jdd->DrawPicture(dlgbox,"_dlgbox", (j + 1) * DLG_TILE_SIZE, (i + 1) * DLG_TILE_SIZE,&src_rect[2]);
				else jdd->DrawPicture(dlgbox,"_dlgbox", (j + 1) * DLG_TILE_SIZE, (i + 1) * DLG_TILE_SIZE, &src_rect[1]);
		}
	}

	this->dlgbox = dlgbox;
	show_dlgbox = TRUE;
}

//��ȭâ �̵�
void CDlg::Move(int mov_x, int mov_y)
{
	if(!setting || !setting2)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	x += mov_x;
	y += mov_y;
}

void CDlg::SetTextColor(BYTE r, BYTE g, BYTE b)
{
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
}

void CDlg::SetDlgBox(char* dlgbox)
{
	//����ڰ� ������ ��ȭâ ����
	this->dlgbox = dlgbox;
	show_dlgbox = TRUE;
}

void CDlg::SetFont(JFont *font)
{
	if(font==NULL)
	{
		printf("Warning : ��Ʈ�� �������� �ʽ��ϴ�.");
		return;
	}

	text_font=font;
}

void CDlg::DrawDlg()
{
	//�ʱ�ȭ�� �� �Ǿ� ������ ����
	if(!setting)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	//��ȭâ ���
	if(show_dlgbox && dlgbox != NULL)
	{
		jdd->DrawPicture(backbuffer, dlgbox, x - frame_x, y - frame_y, NULL);
	}
}

//�׸��� ���̱� �����
void CDlg::ShowShadow()
{
	b_shadow = true;
}

void CDlg::HideShadow()
{
	b_shadow = false;
}

//�ۻ��� ���̱� �����
void CDlg::ShowDlg()
{
	show_dlgbox = true;
}

void CDlg::HideDlg()
{
	show_dlgbox = false;
}

//���콺�� ��ȭâ ���� �ִ��� �˻�
BOOL CDlg::MouseInBox()
{
	if(mouse_control && MouseX >= x && MouseX < x + width * DLG_TILE_SIZE && MouseY >= y && MouseY < y + line * DLG_TILE_SIZE * 2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//������ �˻�
void CDlg::SetOpacity(float opacity)
{
	//��ȭâ�� ������ ���ư���
	if(dlgbox == NULL)
	{
		printf("Warning : ��ȭâ�� �����Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	JPictureInfo pi;
	jdd->GetPictureInfo(dlgbox, &pi);
	pi.SetOpacity(opacity);
	jdd->SetPictureInfo(dlgbox, &pi);

	this->opacity = opacity;
}

//��ȭâ ���� ����
void CDlg::SetFrameSize(int vx, int vy)
{
	frame_x = vx;
	frame_y = vy;
}