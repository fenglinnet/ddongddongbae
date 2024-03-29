#include <string.h>
#include <tchar.h>

#include "donglib.h"

using namespace Gdiplus;

JDirectDraw* jdd;
JResourceManager* jre;

char* backbuffer;

JFont global_font;

HWND hwnd;
MSG msg;
BOOL activate;
BOOL gameover;

//사운드
LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;
BOOL ReplayFlag;

BOOL mouse_control;	//마우스 사용 여부
BOOL keyboard_control;	//키보드 사용 여부

int MouseX, MouseY;	//마우스 좌표
BOOL LButton=FALSE, RButton=FALSE;	//마우스 클릭여부
BOOL left_click=FALSE, right_click=FALSE;	//마우스 클릭은 한번만 검사하기 위한 변수

//한글 처리
CHan m_han;
static int is_eng=1;

//프레임 측정
int fps_n=0, fps_count=0;
DWORD dwStart = GetTickCount();

//메세지 처리
BOOL ProcessMessage()
{
	if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg,NULL,0,0)) return FALSE;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//프레임 측정
	/*++fps_count;
	if(GetTickCount()-dwStart>=1000)
	{
		dwStart = GetTickCount();
		fps_n=fps_count;
		fps_count=0;
	}*/

	if(gameover)return FALSE;	//강제 종료

	return TRUE;
}

int GetFPS()
{
	return fps_n;
}

//미디 연주
BOOL _MidiPlay(char* pszMidiFN, BOOL bReplayFlag)
{
    char szMCISendString[256];

    wsprintf(szMCISendString,"open %s type sequencer alias MUSIC", pszMidiFN);

    if ( mciSendString ( "close all", NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( szMCISendString, NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd ) != 0) return(FALSE);

    ReplayFlag = bReplayFlag; 
    return TRUE;
}

BOOL _MidiStop()
{
    if ( mciSendString ( "close all", NULL, 0, NULL) != 0 ) return ( FALSE );
    return TRUE;
}

BOOL _MidiReplay()
{
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd) != 0 ) return ( FALSE );
    return TRUE;
}

void _Play( HSNDOBJ sound )
{
    if ( SoundCard ) SndObjPlay( sound, NULL );
}

//동영상 재생
DWORD AviCount;

BOOL MciOpen( HWND hwnd, MCIDEVICEID *wDeviceID, char *filename )
{
    DWORD dwError;
    MCI_DGV_OPEN_PARMS mciOpen;
    MCI_STATUS_PARMS mciStatus;
    MCI_DGV_RECT_PARMS mciRect;

    mciOpen.lpstrDeviceType="avivideo";//MPEGVideo";//"avivideo";//"ActiveMovie";
    mciOpen.hWndParent=hwnd;
    mciOpen.dwStyle=WS_CHILD;
    mciOpen.lpstrElementName=filename;

    dwError=mciSendCommand (
            (WORD)NULL, 
            (WORD)MCI_OPEN,
            (DWORD)(MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS|MCI_OPEN_TYPE|MCI_OPEN_ELEMENT),
            (DWORD)(LPVOID)&mciOpen );

    if (dwError)
    {
        char lpszErrorText[80];
        mciGetErrorString( dwError, lpszErrorText, 79 );
    }
    *wDeviceID=mciOpen.wDeviceID;

    mciRect.rc.left   = 0;
    mciRect.rc.top    = 0;
    mciRect.rc.right  = 639;
    mciRect.rc.bottom = 479;

	mciSendCommand( *wDeviceID, MCI_PUT, MCI_DGV_PUT_WINDOW | MCI_DGV_RECT, (DWORD)(LPVOID)&mciRect );

    mciStatus.dwItem = MCI_STATUS_LENGTH;
    mciSendCommand( *wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus );
    AviCount = mciStatus.dwReturn;

    return TRUE;
}

DWORD PlayMovie( WORD wDevID, DWORD dwFrom, DWORD dwTo ) 
{ 
    DWORD dwFlags = 0; 
    MCI_DGV_PLAY_PARMS mciPlay;
 
    if (dwFrom)
    {
        mciPlay.dwFrom = dwFrom;
        dwFlags |= MCI_FROM;
    } 
 
    if (dwTo)
    {
        mciPlay.dwTo = dwTo;
        dwFlags |= MCI_TO;
    } 
    dwFlags |= MCI_NOTIFY;

    return mciSendCommand(wDevID, MCI_PLAY, dwFlags, (DWORD)(LPVOID)&mciPlay);
} 

DWORD CloseMovie( WORD wDevID )
{
    return mciSendCommand(wDevID, MCI_CLOSE, 0, NULL);
}

DWORD _PlayAVI( char* name )
{
    MCIDEVICEID mcidec;
    DSBUFFERDESC DSB_desc;
    MCI_STATUS_PARMS mciStatus;
    FILE *file;

    file = fopen( name, "rb" ); if ( file == NULL ) return FALSE;
    fclose( file );

    MciOpen( hwnd, &mcidec, name );
    PlayMovie( mcidec, 0, 0 );

    mciStatus.dwItem = MCI_STATUS_POSITION;

    while ( !_GetKeyState(VK_ESCAPE) )
    {
        mciSendCommand( mcidec, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus );
        if ( mciStatus.dwReturn == AviCount ) break;
    }
    CloseMovie( mcidec );

    return TRUE;
}

//최대 최소값
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

int MaxMin(int x, int min, int max)
{
	if(x<min)return min;
		else if(x>max) return max;
		else return x;
}

BOOL Between(int value, int min, int max)
{
	if(value>=min && value<=max)return TRUE;
		else return FALSE;
}

//키 처리
int keypush[ALL_KEYS];

BOOL GetKey(ValidKey key, int delay)
{
	if(!activate)return false;

	BOOL push;
	if(delay==-1)push=FALSE;
		else push=TRUE;

	if(_GetKeyState(keys[key]))
	{
		if(!keypush[key])
		{
			keypush[key]=delay;
			return TRUE;
		}
		else if(push)
		{
			keypush[key]=Max(0,keypush[key]-1);
		}
	}
	else keypush[key]=0;
	
	return FALSE;
}

//마우스 버튼
BOOL LeftDown()
{
	if(LButton)
	{
		if(!left_click)
		{
			left_click=TRUE;
			return TRUE;
		}
	}
	else left_click=FALSE;

	return FALSE;
}

BOOL RightDown()
{
	if(RButton)
	{
		if(!right_click)
		{
			right_click=TRUE;
			return TRUE;
		}
	}
	else right_click=FALSE;

	return FALSE;
}

//폰트가 '궁서', 크기20일 때(global_font), 각 캐릭터들의 길이를 돌려주는 함수
//(궁서가 아니면 자동 줄바꿈이 제대로 작동 안 할 수 있음
//글꼴마다 길이가 틀리니 어쩔 수 없이 궁서를 표준으로 했음
int GetCharLength(char tmp_char)
{
 	if(tmp_char<0x20 || tmp_char>0x7e)return 10;
 
 	int char_length[95]={7/*sp*/, 7/*!*/, 7/*"*/, 11/*#*/, 10/*$*/, 13/*%*/, 12/*&*/, 7/*'*/,
		7/*(*/, 7/*)*/, 10/***/, 10/*+*/, 7/*,*/, 10/*-*/, 7/*.*/, 7/*/*/, 11/*0*/, 11, 11,
		11, 11, 11, 11, 11, 11, 11/*9*/, 7/*:*/, 7/*;*/, 13/*<*/, 11/*=*/, 13/*>*/, 7/*?*/,
		13/*@*/, 13/*A*/, 12, 12, 12, 12, 11, 13, 13/*H*/, 8, 11, 12, 12, 14, 12, 13/*O*/,
		11, 14, 12, 11, 10, 13, 12/*V*/, 16, 12, 12, 11/*Z*/, 10/*[*/, 16/*\*/, 10/*]*/,
		11/*^*/, 10/*_*/, 7/*`*/, 11/*a*/, 13, 12, 12, 12, 8, 12, 12/*h*/, 8, 9, 11, 8, 14, 12,
		12/*o*/, 12, 12, 10, 10, 9, 12, 12/*v*/, 13, 10, 11, 11/*z*/, 10/*{*/, 10/*|*/, 10/*}*/, 16/*~*/};

	return char_length[tmp_char-0x20];
}

//근사값, 서페이스를 만들어 출력하는 방식이라면 쓸 생각이었음
// int GetCharLength(char tmp_char)
// {
// 	if(tmp_char>=0x41 && tmp_char<=0x5a)return 15;
// 	if(tmp_char=='~')return 16;
// 
// 	return 10;
// }

//문자열 처리 함수
char str_buffer[3][1024];	//버퍼

char* StrAdd(char* msg, ...)
{
	static int buffer_id;		//지금 사용하는 버퍼(3개의 버퍼를 돌아가면서 써서 동시 3개처리 가능하게 함)
	if(++buffer_id>=3)buffer_id=0;
	char* use_buffer=str_buffer[buffer_id];
	size_t buffer_pointer=0;	//현재 버퍼에 쓰고 있는 위치

	strcpy(use_buffer,"");
	va_list ap;
	va_start(ap,msg);
	size_t max=strlen(msg);
	for(size_t i=0; i<max; ++i)
	{
		//변수 집어 넣기
		if(msg[i]=='%' && i<max-1)
		{
			//문자
			if(msg[i+1]=='s')
			{
				use_buffer[buffer_pointer]=NULL;
				strcat(use_buffer,va_arg(ap,char*));
				buffer_pointer=strlen(use_buffer);
				++i;
			}
			//숫자
			else if(msg[i+1]=='d')
			{
				use_buffer[buffer_pointer]=NULL;
				char int_buffer[10];
				_itoa(va_arg(ap,int),int_buffer,10);
				strcat(use_buffer,int_buffer);
				buffer_pointer=strlen(use_buffer);
				++i;
			}
		}
		//그냥 복사
		else
		{
			use_buffer[buffer_pointer]=msg[i];			
			++buffer_pointer;
		}
	}
	use_buffer[buffer_pointer]=NULL;

	return use_buffer;
}

//잠시 정지
void Pause()
{
	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리
        //마우스나 키보드의 Esc나 엔터를 누르면 계속 진행
		if(keyboard_control && (GetKey(vkey_esc) || GetKey(vkey_enter)) || mouse_control && LeftDown())break;
	}
}

//화면 지우기
void ClearScreen()
{
	ColorMatrix2 CM_bg={{{0,0,0,0},{0,0,0,0},{0,0,0,0}}};
	jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_bg);
}

//페이드 인/아웃
void FadeOut(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=0; fade<=256; fade+=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_fadeout(fade));
		jdd->Render();
		Sleep(delay);
	}
}

//페이드 아웃 후 바로 페이드인 할 수 없습니다.(BackBuffer가 검게 지워졌기 때문에)
//다음 장면을 그린 후 FadeIn을 해주세요. WhiteOut도 마찬가지
void FadeIn(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=256; fade>=0; fade-=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_fadeout(fade));
		jdd->Render();
		Sleep(delay);
	}
}

//화이트 인/아웃
void WhiteOut(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=0; fade<=256; fade+=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_whiteout(fade));
		jdd->Render();
		Sleep(delay);
	}
}

void WhiteIn(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=256; fade>=0; fade-=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_whiteout(fade));
		jdd->Render();
		Sleep(delay);
	}
}

//////////////////////////////////////////////////////////
//한글 클래스 메소드
#define HAN_MAX 2364
#define	MOUM	19

