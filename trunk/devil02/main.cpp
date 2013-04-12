#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <ddraw.h>
#include <dsound.h>
#include <stdio.h>

#include "dsutil.h"
#include "ddutil.h"
#include "extern.h"
#include "data.h"

BOOL _MidiPlay( char* pszMidiFN, BOOL bReplayFlag )
{
    char szMCISendString[256];

    wsprintf(szMCISendString,"open %s type sequencer alias MUSIC", pszMidiFN);
   
    if ( mciSendString ( "close all", NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( szMCISendString, NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, MainHwnd ) != 0) return(FALSE);

    ReplayFlag = bReplayFlag; 
    return TRUE;
}

BOOL _MidiStop( void )
{
    if ( mciSendString ( "close all", NULL, 0, NULL) != 0 ) return ( FALSE );
    return TRUE;
}

BOOL _MidiReplay( void )
{
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, MainHwnd) != 0 ) return ( FALSE );
    return TRUE;
}

void _Play( int num )
{
    if ( SoundCard ) SndObjPlay( Sound[num], NULL );
}

long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch ( message )
    {
		case	WM_LBUTTONDOWN	:	LeftButton = TRUE;
									break;
		case	WM_LBUTTONUP	:	LeftButton = FALSE;
									break;
		case	WM_RBUTTONDOWN	:	RightButton = TRUE;
									break;
		case	WM_RBUTTONUP	:	RightButton = FALSE;
									break;
        case    WM_MOUSEMOVE    :   MouseX = LOWORD(lParam);
                                    MouseY = HIWORD(lParam);
                                    break;
        case    MM_MCINOTIFY    :   if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL ) _MidiReplay();
                                    break;
		case    WM_DESTROY      :   _WindowMode();
								    PostQuitMessage( 0 );
                                    break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}

int Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

int Min(int x, int y)
{
	if(x<y)return x;
		else return y;
}

SColor1(int hp)
{
	return (hp<50)?255:250-(hp-50)*5;
}

SColor2(int hp)
{
	return (hp<50)?hp*5:255;
}

void ani(int limit)
{
	Count++;
	if(Count>limit)
	{
		RFrame++;
		if(RFrame>3)RFrame=0;
		if(RFrame==3)Frame=1;
			else Frame=RFrame;
		Count=0;
	}
}

int Click()
{
	bool loop=true;
	key=true;
	while(loop)
	{
		PRC
		if(LeftButton)
		{
			if(!key)
			{
				key=true;
				loop=false;
			}
		}
		else key=false;	
	}
	return 0;
}

void Title()
{
	if(title==0)BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//title.bmp", 0, 0, SYSTEM);
	if(title==1)BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//title2.bmp", 0, 0, SYSTEM);
	if(title==2)BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//title3.bmp", 0, 0, SYSTEM);
	if(title==3)BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//title4.bmp", 0, 0, SYSTEM);
}