char* han1[]={"가","rk","각","rkr","갂","rkR","간","rks","갇","rke","갈","rkf","갉","rkfr","갊","rkfa","갋","rkfq","갌","rkft",
			"갍","rkfx","갎","rkfv","갏","rkfg","감","rka","갑","rkq","값","rkqt","갓","rkt","갔","rkT","강","rkd","갖","rkw",
			"갗","rkc","같","rkx","갚","rkv","갛","rkg","개","ro","객","ror","갠","ros","갤","roe","갬","roa","갭","roq",
			"갯","rot","갰","roT","갱","rod","갸","ri","갹","rir","갼","ris","걀","rif","걋","rit","걍","rid","걔","rO",
			"걘","rOs","걜","rOf","거","rj","걱","rjr","건","rjs","걷","rje","걸","rjf","걺","rjfa","검","rja","겁","rjq",
			"것","rjt","겄","rjT","겅","rjd","겆","rjw","겉","rjx","겊","rjv","겋","rjg","게","rp","겐","rps","겔","rpf",
			"겜","rpa","겝","rpq","겟","rpt","겠","rpT","겡","rpd","겨","ru","격","rur","겪","rur","견","rus","겯","rue",
			"결","ruf","겸","rua","겹","ruq","겻","rut","겼","ruT","경","rud","곁","rux","계","rP","곈","rPs","곌","rPf",
			"곕","rPq","곗","rPt","고","rh","곡","rhr","곤","rhs","곧","rhe","골","rhf","곪","rhfa","곬","rhft","곯","rhfg",
			"곰","rha","곱","rhq","곳","rht","공","rhd","곶","rhw","과","rhk","곽","rhkr","관","rhks","괄","rhkf","괆","rhkfa",
			"괌","rhka","괍","rhkq","괏","rhkt","광","rhkd","괘","rho","괜","rhos","괠","rhof","괩","rhoq","괬","rhoT","괭","rhod",
			"괴","rhl","괵","rhlr","괸","rhls","괼","rhlf","굄","rhla","굅","rhlq","굇","rhlt","굉","rhld","교","ry","굔","rys",
			"굘","ryf","굡","ryq","굣","ryt","구","rn","국","rnr","군","rns","굳","rne","굴","rnf","굵","rnfr","굶","rnfa",
			"굻","rnfg","굼","rna","굽","rnq","굿","rnt","궁","rnd","궂","rnw","궈","rnj","궉","rnjr","권","rnjs","궐","rnjf",
			"궜","rnjT","궝","rnjd","궤","rnp","궷","rnpt","귀","rnl","귁","rnlr","귄","rnls","귈","rnlf","귐","rnla","귑","rnlq",
			"귓","rnlt","규","rb","균","rbs","귤","rbf","그","rm","극","rmr","근","rms","귿","rme","글","rmf","긁","rmfr",
			"금","rma","급","rmq","긋","rmt","긍","rmd","긔","rml","기","rl","긱","rlr","긴","rls","긷","rle","길","rlf",
			"긺","rlfa","김","rla","깁","rlq","깃","rlt","깅","rld","깆","rlw","깊","rlv","까","Rk","깍","Rkr","깎","RkR",
			"깐","Rks","깔","Rkf","깖","Rkfa","깜","Rka","깝","Rkq","깟","Rkt","깠","RkT","깡","Rkd","깥","Rkx","깨","Ro",
			"깩","Ror","깬","Ros","깰","Rof","깸","Roa","깹","Roq","깻","Rot","깼","RoT","깽","Rod","꺄","Ri","꺅","Rir",
			"꺌","Rif","꺼","Rj","꺽","Rjr","꺾","RjR","껀","Rjs","껄","Rjf","껌","Rja","껍","Rjq","껏","Rjt","껐","RjT",
			"껑","Rjd","께","Rp","껙","Rpr","껚","RpR","껜","Rps","껨","Rpa","껫","Rpt","껭","Rpd","껴","Ru","껸","Rus",
			"껼","Ruf","꼇","Rut","꼈","RuT","꼍","Rux","꼐","RP","꼬","Rh","꼭","Rhr","꼰","Rhs","꼼","Rha","꼲","Rhsg",
			"꼴","Rhf","꼼","Rha","꼽","Rhq","꼿","Rht","꽁","Rhd","꽂","Rhw","꽃","Rhc","꽈","Rhk","꽉","Rhkr","꽐","Rhkf",
			"꽜","RhkT","꽝","Rhkd","꽤","Rho","꽥","Rhor","꽹","Rhod","꾀","Rhl","꾄","Rhls","꾈","RHlf","꾐","Rhla","꾑","Rhlq",
			"꾕","Rhld","꾜","Ry","꾸","Rn","꾹","Rnr","꾼","Rns","꿀","Rnf","꿇","Rnfg","꿈","Rna","꿉","Rnq","꿋","Rnt",
			"꿍","Rnd","꿎","Rnw","꿔","Rnj","꿜","Rnjf","꿨","RnjT","꿩","Rnjd","꿰","Rnp","꿱","Rnpr","꿴","Rnps","꿸","Rnpf",
			"뀀","Rnpa","뀁","Rnpq","뀄","RnpT","뀌","Rnl","뀐","Rnls","뀔","Rnlf","뀜","Rnla","뀝","Rnlq","뀨","Rb","끄","Rm",
			"끅","Rmr","끈","Rms","끊","Rmsg","끌","Rmf","끎","Rmfa","끓","Rmfg","끔","Rma","끕","Rmq","끗","Rmt","끙","Rmd",
			"끝","Rmx","끼","Rl","끽","Rlr","낀","Rls","낄","Rlf","낌","Rla","낍","Rlq","낏","Rlt","낑","Rld","나","sk",
			"낙","skr","낚","skR","난","sks","낟","ske","날","skf","낡","skfr","낢","skfa","남","ska","납","skq","낫","skt",
			"났","skT","낭","skd","낮","skw","낯","skc","낱","skx","낳","skg","내","so","낵","sor","낸","sos","낼","sof",
			"냄","soa","냅","soq","냇","sot","냈","soT","냉","sod","냐","si","냑","sir","냔","sis","냘","sif","냠","sia",
			"냥","sid","너","sj","넉","sjr","넋","sjt","넌","sjs","널","sjf","넒","sjfa","넓","sjfq","넘","sja","넙","sjq",
			"넛","sjt","넜","sjT","넝","sjd","넣","sjg","네","sp","넥","spr","넨","sps","넬","spf","넴","spa","넵","spq",
			"넷","spt","넸","spT","넹","spd","녀","su","녁","sur","년","sus","녈","suf","념","sua","녑","suq","녔","suT",
			"녕","sud","녘","suz","녜","sP","녠","sPs","노","sh","녹","shr","논","shs","놀","shf","놂","shfa","놈","sha",
			"놉","shq","놋","sht","농","shd","높","shv","놓","shg","놔","shk","놘","shks","놜","shkf","놨","shkT","뇌","shl",
			"뇐","shls","뇔","shlf","뇜","shla","뇝","shlq","뇟","shlt","뇨","sy","뇩","syr","뇬","sys","뇰","syf","뇹","syq",
			"뇻","syt","뇽","syd","누","sn","눅","snr","눈","sns","눋","sne","눌","snf","눔","sna","눕","snq","눗","snt",
			"눙","snd","눠","snj","눴","snjT","눼","snp","뉘","snl","뉜","snls","뉠","snlf","뉨","snla","뉩","snlq","뉴","sb",
			"뉵","sbr","뉼","sbf","늄","sba","늅","sbq","늉","sbd","느","sm","늑","smr","는","sms","늘","smf","늙","smfr",
			"늚","smfa","늠","sma","늡","smq","늣","smt","능","smd","늦","smw","늪","smv","늬","sml","늰","smls","늴","smlf",
			"니","sl","닉","slr","닌","sls","닐","slf","닒","slfa","님","sla","닙","slq","닛","slt","닝","sld","닢","slv",
			"다","ek","닥","ekr","닦","ekR","단","eks","닫","eke","달","ekf","닭","ekfr","닮","ekfa","닯","ekfq","닳","ekfg",
			"담","eka","답","ekq","닷","ekt","닸","ekT","당","ekd","닺","ekw","닻","ekc","닿","ekg","대","eo","댁","eor",
			"댄","eos","댈","eof","댐","eoa","댑","eoq","댓","eot","댔","eoT","댕","eod","댜","ei","더","ej","덕","ejr",
			"덖","ejR","던","ejs","덛","eje","덜","ejf","덞","ejfa","덟","ejfq","덤","eja","덥","ejq","덧","ejt","덩","ejd",
			"덫","ejc","덮","ejv","데","ep","덱","epr","덴","eps","델","epf","뎀","epa","뎁","epq","뎃","ept","뎄","epT",
			"뎅","epd","뎌","eu","뎐","eus","뎔","euf","뎠","euT","뎡","eud","뎨","eP","뎬","ePs","도","eh","독","ehr",
			"돈","ehs","돋","ehe","돌","ehf","돎","ehfa","돐","ehft","돔","eha","돕","ehq","돗","eht","동","ehd","돛","ehc",
			"돝","ehx","돠","ehk","돤","ehks","돨","ehkf","돼","eho","됐","ehoT","되","ehl","된","ehls","될","ehlf","됨","ehla",
			"됩","ehlq","됫","ehlt","됴","ey","두","en","둑","enr","둔","ens","둘","enf","둠","ena","둡","enq","둣","ent",
			"둥","end","둬","enj","뒀","enjT","뒈","enp","뒝","enpd","뒤","enl","뒨","enls","뒬","enlf","뒵","enlq","뒷","enlt",
			"뒹","enld","듀","eb","듄","ens","듈","enf","듐","ena","듕","end","드","em","득","emr","든","ems","듣","eme",
			"들","emf","듦","emfa","듬","ema","듭","emq","듯","emt","등","emd","듸","eml","디","el","딕","elr","딘","els",
			"딛","ele","딜","elf","딤","ela","딥","elq","딧","elt","딨","elT","딩","eld","딪","elw","따","Ek","딱","Ekr",
			"딴","Eks","딸","Ekf","땀","Eka","땁","Ekq","땃","Ekt","땄","EkT","땅","Ekd","땋","Ekg","때","Eo","땍","Eor",
			"땐","Eos","땔","Eof","땜","Eoa","땝","Eoq","땟","Eot","땠","EoT","땡","Eod","떠","Ej","떡","Ejr","떤","Ejs",
			"떨","Ejf","떪","Ejfa","떫","Ejfq","떰","Eja","떱","Ejq","떳","Ejt","떴","EjT","떵","Ejd","떻","Ejg","떼","Ep",
			"떽","Epr","뗀","Eps","뗄","Epf","뗌","Epa","뗍","Epq","뗏","Ept","뗐","EpT","뗑","Epd","뗘","Eu","뗬","EuT",
			"또","Eh","똑","Ehr","똔","Ehs","똘","Ehf","똥","Ehd","똬","Ehk","똴","Ehkf","뙈","Eho","뙤","Ehl","뙨","Ehls",
			"뚜","En","뚝","Enr","뚠","Ens","뚤","Enf","뚫","Enfg","뚬","Ena","뚱","End","뛔","Enp","뛰","Enl","뛴","Enls",
			"뛸","Enlf","뜀","Enla","뜁","Enlq","뜅","Enld","뜨","Em","뜩","Emr","뜬","Ems","뜯","Eme","뜰","Emf","뜸","Ema",
			"뜹","Emq","뜻","Emt","띄","Eml","띈","Emls","띌","Emlf","띔","Emla","띕","Emlq","띠","El","띤","Els","띨","Elf",
			"띰","Ela","띱","Elq","띳","Elt","띵","Eld","라","fk","락","fkr","란","fks","랄","fkf","람","fka","랍","fkq",
			"랏","fkt","랐","fkT","랑","fkd","랒","fkw","랖","fkv","랗","fkg","래","fo","랙","for","랜","fos","랠","fof",
			"램","foa","랩","foq","랫","fot","랬","foT","랭","fod","랴","fi","략","fir","랸","fis","럇","fit","량","fid",
			"러","fj","럭","fjr","런","fjs","럴","fjf","럼","fja","럽","fjq","럿","fjt","렀","fjT","렁","fjd","렇","fjg",
			"레","fp","렉","fpr","렌","fps","렐","fpf","렘","fpa","렙","fpq","렛","fpt","렝","fpd","려","fu","력","fur",
			"련","fus","렬","fuf","렴","fua","렵","fuq","렷","fut","렸","fuT","령","fud","례","fP","롄","fPs","롑","fPq",
			"롓","fPt","로","fh","록","fhr","론","fhs","롤","fhf","롬","fha","롭","fhq","롯","fht","롱","fhd","롸","fhk",
			"롼","fhks","뢍","fhkd","뢨","fhoT","뢰","fhl","뢴","fhls","뢸","fhlf","룀","fhla","룁","fhlq","룃","fhlt","룅","fhld",
			"료","fy","룐","fys","룔","fyf","룝","fyq","룟","fyt","룡","fyd","루","fn","룩","fnr","룬","fns","룰","fnf",
			"룸","fna","룹","fnq","룻","fnt","룽","fnd","뤄","fnj","뤘","fnjT","뤠","fnp","뤼","fnl","뤽","fnlr","륀","fnls",
			"륄","fnlf","륌","fnla","륏","fnlt","륑","fnld","류","fb","륙","fbr","륜","fbs","률","fbf","륨","fba","륩","fbq",
			"륫","fbt","륭","fbd","르","fm","륵","fmr","른","fms","를","fmf","름","fma","릅","fmq","릇","fmt","릉","fmd",
			"릊","fmw","릍","fmx","릎","fmv","리","fl","릭","flr","린","fls","릴","flf","림","fla","립","flq","릿","flt",
			"링","fld","마","ak","막","akr","만","aks","많","aksg","맏","ake","말","akf","맑","akfr","맒","aka","맘","aka",
			"맙","akq","맛","akt","망","akd","맞","akw","맡","akx","맣","akg","매","ao","맥","aor","맨","aos","맬","aof","맴","aoa",
			"맵","aoq","맷","aot","맸","aoT","맹","aod","맺","aow","먀","ai","먁","air","먈","aif","먕","aid","머","aj","먹","ajr",
			"먼","ajs","멀","ajf","멂","ajfa","멈","aja","멉","ajq","멋","ajt","멍","ajd","멎","ajw","멓","ajg","메","ap","멕","apr",
			"멘","aps","멜","apf","멤","apa","멥","apq","멧","apt","멨","apT","멩","apd","며","au","멱","aur","면","aus",
			"멸","auf","몃","aut","몄","auT","명","aud","몇","auc","몌","aP","모","ah","목","ahr","몫","ahrt","몬","ahs","몰","ahf",
			"몲","ahfa","몸","aha","몹","ahq","못","aht","몽","ahd","뫄","ahk","뫈","ahks","뫘","ahkT","뫙","ahkd","뫼","ahl","묀","ahls",
			"묄","ahlf","묍","ahlq","묏","ahlt","묑","ahld","묘","ay","묜","ays","묠","ayf","묩","ayq","묫","ayt","무","an","묵","anr",
			"묶","anR","문","ans","묻","ane","물","anf","묽","anfr","묾","anfa","뭄","ana","뭅","anq","뭇","ant","뭉","and",
			"뭍","anx","뭏","ang","뭐","anj","뭔","anjs","뭘","anjf","뭡","anjq","뭣","anjt","뭬","anp","뮈","anl","뮌","anls","뮐","anlf",
			"뮤","ab","뮨","abs","뮬","abf","뮴","aba","뮷","abt","므","am","믄","ams","믈","amf","믐","ama","믓","amt","미","al",
			"믹","alr","민","als","믿","ale","밀","alf","밂","alfa","밈","ala","밉","alq","밋","alt","밌","alT","밍","ald","및","alC",
			"밑","alx","바","qk","박","qkr","밖","qkR","밗","qkrt","반","qks","받","qke","발","qkf","밝","qkfr","밞","qkfa",
			"밟","qkfq","밤","qka","밥","qkq","밧","qkt","방","qkd","밭","qkx","배","qo","백","qor","밴","qos","밸","qof","뱀","qoa",
			"뱁","qoq","뱃","qot","뱄","qoT","뱅","qod","뱉","qox","뱌","qi","뱍","qir","뱐","qis","뱝","qiq","버","qj","벅","qjr",
			"번","qjs","벋","qje","벌","qjf","벎","qjfa","범","qja","법","qjq","벗","qjt","벙","qjd","벚","qjw","베","qp","벡","qpr",
			"벤","qps","벧","qpe","벨","qpf","벰","qpa","벱","qpq","벳","qpt","벴","qpT","벵","qpd","벼","qu","벽","qur",
			"변","qus","별","quf","볍","quq","볏","qut","볐","quT","병","qud","볕","qux","볘","qP","볜","qPs","보","qh","복","qhr",
			"볶","qhR","본","qhs","볼","qhf","봄","qha","봅","qhq","봇","qht","봉","qhd","봐","qhk","봔","qhks","봤","qhkT","봬","qho",
			"뵀","qhoT","뵈","qhl","뵉","qhlr","뵌","qhls","뵐","qhlf","뵘","qhla","뵙","qhlq","뵤","qy","뵨","qys","부","qn","북","qnr",
			"분","qns","붇","qne","불","qnf","붉","qnfr","붊","qnfa","붐","qna","붑","qnq","붓","qnt","붕","qnd","붙","qnx",
			"붚","qnv","붜","qnj","붤","qnjf","붰","qnjT","붸","qnp","뷀","qnpf","뷁","qnpfr","뷔","qnl","뷕","qnlr","뷘","qnls","뷜","qnlf","뷩","qnld","뷰","qb",
			"뷴","qbs","뷸","qbf","븀","qba","븃","qbt","븅","qbd","브","qm","븍","qmr","븐","qms","블","qmf","븜","qma","븝","qmq",
			"븟","qmt","비","ql","빅","qlr","빈","qls","빌","qlf","빎","qlfa","빔","qla","빕","qlq","빗","qlt","빙","qld","빚","qlw",
			"빛","qlc","빠","Qk","빡","Qkr","빤","Qks","빨","Qkf","빪","Qkfa","빰","Qka","빱","Qkq","빳","Qkt","빴","QkT",
			"빵","Qkd","빻","Qkg","빼","Qo","빽","Qor","뺀","Qos","뺄","Qof","뺌","Qoa","뺍","Qoq","뺏","Qot","뺐","QoT","뺑","Qod",
			"뺘","Qi","뺙","Qir","뺨","Qia","뻐","Qj","뻑","Qjr","뻔","Qjs","뻗","Qje","뻘","Qjf","뻠","Qja","뻣","Qjt","뻤","QjT",
			"뻥","Qjd","뻬","Qp","뼁","Qpd","뼈","Qu","뼉","Qur","뼘","Qua","뼙","Quq","뼛","Qut","뼜","QuT","뼝","Qud","뽀","Qh",
			"뽁","Qhr","뽄","Qhs","뽈","Qhf","뽐","Qha","뽑","Qhq","뽕","Qhd","뾔","Qhl","뾰","Qy","뿅","Qyd","뿌","Qn",
			"뿍","Qnr","뿐","Qns","뿔","Qnf","뿜","Qna","뿟","Qnt","뿡","Qnd","쀼","Qb","쁑","Qbd","쁘","Qm","쁜","Qms","쁠","Qmf",
			"쁨","Qma","쁩","Qmq","삐","Ql","삑","Qlr","삔","Qls","삘","Qlf","삠","Qla","삡","Qlq","삣","Qlt","삥","Qld","사","tk",
			"삭","tkr","삯","tkrt","산","tks","삳","tke","살","tkf","삵","tkfr","삶","tkfa","삼","tka","삽","tkq","삿","tkt","샀","tkT",
			"상","tkd","샅","tkx","새","to","색","tor","샌","tos","샐","tof","샘","toa","샙","toq","샛","tot","샜","toT",
			"생","tod","샤","ti","샥","tir","샨","tis","샬","tif","샴","tia","샵","tiq","샷","tit","샹","tid","섀","tO",
			"섄","tOs","섈","tOf","섐","tOa","섕","tOd","서","tj","석","tjr","섞","tjR","섟","tjrt","선","tjs","섣","tje",
			"설","tjf","섦","tjfa","섧","tjfq","섬","tja","섭","tjq","섯","tjt","섰","tjT","성","tjd","섶","tjv","세","tp",
			"섹","tpR","센","tps","셀","tpf","셈","tpa","셉","tpq","셋","tpt","셌","tpT","셍","tpd","셔","tu","셕","tur",
			"션","tus","셜","tuf","셤","tua","셥","tuq","셧","tut","셨","tuT","셩","tud","셰","tP","셴","tPs","셸","tPf",
			"솅","tPd","소","th","속","thr","솎","thR","손","ths","솔","thf","솖","thfa","솜","tha","솝","thq","솟","tht",
			"송","thd","솥","thx","솨","thk","솩","thkr","솬","thks","솰","thkf","솽","thkd","쇄","tho","쇈","thos","쇌","thof",
			"쇔","thoa","쇗","thot","쇘","thoT","쇠","thl","쇤","thls","쇨","thlf","쇰","thla","쇱","thlq","쇳","thlt","쇼","ty",
			"쇽","tyr","숀","tys","숄","tyf","숌","tya","숍","tyq","숏","tyt","숑","tyd","수","tn","숙","tnr","순","tns",
			"숟","tne","술","tnf","숨","tna","숩","tnq","숫","tnt","숭","tnd","숯","tnc","숱","tnx","숲","tnv","숴","tnj",
			"쉈","tnjT","쉐","tnp","쉑","tnpr","쉔","tnps","쉘","tnpf","쉠","tnpa","쉥","tnpd","쉬","tnl","쉭","tnlr","쉰","tnls",
			"쉴","tnlf","쉼","tnla","쉽","tnlq","쉿","tnlt","슁","tnld","슈","tb","슉","tbr","슐","tbf","슘","tba","슛","tbt",
			"슝","tbd","스","tm","슥","tmr","슨","tms","슬","tmf","슭","tmfr","슴","tma","습","tmq","슷","tmt","승","tmd",
			"시","tl","식","tlr","신","tls","싣","tle","실","tlf","싫","tlfg","심","tla","십","tlq","싯","tlt","싱","tld",
			"싶","tlv","싸","Tk","싹","Tkr","싻","Tkrt","싼","Tks","쌀","Tkf","쌈","Tka","쌉","Tkq","쌌","TkT","쌍","Tkd",
			"쌓","Tkg","쌔","To","쌕","Tor","쌘","Tos","쌜","Tof","쌤","Toa","쌥","Toq","쌨","ToT","쌩","Tod","썅","Tid",
			"써","Tj","썩","Tjr","썬","Tjs","썰","Tjf","썲","Tjfa","썸","Tja","썹","Tjq","썼","TjT","썽","Tjd","쎄","Tp",
			"쎅","Tpr","쎈","Tps","쎌","Tpf","쏀","TPs","쏘","Th","쏙","Thr","쏜","Ths","쏟","The","쏠","Thf","쏢","Thfa",
			"쏨","Tha","쏩","Thq","쏭","Thd","쏴","Thk","쏵","Thkr","쏸","Thks","쐈","ThkT","쐐","Tho","쐤","ThoT","쐬","Thl",
			"쐰","Thls","쐴","Thlf","쐼","Thla","쐽","Thlq","쑈","Ty","쑤","Tn","쑥","Tnr","쑨","Tns","쑬","Tnf","쑴","Tna",
			"쑵","Tnq","쑹","Tnd","쒀","Tnj","쒔","TnT","쒜","Tnp","쒸","Tnl","쒼","Tnls","쓩","Tbd","쓰","Tm","쓱","Tmr",
			"쓴","Tms","쓸","Tmf","쓺","Tmfa","쓿","Tmfg","씀","Tma","씁","Tmq","씌","Tml","씐","Tmls","씔","Tmlf","씜","Tmla",
			"씨","Tl","씩","Tlr","씬","Tls","씰","Tlf","씸","Tla","씹","Tlq","씻","Tlt","씽","Tld","아","dk","악","dkr",
			"안","dks","앉","dksw","않","dksg","알","dkf","앍","dkfr","앎","dkfa","앓","dkfg","암","dka","압","dkq","앗","dkt",
			"았","dkT","앙","dkd","앝","dkx","앞","dkv","애","do","액","dor","앤","dos","앨","dof","앰","doa","앱","doq",
			"앳","dot","앴","doT","앵","dod","야","di","약","dir","얀","dis","얄","dif","얇","difq","얌","dia","얍","diq",
			"얏","dit","양","did","얕","dix","얗","dig","얘","dO","얜","dOs","얠","dOf","얩","dOq","어","dj","억","djr",
			"언","djs","얹","djsw","얻","dje","얼","djf","얽","djfr","얾","djfa","엄","dja","업","djq","없","djqt","엇","djt",
			"었","djT","엉","djd","엊","djw","엌","djz","엎","djv","에","dp","엑","dpr","엔","dps","엘","dpf","엠","dpa",
			"엡","dpq","엣","dpt","엥","dpd","여","du","역","dur","엮","duR","연","dus","열","duf","엶","dufa","엷","dufq",
			"염","dua","엽","duq","엾","duqt","엿","dut","였","duT","영","dud","옅","dux","옆","duv","옇","dug","예","dP",
			"옌","dPs","옐","dPf","옘","dPa","옙","dPq","옛","dPt","옜","dPT","오","dh","옥","dhr","온","dhs","올","dhf",
			"옭","dhfr","옮","dhfa","옰","dhft","옳","dhfg","옴","dha","옵","dhq","옷","dht","옹","dhd","옻","dhc","와","dhk",
			"왁","dhkr","완","dhks","왈","dhkf","왐","dhka","왑","dhkq","왓","dhkt","왔","dhkT","왕","dhkd","왜","dho","왝","dhor",
			"왠","dhos","왬","dhoa","왯","dhot","왱","dhod","외","dhl","왹","dhlr","왼","dhls","욀","dhlf","욈","dhla","욉","dhlq",
			"욋","dhlt","욍","dhld","요","dy","욕","dyr","욘","dys","욜","dyf","욤","dya","욥","dyq","욧","dyr","용","dyd",
			"우","dn","욱","dnr","운","dns","울","dnf","욹","dnfr","욺","dnfa","움","dna","웁","dnq","웃","dnt","웅","dnd",
			"워","dnj","웍","dnjr","원","dnjs","월","dnjf","웜","dnja","웝","dnjq","웠","dnjT","웡","dnjd","웨","dnp","웩","dnpr",
			"웬","dnps","웰","dnpf","웸","dnpa","웹","dnpq","웽","dnpd","위","dnl","윅","dnlr","윈","dnls","윌","dnlf","윔","dnla",
			"윕","dnlq","윗","dnlt","윙","dnld","유","db","육","dbr","윤","dbs","율","dbf","윰","dba","윱","dbq","윳","dbt",
			"융","dbd","윷","dbc","으","dm","윽","dmr","은","dms","을","dmf","읊","dmfv","음","dma","읍","dnq","읏","dmt",
			"응","dmd","읒","dmw","읓","dmc","읔","dmz","읕","dmx","읖","dmv","읗","dmg","의","dml","읜","dmls","읠","dmlf",
			"읨","dmla","읫","dmlt","이","dl","익","dlr","인","dls","일","dlf","읽","dlfr","읾","dlfa","잃","dlfg","임","dla",
			"입","dlq","잇","dlt","있","dlT","잉","dld","잊","dlw","잎","dlv","자","wk","작","wkr","잔","wks","잖","wksg",
			"잗","wke","잘","wkf","잚","wkfa","잠","wka","잡","wkq","잣","wkt","잤","wkT","장","wkd","잦","wkw","재","wo",
			"잭","wor","잰","wos","잴","wof","잼","woa","잽","woq","잿","wot","쟀","woT","쟁","wod","쟈","wi","쟉","wir",
			"쟌","wis","쟎","wisg","쟐","wif","쟘","wia","쟝","wid","쟤","wO","쟨","wOs","쟬","wOf","저","wj","적","wjr",
			"전","wjs","절","wjf","젊","wjfa","점","wja","접","wjq","젓","wjt","정","wjd","젖","wjw","제","wp","젝","wpr",
			"젠","wps","젤","wpf","젬","wpa","젭","wpq","젯","wpt","젱","wpd","져","wu","젼","wus","졀","wuf","졈","wua",
			"졉","wuq","졌","wuT","졍","wud","졔","wP","조","wh","족","whr","존","whs","졸","whf","졺","whfa","좀","wha",
			"좁","whq","좃","wht","종","whd","좆","whw","좇","whc","좋","whg","좌","whk","좍","whkr","좔","whkf","좝","whkq",
			"좟","whkt","좡","whkd","좨","who","좼","whoT","좽","whod","죄","whl","죈","whls","죌","whlf","죔","whla","죕","whlq",
			"죗","whlt","죙","whld","죠","wy","죡","wyr","죤","wys","죵","wyd","주","wn","죽","wnr","준","wns","줄","wnf",
			"줅","wnfr","줆","wnfa","줌","wna","줍","wnq","줏","wnt","중","wnd","줘","wnj","줬","wnjT","줴","wnp","쥐","wnl",
			"쥑","wnlr","쥔","wnls","쥘","wnlf","쥠","wnla","쥡","wnlq","쥣","wnlt","쥬","wb","쥰","wbs","쥴","wbf","쥼","wba",
			"즈","wm","즉","wmr","즌","wms","즐","wmf","즘","wma","즙","wmq","즛","wmt","증","wmd","지","wl","직","wlr",
			"진","wls","짇","wle","질","wlf","짊","wlfa","짐","wla","집","wlq","짓","wlt","징","wld","짖","wlw","짙","wlx",
			"짚","wlv","짜","Wk","짝","Wkr","짠","Wks","짢","Wksg","짤","Wkf","짧","Wkfq","짬","Wka","짭","Wkq","짯","Wkt",
			"짰","WkT","짱","Wkd","째","Wo","짹","Wor","짼","Wos","쨀","Wof","쨈","Woa","쨉","Woq","쨋","Wot","쨌","WoT",
			"쨍","Wod","쨔","Wi","쨘","Wis","쨩","Wid","쩌","Wj","쩍","Wjr","쩐","Wjs","쩔","Wjf","쩜","Wja","쩝","Wjq",
			"쩟","Wjt","쩠","WjT","쩡","Wjd","쩨","Wp","쩽","Wpd","쪄","Wu","쪘","WuT","쪼","Wh","쪽","Whr","쫀","Whs",
			"쫄","Whf","쫌","Wha","쫍","Whq","쫏","Wht","쫑","Whd","쫓","Whc","쫘","Whk","쫙","Whkr","쫠","Whkf","쫬","WhkT",
			"쫴","Who","쬈","WhoT","쬐","Whl","쬔","Whls","쬘","Whlf","쬠","Whla","쬡","Whlq","쭁","Wyd","쭈","Wn","쭉","Wnr",
			"쭌","Wns","쭐","Wnf","쭘","Wna","쭙","Wnq","쭝","Wnd","쭤","Wnj","쭸","WnjT","쭹","Wnjd","쮜","Wnl","쮸","Wb",
			"쯔","Wm","쯤","Wma","쯧","Wmt","쯩","Wmd","찌","Wl","찍","Wlr","찐","Wls","찔","Wlf","찜","Wla","찝","Wlq",
			"찡","Wld","찢","Wlw","찧","Wlg","차","ck","착","ckr","찬","cks","찮","cksg","찰","ckf","참","cka","찹","ckq",
			"찻","ckt","찼","ckT","창","ckd","찾","ckw","채","co","책","cor","챈","cos","챌","cof","챔","coa","챕","coq",
			"챗","cot","챘","coT","챙","cod","챠","ci","챤","cis","챦","cisg","챨","cif","챰","cia","챵","cid","처","cj",
			"척","cjr","천","cjs","철","cjf","첨","cja","첩","cjq","첫","cjt","첬","cjT","청","cjd","체","cp","첵","cpr",
			"첸","cps","첼","cpf","쳄","cpa","쳅","cpq","쳇","cpt","쳉","cpd","쳐","cu","쳔","cus","쳤","cuT","쳬","cP",
			"쳰","cPs","촁","cPd","초","ch","촉","chr","촌","chs","촐","chf","촘","cha","촙","chq","촛","cht","총","cha",
			"촤","chk","촨","chks","촬","chkf","촹","chkd","최","chl","쵠","chls","쵤","chlf","쵬","chla","쵭","chlq","쵯","chlt",
			"쵱","chld","쵸","cy","춈","cya","춉","cyq","추","cn","축","cnr","춘","cns","출","cnf","춤","cna","춥","cnq",
			"춧","cnt","충","cnd","춰","cnj","췄","cnjT","췌","cnp","췐","cnps","췟","cnpt","취","cnl","췬","cnls","췰","cnlf",
			"췸","cnla","췹","cnlq","췻","cnlt","췽","cnld","츄","cb","츈","cbs","츌","cbf","츔","cba","츙","cbd","츠","cm",
			"측","cmr","츤","cms","츨","cmf","츰","cma","츱","cmq","츳","cmt","층","cmd","치","cl","칙","clr","친","cls",
			"칟","cle","칠","clf","칡","clfr","침","cla","칩","clq","칫","clt","칭","cld","카","zk","칵","zkr","칸","zks",
			"칼","zkf","캄","zka","캅","zkq","캇","zkt","캉","zkd","캐","zo","캑","zor","캔","zos","캘","zof","캠","zoa",
			"캡","zoq","캣","zot","캤","zoT","캥","zod","캬","zi","캭","zir","컁","zid","커","zj","컥","zjr","컨","zjs",
			"컫","zje","컬","zjf","컴","zja","컵","zjq","컷","zjt","컸","zjT","컹","zjd","케","zp","켁","zp","켄","zps",
			"켈","zpf","켐","zpa","켑","zpq","켓","zpt","켕","zpd","켜","zu","켠","zus","켤","zuf","켬","zua","켭","zuq",
			"켯","zut","켰","zuT","켱","zud","켸","zP","코","zh","콕","zhr","콘","zhs","콜","zhf","콤","zha","콥","zhq",
			"콧","zht","콩","zhd","콰","zhk","콱","zhkr","콴","zhks","콸","zhkf","쾀","zhka","쾅","zhkd","쾌","zho","쾡","zhod",
			"쾨","zhl","쾰","zhlf","쿄","zy","쿠","zn","쿡","znr","쿤","zns","쿨","znf","쿰","zna","쿱","znq","쿳","znt",
			"쿵","znd","쿼","znj","퀀","znjs","퀄","znjf","퀑","znjd","퀘","znp","퀭","znpd","퀴","znl","퀵","znlr","퀸","znls",
			"퀼","znlf","큄","znla","큅","znlq","큇","znlt","큉","znld","큐","zb","큔","zbs","큘","zbf","큠","zba","크","zm",
			"큭","zmr","큰","zms","클","zmf","큼","zma","큽","zmq","킁","zmd","키","zl","킥","zlr","킨","zls","킬","zlf",
			"킴","zla","킵","zlq","킷","zlt","킹","zld","타","xk","탁","xkr","탄","xks","탈","xkf","탉","xkfr","탐","xka",
			"탑","xkq","탓","xkt","탔","xkT","탕","xkd","태","xo","택","xor","탠","xos","탤","xof","탬","xoa","탭","xoq",
			"탯","xot","탰","xoT","탱","xod","탸","xi","턍","xid","터","xj","턱","xjr","턴","xjs","털","xjf","턺","xjfa",
			"텀","xja","텁","xjq","텃","xjt","텄","xjT","텅","xjd","테","xp","텍","xpr","텐","xps","텔","xpf","템","xpa",
			"텝","xpq","텟","xpt","텡","xpd","텨","xu","텬","xus","텼","xuT","톄","xP","톈","xPs","토","xh","톡","xhr",
			"톤","xhs","톨","xhf","톰","xha","톱","xhq","톳","xht","통","xhd","톺","xhv","톼","xhk","퇀","xhks","퇘","xho",
			"퇴","xhl","퇸","xhls","툇","xhlt","툉","xhld","툐","xy","투","xn","툭","xnr","툰","xns","툴","xnf","툼","xna",
			"툽","xnq","툿","xnt","퉁","xnd","퉈","xnj","퉜","xnjT","퉤","xnp","튀","xnl","튁","xnlr","튄","xnls","튈","xnlf",
			"튐","xnla","튑","xnlq","튕","xnld","튜","xb","튠","xbs","튤","xb","튬","xba","튱","xbd","트","xm","특","xmr",
			"튼","xms","튿","xme","틀","xmf","틂","xmfa","틈","xma","틉","xmq","틋","xmt","틔","xml","틘","xls","틜","xmlf",
			"틤","xmla","틥","xmlq","티","xl","틱","xlr","틴","xls","틸","xlf","팀","xla","팁","xlq","팃","xlt","팅","xld",
			"파","vk","팍","vkr","팎","vkR","판","vks","팔","vkf","팖","vkfa","팜","vka","팝","vkq","팟","vkt","팠","vkT",
			"팡","vkd","팥","vkx","패","vko","팩","vor","팬","vos","팰","vof","팸","voa","팹","voq","팻","vot","팼","voT",
			"팽","vod","퍄","vi","퍅","vir","퍼","vj","퍽","vjr","펀","vjs","펄","vjf","펌","vja","펍","vjq","펏","vjt",
			"펐","vjT","펑","vjd","페","vp","펙","vpr","펜","vps","펠","vpf","펨","vpa","펩","vpq","펫","vpt","펭","vpd",
			"펴","vu","편","vus","펼","vuf","폄","vua","폅","vuq","폈","vuT","평","vud","폐","vP","폘","vPf","폡","vPq",
			"폣","vPt","포","vh","폭","vhr","폰","vhs","폴","vhf","폼","vha","폽","vhq","폿","vht","퐁","vhd","퐈","vhk",
			"퐝","vhkd","푀","vhl","푄","vhls","표","vy","푠","vys","푤","vyf","푭","vyq","푯","vyt","푸","vn","푹","vnr",
			"푼","vns","푿","vne","풀","vnf","풂","vnfa","품","vna","풉","vnq","풋","vnt","풍","vnd","풔","vnj","풩","vnjd",
			"퓌","vnl","퓐","vnls","퓔","vnlf","퓜","vnla","퓟","vnlt","퓨","vb","퓬","vbs","퓰","vbf","퓸","vba","퓻","vbt",
			"퓽","vbd","프","vm","픈","vms","플","vmf","픔","vma","픕","vmq","픗","vmt","피","vl","픽","vlr","핀","vls",
			"필","vlf","핌","vla","핍","vlq","핏","vlt","핑","vld","하","gk","학","gkr","한","gks","할","gkf","핥","gkfx",
			"함","gka","합","gkq","핫","gkt","항","gkd","해","go","핵","gor","핸","gos","핼","gof","햄","goa","햅","goq",
			"햇","got","했","goT","행","god","햏","gog","햐","gi","향","gid","허","gj","헉","gjr","헌","gjs","헐","gjf",
			"헒","gjfa","험","gja","헙","gjq","헛","gjt","헝","gjd","헤","gp","헥","gpr","헨","gps","헬","gpf","헴","gpa",
			"헵","gpq","헷","gpt","헹","gpd","혀","gu","혁","gur","현","gus","혈","guf","혐","gua","협","guq","혓","gut",
			"혔","guT","형","gud","혜","gP","혠","gPs","혤","gPf","혭","gPq","호","gh","혹","ghr","혼","ghs","홀","ghf",
			"홅","ghfx","홈","gha","홉","ghq","홋","ght","홍","ghd","홑","ghx","화","ghk","확","ghkr","환","ghks","활","ghkf",
			"홧","ghkt","황","ghkd","홰","gho","홱","ghor","홴","ghos","횃","ghot","횅","ghod","회","ghl","획","ghlr","횐","ghls",
			"횔","ghlf","횝","ghlq","횟","ghlt","횡","ghld","효","gy","횬","gys","횰","gyf","횹","gyq","횻","gyt","후","gn",
			"훅","gnr","훈","gns","훌","gnf","훑","gnfx","훔","gna","훗","gnt","훙","gnd","훠","gnj","훤","gnjs","훨","gnjf",
			"훰","gnja","훵","gnjd","훼","gnp","훽","gnpr","휀","gnps","휄","gnpf","휑","gnpd","휘","gnl","휙","gnlr","휜","gnls",
			"휠","gnlf","휨","gnla","휩","gnlq","휫","gnlt","휭","gnld","휴","gb","휵","gbr","휸","gbs","휼","gbf","흄","gba",
			"흇","gbt","흉","gbd","흐","gm","흑","gmr","흔","gms","흖","gmsg","흗","gme","흘","gmf","흙","gmfr","흠","gma",
			"흡","gmq","흣","gmt","흥","gmd","흩","gmx","희","gml","흰","gmls","흴","gmlf","흼","gmla","흽","gmlq","힁","gmld",
			"히","gl","힉","glr","힌","gls","힐","glf","힘","gla","힙","glq","힛","glt","힝","gld"};

char jamo[34]="rRseEfaqQtTdwWczxvgkoiOjpuPhynbml";
char* han_jamo[33]={"ㄱ","ㄲ","ㄴ","ㄷ","ㄸ","ㄹ","ㅁ","ㅂ","ㅃ","ㅅ","ㅆ","ㅇ","ㅈ","ㅉ","ㅊ","ㅋ","ㅌ","ㅍ","ㅎ",
					"ㅏ","ㅐ","ㅑ","ㅒ","ㅓ","ㅔ","ㅕ","ㅖ","ㅗ","ㅛ","ㅜ","ㅠ","ㅡ","ㅣ"};

CHan::CHan()
{
	//검색 속도 향상을 위한 빨리가기
	int mp=0;
	char* check_text[19]={"가","까","나","다","따","라","마","바","빠","사","싸","아","자","짜","차","카","타","파","하"};
	for(unsigned int i=0; i<HAN_MAX; ++i)
	{
		if(strcmp(han1[i*2], check_text[mp])==0)
		{
			mark[mp]=i;
			++mp;
			if(mp>=19)break;
		}
	}
	mark[19]=HAN_MAX;
}

unsigned int CHan::FindMark(char eng)
{
	for(unsigned int j=0; j<19; ++j)
		if(eng==jamo[j])return j;

	return 0;
}