int YesNo(int text)
{
	int answer=0, Select=0;
	while(!Select){
		PRC
		{
			ani(30);
			SetRect(&BackRect, 0, 160, 180, 300);
			_DrawBmp(BackRect, 230, 230, BmpScreen[9], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			//�޴�
			if(MouseX<240)MouseX=240;
			if(MouseX>386)MouseX=386;
			if(MouseY<240)MouseY=240;
			if(MouseY>336)MouseY=336;
			for(int i=0; i<2; i++){
				if(MouseY>=280 && MouseY<340 && MouseX>=(i*60)+260 && MouseX<(i*60)+320 && LeftButton)
				{
					_Play(0);
					Select=i+1;
					SetRect(&BackRect, 180, 0, 240, 60);
				}
				else
					SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
				_DrawBmp(BackRect, i*60+260, 280, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			if(text==0)PutFontOutline(260,260,WHITE, "�����ðڽ��ϱ�?");
			if(text==1)PutFontOutline(260,260,WHITE, "�³��Ͻðڽ��ϱ�?");
			if(text==2)PutFontOutline(260,260,WHITE, "�� �Ͻðڽ��ϱ�?");
			if(text==3)PutFontOutline(260,260,WHITE, "���� �Ͻðڽ��ϱ�?");
			PutFontOutline(280,300,WHITE, "��");
			PutFontOutline(330,300,WHITE, "�ƴ�");
			//���콺 ����Ʈ
			SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
			_DrawBmp(BackRect, MouseX, MouseY, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
		}
	}
	return Select;
}

void Change(int to)
{
	//�ʱ�ȭ
	int i;
	if(to==99)
	{
		for(i=1;i<=81;i++)
		{
			_DrawBar(320-i*4,240-i*3,320+i*4,240+i*3,BLACK);
			_CopyScreen(false);
		}
	}
	else
	{
		for(i=1;i<=81;i++){
			SetRect(&BackRect,320-i*4,240-i*3,320+i*4,240+i*3);
			_DrawBmp(BackRect,320-i*4,240-i*3, BmpScreen[to], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
		}
	}
}

void PreMeeting()
{
	for(int i=0;i<5;i++)
		for(int j=0;j<9;j++){
			devil.idea[i]=rand()%5;
			switch(devil.idea[i]){
				case 0: //����
					int tmp;
					for(tmp=0;tmp<4;tmp++)
					{
						if(devil.monster[tmp]>0)break;
						if(tmp==3)devil.idea[i]=9;
					}
					break;
				case 1: //¡��
					if(i==0&&devil.mp<10)devil.idea[i]=9;
					if(i==1&&devil.mp<5)devil.idea[i]=9;
					if(i==2&&devil.mp<5)devil.idea[i]=9;
					if(i==3&&devil.mp<2)devil.idea[i]=9;
					if(i==4&&devil.mp<2)devil.idea[i]=9;
					break;
				case 2: //�������
					if(devil.mp<20)devil.idea[i]=9;
					if(i==0||i==2)if(devil.lv[1]==1)devil.idea[i]=9;
					if(i==1)if(devil.lv[0]==1)devil.idea[i]=9;
					if(i==3||i==4)if(devil.lv[2]==1)devil.idea[i]=9;
					break;
				case 3: //����
					if(devil.lv2[i]>=4)devil.idea[i]=9;
					break;
			}
			if(devil.idea[i]!=9)break;
		}

	for(i=0;i<4;i++)
		if(devil.monster[i]<0)devil.monster[i]=0;
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//back.bmp", 0, 0, SYSTEM);
}

void Talk(int caracter, char* sen1, char* sen2, char* sen3)
{
	char name[20];
	switch(caracter)
	{
		case 0: strcpy(name, "���");break;
		case 1: strcpy(name, "��");break;
		case 2: strcpy(name, "����");break;
		case 3: strcpy(name, "�úҽú�");break;
		case 4: case 5: case 6: case 7: case 8: case 9: 
			strcpy(name, general[caracter-4]);break;
		case 12: strcpy(name, "�ǹ��� �糪��");break;
		case 13: strcpy(name, "�ǹ��� ����");break;
		case 14: strcpy(name, "�г��� �밥");break;
		case 15: strcpy(name, "��������");break;
		case 16: strcpy(name, "����");break;
		case 17: strcpy(name, "�渶����");break;
		case 18: strcpy(name, "�鸶����");break;
		case 19: strcpy(name, "������");break;
		case 20: strcpy(name, "�ǹ��� �׸���");break;
		case 21: strcpy(name, "����");break;
		case 22: strcpy(name, "�÷�");break;
		case 23: strcpy(name, "�̵���");break;
	}
	if(caracter<16){
		SetRect(&BackRect, (caracter%4)*160, (caracter/4)*120, (caracter%4)*160+160, (caracter/4)*120+120);
		_DrawBmp(BackRect, 0, 280, BmpScreen[8], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	}
	PutFontOutline(0,400,YELLOW,"[%s]",name);
	PutFontOutline(0,420,WHITE,sen1);
	PutFontOutline(0,440,WHITE,sen2);
	PutFontOutline(0,460,WHITE,sen3);
}

void _Story(int page, int end, int pic)
{
	if(number==0){
		if(devil.goal<6)_MidiPlay("music//story01.mid",true);
			else _MidiPlay("music//story02.mid",true);
	}
	else if(number==1)_MidiPlay("music//story02.mid",true);
	else if(number==2)_MidiPlay("music//story03.mid",true);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, BattleBack[pic], 0, 0, SYSTEM);
	Change(0);
	while(page!=(end+1))
	{
		SCENE;
		int face=atoi(snr1[page*4]);
		Talk(face,snr1[page*4+1],snr1[page*4+2],snr1[page*4+3]);
		_CopyScreen(false);
		Click();
		page++;
	}
}

int equip()
{
	int tmp, temp=0;
	bool hpup;
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//equip.bmp", 0, 0, SYSTEM);
	Change(0);

	_MidiPlay("music//equip.mid",true);

	while(!_GetKeyState(VK_ESCAPE)){
	PRC{
		SCENE;
		ani(10);
		temp++; if(temp>10)temp=0;
		SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
		_DrawBmp(BackRect, MouseX, MouseY, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		//����
		tmp=((hero.weapon-1)*220);
		_DrawBox(0+tmp+Frame,100+Frame,200+tmp-Frame,300-Frame,GREEN);
		if(MouseX>=272 && MouseX<373 && MouseY>=404 && MouseY<457)hpup=true;
			else hpup=false;
		if(hpup)_DrawBox(0+272+Frame,404+Frame,373-Frame,457-Frame,GREEN);
		_DrawBar(220,349,220+hero.hp,375,GREEN);
		PutFontOutline(270,460,WHITE,"��: %dī��",hero.money);
		//Ŭ��
		if(LeftButton)
		{
			for(int i=0;i<3;i++)
				if(MouseX>=(i*220) && MouseX<(i*220+200) && MouseY>=100 && MouseY<300)
					hero.weapon=i+1;
			if(hpup && hero.hp<200 && !temp)
			{
				if(hero.hp>=100)
				{
					if(hero.money>=2)
					{
						hero.hp++; hero.money-=2;
					}
				}
				else if(hero.money>0)
				{
					hero.hp++; hero.money--;
				}
			}
		}
		_CopyScreen(false);
	}}
	return 0;
}

void Ending1();
void Ending2();
void Ending3();

void StartGame()
{
	int i;
	bool herowin;
	int heroSnr[]={113,128,133,142,150,156,160};
	int heroback[]={0,0,0,0,8,3};
	int heroSnr2[]={174,182,185,188};
	switch(Select)
	{
		case 1: number=0;
				_Story(0,7,8);
				mode=2;	first=false;
				for(i=0;i<4;i++)devil.monster[i]=12/(i+1);
				for(i=0;i<3;i++)devil.lv[i]=5;
				for(i=0;i<5;i++)devil.lv2[i]=1;
				devil.score=100;devil.mp=20; devil.fear=20;
				devil.goal=devil.turn=0;
				PreMeeting(); Change(0);
				_MidiPlay("music//meeting.mid",true);
				break;
		case 2: number=1;
				herowin=true;
				devil.score=200;
				hero.hp=100;
				hero.weapon=1;
				hero.money=0;
				for(i=0;i<6;i++)
				{
					equip();
					_Story(heroSnr[i]+1,heroSnr[i+1],heroback[i]);
					if(spr[0].Battle(i)!=1)
					{
						_MidiPlay("music//open.mid", true);
						Title(); herowin=false;
						Change(0);mode=0;
						break;
					}
					devil.score-=(100-hero.hp);
				}
				//����
				if(herowin)
				{
					_Story(161,163,6);
					Ending1(); Quit=true;
				}
				break;
		case 3: number=2;
				_Story(164,174,0);
				_MidiStop();
				_PlayAVI("ending.avi");
				_MidiPlay("music//open.mid", true);
				Title(); herowin=false;
				Change(0);mode=0;
				break;
		case 4: number=1;
				herowin=true;
				devil.score=200;
				hero.hp=100;
				hero.weapon=1;
				hero.money=0;
				for(i=0;i<3;i++)
				{
					equip();
					_Story(heroSnr2[i]+1,heroSnr2[i+1],5);
					if(spr[0].Battle(i+6)!=1)
					{
						_MidiPlay("music//open.mid", true);
						Title(); herowin=false;
						Change(0);mode=0;
						break;
					}
					devil.score-=(100-hero.hp);
				}
				if(herowin)
				{
					_Story(189,192,5);
					Ending2(); Quit=true;
				}
				break;
	}
}

void magics(int who)
{
	int target;
	if(cara[who].kind==6||cara[who].kind==7)target=who;
		else target=(who==0)?1:0;
	PutFontOutline(cara[who].x,180,RED,"�����̴�~!!!");
	switch(cara[who].kind){
		case 1: cara[target].shield=true; _Play(22); break;
		case 3: cara[target].num-=1; _Play(23); break;
		case 6: cara[who].num+=1; _Play(21); break;
		case 7: cara[who].num+=2; _Play(21); break;
		case 8: cara[target].hp-=10; _Play(19); break;
	}
	for(int i=0;i<3;i++)
	{
		SetRect(&BackRect, i*120+880, 0, i*120+1000, 200);
		_DrawBmp(BackRect, cara[target].x, 200, BmpScreen[who+6], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		_CopyScreen(false);
		_Delay(200);
	}
	cara[who].mp=0;
	if(who!=target){
		cara[target].state=3;
		cara[target].dam=60;
	}
}

int VS(int force, int enemy, int who1, int who2, bool control)
{
	DDBLTFX fx;
	RECT Srect, Drect;

	fx.dwSize=sizeof(fx);
	fx.dwDDFX=DDBLTFX_MIRRORLEFTRIGHT;
	
	bool Key[2];
	int i, j, k, l, sndd=0, damage, fuzz, kind, dash[8]={0,0,0,0,0,0,0,0}, magic=-1;
	for(i=0;i<4;i++)combo[i]=1;
	cara[1].ctr=control;
	cara[0].hp=monster[who1].hp;cara[1].hp=monster[who2].hp;
	cara[0].num=devil.monster[who1];cara[1].num=devil.monster[who2];
	cara[0].mp=cara[1].mp=0;
	cara[0].kind=force; cara[1].kind=enemy;
	cara[0].x=520; cara[1].x=0;
	cara[0].state=cara[1].state=cara[0].dam=cara[1].dam=0;
	cara[0].poison=false; cara[1].poison=false; cara[0].shield=false; cara[1].shield=false;
	if(boss==1)cara[1].poison=true;
	if(boss==1&&devil.lv2[1]>=4)cara[1].shield=true;
	if(cara[1].ctr)BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//back1.bmp", 0, 0, SYSTEM);
	int time=999;
	key=true;
	BmpScreen[6] = DDLoadBitmap( DirectOBJ, Carac[cara[0].kind], 0, 0, SYSTEM);
	BmpScreen[7] = DDLoadBitmap( DirectOBJ, Carac[cara[1].kind], 0, 0, SYSTEM);
	DDSetColorKey( BmpScreen[6], RGB(0,0,255) );
	DDSetColorKey( BmpScreen[7], RGB(0,0,255) );

	while(cara[0].num>0&&cara[1].num>0&&time>0){
	PRC{
		//�浹�˻�
		for(i=0;i<2;i++)
		{
			j=(i==0)?1:0;k=(i==0)?0:1;l=(i==0)?-1:1;
			if(!j)damage=Matt[cara[j].kind]*(6-devil.lv[0])-Mdef[cara[k].kind];
				else damage=Matt[cara[j].kind]-Mdef[cara[k].kind]*(6-devil.lv[1]);
			if(damage<1)damage=1;

			//�Ϲݰ���
			if(cara[j].state==1&&cara[j].dam==13&&(cara[k].x*l)>((cara[j].x-140+(280*j))*l))
			{
				if(cara[k].state!=2)
				{
					if(cara[k].kind!=9)
					{
						cara[k].hp-=damage;
						cara[k].state=3;
						if(cara[k].kind!=10)cara[k].dam=Max(cara[k].dam,9);
						if(combo[j*2+1]>0)combo[j*2]++;
						combo[j*2+1]=30;
						_Play(8);
					}
				}
				else
				{
					_Play(10);
				}
			}
			//�뽬����
			if(cara[j].state==5&&cara[j].dam==13&&(cara[k].x*l)>((cara[j].x-140+(280*j))*l))
			{
				if(cara[k].state!=2)
				{
					cara[k].hp-=damage*2;
					cara[k].state=3;
					if(cara[k].kind!=10)cara[k].dam=Max(cara[k].dam,18);
					if(combo[j*2+1]>0)combo[j*2]++;
					combo[j*2+1]=30;
					_Play(8);
				}
				else
				{
					_Play(10);
				}
			}
			//ȸ���� �ϰ�
			if(cara[j].state==4&&cara[j].dam==1&&(cara[k].x*l)>((cara[j].x-140+(280*j))*l))
			{
				_Play(11);
				if(cara[j].kind==3)damage*=3;
				if(cara[j].kind==4)damage*=2;
				if(cara[j].kind==5)damage*=4;
				cara[k].hp-=damage;
				if(cara[j].kind==1)cara[j].hp+=damage;
				cara[k].state=3;
				if(cara[j].kind==2)cara[k].dam=Max(cara[k].dam,200);
					else cara[k].dam=Max(cara[k].dam,60);
				if(cara[j].kind==0)cara[k].poison=true;
				if(cara[j].kind==6||cara[j].kind==7)cara[i].num--;
				if(cara[j].kind==8)cara[j].mp=185;
				if(cara[k].kind==10)cara[k].dam=0;
				_Play(8);
			}
		}		
		//������
		for(i=0;i<2;i++){
			if(cara[i].hp<=0){
				cara[i].num--;
				cara[i].hp+=25;
			}
			if(cara[i].hp>25)cara[i].hp=25;
		}
		//����
		SCENE;
		for(j=0;j<2;j++){
			_DrawBar(((j==0)?455:0),35,((j==0)?455:0)+cara[j].mp,39,RGB2(60+cara[j].mp,0,195-cara[j].mp));
			for(i=0;i<25;i++){
				if(i<cara[j].hp) SetRect(&BackRect, 2, 140, 19, 155);
					else SetRect(&BackRect, 19, 140, 36, 155);
				_DrawBmp(BackRect, i*7+((j-1)*-455), 20, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			PutFontOutline((j-1)*-455,0,WHITE,"%d",cara[j].num);
			if(cara[j].poison){
				SetRect(&BackRect, 53, 134, 75, 159);
				_DrawBmp(BackRect, (j-1)*-455, 40, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			if(cara[j].shield){
				SetRect(&BackRect, 75, 134, 97, 159);
				_DrawBmp(BackRect, (j-1)*-455+22, 40, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
		}
		//����
		//1P
		if(!cara[0].state)
		{
			Key[0]=false;
			if(_GetKeyState(VK_LEFT)&&!Key[0]){
				if(dash[2]>0)dash[6]=1;
				j=(dash[6]>0)?3:1;
				for(i=0;i<j;i++){
					if(cara[0].x<(cara[1].x+120)){
						if(cara[1].x<=0);
						else{
							cara[0].x--;
							cara[1].x--;
						}
					}
					else cara[0].x--;
				}
				MSND;
				if(dash[0]>0)dash[1]=15;
				Key[0]=true;
			}
			if(_GetKeyState(VK_RIGHT)&&!Key[0]){
				if(dash[2]>0)dash[6]=1;
				j=(dash[6]>0)?3:1;
				for(i=0;i<j;i++){
					if(cara[0].x<520)cara[0].x++;
				}
				MSND;
				if(dash[0]>0)dash[1]=15;
				Key[0]=true;
			}
			if(_GetKeyState(VK_DOWN)&&!Key[0]&&!cara[0].shield)
			{
				cara[0].state=2;
				cara[0].dam=29;
				Key[0]=true;
			}
			if(!Key[0]){
				if(dash[1]>0)dash[2]=15;
					else dash[0]=15;
				dash[6]=0;
			}
			if(_GetKeyState(VK_RETURN)){
				if(!key)
				{
					if(dash[6]>0)cara[0].state=5;
						else cara[0].state=1;
					cara[0].dam=14;
					if(boss==0){if(!(rand()%(10-devil.lv2[0]*2)))cara[0].state=4;}
						else if(!(rand()%10))cara[0].state=4;
					key=true;
					_Play(9);
				}
				if(cara[0].kind==1||cara[0].kind==3||cara[0].kind==6||cara[0].kind==7||cara[0].kind==8)
				{
					if(cara[0].mp<185)cara[0].mp++;
						else magic=0;
				}
			}
			else{
				key=false;
				if(cara[0].mp>0)cara[0].mp--;
			}
		}
		if(cara[0].state==3&&cara[0].x<520)cara[0].x++;
		//2P
		if(!cara[1].ctr){
			fuzz=rand()%30; 
			if(cara[1].kind>8)kind=0;
				else kind=cara[1].kind-4;
			if(!cara[1].state){
				if(cara[0].state==4&&fuzz<(kind*5)&&cara[1].x>0)cara[1].x--;
				else if(cara[0].state==1&&fuzz<(kind*5)&&cara[1].x+140>cara[0].x&&!cara[1].shield){
					cara[1].state=2;
					cara[1].dam=29;
				}
				else if((cara[1].x+120+(kind*4))>cara[0].x){
					cara[1].state=1;
					cara[1].dam=14;
					if(!(rand()%10))cara[1].state=4;
				}
				else if(kind<2)cara[1].x+=(kind*2+1);
				else if(kind>=2&&kind<=3&&cara[1].num>5)cara[1].x+=3;
				else if(cara[1].mp<185)cara[1].mp++;
				else magic=1;
			}
			if(cara[1].state==3&&cara[1].x>0)cara[1].x--;
		}
		else //2P�� ����
		{
			if(!cara[1].state)
			{
				Key[1]=false;
				if(_GetKeyState('D')&&!Key[1]){
					if(dash[5]>0)dash[7]=1;
					j=(dash[7]>0)?3:1;
					for(i=0;i<j;i++){
						if(cara[1].x>(cara[0].x-120)){
							if(cara[0].x>=520);
							else{
								cara[0].x++;
								cara[1].x++;
							}
						}
						else cara[1].x++;
					}
					MSND2;
					if(dash[3]>0)dash[4]=15;
					Key[1]=true;
				}
				if(_GetKeyState('A')&&!Key[1]){
					if(dash[5]>0)dash[7]=1;
					j=(dash[7]>0)?3:1;
					for(i=0;i<j;i++){
						if(cara[1].x>0)cara[1].x--;
					}
					MSND2;
					if(dash[3]>0)dash[4]=15;
					Key[1]=true;
				}
				if(_GetKeyState('X')&&!Key[1]&&!cara[1].shield)
				{
					cara[1].state=2;
					cara[1].dam=29;
					Key[1]=true;
				}
				if(!Key[1]){
					if(dash[4]>0)dash[5]=15;
						else dash[3]=15;
					dash[7]=0;
				}
				if(_GetKeyState(VK_TAB)){
					if(!key2)
					{
						if(dash[7]>0)cara[1].state=5;
							else cara[1].state=1;
						cara[1].dam=14;
						if(!(rand()%10))cara[1].state=4;
						key2=true;
						_Play(9);
					}
					if(cara[1].kind==1||cara[1].kind==3||cara[1].kind==6||cara[1].kind==7||cara[1].kind==8)
					{
						if(cara[1].mp<185)cara[1].mp++;
						else magic=1;
					}
				}
				else{
					key2=false;
					if(cara[1].mp>0)cara[1].mp--;
				}
			}
			if(cara[1].state==3&&cara[1].x>0)cara[1].x--;
		}
		//ǥ��
		ani(15);
		for(i=0;i<2;i++){
			switch(cara[i].state)
			{
				case 0: //����
				SetRect(&Srect,Frame*120,0,Frame*120+120,200);
				SetRect(&Drect,cara[i].x,200,cara[i].x+120,400);
				break;
				case 4:
				PutFontOutline(cara[i].x,160,RED,"�̰ų� �Ծ��!");
				case 1: case 5://����
				SetRect(&Srect,140*((cara[i].dam>8)?1:0)+360,0,140*((cara[i].dam>8)?1:0)+500,200);
				SetRect(&Drect,cara[i].x-(20*(1-i)),200,cara[i].x+120+(20*i),400);
				break;
				case 2: //���
				SetRect(&Srect,640,0,760,200);
				SetRect(&Drect,cara[i].x,200,cara[i].x+120,400);
				PutFontOutline(cara[i].x,180,RED,"���� ������~!");
				break;
				case 3: //Ÿ��
				SetRect(&Srect,760,0,880,200);
				SetRect(&Drect,cara[i].x,200,cara[i].x+120,400);
				PutFontOutline(cara[i].x,180,RED,"�߾�~");
				if(cara[i].mp>0)cara[i].mp-=5;
				break;
			}
			if(!i)BackScreen -> Blt(&Drect, BmpScreen[6], &Srect, DDBLT_WAIT | DDBLT_KEYSRC, &fx);
				else BackScreen -> Blt(&Drect, BmpScreen[7], &Srect, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &fx);
		}
		if(!cara[1].ctr){
			PutFontOutline(300,0,WHITE,"%d",time/100);
			time--;
		}
		PutFontOutline(250,20,WHITE,"Best Combo : %d",combo[4]);

		if(combo[0]>1)PutFontOutline(cara[0].x,180,RED,"%d���~",combo[0]);
		if(combo[2]>1)PutFontOutline(cara[1].x,180,RED,"%d���~",combo[2]);
		if(magic>=0){magics(magic);magic=-1;}
		_CopyScreen(false);

		for(i=0;i<6;i++)if(dash[i]>0)dash[i]--;
		for(i=0;i<2;i++){
			if(combo[i*2]>combo[4])combo[4]=combo[i*2];
			if(combo[i*2+1]>0)combo[i*2+1]--;else combo[i*2]=1;
		}
		for(i=0;i<2;i++)if(cara[i].dam>0)cara[i].dam--;	else cara[i].state=0;
		if(sndd>=0)sndd--;
		else{
			sndd=30;
			for(i=0;i<2;i++)if(cara[i].poison)cara[i].hp--;
		}
		_Delay(1);
	}}
	monster[who1].hp=cara[0].hp; monster[who2].hp=cara[1].hp;
	devil.monster[who1]=cara[0].num; devil.monster[who2]=cara[1].num;
	return 0;
}

bool load(char* fname)
{
	int i;
	FILE *Fp;
	if(!(Fp=fopen(fname,"r")))return false;
	fscanf(Fp,"%c%c",&number,&first);
	for(i=0;i<3;i++)
		fscanf(Fp,"%c",&devil.lv[i]);
	for(i=0;i<5;i++)
		fscanf(Fp,"%c",&devil.idea[i]);
	for(i=0;i<4;i++)
		fscanf(Fp,"%3d",&devil.monster[i]);
	fscanf(Fp,"%4d%c%3d%3d",&devil.mp , &devil.goal, &devil.turn, &devil.score);
	for(i=0;i<5;i++)
		fscanf(Fp,"%c",&devil.lv2[i]);
	fscanf(Fp,"%c",&devil.fear);
	fclose(Fp);
	return true;
}

void save(char* fname)
{
	int i;
	FILE *Fp;
	Fp=fopen(fname,"w");
	fprintf(Fp,"%c%c",number,first);
	for(i=0;i<3;i++)
		fprintf(Fp,"%c",devil.lv[i]);
	for(i=0;i<5;i++)
		fprintf(Fp,"%c",devil.idea[i]);
	for(i=0;i<4;i++)
		fprintf(Fp,"%03d",devil.monster[i]);
	fprintf(Fp,"%04d%c%03d%03d",devil.mp , devil.goal, devil.turn, devil.score);
	for(i=0;i<5;i++)
		fprintf(Fp,"%c",devil.lv2[i]);
	fprintf(Fp,"%c",devil.fear);
	fclose(Fp);
}

int View(bool input)
{
	int answer=0, Select=0;
	key=true;
	char* fname[4]={"devil1.sav","devil2.sav","devil3.sav","devil4.sav"};
	char* snn[3]={"������ ����","����� ����","�ֽ����� ����"};
	unsigned char fnum[4];int fturn[4]; bool fabl[4];
	FILE* Fp;
	for(int i=0;i<4;i++)
	{
		fabl[i]=true;
		if(!(Fp=fopen(fname[i],"r"))){
			fabl[i]=false;
			continue;
		}
		fscanf(Fp,"%c",&fnum[i]);
		fseek(Fp,27,SEEK_SET);
		fscanf(Fp,"%3d",&fturn[i]);
		fclose(Fp);
	}
	while(!Select){
		PRC
		{
			ani(30);
			SetRect(&BackRect, 0, 160, 180, 300);
			_DrawBmp(BackRect, 230, 230, BmpScreen[9], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			//�޴�
			if(MouseX<240)MouseX=240;
			if(MouseX>386)MouseX=386;
			if(MouseY<240)MouseY=240;
			if(MouseY>336)MouseY=336;
			for(i=0; i<4; i++){
				if(fabl[i])PutFontOutline(240,240+(i*20),WHITE,"%s %d��",snn[fnum[i]],fturn[i]);
					else PutFontOutline(240,240+(i*20),BLUE,"����ִܸ��̴�!");
				if(MouseY>=240+(i*20) && MouseY<260+(i*20))
				{
					_DrawBarAlpha(240,240+(i*20),399,259+(i*20),BLUE,50);
					if(LeftButton)
					{
						if(!key){
							if(!input&&!fabl[i]);
							else{
								_Play(0);
								Select=i+1;
							}
						}
					}
					else key=false;
				}
			}
			PutFontOutline(300,340,RED,"���");
			if(MouseY>325)
			{
				_DrawBarAlpha(240,340,399,359,BLUE,50);
				if(LeftButton)
				{
					if(!key)
					{
						_Play(0);
						return 0;
					}
				}
				else key=false;
			}
			//���콺 ����Ʈ
			SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
			_DrawBmp(BackRect, MouseX, MouseY, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
		}
	}
	if(!input){
		load(fname[Select-1]);
		BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//back.bmp", 0, 0, SYSTEM);
		Change(0); _MidiPlay("music//meeting.mid",true);
		mode=2;
	}
	else save(fname[Select-1]);
	return 0;
}

void Player(int P1P, int P2P)
{
	BmpScreen[6] = DDLoadBitmap( DirectOBJ, Carac[P1P], 0, 0, SYSTEM);
	BmpScreen[7] = DDLoadBitmap( DirectOBJ, Carac[P2P], 0, 0, SYSTEM);
	DDSetColorKey( BmpScreen[6], RGB(0,0,255) );
	DDSetColorKey( BmpScreen[7], RGB(0,0,255) );
}

int OneOne()
{
	DDBLTFX fx;
	RECT Srect, Drect;

	fx.dwSize=sizeof(fx);
	fx.dwDDFX=DDBLTFX_MIRRORLEFTRIGHT;
	//����
	bool P1=false, P2=false;
	int P1P=0, P2P=0;
	int i;
	for(i=0;i<2;i++){
		monster[i].hp=25;
		devil.monster[i]=1;
	}
	for(i=0;i<3;i++)devil.lv[i]=5;
	boss=99; help=0;
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//vs.bmp", 0, 0, SYSTEM);
	Change(0); _MidiPlay("music//battle2.mid",true);
	Player(P1P, P2P);
	while(!P1||!P2){
		PRC{
			//����
			if(_GetKeyState(VK_LEFT)&&P1P>0&&!P1){
				P1P--;
				Player(P1P, P2P);
			}
			if(_GetKeyState(VK_RIGHT)&&P1P<10&&!P1){
				P1P++;
				Player(P1P, P2P);
			}
			if(_GetKeyState(VK_RETURN)&&!P1)P1=true;
			if(_GetKeyState('A')&&P2P>0&&!P2){
				P2P--;
				Player(P1P, P2P);
			}
			if(_GetKeyState('D')&&P2P<10&&!P2){
				P2P++;
				Player(P1P, P2P);
			}
			if(_GetKeyState(VK_TAB)&&!P2)P2=true;
			//ǥ��
			ani(15);
			SCENE;
			for(i=0;i<2;i++){
				SetRect(&Srect,Frame*120,0,Frame*120+120,200);
				SetRect(&Drect,500-(480*i),200,620-(480*i),400);
				if(!i)BackScreen -> Blt(&Drect, BmpScreen[6], &Srect, DDBLT_WAIT | DDBLT_KEYSRC, &fx);
					else BackScreen -> Blt(&Drect, BmpScreen[7], &Srect, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &fx);
			}
			_CopyScreen(false);
		}
	}
	VS(P1P,P2P,0,1,true);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//vs.bmp", 0, 0, SYSTEM);
	Change(0);
	for(i=0;i<2;i++){
		SetRect(&Srect,Frame*120,0,Frame*120+120,200);
		SetRect(&Drect,500-(480*i),200,620-(480*i),400);
		if(!i)BackScreen -> Blt(&Drect, BmpScreen[6], &Srect, DDBLT_WAIT | DDBLT_KEYSRC, &fx);
			else BackScreen -> Blt(&Drect, BmpScreen[7], &Srect, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &fx);
	}
	P1P=(devil.monster[1]<1)?0:1;
	P2P=(devil.monster[1]<1)?1:0;
	SetRect(&BackRect, (P2P*110)+380, 220, (P2P*110)+490, 300);
	_DrawBmp(BackRect, 20, 200, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	SetRect(&BackRect, (P1P*110)+380, 220, (P1P*110)+490, 300);
	_DrawBmp(BackRect, 500, 200, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	_CopyScreen(false);
	_Delay(2000);
	Title();
	Change(0); _MidiPlay("music//open.mid",true);

	return 0;
}

//���Ӹ�� 0
void _MainMenu()
{
	ani(20);
	SCENE;
	//�޴�
	for(int i=0; i<8; i++){
		if(i==1&&title<1)continue;
		if(i==2&&title<2)continue;
		if(i==3&&title<3)continue;
		if(MouseX>=0 && MouseX<60 && MouseY>=(i*60) && MouseY<(i*60)+60 && LeftButton)
		{
			if(!key)
			{
				key=true;
				_Play(0);
				Select=i+1;
				SetRect(&BackRect, 180, 0, 240, 60);
			}
		}
		else
		{
			key=false;
			SetRect(&BackRect, Frame*60, 0, Frame*60+60, 60);
		}
		_DrawBmp(BackRect, 0, i*60, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	}
	if(MouseX>=330 && MouseX<364 && MouseY>=143 && MouseY<170 && LeftButton && title==0)Ending3();
	//���콺 ����Ʈ
	SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
	_DrawBmp(BackRect, MouseX, MouseY, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	_CopyScreen(false);
	if(Select){
		switch(Select)
		{
			case 1:	case 2: case 3: case 4:
				StartGame(); break;
			case 5: OneOne(); break;
			case 6: View(false);
				break;
			case 7: BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//staff.bmp", 0, 0, SYSTEM);
				Change(0); _CopyScreen(false); MouseX+=60; Click();
				Title();
				Change(0);
				break;
			case 8: if(YesNo(0)==1)Quit=true; break;
		}
		Select=0;
	}
}

void Round(int x,int y)
{
	int o,i,j,k=0;
	//����
	SCENE;
	for(i=0;i<7;i++)
		for(j=0;j<7;j++){
			SetRect(&BackRect, k*50, 84, k*50+50, 134);
			_DrawBmp(BackRect, i*50+145, j*50+65, BmpScreen[9], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			o=tile[j+1][i+1]-49;
			if(o>=0&&o<=6)
			{
				if(monster[o].kind<4)SetRect(&BackRect, o*50+240, 0, o*50+290, 60);
					else SetRect(&BackRect, 400+(BattleMal[monster[o].kind-4]*50), 160, 450+(BattleMal[monster[o].kind-4]*50), 220);
				_DrawBmp(BackRect, i*50+145, j*50+55, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				PutFontOutline(i*50+145, j*50+65,WHITE,"%d",devil.monster[o]);
				SetRect(&BackRect, 0, 140, 2, 145);
				for(int p=0;p<monster[o].hp;p++)_DrawBmp(BackRect, i*50+p*2+145, j*50+110, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				if(!monster[o].turn)PutFontOutline(i*50+180, j*50+100,WHITE,"��");
				if(monster[o].kind>3 && !BattleMal[monster[o].kind-4]){
					switch(monster[o].kind-4)
					{
						case 0:	_DrawBar(i*50+185, j*50+65,i*50+195, j*50+75,WHITE); break;
						case 1:	_DrawBar(i*50+185, j*50+65,i*50+195, j*50+75,YELLOW); break;
						case 5:	_DrawBar(i*50+185, j*50+65,i*50+195, j*50+75,RGB2(128,128,128)); break;
						case 6:	_DrawBar(i*50+185, j*50+65,i*50+195, j*50+75,BLUE); break;
					}
					_DrawBox(i*50+185, j*50+65,i*50+194, j*50+74,BLACK);
				}
			}
			if(o>=-4&&o<=-2)
			{
				SetRect(&BackRect, 180+((o+4)*40), 260, 220+((o+4)*40), 300);
				_DrawBmp(BackRect, i*50+150, j*50+70, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			k=(k==0)?1:0;
		}
	//Ŀ��
	ani(10);
	_DrawBox(x*50+95-Frame, y*50+15-Frame, x*50+144+Frame, y*50+64+Frame, GREEN);
	//���ְ�
	if(boss<3)SetRect(&BackRect, 440, 0, 490, 60);
		else SetRect(&BackRect, 490, 0, 540, 60);
	_DrawBmp(BackRect, 295, 0, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	PutFontOutline(350,25,WHITE,"���ְ�: %s",general[boss]);
	//����� ����
	if(help!=99){
		if(help<2)SetRect(&BackRect, 500, 160, 550, 220);
			else SetRect(&BackRect, 550, 160, 600, 220);
		_DrawBmp(BackRect, 295, 420, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	}
	//���
	for(i=0;i<7;i++){
		if(i<4)SetRect(&BackRect, i*50+240, 0, i*50+290, 60);
			else SetRect(&BackRect, 400+((monster[i].kind-4)/2*50), 160, 450+((monster[i].kind-4)/2*50), 220);
		if(devil.monster[i]<1&&monster[i].life)
		{
			_Play(20);
			monster[i].life=false;
			tile[monster[i].y+1][monster[i].x+1]='0';
			for(j=0;j<100;j++){
				k=(monster[i].y*50+55)-((monster[i].y*50+105)*j/100);
				_DrawBmp(BackRect, monster[i].x*50+145, k, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				_CopyScreen(false);
			}
		}
	}
	_CopyScreen(false);
}

int Move(int who, int x, int y)
{
	monster[who].turn=false;
	int i, x2, y2, dir;
	bool End, att;
	int dir1[]={-1,0,1,0}; int dir2[]={0,-1,0,1};

	End=false;
	x2=x;y2=y;dir=-1;
	//�̵������� ǥ��
	for(i=0;i<4;i++){
		if(tile[y+dir1[i]][x+dir2[i]]<48)_DrawBarAlpha((x+dir2[i])*50+95, (y+dir1[i])*50+15, (x+dir2[i])*50+144, (y+dir1[i])*50+64, YELLOW, 50);
		if(tile[y+dir1[i]][x+dir2[i]]==48)_DrawBarAlpha((x+dir2[i])*50+95, (y+dir1[i])*50+15, (x+dir2[i])*50+144, (y+dir1[i])*50+64, BLUE, 50);
		if(tile[y+dir1[i]][x+dir2[i]]>=53&&tile[y+dir1[i]][x+dir2[i]]<=55){_DrawBarAlpha((x+dir2[i])*50+95, (y+dir1[i])*50+15, (x+dir2[i])*50+144, (y+dir1[i])*50+64, RED, 50); att=true;}
	}
	_CopyScreen(false);
	while(!End){
	PRC
	{
		//�̵�
		if(_GetKeyPush(VK_ESCAPE))return 0;
		if(_GetKeyPush(VK_LEFT))dir=0;
		if(_GetKeyPush(VK_UP))dir=1;
		if(_GetKeyPush(VK_RIGHT))dir=2;
		if(_GetKeyPush(VK_DOWN))dir=3;
		if(dir>=0){
			x2=x+dir1[dir];
			y2=y+dir2[dir];
		}
		if(tile[y2][x2]<=48){
			End=true;
			monster[who].x=x2-1;
			monster[who].y=y2-1;
			if(tile[y2][x2]<=47)_Play(21);
			if(tile[y2][x2]==45)monster[who].hp=25;
			if(tile[y2][x2]==46)devil.monster[who]++;
			if(tile[y2][x2]==47)devil.score++;
			tile[y2][x2]=tile[y][x];
			tile[y][x]=48;
			_Play(1);
		}
		if(tile[y2][x2]>=53&&tile[y2][x2]<=55){
			End=true;
			VS(who,monster[tile[y2][x2]-49].kind,who,tile[y2][x2]-49, false);
			return 0;
		}
	}}
	End=false;att=false;
	x=x2;y=y2;dir=-1;
	Round(x,y);
	//���ݰ����� ǥ��
	for(i=0;i<4;i++)
		if(tile[y+dir1[i]][x+dir2[i]]>=53&&tile[y+dir1[i]][x+dir2[i]]<=55){_DrawBarAlpha((x+dir2[i])*50+95, (y+dir1[i])*50+15, (x+dir2[i])*50+144, (y+dir1[i])*50+64, RED, 50); att=true;}
	if(!att)End=true;
	_CopyScreen(false);
	while(!End){
	PRC
	{
		//����
		if(_GetKeyPush(VK_ESCAPE))End=true;
		if(_GetKeyPush(VK_LEFT))dir=0;
		if(_GetKeyPush(VK_UP))dir=1;
		if(_GetKeyPush(VK_RIGHT))dir=2;
		if(_GetKeyPush(VK_DOWN))dir=3;
		if(dir>=0){
			x2=x+dir1[dir];
			y2=y+dir2[dir];
		}
		if(tile[y2][x2]>=53&&tile[y2][x2]<=55){
			End=true;
			VS(who,monster[tile[y2][x2]-49].kind,who,tile[y2][x2]-49, false);
		}
	}}
	return 0;
}

int det(int target, int j)
{
	int x, y, d;
	x=(monster[j].x-monster[target].x);
	if(x<0)x*=-1;
	y=(monster[j].y-monster[target].y);
	if(y<0)y*=-1;
	d=x+y;
	return d;
}

int winlose()
{
	if(devil.monster[4]<=0&&devil.monster[5]<=0&&devil.monster[6]<=0)return 1;
	if(devil.monster[0]<=0&&devil.monster[1]<=0&&devil.monster[2]<=0&&devil.monster[3]<=0)return 2;
	return 0;
}

void Ending1()
{
	_MidiPlay("music//end1.mid",false);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending1.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending2.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending3.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	Change(99);
	PutFontOutline(280,200,WHITE,"���...");
	PutFontOutline(280,240,WHITE,"����� ����: %d", devil.score);
	char rank;
	if(devil.score>100)rank='P';
		else if(devil.score>90)rank='S';
		else if(devil.score>80)rank='A';
		else if(devil.score>60)rank='B';
		else if(devil.score>40)rank='C';
		else rank='D';
	PutFontOutline(280,260,WHITE,"���: %c",rank);
	_Delay(15000);
	if(title==0 && number==0){
		FILE *Fp;
		Fp=fopen("game.sav","w");
		fprintf(Fp,"%c",1);
		fclose(Fp);
	}
	else if(rank=='P' && number==1 && title!=3){
		FILE *Fp;
		Fp=fopen("game.sav","w");
		fprintf(Fp,"%c",3);
		fclose(Fp);
	}
	else if(title==1 && number==1){
		FILE *Fp;
		Fp=fopen("game.sav","w");
		fprintf(Fp,"%c",2);
		fclose(Fp);
	}
}

void Ending2()
{
	_MidiPlay("music//end2.mid",false);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending1.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending2.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending3.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending4.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending5.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//ending6.bmp", 0, 0, SYSTEM);
	Change(0);
	_Delay(15000);
	Change(99);
	PutFontOutline(280,200,WHITE,"��...");
	PutFontOutline(280,240,WHITE,"����� ����: %d", devil.score);
	char rank;
	if(devil.score>100)rank='P';
		else if(devil.score>90)rank='S';
		else if(devil.score>80)rank='A';
		else if(devil.score>60)rank='B';
		else if(devil.score>40)rank='C';
		else rank='D';
	PutFontOutline(280,260,WHITE,"���: %c",rank);
	_Delay(15000);
}

void Ending3()
{
	_Play(35);
	_Delay(100);
	FILE *Fp;
	Fp=fopen("game.sav","w");
	fprintf(Fp,"%c",3);
	fclose(Fp);
	Quit=true;
}

int _Battle()
{
	//����
	if(!first)_Story(Nstory[devil.goal],Nstory[devil.goal+1]-1,devil.goal);
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, BattleBack[devil.goal], 0, 0, SYSTEM);
	Change(0);
	if(devil.goal<4)_MidiPlay("music//battle1.mid",true);
		else if(devil.goal<7)_MidiPlay("music//battle2.mid",true);
			else _MidiPlay("music//battle3.mid",true);
	first=true; help=Help[devil.goal];
	bool turn;
	int x=1, y=1, i, j, k, End=0, target;
	for(i=0;i<9;i++)strcpy(tile[i],base[i]);
	i=rand()%5+2; j=rand()%7+1;
	tile[i][j]='/';
	for(k=0;k<3;k++){
		i=rand()%5+2; j=rand()%7+1;
		tile[i][j]='.';
	}
	for(k=0;k<6;k++){
	i=rand()%5+2; j=rand()%7+1;
	tile[i][j]='-';
	}
	for(i=0;i<4;i++){
		monster[i].x=i*2;
		monster[i].y=0;
		monster[i].hp=25;
		monster[i].kind=i;
	}
	for(i=4;i<7;i++)
	{
		devil.monster[i]=BattleMon[(i-4)*8+devil.goal];
		monster[i].x=(i-4)*2+1;
		monster[i].y=6;
		monster[i].hp=25;
		monster[i].kind=BattleMan[(i-4)*8+devil.goal];
		monster[i].turn=true;
	}
	for(i=0;i<7;i++)
		if(devil.monster[i]>0)monster[i].life=true;
			else {monster[i].life=false; tile[monster[i].y+1][monster[i].x+1]='0';}
	//����
	while(!End)
	{
		for(i=0;i<4;i++){
			if(devil.monster[i]>0)monster[i].turn=true;
				else monster[i].turn=false;
		}
		turn=false;
		while(!End&&!turn){
		PRC
		{
			if(_GetKeyPush(VK_UP)&&y>1)y--;
			if(_GetKeyPush(VK_DOWN)&&y<7)y++;
			if(_GetKeyPush(VK_LEFT)&&x>1)x--;
			if(_GetKeyPush(VK_RIGHT)&&x<7)x++;
			//��Ÿ
			int o=tile[y][x]-49;
			if(o>=0&&o<=3&&monster[o].turn)if(_GetKeyState(VK_RETURN))Move(o,x,y);
			Round(x,y);
			End=winlose();
			if(_GetKeyPush(VK_ESCAPE))if(YesNo(2)==1)End=2;
			if(!monster[0].turn&&!monster[1].turn&&!monster[2].turn&&!monster[3].turn)turn=true;
			Round(x,y);
		}}
		if(End)break;
		//100������ ������
		if(boss==2){
			_Play(17);
			k=rand()%7;
			for(i=65;i<415;i++){
			_DrawBarAlpha((k*50)+145,i,(k*50)+195,i,RED,100);
			_CopyScreen(false);
			}
			for(j=0;j<7;j++)
				if(monster[j].x==k&&monster[j].life)devil.monster[j]-=devil.lv2[2];
			Round(x,y);
		}
		//�չ̸��� ���ݸ���
		if(boss==3){
			_Play(19);
			do{
				target=rand()%3+4;
			}while(devil.monster[target]<1);
			SetRect(&BackRect, 53, 134, 75, 159);
			for(i=0;i<100;i++){
				j=((monster[target].x*50+145)-295)*i/100;
				k=(monster[target].y*50+65)*i/100;
				_DrawBmp(BackRect, 307+j, k+17, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				_CopyScreen(false);
			}
			monster[target].hp-=((6-devil.lv[2])*(devil.lv2[3])*3);
			while(monster[target].hp<1){
				monster[target].hp+=25;
				devil.monster[target]--;
			}
		}
		//�������� ȸ������
		if(boss==4){
			for(int zet=0;zet<devil.lv2[4];zet++){
				for(target=3;target>=0;target--){
					if(monster[target].hp<25&&monster[target].life)break;
				}
				if(target!=-1)
				{
					_Play(18);
					SetRect(&BackRect, 2, 140, 19, 155);
					for(i=0;i<100;i++){
						j=((monster[target].x*50+145)-295)*i/100;
						k=(monster[target].y*50+65)*i/100;
						_DrawBmp(BackRect, 307+j, k+17, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
						_CopyScreen(false);
					}
					monster[target].hp+=((6-devil.lv[2])*5);
					if(monster[target].hp>25)monster[target].hp=25;
				}
			}
		}
		Round(x,y);
		End=winlose();
		if(End)break;
		//���� ��
		for(j=4;j<7;j++)
		{
			if(!monster[j].life)continue;
			_Delay(100);
			do{
				target=rand()%4;
			}while(!monster[target].life);

			for(i=0;i<((devil.goal/7)+1);i++){
				int dir, dir1[]={-1,0,1,0}, dir2[]={0,-1,0,1};
				if(monster[j].y>monster[target].y)dir=1;
					else if(monster[j].y<monster[target].y)dir=3;
						else if(monster[j].x>monster[target].x)dir=0;
							else if(monster[j].x<monster[target].x)dir=2;
				if(tile[monster[j].y+1+dir2[dir]][monster[j].x+1+dir1[dir]]<=48)
				{
					if(tile[monster[j].y+1+dir2[dir]][monster[j].x+1+dir1[dir]]==45)monster[j].hp=25;
					if(tile[monster[j].y+1+dir2[dir]][monster[j].x+1+dir1[dir]]==46)devil.monster[j]++;
					if(tile[monster[j].y+1+dir2[dir]][monster[j].x+1+dir1[dir]]==47)if(devil.score>0)devil.score--;
					tile[monster[j].y+1+dir2[dir]][monster[j].x+1+dir1[dir]]=tile[monster[j].y+1][monster[j].x+1];
					tile[monster[j].y+1][monster[j].x+1]='0';
					monster[j].x+=dir1[dir];
					monster[j].y+=dir2[dir];
					_Delay(500);
					_Play(1);
				}
				Round(x,y);
			}
			if(det(0,j)==1&&devil.monster[0]>0)VS(0,monster[j].kind,0,j, false);
			else if(det(1,j)==1&&devil.monster[1]>0)VS(1,monster[j].kind,1,j, false);
			else if(det(2,j)==1&&devil.monster[2]>0)VS(2,monster[j].kind,2,j, false);
			else if(det(3,j)==1&&devil.monster[3]>0)VS(3,monster[j].kind,3,j, false);
			Round(x,y);
			End=winlose();
			if(End)break;
		}
		if(End)break;
		//�鸶������ ȸ������
		if(help==0){
			for(target=6;target>=4;target--){
				if(monster[target].hp<25&&monster[target].life)break;
			}
			if(target!=3)
			{
				_Play(18);
				SetRect(&BackRect, 2, 140, 19, 155);
				for(i=0;i<100;i++){
					j=((monster[target].x*50+145)-295)*i/100;
					k=(445-(monster[target].y*50+65))*i/100;
					_DrawBmp(BackRect, 307+j, 463-k, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
					_CopyScreen(false);
				}
				monster[target].hp+=devil.goal;
				if(monster[target].hp>25)monster[4].hp=25;
			}
		}
		//�渶������ ���ݸ���
		if(help==1||help==2){
			_Play(19);
			do{
				target=rand()%4;
			}while(devil.monster[target]<1);
			SetRect(&BackRect, 53, 134, 75, 159);
			for(i=0;i<100;i++){
				j=((monster[target].x*50+145)-295)*i/100;
				k=(445-(monster[target].y*50+65))*i/100;
				_DrawBmp(BackRect, 307+j, 463-k, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				_CopyScreen(false);
			}
			monster[target].hp-=(5*help);
			if(monster[target].hp<1){
				monster[target].hp+=25;
				devil.monster[target]--;
			}
		}

	}
	SetRect(&BackRect, 0, 160, 180, 300);
	_DrawBmp(BackRect, 230, 180, BmpScreen[9], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	SetRect(&BackRect, (End-1)*110+380, 220, (End-1)*110+490, 300);
	_DrawBmp(BackRect, 265, 210, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	_CopyScreen(false);
	_Play(End+14);
	if(End==1){
		devil.goal++;
		first=false;
		if(devil.goal>7)mode=3;
		if(devil.lv2[boss]<4)devil.lv2[boss]++;
		devil.score+=5;
	}
	else if(devil.score>0)devil.score--;

	_Delay(2000);
	if(End==1)
	{
		switch(devil.goal)
		{
			case 2: _Story(69,113,8); break;
			case 5: _Story(38,43,4); break;
			case 7: _Story(44,57,5); break;
			case 8:
			_Story(58,68,5);
			Ending1();
			Quit=true; mode=0; break;
		}
	}
	if(devil.goal<8)_MidiPlay("music//meeting.mid",true);
	return 0;
}

int kill()
{
	int time=4999, die=0, pow=0, many=5, i;
	int manx[5], manl[5], mand[5];
	for(i=0;i<many;i++){
		manl[i]=rand()%300; mand[i]=rand()%2;
		manx[i]=rand()%620;
	}
	BmpScreen[0] = DDLoadBitmap(DirectOBJ, "DATA//back1.bmp", 0, 0, SYSTEM);
	_MidiPlay("music//hunt.mid", true);

	while(time>0){
	PRC
	{
		ani(20);
		SCENE;
		for(i=0;i<many;i++)
		{
			if(manl[i]>0)manl[i]--;
			if(!(rand()%100))mand[i]=(mand[i]==0)?1:0;
			if(manx[i]<=0)mand[i]=1;
			if(manx[i]>=619)mand[i]=0;
			if(mand[i])manx[i]++;
			if(!mand[i])manx[i]--;
			if(!manl[i])
			{
				SetRect(&BackRect, 380, 160+Frame*20, 400, 180+Frame*20);
				_DrawBmp(BackRect, manx[i], 380+i*20, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
		}
		//���콺 ����Ʈ
		if(pow<100)_DrawBox(MouseX-(5+Frame),MouseY-(5+Frame),MouseX+(5+Frame),MouseY+(5+Frame),RED);
			else _DrawBox(MouseX-(5+Frame),MouseY-(5+Frame),MouseX+(5+Frame),MouseY+(5+Frame),BLUE);
		_Pixel(MouseX, MouseY, BLUE);

		if(LeftButton&&pow==100){
			_Play(25);
			pow=0;
			for(i=0;i<many;i++){
				if(MouseX>=manx[i] && MouseX<=manx[i]+20 && MouseY>=i*20+380 && MouseY<=i*20+400 && !manl[i])
				{
					_Play(24);
					die++; manl[i]=rand()%300;
				}
			}
		}

		PutFontOutline(0,0,WHITE,"�ð�: %d",time/100);
		PutFontOutline(100,0,WHITE,"�θ���: %d",die);
		_DrawBar(300,0,300+pow,20,RED);
		_DrawBox(300,0,400,20,WHITE);
		time--; if(pow<100)pow++;
		_CopyScreen(false);
	}}

	SetRect(&BackRect, 0, 160, 180, 300);
	_DrawBmp(BackRect, 230, 170, BmpScreen[9], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	PutFontOutline(290,230,YELLOW,"����+%d",die);
	_MidiPlay("music//meeting.mid", true);
	_CopyScreen(false);
	_Delay(3000);

	return die;
}

void think(int i)
{
	char next[40];
	//���� �˻�
	if(devil.goal==0)strcpy(next,"����");
	if(devil.goal==1)strcpy(next,"Į�����");
	if(devil.goal==2)strcpy(next,"������ ��");
	if(devil.goal==3)strcpy(next,"ī���縷");
	if(devil.goal==4)strcpy(next,"����� ��");
	if(devil.goal>=5)strcpy(next,"��");
	strcat(next,"���� �����ֽʽÿ�.");
	switch(devil.idea[i]){
		case 0: //����
			if(i==0)Talk(i+4,"�����̾� ���� ���並 �ø� ���ϴ�!",next,"");
			if(i==1)Talk(i+4,"űűű~ �ΰ���鿡�� ������ �����ݽô�.",next,"(��� �ΰ��� �� ���� ����� �ִµ�...)");
			if(i==2)Talk(i+4,"���ݸ���� ���� �ֽʽÿ�.",next,"�� ���� 50���ηε� �̱� �ڽ��� �ֽ��ϴ�.");
			if(i==3)Talk(i+4,"ȣȣȣ, �ΰ��༮�� ȥ�� ���״�,",next,"");
			if(i==4)Talk(i+4,"......",next,"");
			break;
		case 1: //¡��
			if(i==0)Talk(i+4,"���縦 ��ƾ� �ڽ��ϴ�.","���� ��������� ��� ���ڽ��ϴ�.","");
			if(i==1)Talk(i+4,"������, �ĸ����߸� ���� ����սô�!","��������!","(�� ������ �ʹ� �ߺ���... ���ľ���!");
			if(i==2)Talk(i+4,"���簡 �ʿ��մϴ�.","ū������ ������ �� �ֽʽÿ�.","");
			if(i==3)Talk(i+4,"�氥 ���縦 �����ô�!","�ܶ�~ �ܶ� ������. ȣȣȣȪ!","");
			if(i==4)Talk(i+4,"�氥 ���簡 �� �ʿ��ϰ���...","","");
			break;
		case 2: //�������
			if(i==0)Talk(i+4,"�츮 ���ձ��� ������ ���� �������� �ϴ±���.","","");
			if(i==1)Talk(i+4,"������ �ְ��Դϴ�. �ΰ������ �����������!","���ݱ���� �����մϽô�!","(�� �ʹ� �� �ƾ�. ���ϰ� ��ƾ� �ϴµ�...)");
			if(i==2)Talk(i+4,"���ձ� ������� ���Խô�.","","");
			if(i==3)Talk(i+4,"������ �߿��մϴ�. ����!","�� ���� ��������� �����սô�.","");
			if(i==4)Talk(i+4,"���ձ� ��������� �� �� �������� �ؾ߰���.","","");
			break;
		case 3: //����
			if(i==0)Talk(i+4,"��� ���� �� �ϰ� ���ڽ��ϴ�.","","");
			if(i==1)Talk(i+4,"������ ������ �ʿ��մϴ�!","�� �������� �ͽ��ϴ�!","(��� �� �����غ��� �ҿ���µ�, �ٺ�~ �ٺ�~)");
			if(i==2)Talk(i+4,"���� 100���θ� 200���η� ����� �ͽ��ϴ�.","������ ���� ���ֽʽÿ�.","");
			if(i==3)Talk(i+4,"�������� ������ �� �����ϴ�.","��𰡼� �ֳ� �ϳ� ��� �Ծ�� �ڽ��ϴ�.","");
			if(i==4)Talk(i+4,"...���� ������ ������ �մϴ�.","","");
			break;
		case 4: //����
			if(i==0)Talk(i+4,"�ΰ����� ������ ��ƿ��ô� ���� ���ڽ��ϴ�.","","");
			if(i==1)Talk(i+4,"�ΰ������ ���� ���� �� �����ϴ�.","������ �׿������ô�! ũ������~","(�θ��� �����Ѱž�. ����~)");
			if(i==2)Talk(i+4,"�ΰ����� 50���θ� ���ֹ��� ���� �ݽô�!","","");
			if(i==3)Talk(i+4,"������ ������ ����� �����ô�.","���� �ΰ����� �л��սô�!","");
			if(i==4)Talk(i+4,"...���� ���� ������ �ʿ��մϴ�.","","");
			break;
		case 9: //�޽�
			if(i==0)Talk(i+4,"�̹��� ���鼭 ��ȸ�� �븮�� ���� ���ڱ���.","","");
			if(i==1)Talk(i+4,"��, ���� �� ���� ����!","�̹� ���� �׳� �ѱ�ô�!","(���� ���մԲ� �̷� ������...)");
			if(i==2)Talk(i+4,"�޽� �ʿ�. �̹��� ���ϴ�.","","");
			if(i==3)Talk(i+4,"�̹� ���� �׳� ������ �Ǻι̿��̳� �ؾ߰ڽ��ϴ�. ��!","","");
			if(i==4)Talk(i+4,"������ ������.","","");
			break;
	}
}

void turn(int who)
{
	int i;
	SetRect(&BackRect, 180, 160, 380, 260);
	_DrawBmp(BackRect, 220, 190, BmpScreen[9], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	if(who==5){
		_Play(12);
		PutFontOutline(230,200,WHITE,"�� �ϸ� �ٺ����� ����");
		PutFontOutline(230,220,YELLOW,"��û��+1");
		PutFontOutline(230,240,YELLOW,"�����+1");
	}
	else
	switch(devil.idea[who])
	{
		case 0:
			_Play(2);
			PutFontOutline(230,200,WHITE,"���￡ ����");break;
		case 1:
			_Play(3);
			switch(who)
			{
				case 0:
					PutFontOutline(230,200,WHITE,"�����縦 ���");
					PutFontOutline(230,220,YELLOW,"������+%d",devil.mp/10);
					devil.monster[3]+=devil.mp/10;devil.mp%=10;
					break;
				case 1:
					PutFontOutline(230,200,WHITE,"�ĸ����߸� ���");
					PutFontOutline(230,220,YELLOW,"�ĸ�����+%d",devil.mp/5);
					devil.monster[1]+=devil.mp/5;devil.mp%=5;
					break;
				case 2:
					PutFontOutline(230,200,WHITE,"ū������ ���");
					PutFontOutline(230,220,YELLOW,"ū����+%d",devil.mp/5);
					devil.monster[2]+=devil.mp/5;devil.mp%=5;
					break;
				case 3: case 4:
					PutFontOutline(230,200,WHITE,"�氥�� ���");
					PutFontOutline(230,220,YELLOW,"�氥+%d",devil.mp/2);
					devil.monster[0]+=devil.mp/2;devil.mp%=2;
					break;
			}
			break;
		case 2:
			_Play(14);
			switch(who)
			{
				case 0:case 2:
					PutFontOutline(230,200,WHITE,"����� ����");
					PutFontOutline(230,220,YELLOW,"���ձ� �������+1 ");
					devil.lv[1]--;devil.mp-=20;
					break;
				case 1:
					PutFontOutline(230,200,WHITE,"���ݱ�� ����");
					PutFontOutline(230,220,YELLOW,"���ձ� ���ݱ����+1 ");
					devil.lv[0]--;devil.mp-=20;
					break;
				case 3: case 4:
					PutFontOutline(230,200,WHITE,"������� ����");
					PutFontOutline(230,220,YELLOW,"���ձ� ���������+1 ");
					devil.lv[2]--;devil.mp-=20;
					break;
			}
			break;
		case 3:
			_Play(21);
			PutFontOutline(230,200,WHITE,"������ �Ϸ� ��");
			PutFontOutline(230,220,YELLOW,"���� ��!!!");
			devil.lv2[who]++;
			break;
		case 4:
			_Play(23);
			PutFontOutline(230,200,WHITE,"�ΰ������ �Ϸ� ��");
			break;
		case 9:
			_Play(13);
			PutFontOutline(230,200,WHITE,"�׳� ��");
			break;
	}
	for(i=230;i<410;i++){
		_DrawBar(i,260,i+1,280,RGB2(i-200,0,0));
		_Delay(10);
		_CopyScreen(false);
	}
	boss=who;
	if(who!=5 && !devil.idea[who])_Battle();
	if(devil.idea[who]==4)devil.fear+=kill();
	if(devil.fear>100)devil.fear=100;
	devil.mp+=((devil.fear/10)+(5-devil.lv[2]));
	devil.turn++; if(devil.score>0)devil.score--; if(devil.fear>0)devil.fear--;
	PreMeeting();
}
void CBattle::SetData(int num, char* Name, int X, int Y, int SizeX, int SizeY, int Speed, int Hp, int At)
{
	name[num]=Name;
	mondat[num][0]=X;
	mondat[num][1]=Y;
	mondat[num][2]=SizeX;
	mondat[num][3]=SizeY;
	mondat[num][4]=Speed;
	monHP[num]=Hp;
	monAT[num]=At;
}

void CBattle::PreBattle()
{
	/**************************************************************
						���������� �ʱ�ȭ
	**************************************************************/
	int X=0; time=100;
	//����, �̸�, XY��ǥ, XYũ��, �ӵ�, ü��, ��
	SetData(0,"�Ƶ鳻��",0,10,40,60,3,100,X);
	SetData(1,"�ǹ��� �ٺ�",0,75,80,75,1,40,1);
	SetData(2,"��Ʈ",120,385,50,50,3,X,X);
	SetData(3,"��",150,325,35,35,8,X,X);
	SetData(4,"���ź",0,0,10,10,3,X,X);
	SetData(5,"������",240,100,20,50,5,X,X);
	SetData(6,"�ǹ��� �糪��",0,150,60,100,2,100,X);
	SetData(7,"�г��� �밥",0,250,80,75,1,100,1);
	SetData(8,"�ǹ��� �׸���",30,0,10,10,5,50,X);
	SetData(9,"��",0,385,40,60,X,10,X);
	SetData(10,"�����",0,325,50,55,3,20,X);
	SetData(11,"��緹����",0,380,100,5,30,50,X);
	SetData(12,"ȸ����",180,210,40,40,10,20,1);
	SetData(13,"��������",120,0,40,75,5,100,X);
	SetData(14,"����",120,435,50,75,3,100,3);
	SetData(15,"�ʽ´�",240,0,20,50,6,X,X);
	SetData(16,"����",150,585,45,100,X,25,X);
	SetData(17,"�÷�",0,575,50,60,3,100,5);
	SetData(18,"�̵���",0,445,30,60,3,100,X);
	SetData(19,"����Ÿ",0,505,30,60,3,100,X);
	SetData(20,"����",240,50,20,20,5,X,X);
	SetData(21,"��",180,510,55,75,5,X,X);
	SetData(22,"��",0,385,40,60,X,10,X);
	//����
	int OdirX[9]={-1,-1,-1,0,1,1,1,0,0}, OdirY[9]={1,0,-1,-1,-1,0,1,1,0};
	for(int i1=0;i1<9;i1++)
	{
		dirX[i1]=OdirX[i1];
		dirY[i1]=OdirY[i1];
	}
	//����Ƽ
	int Oenemy[ENEMYS][4]={
		{1,0,0,0},{6,0,0,0},{7,0,0,0},
		{8,0,0,0},{13,0,0,0},{14,0,0,0},
		{16,16,16,16},{17,0,0,0},{18,0,0,19}};
	for(i1=0;i1<ENEMYS;i1++)
		for(int i2=0;i2<6;i2++)enemy[i1][i2]=Oenemy[i1][i2];
}

int CBattle::Key()
{
	int direct=8;
	bool left, up, right, down;
	left=up=right=down=false;
	if(_GetKeyState(VK_LEFT))left=true;
	if(_GetKeyState(VK_UP))up=true;
	if(_GetKeyState(VK_RIGHT))right=true;
	if(_GetKeyState(VK_DOWN))down=true;
	if(left && !up && !right && down)direct=0;
	if(left && !up && !right && !down)direct=1;
	if(left && up && !right && !down)direct=2;
	if(!left && up && !right && !down)direct=3;
	if(!left && up && right && !down)direct=4;
	if(!left && !up && right && !down)direct=5;
	if(!left && !up && right && down)direct=6;
	if(!left && !up && !right && down)direct=7;

	return direct;
}

int CBattle::CenterX(int s1, int s2)
{
	return spr[s1].x+spr[s1].sizeX/2-mondat[s2][2]/2;
}

int CBattle::CenterY(int s1, int s2)
{
	return spr[s1].y+spr[s1].sizeY/2-mondat[s2][3]/2;
}

bool CBattle::Crush(int x, int y)
{
	int left1=x2, left2=spr[y].x;
	int top1=y2, top2=spr[y].y;
	int right1=x2+spr[x].sizeX, right2=spr[y].x+spr[y].sizeX;
	int bottom1=y2+spr[x].sizeY, bottom2=spr[y].y+spr[y].sizeY;
	if(left1<right2 && right1>left2 && top1<bottom2 && bottom1>top2)
		return true;
	else
		return false;
}

bool CBattle::Bump(int i1)
{
	bool bump=false;
	if(Crush(i1,0)&&!spr[0].dam){
		if(spr[0].hp>0)spr[0].hp-=spr[i1].att;
		spr[0].dam=15;
		bump=true;
	}
	return bump;
}

void CBattle::PutSpr()
{
	ani(15);
	bool ok;
	int putN[SMAX], tmp;
	for(int i1=0;i1<SMAX;i1++)putN[i1]=i1;
	//����������
	do{
		ok=false;
		for(i1=0; i1<SMAX-1; i1++)
		{
			if((spr[putN[i1]].y+spr[putN[i1]].sizeY)>(spr[putN[i1+1]].y+spr[putN[i1+1]].sizeY)){
				tmp=putN[i1+1];
				putN[i1+1]=putN[i1];
				putN[i1]=tmp;
				ok=true;
			}
		}
	}while(ok);
	for(i1=0;i1<SMAX;i1++)
	{
		if(!spr[putN[i1]].life || spr[putN[i1]].dam%2)continue;
		RFrame=(spr[putN[i1]].temp/15)%4;
		Frame=(RFrame==3)?1:RFrame;
		if(spr[putN[i1]].kind==21)Frame=0;
		if(spr[putN[i1]].kind!=3)//�̽��Ҷ� ����
			SetRect(&BackRect, spr[putN[i1]].sizeX*Frame+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1], spr[putN[i1]].sizeX*(Frame+1)+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1]+spr[putN[i1]].sizeY);
		else
			SetRect(&BackRect, spr[putN[i1]].sizeX*RFrame+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1], spr[putN[i1]].sizeX*(RFrame+1)+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1]+spr[putN[i1]].sizeY);
		_DrawBmp(BackRect, spr[putN[i1]].x, spr[putN[i1]].y, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	}
	//������
	for(i1=0;i1<5;i1++)
	{
		if(!spr[i1].life)continue;
		PutFontOutline(i1*130, 0, WHITE, name[spr[i1].kind]);
		if(spr[i1].hp>100)
			_DrawBar(i1*130, 20, i1*130+100, 35, BLACK);
		else
			_DrawBar(i1*130, 20, i1*130+spr[i1].hp, 35, RGB2(SColor1(spr[i1].hp),SColor2(spr[i1].hp),0));
		if(monHP[spr[i1].kind]>100)
			_DrawBox(i1*130, 20, i1*130+100, 35, WHITE);
		else
			_DrawBox(i1*130, 20, i1*130+monHP[spr[i1].kind], 35, WHITE);
		PutFontOutline(300,50,YELLOW,"%d",time);
	}
	//�¸���
	if(vic>0)
	{
		PutFontOutline(200,220,WHITE,"�������� �¸��߽��ϴ�!");
		PutFontOutline(200,240,WHITE,"��+%dK",time);
		vic--;
	}
	//�й��
	if(vic<0)PutFontOutline(230,190,RED,"G A M E   O V E R");
}

void CBattle::NewSpr(int n, int kind, int x, int y, int hp, int at, int dr)
{
	spr[n].life=true;
	spr[n].kind=kind;

	spr[n].x=x;
	spr[n].y=y;
	spr[n].sizeX=mondat[spr[n].kind][2];
	spr[n].sizeY=mondat[spr[n].kind][3];
	spr[n].speed=mondat[spr[n].kind][4];

	spr[n].hp=hp;
	spr[n].att=at;
	spr[n].dir=dr;
	spr[n].dam=0;
}

void CBattle::CtrSpr()
{
	if(!vic && !(temp%100) && time>0)time--;
	for(int i1=0;i1<SMAX;i1++)
	{
		if(vic==1)win=1;
		if(!spr[i1].life)continue;
		spr[i1].temp++;
		if(spr[i1].dam>0)spr[i1].dam--;
		if(spr[i1].hp<=0&&spr[i1].life){
			spr[i1].life=false;
			if(i1<5){
				_Play(27);
				ENSPR NewSpr(i2,9,CenterX(i1,9),CenterY(i1,9),100,0,8);
			}
		}
		if(!spr[1].life && !spr[2].life && !spr[3].life && !spr[4].life && !vic){
			vic=600;
			_MidiPlay("music//win.mid",false);
		}//�¸�
		if(!spr[0].life && !vic){
			vic=-1;
			_MidiPlay("music//gameover.mid",true);
		}//�й�
		x2=spr[i1].x, y2=spr[i1].y;
		//������
		if(i1==0)//�Ʊ�
		{
			x2+=(dirX[dir]*spr[i1].speed);
			y2+=(dirY[dir]*spr[i1].speed);
		}
		if(i1>=1 && i1<=4)//����
		{
			int herodir, tmp1, tmp2;
			//���ΰ��� �ִ� ��
			tmp1=spr[i1].x-spr[0].x;
			tmp2=spr[i1].y-spr[0].y;
			if(tmp1>=spr[0].sizeX&&tmp2<0)herodir=0;
			if(tmp1>=spr[0].sizeX&&tmp2>=0&&tmp2<spr[0].sizeY)herodir=1;
			if(tmp1>spr[0].sizeX&&tmp2>=spr[0].sizeY)herodir=2;
			if(tmp1<=spr[0].sizeX&&tmp1>0&&tmp2>=spr[0].sizeY)herodir=3;
			if(tmp1<=0&&tmp2>spr[0].sizeY)herodir=4;
			if(tmp1<=0&&tmp2<=spr[0].sizeY&&tmp2>0)herodir=5;
			if(tmp1<0&&tmp2<=0)herodir=6;
			if(tmp1<spr[0].sizeX&&tmp1>=0&&tmp2<=0)herodir=7;
			if(herodir<0 || herodir>7)herodir=rand()%7;

			if(spr[i1].kind==1||spr[i1].kind==7)//������ ����
			{
				if(Crush(i1,0)&&spr[0].life)Bump(i1);
				else{
					x2+=dirX[herodir]*spr[i1].speed;
					y2+=dirY[herodir]*spr[i1].speed;
				}
				if(spr[i1].kind==16 && !(spr[i1].temp%30))_Play(5);
			}
			if(spr[i1].kind==10)//�����
			{
				int golem;
				if((spr[i1].temp%800)>=600)golem=4;
				else if((spr[i1].temp%800)>=400)golem=7;
				else if((spr[i1].temp%800)>=200)golem=2;
				else golem=7;
				x2+=(dirX[golem]*spr[i1].speed);
				y2+=(dirY[golem]*spr[i1].speed);
				if(!(spr[i1].temp%50)){
					ENSPR NewSpr(i2,4,CenterX(i1,4),CenterY(i1,4),1000,2,herodir);
				}
			}
			if(spr[i1].kind==9)//��
			{
				if(!(spr[i1].temp%30)){
					for(int i3=0;i3<3;i3++){
						ENSPR NewSpr(i2,10,CenterX(i1,10),CenterY(i1,10),1000,5,i3);
					}
				}
			}
			if(spr[i1].kind==12)//ȸ����
			{
				Bump(i1);
				int fire=spr[i1].temp%8;
				if(!(spr[i1].temp%10))fire=herodir;
				x2+=(dirX[fire]*spr[i1].speed);
				y2+=(dirY[fire]*spr[i1].speed);
			}
			if(spr[i1].kind==1)//�ǹ��� �ٺ�
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(spr[i1].temp%80))spr[i1].dir=rand()%8;
				if((spr[i1].temp%500)==1)
				{
					for(int i2=1;i2<5;i2++)
						if(!spr[i2].life)break;
					if(i2<5){
						NewSpr(i2,1,spr[i1].x,spr[i1].y,monHP[1],monAT[1],0);
						_Play(12);
					}
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			if(spr[i1].kind==6)//�ǹ��� �糪��
			{
				if(spr[i1].dir!=3&&spr[i1].dir!=7)spr[i1].dir=3;
				y2+=(spr[i1].speed*dirY[spr[i1].dir]);
				if(y2<240)spr[i1].dir=7;
				if(y2>380)spr[i1].dir=3;

				if(!(spr[i1].temp%50))
				{
					for(int i2=1;i2<5;i2++)
						if(!spr[i2].life)break;
					if(i2<5){
						NewSpr(i2,12,spr[i1].x,spr[i1].y,monHP[12],monAT[12],0);
						_Play(29);
					}
				}
			}
			if(spr[i1].kind==7)//�г��� �밥
			{
				if(!(spr[i1].temp%500))_Play(26);
				if(!(spr[i1].temp%7))
				{
					ENSPR NewSpr(i2,2,CenterX(i1,2),CenterY(i1,2),1000,1,spr[i1].temp%8);
				}
			}
			if(spr[i1].kind==8)//�ǹ��� �׸���
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(spr[i1].temp%80))spr[i1].dir=rand()%8;
				if(spr[i1].x<200)spr[i1].dir=5;
				if(!(spr[i1].temp%100))
				{
					int shadow=rand()%4;
					shadow=(shadow>1)?shadow+4:shadow;
					ENSPR NewSpr(i2,shadow,CenterX(i1,4),spr[i1].y-100 ,1000,1,1);
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			if(spr[i1].kind==13)//��������
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(spr[i1].temp%80))spr[i1].dir=rand()%8;
				if(!(spr[i1].temp%200))
				{
					for(int i2=1;i2<5;i2++)
						if(!spr[i2].life)break;
					if(i2<5){
						NewSpr(i2,10,spr[i1].x,spr[i1].y,monHP[10],monAT[10],0);
						_Play(28);
					}
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			if(spr[i1].kind==14)//����
			{
				if(Crush(i1,0)&&spr[0].life)Bump(i1);
				
				if(spr[i1].dir!=3&&spr[i1].dir!=7)spr[i1].dir=3;
				y2+=(spr[i1].speed*dirY[spr[i1].dir]);
				if(y2<265)spr[i1].dir=7;
				if(y2>405)spr[i1].dir=3;
				x2-=spr[i1].speed;

				if((spr[i1].temp%200)==1)
				{
					ENSPR{
						_Play(30);
						NewSpr(i2,15,spr[i1].x,CenterY(i1,15),100,3,1);
					}
				}
				if((spr[i1].temp%500)>=50 && (spr[i1].temp%500)<60)
				{
					if((spr[i1].temp%500)==50)
					{
						_Play(17);
						laser=(rand()%100)+330;
					}
					ENSPR{
						NewSpr(i2,5,620,laser,200,1,1);
					}
				}
			}
			if(spr[i1].kind==16)//����
			{
				if(!(spr[i1].temp%160)){
					x2=rand()%(640-spr[i1].sizeX);
					y2=rand()%(480-spr[i1].sizeY);
					_Play(34);
				}
				if(!(spr[i1].temp%20)){
					int elf=spr[i1].temp%160/20;
					ENSPR NewSpr(i2,4,CenterX(i1,10),CenterY(i1,10),1000,2,elf);
				}
			}
			if(spr[i1].kind==17)//�÷�
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(spr[i1].temp%80))spr[i1].dir=rand()%8;
				if(Crush(i1,0)&&spr[0].life)Bump(i1);
				if(!(spr[i1].temp%20))
				{
					ENSPR NewSpr(i2,21,rand()%580,480,1000,2,3);
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			if(spr[i1].kind==18)//�̵���
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(spr[i1].temp%80))spr[i1].dir=rand()%8;
				if(Crush(i1,0)&&spr[0].life)Bump(i1);
				if(!(spr[i1].temp%100))
				{
					ENSPR NewSpr(i2,22,CenterX(i1,22),CenterY(i1,22),1000,3,8);
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			if(spr[i1].kind==19)//����Ÿ
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(spr[i1].temp%80))spr[i1].dir=rand()%8;
				if(Crush(i1,0)&&spr[0].life)Bump(i1);
				if(!(spr[i1].temp%10))
				{
					ENSPR NewSpr(i2,20,CenterX(i1,20),CenterY(i1,20),1000,1,(temp%40/10)*2+1);
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
		}
		if(i1>=5 && i1<15)//�Ʊ� �̻���
		{
			for(int i2=1;i2<5;i2++)
				if(Crush(i1,i2)&&spr[i1].life&&!spr[i2].dam){
					if(spr[i2].hp>0)spr[i2].hp-=spr[i1].att;
					spr[i2].dam=15;
					if(hero.weapon!=3)spr[i1].life=false;
				}
			x2=spr[i1].x+dirX[spr[i1].dir]*spr[i1].speed;
			y2=spr[i1].y+dirY[spr[i1].dir]*spr[i1].speed;
			spr[i1].hp--;
		}
		if(i1>=15)//���� �̻���
		{
			spr[i1].hp--;
			if(spr[i1].kind==9)continue;
			if(Bump(i1))if(spr[i1].kind!=22)spr[i1].life=false;
			x2=spr[i1].x+dirX[spr[i1].dir]*spr[i1].speed;
			y2=spr[i1].y+dirY[spr[i1].dir]*spr[i1].speed;
		}
		//���
		if(i1<5)
		{
			if(spr[i1].kind==14)
			{
				if(x2<(-60))x2=640;
			}
			else
			{
				x2=Max(x2,0);
				y2=Max(y2,340-spr[i1].sizeY);
			}
			x2=Min(x2,640-spr[i1].sizeX);
			y2=Min(y2,480-spr[i1].sizeY);
		}
		//else if(x2<0 || y2<0 || (x2-spr[i1].sizeX)>640 || (y2-spr[i1].sizeY)>480)
		else if(x2<0 || y2<0 || x2>640 || y2>480)
			spr[i1].life=false;
		spr[i1].x=x2; spr[i1].y=y2;
	}
}

int CBattle::Battle(int num)
{
	PreBattle();
	//���� �⺻ ����(1):�ʱ�ȭ
	for(int i1=0;i1<SMAX;i1++){
		spr[i1].life=false;
		spr[i1].dam=0;
		spr[i1].temp=0;
	}
	//(2)�Ʊ�����
	NewSpr(0,0,100,100,hero.hp,1,0);
	//(3)���� ����
	for(i1=1;i1<5;i1++){//�� ������
		int kind=enemy[num][i1-1];
		if(kind)NewSpr(i1,kind,600,(i1-1)*60+300,monHP[kind],monAT[kind],0);
	}

	win=vic=0;
		
	_MidiPlay("music//battle.mid",true);
	while (!win){PRC{
		//���
		SetRect(&BackRect, 0, 0, 640, 480);
		_DrawBmp(BackRect, 0, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
		//����
		dir=Key();
		if(_GetKeyState(VK_RETURN))
		{
			//������
			if(!spr[0].life)continue;
			if(!(temp%10))_Play(30+hero.weapon);
			int shotdir=(dir==8)?5:dir;
			ENSHOT{
				switch(hero.weapon)
				{
					case 1:	NewSpr(i2,3,CenterX(0,3),CenterY(0,3),100,1,shotdir);
						break;
					case 2:	if(i2<8)NewSpr(i2,3,CenterX(0,3),CenterY(0,3),100,1,((i2%3)+(shotdir+7))%8);
						break;
					case 3:	if(i2==5)NewSpr(i2,11,spr[0].x+25,CenterY(0,11),100,1,5);
						break;
				}
			}
		}
		if(vic==-1)
			if(_GetKeyState(VK_ESCAPE) || LeftButton)win=2;

		PutSpr();
		CtrSpr();
		_CopyScreen(false);
	}}
	if(win==1)
	{
		hero.money+=time;
		hero.hp=spr[0].hp;
		Change(99);
	}
	else{
		Change(99);
	}
	return win;
}

//���Ӹ�� 2
void _Meeting()
{
	ani(5);
	int i, j, k;
	//���
	SetRect(&BackRect, 0, 0, 640, 480);
	_DrawBmp(BackRect, 0, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	if(devil.goal<=5){
		_DrawBox(544-(devil.goal*90)-Frame,304-Frame,635+Frame,475+Frame, RED);
	}
	else{
		_DrawBox(93-Frame,303-Frame,636+Frame,476+Frame, RED);
	}
	char *land[7]={"��","����� ��","ī���縷","������ ��","Į�����","����","���ռ�"};
    for(i=0;i<7;i++)PutFontOutline(i*90,300,GREEN,land[i]);
	//����
	PutFontOutline(0,240,WHITE,"�Ѹ���: %d",devil.mp);
	PutFontOutline(0,260,WHITE,"����(���¼���): %d(%d)",devil.fear,devil.fear/10+(5-devil.lv[2]));
	PutFontOutline(0,280,BLUE,"�ҿ� �ϼ�: %d",devil.turn);
	PutFontOutline(200,240,WHITE,"���ձ� ���ݵ��: %d",devil.lv[0]);
	PutFontOutline(200,260,WHITE,"���ձ� �����: %d",devil.lv[1]);
	PutFontOutline(200,280,WHITE,"���ձ� �������: %d",devil.lv[2]);
	for(i=0;i<4;i++){
		SetRect(&BackRect, i*50+240, 0, i*50+290, 60);
		_DrawBmp(BackRect, i*50+400, 240, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		PutFontOutline(i*50+400, 240,WHITE,"%d",devil.monster[i]);
	}
	//����
	for(i=0;i<8;i++){
		j=(i+4)%4*160; k=((i+4)/4)*120;
		SetRect(&BackRect, j, k, j+160, k+120);
		j=(i%4)*160; k=(i/4)*120;
		_DrawBmp(BackRect, j, k, BmpScreen[8], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		if(i<5)PutFontOutline(j,k,WHITE,"LV: %d",devil.lv2[i]);
	}
	for(i=0;i<8;i++){
		j=(i%4)*160; k=(i/4)*120;
		if(MouseX>=j && MouseX<j+160 && MouseY>=k && MouseY<k+120)
		{
			if(LeftButton){
				if(!key)
				{
					key=true;
					if(i<5)think(i);
						else if(i==5)Talk(i+4,"��~","�������������","");
					if(i<=5)if(YesNo(1)==1)turn(i);
					if(i==6)View(true);
					if(i==7)if(YesNo(0)==1){
						_MidiPlay("music//open.mid", true);
						Title();
						Change(0);mode=0;
					}
				}
			}
			else
			{
				key=false;
				_DrawBarAlpha(j,k,j+160,k+120, BLUE, 50);
			}
		}
	}
	//����
	if(RightButton && !LeftButton)
	{
		if(MouseX>=0 && MouseX<160 && MouseY>=0 && MouseY<120){
			BOX1;
			PutFontOutline(100,200,WHITE,"�̸�: ���� / ����: �屺");
			PutFontOutline(100,220,WHITE,"���ձ� �ְ��� ����");
			PutFontOutline(100,240,WHITE,"����, �Ǹ��� �߽��Ѵ�.");
		}
		if(MouseX>=160 && MouseX<320 && MouseY>=0 && MouseY<120){
			BOX1;
			PutFontOutline(100,200,WHITE,"�̸�: ��µ�� / ����: ������");
			PutFontOutline(100,220,WHITE,"������ ����ϰ� µ���ϴ�.");
			PutFontOutline(100,240,WHITE,"������ ���� ���� ������.");
		}
		if(MouseX>=320 && MouseX<480 && MouseY>=0 && MouseY<120){
			BOX1;
			PutFontOutline(100,200,WHITE,"�̸�: 100���� / ����: �κ�");
			PutFontOutline(100,220,WHITE,"������ 50������ �������� �ο�ٰ� �Ѵ�.");
			PutFontOutline(100,240,WHITE,"�װ� 100���θ� ���� ���� �� ����� �ƹ��� ����.");
			PutFontOutline(100,260,WHITE,"�ڱ� ���δ� '���� 100���θ� ���� �� �״´�'�� �Ѵ�.");
		}
		if(MouseX>=480 && MouseX<640 && MouseY>=0 && MouseY<120){
			BOX1;
			PutFontOutline(100,200,WHITE,"�̸�: �չ̸� / ����: �渶����");
			PutFontOutline(100,220,WHITE,"�ڱⰡ ���ڴٴ� ������ ���� �Ҹ���.");
			PutFontOutline(100,240,WHITE,"������ �����Ƿ¸��� �Ϸ��̴�.");
		}
		if(MouseX>=0 && MouseX<160 && MouseY>=120 && MouseY<240){
			BOX1;
			PutFontOutline(100,200,WHITE,"�̸�: ������ / ����: �鸶����");
			PutFontOutline(100,220,WHITE,"���� ���� ���� �����̴�.");
		}
		if(MouseX>=160 && MouseX<320 && MouseY>=120 && MouseY<240){
			BOX1;
			PutFontOutline(100,200,WHITE,"�̸�: �ǹ��� �ٺ� / ����: �ٺ�");
			PutFontOutline(100,220,WHITE,"�� �״�� �ٺ��̴�. ��� ���� �ǹ�.");
		}
		if(MouseX>=0 && MouseX<100 && MouseY>=240 && MouseY<300){
			BOX1;
			PutFontOutline(100,200,WHITE,"������ ���ձ��� ��� �۾��� ���̴� �����Դϴ�.");
			PutFontOutline(100,220,WHITE,"���� ���� ������ �������� �ΰ������ �ϰų�,");
			PutFontOutline(100,240,WHITE,"��������� �������Ѿ� �մϴ�.");
		}
		if(MouseX>=200 && MouseX<340 && MouseY>=240 && MouseY<300){
			BOX1;
			PutFontOutline(100,200,WHITE,"���ձ��� ����� ����Դϴ�.");
			PutFontOutline(100,220,WHITE,"����� ����Ӹ� �ƴ϶� ���ε鿡�Ե� ������ �ݴϴ�.");
			PutFontOutline(100,240,WHITE,"Ư�� ��������� �����ϸ� ���¼����� �Ǵϴ�.");
		}
		if(MouseX>=400 && MouseX<640 && MouseY>=240 && MouseY<300){
			BOX1;
			PutFontOutline(100,200,WHITE,"���ձ��� ���� �氥, �ĸ�����, ū����, �������Դϴ�.");
			PutFontOutline(100,220,WHITE,"�氥,�ĸ�����,ū����,�������� ���ݷ�,������ �� ����");
			PutFontOutline(100,240,WHITE,"���ݷ�:1:2:1:2 ���� 1:1:2:2 �� �˴ϴ�.");
			PutFontOutline(100,260,WHITE,"���� ���ڴ� ������ ���� ��Ÿ���ϴ�.");
		}
	}
	//���콺 ����Ʈ
	SetRect(&BackRect, Frame*14, 60, Frame*14+14, 84);
	_DrawBmp(BackRect, MouseX, MouseY, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	_CopyScreen(false);
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    if ( !_GameMode(hInstance, nCmdShow, 640, 480, 16, true) ) return FALSE;

	//�ѱ� �ʱ�ȭ
    InitXddFont( "HANGUL.FNT", "ENGLISH.FNT" );
    SetFontPattern( NORMAL_PATTERN );

	FILE *Fp;
	Fp=fopen("game.sav","r");
	fscanf(Fp,"%c",&title);
	fclose(Fp);
	//�׷��� �ʱ�ȭ
	Title();
	BmpScreen[2] = DDLoadBitmap( DirectOBJ, "DATA//etc.bmp", 0, 0, SYSTEM);
	BmpScreen[8] = DDLoadBitmap( DirectOBJ, "DATA//face.bmp", 0, 0, SYSTEM);
	BmpScreen[9] = DDLoadBitmap( DirectOBJ, "DATA//sprites.bmp", 0, 0, SYSTEM);
	DDSetColorKey( BmpScreen[2], RGB(0,0,255) );
	DDSetColorKey( BmpScreen[8], RGB(0,0,255) );
	DDSetColorKey( BmpScreen[9], RGB(0,0,255) );

	//�����ʱ�ȭ
	_PlayAVI("open.avi");
	_MidiPlay("music//open.mid", true);
	if ( SoundCard )
		{
			Sound[0] = SndObjCreate(SoundOBJ,"Sound//select.WAV",2);
			Sound[1] = SndObjCreate(SoundOBJ,"Sound//move.WAV",2);
			Sound[2] = SndObjCreate(SoundOBJ,"Sound//war.WAV",2);
			Sound[3] = SndObjCreate(SoundOBJ,"Sound//foot.WAV",2);
			Sound[4] = SndObjCreate(SoundOBJ,"Sound//tack.WAV",2);
			Sound[5] = SndObjCreate(SoundOBJ,"Sound//jump.WAV",2);
			Sound[6] = SndObjCreate(SoundOBJ,"Sound//stone.WAV",2);
			Sound[7] = SndObjCreate(SoundOBJ,"Sound//toca.WAV",2);
			Sound[8] = SndObjCreate(SoundOBJ,"Sound//puck.WAV",2);
			Sound[9] = SndObjCreate(SoundOBJ,"Sound//wind.WAV",2);
			Sound[10] = SndObjCreate(SoundOBJ,"Sound//def.WAV",2);
			Sound[11] = SndObjCreate(SoundOBJ,"Sound//shot.WAV",2);
			Sound[12] = SndObjCreate(SoundOBJ,"Sound//fool.WAV",2);
			Sound[13] = SndObjCreate(SoundOBJ,"Sound//zzz.WAV",2);
			Sound[14] = SndObjCreate(SoundOBJ,"Sound//upgrade.WAV",2);
			Sound[15] = SndObjCreate(SoundOBJ,"Sound//win.WAV",2);
			Sound[16] = SndObjCreate(SoundOBJ,"Sound//lose.WAV",2);
			Sound[17] = SndObjCreate(SoundOBJ,"Sound//beam.WAV",2);
			Sound[18] = SndObjCreate(SoundOBJ,"Sound//heal.WAV",2);
			Sound[19] = SndObjCreate(SoundOBJ,"Sound//pung.WAV",2);
			Sound[20] = SndObjCreate(SoundOBJ,"Sound//ak.WAV",2);
			Sound[21] = SndObjCreate(SoundOBJ,"Sound//1up.WAV",2);
			Sound[22] = SndObjCreate(SoundOBJ,"Sound//root.WAV",2);
			Sound[23] = SndObjCreate(SoundOBJ,"Sound//die.WAV",2);
			Sound[24] = SndObjCreate(SoundOBJ,"Sound//uk.WAV",2);
			Sound[25] = SndObjCreate(SoundOBJ,"Sound//tang.WAV",2);
			Sound[26] = SndObjCreate(SoundOBJ,"Sound//fafa.WAV",2);
			Sound[27] = SndObjCreate(SoundOBJ,"Sound//fire.WAV",2);
			Sound[28] = SndObjCreate(SoundOBJ,"Sound//summon.WAV",2);
			Sound[29] = SndObjCreate(SoundOBJ,"Sound//tornado.WAV",2);
			Sound[30] = SndObjCreate(SoundOBJ,"Sound//crescent.WAV",2);
			Sound[31] = SndObjCreate(SoundOBJ,"Sound//vulcan.WAV",2);
			Sound[32] = SndObjCreate(SoundOBJ,"Sound//wind.WAV",2);
			Sound[33] = SndObjCreate(SoundOBJ,"Sound//laser.WAV",2);
			Sound[34] = SndObjCreate(SoundOBJ,"Sound//hoho.WAV",2);
			Sound[35] = SndObjCreate(SoundOBJ,"Sound//ak.WAV",2);
        }
	combo[4]=0;
	randomize();
	while (!Quit){
		PRC	switch(mode)
		{
			case 0: _MainMenu(); break;
			case 2: _Meeting();break;
		}
	}
	Change(99);
	_MidiStop();
	DestroyWindow( MainHwnd );

    return TRUE;
}