char* CHan::EngToHan(char* text, char* han_area)
{
	unsigned int pp=0;
	unsigned int begin[STR_MAX], end[STR_MAX];

	//입력 받기
	strcpy(han, "");	

	//한글에 대치 안 되는 대문자를 소문자로 바꿈
	for(unsigned int i=0; i<strlen(text); ++i)
	{
		if(han_area[i]==0)continue;
		if(text[i]>='A' && text[i]<='Z' && text[i]!='Q' && text[i]!='W' && text[i]!='E'	&& text[i]!='R'
			&& text[i]!='T' && text[i]!='O' && text[i]!='P')text[i]+=0x20;
	}

	//모음, 자음 한글 아님 여부 확인(자음 인 부분을 포인트로 두어서 변환에 도움이 되게 함)
	for(unsigned int i=1; i<strlen(text); ++i)
	{
		//자음인지 모음인지 구분
		for(int j=0; j<33; ++j)
			if(text[i]==jamo[j])
			{
				if(j>=MOUM)
				{
					begin[pp]=i-1;
					if(pp>0)end[pp-1]=i-2;
					++pp;
					++i;	//이중 모음의 경우를 생각해서 다음을 넘김(이중 모음을 끊어 읽어 글자가 되는 경우는 없으므로 예: 뒷산(O) 두ㅣㅅ산(X))
				}
				break;
			}
	}
	if(pp>0)end[pp-1]=(unsigned int)strlen(text)-1;

	//포인트 조정(한글인 부분만 해석)
	for(unsigned int i=0; i<pp; ++i)
	{
		char buffer[10];
		int bp=0;
		BOOL head=TRUE;	//앞 부분인지 글자 뒷부분인지 알아냄(잘라내야 하는 부분 예: 1@가2$ -> 1@은 head=TRUE인 상태, 2$는 head=FALSE인 상태에서 읽음
		for(unsigned int j=begin[i]; j<=end[i]; ++j)
		{
			if(han_area[j] && (text[j]>='a' && text[j]<='z' || text[j]>='A' && text[j]<='Z'))
			{
				buffer[bp]=text[j];
				++bp;
				head=FALSE;
			}
			else
			{
				if(head)++begin[i];
					else end[i]=j-1;
			}
		}
		buffer[bp]=NULL;

		//영문을 한글로 쳤을 때 일치하는 글자 찾기
		while(strlen(buffer)>1)
		{
			BOOL find=FALSE;
			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];

			for(unsigned int j=loop_start; j<loop_end; ++j)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					find=TRUE;
					break;
				}

			//찾지 못 했을 때 영역을 줄여서 찾아봄
			if(!find)
			{
				buffer[strlen(buffer)-1]=NULL;
				--end[i];
			}
			else break;
		}

		//읽지 못하는 한글임을 표시
		if(strlen(buffer)<=1)begin[i]=9999;
	}

	//변환
	unsigned int pp2=0;
	for(unsigned int i=0; i<strlen(text); ++i)
	{
		while(begin[pp2]==9999)++pp2;

		//한글인 부분
		if(pp2<pp && i==begin[pp2])
		{
			char buffer[10];
			int bp=0;
			for(unsigned int j=begin[pp2]; j<=end[pp2]; ++j)
			{
				buffer[bp]=text[j];
				++bp;
			}
			buffer[bp]=NULL;

			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];
			for(unsigned int j=loop_start; j<loop_end; ++j)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					strcat(han, han1[j*2]);
					break;
				}

			i=end[pp2];
			++pp2;
		}
		//아닌 부분
		else
		{			
			//한글 일때는 끊어진 모음 자음으로 표현
			if(han_area[i]!=0 && (text[i]>='a' && text[i]<='z' || text[i]>='A' && text[i]<='Z'))
			{
				for(int j=0; j<33; ++j)
				{
					if(text[i]==jamo[j])
					{
						strcat(han, han_jamo[j]);
						break;
					}
				}
			}
			//결국 없을 땐 문자 그대로
			else
			{
				char tmp[2];
				tmp[0]=text[i];
				tmp[1]=NULL;
				strcat(han, tmp);
			}			
		}
	}

	han[strlen(han)]=NULL;
	return han;
}

/////////////////////////////////////////////////////
//기본 대화 처리 클래스 메소드
CDlg::CDlg()
{
	setting=setting2=FALSE;
	b_shadow=FALSE;
	dlgbox=NULL;
	show_dlgbox=TRUE;
	SetColor(0,0,0);
	text_font=&global_font;
	frame_x=frame_y=5;
}

void CDlg::Move(int vx, int vy)
{
	if(!setting || !setting2)return;	//대화창이 완성되기 전에 이동시키면 버그가 일어남

	x+=vx;
	y+=vy;
	x=MaxMin(x, 0, SCREEN_X-x_size);
	y=MaxMin(y, 0, SCREEN_Y-y_size);
}

void CDlg::SetColor(BYTE r, BYTE g, BYTE b)
{
	color.r=r;
	color.g=g;
	color.b=b;
}

void CDlg::SetDlgBox(char* box_name)
{
	//사용자가 지정한 대화창 쓰기
	dlgbox=box_name;
}

void CDlg::SetFont(JFont *font)
{
	if(font!=NULL)text_font=font;
}

void CDlg::DrawDlg()
{
	if(!setting || !show_dlgbox)return;	//초기화가 안 되어 있으면 리턴

	if(dlgbox==NULL)
	{
		//위
		jdd->DrawPicture(backbuffer,"CommonDlg1",x-frame_x,y-frame_y,NULL);
		for(int i=0; i<width; ++i)jdd->DrawPicture(backbuffer,"CommonDlg2",x+(i*10),y-frame_y,NULL);
		jdd->DrawPicture(backbuffer,"CommonDlg3",x+x_size,y-frame_y,NULL);
		//중간
		for(int i=0; i<height; ++i)
		{
			int tmp_y=y+i*20;
			jdd->DrawPicture(backbuffer,"CommonDlg4",x-frame_x,tmp_y,NULL);
			for(int j=0; j<width; ++j)jdd->DrawPicture(backbuffer,"CommonDlg5",x+(j*10),tmp_y,NULL);
			jdd->DrawPicture(backbuffer,"CommonDlg4",x+x_size,tmp_y,NULL);
		}
		
		//아래
		jdd->DrawPicture(backbuffer,"CommonDlg7",x-frame_x,y+y_size,NULL);
		for(int i=0; i<width; ++i)jdd->DrawPicture(backbuffer,"CommonDlg2",x+(i*10),y+y_size,NULL);
		jdd->DrawPicture(backbuffer,"CommonDlg9",x+x_size,y+y_size,NULL);
	}
	else
	{
		jdd->DrawPicture(backbuffer,dlgbox,x-frame_x,y-frame_y,NULL);
	}
	/*속도가 느려서 strech로 대화창을 출력하는 식으로 바꿀 생각(사용들이 자신만의 대화창 만들기도 쉬움)*/
}

void CDlg::ShowShadow(BOOL show_or_hide)
{
	b_shadow=show_or_hide;
}

void CDlg::ShowDlg(BOOL show_or_hide)
{
	show_dlgbox=show_or_hide;
}

BOOL CDlg::MouseInBox()
{
	if(mouse_control && MouseX>=x && MouseX<x+width*10 && MouseY>=y && MouseY<y+line*20)return TRUE;
		else return FALSE;
}

void CDlg::SetOpacity(float opacity)
{
	JPictureInfo pi;
	pi.SetOpacity(opacity);
	jdd->SetPictureInfo("CommonDlg1", &pi);
	jdd->SetPictureInfo("CommonDlg2", &pi);
	jdd->SetPictureInfo("CommonDlg3", &pi);
	jdd->SetPictureInfo("CommonDlg4", &pi);
	jdd->SetPictureInfo("CommonDlg5", &pi);
	jdd->SetPictureInfo("CommonDlg6", &pi);
	jdd->SetPictureInfo("CommonDlg7", &pi);
	jdd->SetPictureInfo("CommonDlg8", &pi);
	jdd->SetPictureInfo("CommonDlg9", &pi);
}

void CDlg::SetFrameSize(int vx, int vy)
{
	frame_x=vx;
	frame_y=vy;
}
//////////////////////////////////////////////////////
//출력을 위한 대화상자
#define SNR_START 0

CTextDlg::CTextDlg(unsigned int vbuffer_size)
{
	buffer_size=vbuffer_size;
	dlg_buffer=new char[buffer_size];	//대화 내용 버퍼
	text_buffer=new char[buffer_size];	//현재까지 출력한 내용의 버퍼

	lock=FALSE;
}

CTextDlg::~CTextDlg()
{
	//버퍼 지우기
	if(dlg_buffer!=NULL)
	{
		delete[] dlg_buffer;
		dlg_buffer=NULL;
	}

	if(text_buffer!=NULL)
	{
		delete[] text_buffer;
		text_buffer=NULL;
	}
}

void CTextDlg::SetDlg(int vx, int vy, int vwidth, int vline)
{
	x=vx;
	y=vy;
	width=Max(30,vwidth);
	width/=10;
	line=Max(1,vline);

	setting=TRUE;
}

//대화 만들기
void CTextDlg::MakeText(char* content)
{
	if(!setting || content==NULL)return;	//초기화가 안 되어 있으면 리턴

	//대화내용 복사
	length=(int)strlen(content);
	strncpy(dlg_buffer, content, buffer_size);
	if(strlen(content) >= buffer_size)dlg_buffer[buffer_size-1]=NULL;
	ani_end=FALSE;

	//대화를 출력하기 알맞게 처리
	sp=SNR_START;	//처리되고 있는 포인트
	int space=-1;	//이전까지 처리한 포인트, space가 있던 곳
	int senlen=0;	//문장의 길이
	
	//대화창에 맞게 문장을 줄 바꿈
	if(text_font==&global_font)
	while(sp<length)
	{
		BOOL length_check=TRUE;		//길이 체크를 할지 안 할지 검사
		int sp_add=1;				//이동할 칸 수, 한글일 경우만 2칸.

		//줄 바꿈
		if(dlg_buffer[sp]=='\\')
		{
			dlg_buffer[sp]='\n';
			space=-1;
			senlen=0;
			length_check=FALSE;
		}
		//한글
		else if(dlg_buffer[sp]&0x80)
		{
			senlen+=20;
			sp_add=2;
		}
		//기타
		else
		{
			if(dlg_buffer[sp]==' ')space=sp;
			senlen+=GetCharLength(dlg_buffer[sp]);
		}
		
		//길이가 대화창을 넘었는지 검사
		if(length_check && senlen>width*10)
		{
			if(space<0)	//스페이스가 하나도 없을때 강제 줄 변환
			{
				//사이에 CR을 넣어줌
				char* tmp_text;
				tmp_text = new char[length+2];

				//새로운 문자열(지금 것보다 길이가 1 큰 것)에 내용 복사
				for(int i=0; i<sp; ++i)tmp_text[i]=dlg_buffer[i];
				tmp_text[sp]='\n';
				for(int i=sp+1; i<length+1; ++i)tmp_text[i]=dlg_buffer[i-1];
				tmp_text[length+1]=NULL;
				strcpy(dlg_buffer,tmp_text);
				delete[] tmp_text;

				//지금 문자열 길이는 지금 문자의 길이
				if(dlg_buffer[sp+1]&0x80)senlen=20;
					else senlen=GetCharLength(dlg_buffer[sp+1]);

				++length;
				++sp;
			}
			else
			{
				dlg_buffer[space]='\n';
				senlen=0;
				for(int i=space+1;i<=sp;++i)
				{
					if(dlg_buffer[i]&0x80)
					{
						senlen+=20;
						++i;
					}
					//기타
					else senlen+=GetCharLength(dlg_buffer[i]);
				}
				space=-1;
			}
		}

		//한글일 때 2칸 뛰고 나머지는 1칸
		sp+=sp_add;
	}

	sp=SNR_START, tp=0;;	//출력위치 초기화
	n_of_e=0;				//줄바꿈 횟수
	//대화창 크기 설정
	x_size=width*10;
	y_size=line*20;
	height=line;

	setting2=TRUE;
}

//대화 출력(정지된 상태에서)
void CTextDlg::Print(char* content)
{
	if(!setting)return;	//초기화가 안 되어 있으면 리턴

	MakeText(content);

	//뒷배경 잠시 저장(투명도 있는 대화창일 때 겹쳐 쓰면서 투명도가 없어지는 것을 방지
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		if(Printing()==999)break;	//내용을 다 출력하면 중단
		jdd->Render();
		//빨리 넘기기
		if(keyboard_control && GetKey(vkey_enter, 0) || mouse_control && LButton);
			else Sleep(10);
	}

	jdd->DeleteSurface("CommonCopyScreen");
}

//대화출력(실시간으로)
int CTextDlg::Printing()
{
	if(!setting || !setting2)return -1;	//초기화가 안 되어 있으면 리턴

	if(!ani_end)
	{
		if(sp<length && n_of_e<line)
		{
			if(dlg_buffer[sp]=='\n')++n_of_e;
			text_buffer[tp]=dlg_buffer[sp];
			if(dlg_buffer[sp]&0x80)	//한글일 경우 2칸 전진
			{
				++sp; ++tp;
				text_buffer[tp]=dlg_buffer[sp];
			}
			text_buffer[tp+1]=NULL;
			//포인터 증가
			++sp; ++tp;
		}
		else ani_end=TRUE;
	}
	else
	{
		//다음으로
		if(keyboard_control && GetKey(vkey_enter) || mouse_control && LeftDown())
		{
			if(sp<length)
			{
				ani_end=FALSE;
				n_of_e=0;
				tp=0;
			}
			else
			{
				if(!lock)return 999;
			}
		}
	}
	//대화 출력
	DrawDlg();

	RECT text, shadow;
	//그림자
	if(b_shadow)
	{
		SetRect(&shadow,x+1,y+1,SCREEN_X,SCREEN_Y);
		jdd->DrawText(backbuffer,text_buffer,*text_font,&shadow,JColor(0,0,0));
	}
	//글자
	SetRect(&text,x,y,SCREEN_X,SCREEN_Y);
	jdd->DrawText(backbuffer,text_buffer,*text_font,&text,color);

	return 0;
}

void CTextDlg::Lock()
{
	lock=TRUE;
}

void CTextDlg::UnLock()
{
	lock=FALSE;
}

//////////////////////////////////////////////////////
//선택을 위한 대화상자
CSelectDlg::CSelectDlg(int select_max, unsigned int vbuffer_size)
{
	buffer_size=vbuffer_size;
	dlg_buffer=new char[buffer_size];
	select_point=new char*[select_max];

	setting2=FALSE;
}

CSelectDlg::~CSelectDlg()
{
	//버퍼 지우기
	if(dlg_buffer!=NULL)
	{
		delete [] dlg_buffer;
		dlg_buffer=NULL;
	}

	if(select_point!=NULL)
	{
		delete [] select_point;
		select_point=NULL;
	}
}

void CSelectDlg::SetDlg(int vx, int vy, int vgap, int vline, BOOL vrow, int start)
{
	x=vx;
	y=vy;
	gap=vgap;
	line=vline;
	select=start;

	//gap크기를 10으로 떨어지게 수정
	gap=Max(gap, 30);
	gap-=(gap%10);

	//가로선택지인지 세로 선택지인지 확인, 길이 조정
	if(line<=0)line=1;
	row=vrow;

	//셀렉트바 위치 초기화
	if(row)
	{
		sx=x+select%line*gap;
		sy=y+(select/line)*20;
	}
	else
	{
		sx=x+select/line*gap;
		sy=y+(select%line)*20;
	}

	setting=TRUE;
}

void CSelectDlg::MakeSelection(char* input_dlg)
{
	if(!setting || input_dlg==NULL)return;

	//버퍼 크기를 넘어가는 것을 방지
	strncpy(dlg_buffer, input_dlg, buffer_size);
	if(strlen(input_dlg)>=buffer_size)dlg_buffer[buffer_size-1]=NULL;

	int length=(int)strlen(dlg_buffer);

	//대화를 출력하기 알맞게 처리
	n_of_e=0;
	int bp=1;
	select_point[0]=dlg_buffer;
	for(int i=0; i<length; ++i)
	{
		if(dlg_buffer[i]=='\\')
		{
			dlg_buffer[i]=NULL;
			select_point[bp]=dlg_buffer+i+1;
			++n_of_e;
			++bp;
			continue;
		}

		if(dlg_buffer[i]==13)
		{
			continue;
		}
	}

	//스크립트에서 읽어 온 문장의 끝에 음표(0x0d)가 생기는 것 방지
	if(dlg_buffer[length-1]==13)dlg_buffer[length-1]=NULL;

	//대화창 표시에 필요한 것들 계산
	if(row)
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		x_size=line*gap;
		y_size=col*20;
		width=gap/10*line;
		height=col;
	}
	else
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		x_size=col*gap;
		y_size=line*20;
		width=gap/10*col;
		height=line;
	}

	setting2=TRUE;
}

int CSelectDlg::Select(char* input_dlg, BOOL cancel)
{
	if(!setting)return -999;

	MakeSelection(input_dlg);
	//뒷배경 잠시 저장(투명도 있는 대화창일 때 겹쳐 쓰면서 투명도가 없어지는 것을 방지
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		int selected=Selecting();
		if(selected>=0 || (cancel && selected==-999))break;	//선택하거나 취소하면 중단

		jdd->Render();
	}

	jdd->DeleteSurface("CommonCopyScreen");

	return select;
}

int CSelectDlg::Selecting()
{
	if(!setting2)return -999;

	//대화상자
	if(keyboard_control)
	{
		if(row)
		{
			//위 아래 이동
			if(GetKey(vkey_right,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_left,10))select=Max(0,select-1);
			//좌우 이동
			if(GetKey(vkey_down,10))
			{
				if(select+line<=n_of_e)select+=line;
			}
			else if(GetKey(vkey_up,10))
			{
				if(select-line>=0)select-=line;
			}

			sx=x+select%line*gap;
			sy=y+(select/line)*20;
		}
		else
		{
			//위 아래 이동
			if(GetKey(vkey_down,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_up,10))select=Max(0,select-1);

			//좌우 이동
			if(GetKey(vkey_right,10))
			{
				if(select+line<=n_of_e)select+=line;
			}
			else if(GetKey(vkey_left,10))
			{
				if(select-line>=0)select-=line;
			}

			sx=x+select/line*gap;
			sy=y+(select%line)*20;
		}
	}

	//마우스로 선택
	BOOL mouse_select=FALSE;
	if(mouse_control && MouseInBox())
	{
		for(int i=0; i<=n_of_e; ++i)
		{
			int tsx, tsy;
			if(row)
			{
				tsx=x+(i%line)*gap;
				tsy=y+(i/line)*20;
			}
			else
			{
				tsx=x+(i/line)*gap;
				tsy=y+(i%line)*20;
			}

			if(MouseX>=tsx && MouseX<tsx+gap && MouseY>=tsy && MouseY<tsy+20)
			{
				select=i;
				mouse_select=TRUE;
				sx=tsx;
				sy=tsy;
				break;
			}
		}
	}


	DrawDlg();
	//선택 커서
	RECT src_rect;
	SetRect(&src_rect,0,0,gap/2,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx,sy,&src_rect);
	SetRect(&src_rect,320-gap/2,0,320,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx+gap/2,sy,&src_rect);

	//선택지들
	for(int i=0; i<=n_of_e; ++i)
	{
		int tx,ty;
		if(row)
		{
			tx=(i%line)*gap+x;
			ty=(i/line)*20+y;
		}
		else
		{
			tx=(i/line)*gap+x;
			ty=(i%line)*20+y;
		}

		if(b_shadow)jdd->DrawText(backbuffer,select_point[i],*text_font,tx+1,ty+1,JColor(0,0,0));
		jdd->DrawText(backbuffer,select_point[i],*text_font,tx,ty,color);
	}

	if(keyboard_control && GetKey(vkey_enter) || mouse_select && LeftDown())return select;
		else if(keyboard_control && GetKey(vkey_esc) || mouse_control && RightDown())return -999;
		else return -1;
}

void CSelectDlg::SetSelect(int select)
{
	this->select = select;
}

//////////////////////////////////////////////////////
//입력을 위한 대화상자
CInputDlg::CInputDlg(int vx, int vy, int max)
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(vx, vy, max);
}

CInputDlg::CInputDlg()
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(250,230,140);	//기본값
}

void CInputDlg::SetDlg(int vx, int vy, int max)
{
	Clear();
	str_max=max;

	x=vx;
	y=vy;
	width=Max(30,max);
	width/=10;

	str_max*=10;
	//대화창 크기 설정
	setting=TRUE;
	x_size=str_max/10;
	y_size=20;
	height=line=1;

	setting=TRUE;
}

void CInputDlg::Clear()
{
	strcpy(input_text,"");
	strcpy(eng_buffer,"");
	old_len=text_length=0;
}

//한글입력
char* CInputDlg::Input()
{
	//뒷배경 잠시 저장(투명도 있는 대화창일 때 겹쳐 쓰면서 투명도가 없어지는 것을 방지
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		if(Inputing())break;	//입력이 끝나면 중단

		jdd->Render();
	}

	jdd->DeleteSurface("CommonCopyScreen");

	return input_text;
}

//입력가능한 특수문자들
char num_shift_char[]={')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
char special_char[]={';', '=', ',', '-', '.', '/', '`', '[', '\\', ']','\''};	// `는 신명조에서 지원 안 됨
char special_shift_char[]={':', '+', '<', '_', '>', '?', '~', '{', '|', '}','"'};

int CInputDlg::Inputing()
{
	//출력
	DrawDlg();
	if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
	//깜빡이는 커서
	char show_text[101];
	strcpy(show_text, input_text);
	static int blink_count;
	++blink_count;
	if(blink_count<0 || blink_count>=40)blink_count=0;
	if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
	//입력된 정보 표시
	if(b_shadow)jdd->DrawText(backbuffer,show_text,*text_font,x+1,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,show_text,*text_font,x,y,color);

	int eng_len=(int)strlen(eng_buffer);
	//입력
	if(eng_len<80)
	{
		BOOL done=FALSE;
		//문자키
		if(flag & FLAG_TEXT)
		{
			for(int i=vkey_a; i<=vkey_z; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]='A'+(char)(i-vkey_a);
						else eng_buffer[buffer_length]='a'+(char)(i-vkey_a);
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}
		//숫자키
		if(flag & FLAG_NUMBER)
		{
			if(!done)for(int i=vkey_0; i<=vkey_9; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]=num_shift_char[i-vkey_0];
						else eng_buffer[buffer_length]='0'+(char)(i-vkey_0);
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}
		//기타 기호
		if(flag & FLAG_MARK)
		{
			if(!done)for(int i=vkey_colon; i<=vkey_quotation; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]=special_shift_char[i-vkey_colon];
						else eng_buffer[buffer_length]=special_char[i-vkey_colon];
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}

		//스페이스
		if(!done && GetKey(vkey_space))strcat(eng_buffer, " ");

		if(done && strlen(eng_buffer)>0)is_eng_buffer[strlen(eng_buffer)-1]=is_eng;
	}
	//특수 키
	if(GetKey(vkey_enter))return 1;	//엔터
	else if(GetKey(vkey_back, 10))	//백스페이스
	{
		if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
	}
	else if(GetKey(vkey_alt))
	{
		is_eng=1-is_eng;
	}

	//입력 정보 갱신
	if(old_len!=eng_len)
	{
		BOOL able_text=FALSE;
		while(!able_text)
		{
			//문자 전체 길이 계산
			strcpy(input_text, m_han.EngToHan(eng_buffer, is_eng_buffer));
			text_length=0;
			for(size_t i=0; i<strlen(input_text) ; ++i)text_length+=GetCharLength(input_text[i]);

			//만약 대화창 이상의 길이라면 자르기
			if(str_max/10<text_length && eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
				else able_text=TRUE;
		}
		old_len=eng_len;

		//서페이스에 그리기
		if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
		//깜빡이는 커서
		char show_text[101];
		strcpy(show_text, input_text);
		static int blink_count;
		++blink_count;
		if(blink_count<0 || blink_count>=40)blink_count=0;
		if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
		//입력된 정보 표시
		jdd->DrawText(backbuffer,show_text,*text_font,x,y,color);

	}

	return 0;
}

void CInputDlg::SetFlag(BOOL vflag)
{
	flag = vflag;
}

/////////////////////////////////
//명령 처리 클래스 메소드
//(문자 배열로 char com={"이동","대기"...}등을 만들어 놓고, 가능한 명령을 넣어서 선택할 수 있게 한다.

CCommand::CCommand(char* vsource[], int vx, int vy, int vgap, int vline, BOOL vrow, int max)
				//명령어 문자 배열, X,Y좌표, 사이간격, 몇개 이상 표시되면 줄을 바꿀건지, 세로 or 가로, 최대치 (버퍼사이즈 추가할 것!)
{
	comdlg.SetDlg(vx, vy, vgap, vline, vrow);
	Init(vsource,max);
	x=vx;
	y=vy;
	gap=vgap;
	line_max=vline;
	row=vrow;
}

void CCommand::Init(char* vsource[], int max)
{
	count=0;
	source=vsource;
	command_max=max;
	strcpy(commands,"");
	comdlg.MakeSelection(commands);
}

void CCommand::AddComs(int n, ...)	//명령 여러개 추가
{
	va_list ap;
	va_start(ap,n);
	for(int i=0; i<n; ++i)AddCom(va_arg(ap,int));
	va_end(ap);
}

void CCommand::AddCom(int id)	//사용할 수 있는 명령 추가
{
	if(count>=command_max)return;	//명령어가 꽉 참
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	comdlg.SetDlg(x, y, gap, Min(line_max, count+1), row);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

//문자배열을 이용하지 않고 직접 집어 넣기
void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//명령어가 꽉 참
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,str);
	comdlg.SetDlg(x, y, gap, Min(line_max, count+1), row);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

int CCommand::GetComID(int count)
{
	if(count>=0 && count<command_max)return com_id[count];
		else return -1;
}

//가득찼는지 확인
BOOL CCommand::IsFull()
{
	if(count>=command_max)return TRUE;
		else return FALSE;
}

//실시간으로 선택(다른 작업과 병렬로 처리)
int CCommand::CommandSelecting()
{
	if(count<1)return -999;		//사용 가능 명령이 없으면 실행하지 않음

	int selected;
	selected=comdlg.Selecting();

	if(selected>=0)return GetComID(selected);
		else return -1;
}

//선택만 함(프로세스 독점)
int CCommand::CommandSelect()
{
	int selected;
	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		selected=comdlg.Selecting();	//명령 선택
		if(selected>=0)break;
		else if(selected==-999)return -1;

		jdd->Render();
	}

	return GetComID(selected);
}

//비었으면 TRUE, 차 있으면 FALSE를 리턴
BOOL CCommand::Empty()
{
	if(count==0)return TRUE;
		else return FALSE;
}

//////////////////////////////////////
//파일 처리 클래스
CFiles::CFiles()
{
}

CFiles::~CFiles()
{
	std::vector<char*>::iterator it;
	for(it=filename.begin(); it!=filename.end(); ++it)
	{
		delete [] (*it);
	}

	filename.clear();
}

void CFiles::AddFile(char* name)
{
	char* newname = new char[strlen(name)+1];
	strcpy(newname, name);
	filename.push_back(newname);
}

void CFiles::SearchFile(char* dir, char* filetype)
{
	HANDLE hFind;
    WIN32_FIND_DATA FindData;
    int ErrorCode;
    BOOL Continue = TRUE;

	char search_for[256];
	sprintf(search_for, "%s\\*.%s", dir, filetype);

    hFind = FindFirstFile(search_for, &FindData);

    if(hFind == INVALID_HANDLE_VALUE)
    {
        ErrorCode = GetLastError();
        if (ErrorCode == ERROR_FILE_NOT_FOUND)
        {
            //printf("There are no files matching that path/mask\n");
        }
        else
        {
            //printf("FindFirstFile() returned error code %d", ErrorCode);
        }
        Continue = FALSE;
    }
    else
    {
        AddFile(FindData.cFileName);
    }

    if (Continue)
    {
        while (FindNextFile(hFind, &FindData))
        {
           AddFile(FindData.cFileName);
        }

        ErrorCode = GetLastError();

        if (ErrorCode != ERROR_NO_MORE_FILES)
        {
            //printf("FindNextFile() returned error code %d", ErrorCode);
        }

        if (!FindClose(hFind))
        {
            ErrorCode = GetLastError();
            //printf("FindClose() returned error code %d", ErrorCode);
        }
    }
}

//////////////////////////////////////////////////////////////////
//CAnimation 메소드
CAnimationData::CAnimationData(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	frame=delay=y_point=0;
	walk=FALSE;
}

//설정 : X크기, Y크기, 애니메이션타입, 그림 수, 지연시간
void CAnimationData::Set(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	x_size=x;
	y_size=y;
	SetRect(&ani_rect, 0, 0, x_size, y_size);
	type=vtype;
	if(type==2)frame_max=(vframe_max-1)*2;
		else frame_max=vframe_max;
	delay_max=vdelay_max;
}

//애니메이션 처리를 해줌(이것을 해주지 않으면 자동으로 애니메이션이 되지 않음)
void CAnimationData::Process()
{
	if(type==ani_loop)	//일직선 반복 애니메이션
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=0;
		}
		
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
	else if(type==ani_exchange)	//왔다갔다 반복 애니메이션
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=0;
		}
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, y_point, x_size*(frame2+1), y_point+y_size);
	}
	if(type==ani_once)	//일직선 한번 애니메이션
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=frame_max-1;
		}
		
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
}

//애니메이션을 시작점으로
void CAnimationData::Reset()
{
	SetFrame(0);
	delay=0;
}

//상하좌우 애니메이션을 하는 경우 Y좌표를 바꿔서 애니메이션
void CAnimationData::SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos)
{
	up=up_ypos;
	down=down_ypos;
	left=left_ypos;
	right=right_ypos;
	
	walk=TRUE;
}

//장면 정하기
void CAnimationData::SetFrame(int vframe)
{
	frame=vframe;
	SetRect(&ani_rect, x_size*vframe, y_point, x_size*(vframe+1), y_point+y_size);
}

//X,Y 값 가져오기
int CAnimationData::GetXSize()
{
	return x_size;
}

int CAnimationData::GetYSize()
{
	return y_size;
}

void CAnimationData::SetDirection(WalkDirection dir)
{
	if(!walk)return;

	//방향 설정
	if(dir==dir_up)y_point=up;
	else if(dir==dir_left)y_point=left;
	else if(dir==dir_down)y_point=down;
	else if(dir==dir_right)y_point=right;
	//반대로 돌기
	else if(dir==dir_turn)
	{
		if(dir==dir_up)y_point=down;
		else if(dir==dir_left)y_point=right;
		else if(dir==dir_down)y_point=up;
		else if(dir==dir_right)y_point=left;
	}
	//왼쪽으로 돌기
	else if(dir==dir_turn_left)
	{
		if(dir==dir_up)y_point=left;
		else if(dir==dir_left)y_point=down;
		else if(dir==dir_down)y_point=right;
		else if(dir==dir_right)y_point=up;
	}
	//오른쪽으로 돌기
	else if(dir==dir_turn_right)
	{
		if(dir==dir_up)y_point=right;
		else if(dir==dir_left)y_point=up;
		else if(dir==dir_down)y_point=left;
		else if(dir==dir_right)y_point=down;
	}
	//아무데나 보기
	else if(dir==dir_random)
	{
		int wd=rand()%4;
		if(wd==0)y_point=up;
		else if(wd==1)y_point=left;
		else if(wd==2)y_point=down;
		else if(wd==3)y_point=right;
	}

	SetFrame(frame);
}

void CAnimationData::Draw(char* dest, char* src, int x, int y, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPicture(dest, src, x, y, &ani_rect);
}

void CAnimationData::Draw(int dest, int src, int x, int y, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPicture(dest, src, x, y, &ani_rect);
}

void CAnimationData::DrawEx(char* dest, char* src, int x, int y, DWORD dwFlags, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPictureEx(dest, src, x, y, &ani_rect, dwFlags);
}

void CAnimationData::DrawEx(int dest, int src, int x, int y, DWORD dwFlags, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPictureEx(dest, src, x, y, &ani_rect, dwFlags);
}

void CAnimation::CreateAnimation(int id, int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	if(anidata[id] != NULL)delete anidata[id];
	anidata[id]=new CAnimationData(x, y, vtype, vframe_max, vdelay_max);
}

void CAnimation::Process()
{
	//자동 애니메이션
	std::map<int, CAnimationData*>::iterator it;
	for(it=anidata.begin(); it!=anidata.end(); ++it)
	{
		it->second->Process();
	}
}

CAnimationData* CAnimation::GetAni(int id)
{
	if(anidata[id]==NULL)
	{
		OutputDebugString("잘못된 애니메이션 ID를 요청하셨습니다.\n");
		OutputDebugString("에러를 막기 위해 임시 애니메이션 정보를 생성해서 보냅니다.\n");
		//anidata[id]=new CAnimationData(10, 10, ani_normal, 1, 1);
		anidata[id]=new CAnimationData(640, 480, ani_exchange, 3, 1000);
	}

	return anidata[id];
}

CAnimation::CAnimation()
{
}

CAnimation::~CAnimation()
{
	std::map<int, CAnimationData*>::iterator it;
	for(it=anidata.begin(); it!=anidata.end(); ++it)
	{
		if((it->second) != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	anidata.clear();
}

////////////////////////////////////////////
//길 찾기 메서드

CSearchMap::CSearchMap(int* p_map, int x, int y)
{
	//지도 크기 정하기
	x_size=x;
	y_size=y;

	//이동력이 적힌 맵 주소 가져오기
	map=p_map;

	//이동력을 나타낼 맵 생성
	roadmap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		roadmap[i] = new int[y_size];

	//거리를 나타낼 맵 생성
	distancemap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		distancemap[i] = new int[y_size];

	point.x=point.y=0;
}

CSearchMap::~CSearchMap()
{
	//메모리 소거
	if(roadmap)
	{
		for(int i=0; i<x_size; ++i)
		{
			delete [] roadmap[i];
			roadmap[i]=NULL;
		}
		delete [] roadmap;
		roadmap=NULL;
	}

	if(distancemap)
	{
		for(int i=0; i<x_size; ++i)
		{
			delete [] distancemap[i];
			distancemap[i]=NULL;
		}
		delete [] distancemap;
		distancemap=NULL;
	}
}

void CSearchMap::SetStartPoint(MapPoint p, int move_max)
{
	SetStartPoint(p.x, p.y, move_max);
}

void CSearchMap::SetStartPoint(int x, int y, int move_max)
{
	point.x=MaxMin(x, 0, x_size-1);
	point.y=MaxMin(y, 0, y_size-1);

	//맵 초기화
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			roadmap[i][j]=MOVE_DISABLE;

	//길 찾기
	FindRoad(roadmap, point.x, point.y, 0, move_max);
}

void CSearchMap::SetEndPoint(MapPoint p)
{
	SetEndPoint(p.x, p.y);
}

void CSearchMap::SetEndPoint(int x, int y)
{
	//맵 초기화
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			distancemap[i][j]=MOVE_DISABLE;

	//이동비용 검사
	FindRoad(distancemap, x, y, 0);
}

void CSearchMap::FindRoad(int** vmap, int x, int y, int expense, int move_max)
{
	vmap[x][y]=expense;
	if(move_max>=0 && expense>move_max)return;	//이동력 한계일 때는 더 이상 검사하지 않음

	if(x>0 && vmap[x-1][y]>expense+map[(x-1)+y*x_size])
		FindRoad(vmap, x-1, y, expense+map[(x-1)+y*x_size], move_max);	//왼쪽 검사
	if(x<x_size-1 && vmap[x+1][y]>expense+map[(x+1)+y*x_size])
		FindRoad(vmap, x+1, y, expense+map[(x+1)+y*x_size], move_max);	//오른쪽 검사
	if(y>0 && vmap[x][y-1]>expense+map[x+(y-1)*x_size])
		FindRoad(vmap, x, y-1, expense+map[x+(y-1)*x_size], move_max);	//위 검사
	if(y<y_size-1 && vmap[x][y+1]>expense+map[x+(y+1)*x_size])
		FindRoad(vmap, x, y+1, expense+map[x+(y+1)*x_size], move_max);	//아래 검사
}

BOOL CSearchMap::MoveNextPointFast(int move)
{
	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//왼쪽 검사
	if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
	{
		best_dir=0;
		best_expense=distancemap[point.x-1][point.y];
	}
	//오른쪽 검사
	if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
	{
		best_dir=1;
		best_expense=distancemap[point.x+1][point.y];
	}
	//위 검사
	if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
	{
		best_dir=2;
		best_expense=distancemap[point.x][point.y-1];
	}
	//아래 검사
	if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
	{
		best_dir=3;
		best_expense=distancemap[point.x][point.y+1];
	}

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 TRUE, 아니면 FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPoint(int move)
{
	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//첫째로 검사하는 곳을 랜덤하게 정하기 위한 플래그
	//(우선순위가 정해져 있으면 패턴이 단순해서 보기에 안 좋다.)
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//왼쪽 검사
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//오른쪽 검사
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//위 검사
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//아래 검사
		else if(ran==3)
		{
			if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
			{
				best_dir=3;
				best_expense=distancemap[point.x][point.y+1];
			}
			down_ok=TRUE;
		}
	}

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 TRUE, 아니면 FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPointStupid(int move, int iq)
{
	iq=MaxMin(iq, 1, 10);	//아이큐는 1~10까지

	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=distancemap[point.x][point.y]+(10-iq);

	//멍청하기 때문에 모두 검사하지 않고 몇개를 랜덤으로 시도해 본 뒤 그 중에서 이동
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;
	int count=0;

	while(count<iq && (!left_ok || !right_ok || !up_ok || !down_ok))
	{
		int ran=rand()%4;

		//왼쪽 검사
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//오른쪽 검사
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//위 검사
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//아래 검사
		else if(ran==3)
		{
			if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
			{
				best_dir=3;
				best_expense=distancemap[point.x][point.y+1];
			}
			down_ok=TRUE;
		}

		++count;
	}

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 TRUE, 아니면 FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

int CSearchMap::GetRoadMap(int x, int y)
{
	return roadmap[x][y];
}

int CSearchMap::GetDistanceMap(int x, int y)
{
	return distancemap[x][y];
}

int CSearchMap::GetPointX()
{
	return point.x;
}

int CSearchMap::GetPointY()
{
	return point.y;
}

///////////////////////////////////////////////////////////////////////////////
//Direct Music
CDirectMusic::CDirectMusic()
{
	g_pMusicManager          = NULL;
	g_pMusicSegment          = NULL;
	g_hInst                  = NULL;
	g_hDMusicMessageEvent    = NULL;
}

CDirectMusic::~CDirectMusic()
{
    SAFE_DELETE( g_pMusicSegment );
    SAFE_DELETE( g_pMusicManager );
    CloseHandle( g_hDMusicMessageEvent );
}

HRESULT CDirectMusic::InitDirectMusic()
{
    HRESULT hr; 

    g_hDMusicMessageEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    g_pMusicManager = new CMusicManager();
    if( NULL == g_pMusicManager )
        return E_OUTOFMEMORY;

    if( FAILED( hr = g_pMusicManager->Initialize( hwnd ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );

    // Register segment notification
    IDirectMusicPerformance* pPerf = g_pMusicManager->GetPerformance();
    GUID guid = GUID_NOTIFICATION_SEGMENT;
    pPerf->AddNotificationType( guid );
    pPerf->SetNotificationHandle( g_hDMusicMessageEvent, 0 );  

    return S_OK;
}

HRESULT CDirectMusic::LoadSegmentFile(TCHAR* strFileName)
{
    HRESULT hr;

    // Free any previous segment, and make a new one
    SAFE_DELETE( g_pMusicSegment );

    // Have the loader collect any garbage now that the old 
    // segment has been released
    g_pMusicManager->CollectGarbage();

    // Set the media path based on the file name (something like C:\MEDIA)
    // to be used as the search directory for finding DirectMusic content
    // related to this file.
    TCHAR strMediaPath[MAX_PATH];
    _tcsncpy( strMediaPath, strFileName, MAX_PATH );
    strMediaPath[MAX_PATH-1] = 0;
    TCHAR* strLastSlash = _tcsrchr(strMediaPath, TEXT('\\'));
    if( strLastSlash )
    {
        *strLastSlash = 0;
        if( FAILED( hr = g_pMusicManager->SetSearchDirectory( strMediaPath ) ) )
            return DXTRACE_ERR_MSGBOX( TEXT("SetSearchDirectory"), hr );
    }

    // For DirectMusic must know if the file is a standard MIDI file or not
    // in order to load the correct instruments.
    BOOL bMidiFile = FALSE;
    if( strstr( strFileName, ".mid" ) != NULL ||
        strstr( strFileName, ".rmi" ) != NULL ) 
    {
        bMidiFile = TRUE;
    }

    BOOL bWavFile = FALSE;
    if( strstr( strFileName, ".wav" ) != NULL )
    {
        bWavFile = TRUE;
    }        

    // Load the file into a DirectMusic segment 
    if( FAILED( g_pMusicManager->CreateSegmentFromFile( &g_pMusicSegment, strFileName, 
                                                        TRUE, bMidiFile ) ) )
    {
        // Not a critical failure, so just update the status
        return S_FALSE; 
    }

    return S_OK;
}

HRESULT CDirectMusic::ProcessDirectMusicMessages()
{
    HRESULT hr;
    IDirectMusicPerformance8* pPerf = NULL;
    DMUS_NOTIFICATION_PMSG* pPMsg;
        
    if( NULL == g_pMusicManager )
        return S_OK;

    pPerf = g_pMusicManager->GetPerformance();

    // Get waiting notification message from the performance
    while( S_OK == pPerf->GetNotificationPMsg( &pPMsg ) )
    {
        switch( pPMsg->dwNotificationOption )
        {
        case DMUS_NOTIFICATION_SEGEND:
            if( pPMsg->punkUser )
            {
                IDirectMusicSegmentState8* pSegmentState   = NULL;
                IDirectMusicSegment*       pNotifySegment   = NULL;
                IDirectMusicSegment8*      pNotifySegment8  = NULL;
                IDirectMusicSegment8*      pPrimarySegment8 = NULL;

                // The pPMsg->punkUser contains a IDirectMusicSegmentState8, 
                // which we can query for the segment that the SegmentState refers to.
                if( FAILED( hr = pPMsg->punkUser->QueryInterface( IID_IDirectMusicSegmentState8,
                                                                  (VOID**) &pSegmentState ) ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("QueryInterface"), hr );

                if( FAILED( hr = pSegmentState->GetSegment( &pNotifySegment ) ) )
                {
                    // Sometimes the segend arrives after the segment is gone
                    // This can happen when you load another segment as 
                    // a motif or the segment is ending
                    if( hr == DMUS_E_NOT_FOUND )
                    {
                        SAFE_RELEASE( pSegmentState );
                        return S_OK;
                    }

                    return DXTRACE_ERR_MSGBOX( TEXT("GetSegment"), hr );
                }

                if( FAILED( hr = pNotifySegment->QueryInterface( IID_IDirectMusicSegment8,
                                                                 (VOID**) &pNotifySegment8 ) ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("QueryInterface"), hr );

                // Get the IDirectMusicSegment for the primary segment
                pPrimarySegment8 = g_pMusicSegment->GetSegment();

                // Cleanup
                SAFE_RELEASE( pSegmentState );
                SAFE_RELEASE( pNotifySegment );
                SAFE_RELEASE( pNotifySegment8 );
            }
            break;
        }

        pPerf->FreePMsg( (DMUS_PMSG*)pPMsg ); 
    }

    return S_OK;
}

HRESULT CDirectMusic::PlayAudio()
{
    HRESULT hr;

    // Set the segment to repeat many times
	if(g_pMusicSegment != NULL)
	{
		if( FAILED( hr = g_pMusicSegment->SetRepeats( DMUS_SEG_REPEAT_INFINITE ) ) )
			return DXTRACE_ERR_MSGBOX( TEXT("SetRepeats"), hr );

		// Play the segment and wait. The DMUS_SEGF_BEAT indicates to play on the 
		// next beat if there is a segment currently playing. 
		if( FAILED( hr = g_pMusicSegment->Play( DMUS_SEGF_BEAT ) ) )
			return DXTRACE_ERR_MSGBOX( TEXT("Play"), hr );

		return S_OK;
	}
	else return S_FALSE;	
}

void CDirectMusic::StopAudio()
{
    if( g_pMusicManager )
        g_pMusicManager->StopAll();
}

////////////////////////////////////////////////////////////
//초기화 작업(매번 바뀌지 않으니까 main에서 donglib로 옮겼음
BOOL MainInitialize(char* window_name, BOOL use_keyboard, BOOL use_mouse, BOOL window_mode)
{
	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	HINSTANCE hInstance=(HINSTANCE)0x00400000;

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"ICON.ico");
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.lpfnWndProc=WndProc;
	wc.hInstance=hInstance;
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName="Game";
	RegisterClass(&wc);
	
	if(window_mode)
	{
		LONG ws=WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		ws &= ~WS_THICKFRAME;
		ws &= ~WS_MAXIMIZEBOX;

		RECT crt;
		SetRect(&crt, 0, 0, SCREEN_X, SCREEN_Y);
		AdjustWindowRect(&crt, ws, FALSE);

		hwnd = CreateWindow("Game", window_name, ws, CW_USEDEFAULT, CW_USEDEFAULT, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
	    ShowCursor( TRUE );
	}
	else
	{
		hwnd=CreateWindow("Game",window_name,WS_POPUP|WS_VISIBLE,0,0,SCREEN_X,SCREEN_Y,NULL,NULL,hInstance,NULL);
	    ShowCursor( FALSE );
	}

	jdd->Initialize(NULL,hwnd,SCREEN_X,SCREEN_Y,16,TRUE,window_mode);

	//그래픽 초기화
	backbuffer=jdd->GetBackBuffer();
	global_font=jdd->CreateFont("궁서",20);
	//로딩에 긴 시간이 걸릴 때를 대비한 화면
	if(!(jdd->LoadPicture("CommonLoading", "Loading.jpg", NULL, TRUE)))jdd->LoadPicture("CommonLoading", "Loading.gif", NULL, TRUE);
	jdd->DrawPicture(backbuffer, "CommonLoading", 0, 0, NULL);
	jdd->Render();

	//임시 서페이스 생성
	JPictureInfo jpi;
	jpi.SetWidth(SCREEN_X);
	jpi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonBlank", &jpi, TRUE);
	
	jdd->SetFrameRate(100,TRUE);
	jdd->SetVerticalSync(FALSE);

	//사운드 초기화
	if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
	{
		SoundCard = TRUE;
		if (SoundOBJ->SetCooperativeLevel(hwnd,DSSCL_PRIORITY)!=DS_OK) return FALSE;

		memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return FALSE;
		SoundDSB -> SetVolume(0);
		SoundDSB -> SetPan(0);
	}
	else SoundCard = FALSE;

	//기타 초기화
	srand( (unsigned)time( NULL ) );
	keyboard_control=use_keyboard;
	mouse_control=use_mouse;
	gameover=FALSE;

	return TRUE;
}
