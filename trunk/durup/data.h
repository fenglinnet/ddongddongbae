/*  DATA.H에 대하여
-------------------------
DATA.H는 게임 만들 때 공통적으로 쓰이는 함수나 데이터가 들어있다.
그렇다고 하나의 라이브러리로 보긴 어렵다.
사실 메인과 분리되어 쓸 수도 없고, 다른 곳에 결합도 힘들다.
그리고 그대로 쓰이는 게 아니라 게임에 따라 알맞게 약간 수정해야 된다.
그러니까 그냥 만들때마다 반복되는 귀찮은 부분을 줄이는 역활을 하는 소스다.
*/


//뭔가 구분이 필요할 때 쓰는 숫자
#define N_ZERO		0
#define N_ONE		1
#define N_TWO		2
#define N_THREE		3
#define N_FOUR		4
#define N_FIVE		5

//기본 그림 파일들
#define PIC_DLG			"Dlg"
#define PIC_SELECTBAR	"Select"
#define PIC_BLACK		"Black"
#define PIC_WHITE		"White"

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))

FILE* fp;

char global_buffer[1024];

HWND hwnd;
//사운드
HSNDOBJ Sound[100];

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;
BOOL ReplayFlag;

//미디 연주
BOOL _MidiPlay(char* pszMidiFN, BOOL bReplayFlag = TRUE)
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

void _Play( int num )
{
    if ( SoundCard ) SndObjPlay( Sound[num], NULL );
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

//최대 최소값
int MaxMin(int x, int min, int max)
{
	if(x<min)return min;
		else if(x>max) return max;
		else return x;
}

bool Between(int value, int min, int max)
{
	if(value>=min && value<=max)return true;
		else return false;
}

//키
#define ALL_KEYS 71
int keys[ALL_KEYS]={
	VK_UP, VK_LEFT, VK_RIGHT, VK_DOWN, VK_RETURN, VK_ESCAPE, VK_SPACE, VK_CONTROL, VK_MENU,
	VK_TAB, VK_SHIFT, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6,
	VK_NUMPAD7, VK_NUMPAD8,	VK_NUMPAD9, VK_NUMPAD0,
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', VK_ADD, VK_BACK, VK_DECIMAL, VK_DELETE, VK_DIVIDE, VK_END, VK_HELP, VK_HOME,
	VK_INSERT, VK_MULTIPLY, VK_NEXT, VK_PRIOR, VK_PAUSE, VK_SUBTRACT
};

int keypush[ALL_KEYS];

//키 처리
bool GetKey(int key, int delay=-1)
{
	bool push;
	if(delay==-1)push=false;
		else push=true;

	if(_GetKeyState(keys[key]))
	{
		if(!keypush[key])
		{
			keypush[key]=delay;
			return true;
		}
		else if(push)
		{
			keypush[key]=Max(0,keypush[key]-1);
		}
	}
	else keypush[key]=0;
	
	return false;
}

int FindKey(int key)
{
	for(int i=0; i<ALL_KEYS; i++)
		if(keys[i]==key)return i;
	return -1;
}

//리스트 클래스
class CList
{
protected:
	int max;

	void New(int vid);
	void Copy(int to, int from);

public:
	int count;
	int *id;

	void Init();
	void Add(int vid);
	void Delete(int vid);
	int Find(int vid);
	bool IsFull();
};

void CList::New(int vid)
{
	if(IsFull())return;

	id[count]=vid;
	count++;
}

void CList::Copy(int to, int from)
{
	id[to]=id[from];
}

void CList::Init()
{
	count=0;
}

void CList::Add(int vid)
{
	if(Find(vid)==-1)New(vid);
}

void CList::Delete(int vid)
{
	if(vid<0 || vid>=max)return;

	for(int i=vid; i<count-1; i++)Copy(i,i+1);

	count--;
}

int CList::Find(int vid)
{
	for(int i=0; i<count; i++)
		if(id[i]==vid)return i;

	return -1;
}

bool CList::IsFull()
{
	if(count==max)return true;
		else return false;
}

/*화면 그리기와 프로세스, 컨트롤
 대화 출력 클래스 같은 곳에서 프로세스를 독점해야 처리가 편한 부분이 많아서,
 차라리 출력이나 제어 부분을 모드에 따라 달라지게 하는 함수로 만들고,
 클래스가 시스템을 독점하면서 이를 호출하는 방식으로 제작
 즉, 화면 출력의 모든 것은 Screen 하나가 담당하고, 처리는 Process 하나가 담당함.
 텍스트 출력 같은 경우, 텍스트 출력의 루프로 들어가서 그 루프 안에서 이 함수를 출력함으로,
 그 루프에서 돌지만 전체적인 출력과 제어가 되게 함.
*/
int game_mode;
static void Screen();
static void Process();
static void Control();

//문자열 처리 함수
char* StrAdd(char* msg, ...)
{
	strcpy(global_buffer,"");
	va_list ap;
	va_start(ap,msg);
	int max=strlen(msg);
	for(int i=0; i<max; i++)
	{
		if(msg[i]=='c')
		{
			strcat(global_buffer,va_arg(ap,char*));
		}
		else if(msg[i]=='d')
		{
			char int_buffer[10];
			itoa(va_arg(ap,int),int_buffer,10);
			strcat(global_buffer,int_buffer);
		}
	}

	return global_buffer;
}

//숫자 선택지 만들기
void MakeNumSelect(int from, int to, int step=1)
{
	char num_buffer[12];

	strcpy(global_buffer,"");
	for(int i=from; i<=to; i+=step)
	{
		if(i>from)strcat(global_buffer,"\\");
		strcat(global_buffer,itoa(i,num_buffer,10));
	}

}

//암호화를 위한 문자열
char CodeTo[76]="dyKp:8jP;[R^FIqN0WsTX4U`a52Z]1HnbuVvlh76\\ti=SArJ@>Q_wgkoB?LCcYD3<M9EmGxeOfz";

int get_char_num(char chr)
{
	int i;
	for(i=0; i<76; i++)
		if(CodeTo[i]==chr)break;

	return i;
}

//잠시 정지
void Pause()
{
	while(!GetKey(5) && !GetKey(4))
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//Process();
		Screen();

		jdd->Render();
	}
}

//화면 지우기
void Clear()
{
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
}

//페이드 아웃
void FadeOut()
{
	JPictureInfo pi;
	pi.SetOpacity(0.2f);
	jdd->SetPictureInfo(PIC_BLACK,&pi);
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<24; j++)
			jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
		jdd->Render();
		Sleep(30);
	}
	pi.SetOpacity(1.0);
	jdd->SetPictureInfo(PIC_BLACK,&pi);

	//완전하게 지움
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
	jdd->Render();
}

//화이트 아웃
void WhiteOut()
{
	JPictureInfo pi;
	pi.SetOpacity(0.2f);
	jdd->SetPictureInfo(PIC_WHITE,&pi);
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<24; j++)
			jdd->DrawPicture(backbuffer,PIC_WHITE,0,j*20,NULL);
		jdd->Render();
		Sleep(30);
	}
	pi.SetOpacity(1.0);
	jdd->SetPictureInfo(PIC_WHITE,&pi);

	//완전하게 지움
	for(int j=0; j<24; j++)
		jdd->DrawPicture(backbuffer,PIC_WHITE,0,j*20,NULL);
	jdd->Render();
}

/////////////////////////////////////////////////////
//한글 입력 시스템
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

#define STR_MAX	80
#define HAN_MAX 2364
#define	MOUM	19
char jamo[34]="rRseEfaqQtTdwWczxvgkoiOjpuPhynbml";
char* han_jamo[33]={"ㄱ","ㄲ","ㄴ","ㄷ","ㄸ","ㄹ","ㅁ","ㅂ","ㅃ","ㅅ","ㅆ","ㅇ","ㅈ","ㅉ","ㅊ","ㅋ","ㅌ","ㅍ","ㅎ",
					"ㅏ","ㅐ","ㅑ","ㅒ","ㅓ","ㅔ","ㅕ","ㅖ","ㅗ","ㅛ","ㅜ","ㅠ","ㅡ","ㅣ"};

class CHan
{
private:
	int mark[20];
	char han[STR_MAX];

	unsigned int FindMark(char eng);

public:
	char* EngToHan(char* text);

	CHan();
};

CHan::CHan()
{
	//검색 속도 향상을 위한 빨리가기
	int mp=0;
	char* check_text[19]={"가","까","나","다","따","라","마","바","빠","사","싸","아","자","짜","차","카","타","파","하"};
	for(unsigned int i=0; i<HAN_MAX; i++)
	{
		if(strcmp(han1[i*2], check_text[mp])==0)
		{
			mark[mp]=i;
			mp++;
			if(mp>=19)break;
		}
	}
	mark[19]=HAN_MAX;
}

unsigned int CHan::FindMark(char eng)
{
	for(unsigned int j=0; j<19; j++)
		if(eng==jamo[j])return j;

	return 0;
}

char* CHan::EngToHan(char* text)
{
	unsigned int pp=0;
	unsigned int begin[STR_MAX], end[STR_MAX];

	//입력 받기
	strcpy(han, "");	

	//모음, 자음 한글 아님 여부 확인(자음 인 부분을 포인트로 두어서 변환에 도움이 되게 함)
	for(unsigned int i=1; i<strlen(text); i++)
	{
		//한글에 대치 안 되는 대문자를 소문자로 바꿈
		if(text[i]>='A' && text[i]<='Z' && text[i]!='Q' && text[i]!='W' && text[i]!='E'	&& text[i]!='R'
			&& text[i]!='T' && text[i]!='O' && text[i]!='P')text[i]+=0x20;

		//자음인지 모음인지 구분
		for(int j=0; j<33; j++)
			if(text[i]==jamo[j])
			{
				if(j>=MOUM)
				{
					begin[pp]=i-1;
					if(pp>0)end[pp-1]=i-2;
					pp++;
					i++;	//이중 모음의 경우를 생각해서 다음을 넘김(이중 모음을 끊어 읽어 글자가 되는 경우는 없으므로 예: 뒷산(O) 두ㅣㅅ산(X))
				}
				break;
			}
	}
	if(pp>0)end[pp-1]=strlen(text)-1;

	//포인트 조정(한글인 부분만 해석)
	for(int i=0; i<pp; i++)
	{
		char buffer[10];
		int bp=0;
		bool head=true;	//앞 부분인지 글자 뒷부분인지 알아냄(잘라내야 하는 부분 예: 1@가2$ -> 1@은 head=true인 상태, 2$는 head=false인 상태에서 읽음
		for(unsigned int j=begin[i]; j<=end[i]; j++)
		{
			if(text[j]>='a' && text[j]<='z' || text[j]>='A' && text[j]<='Z')
			{
				buffer[bp]=text[j];
				bp++;
				head=false;
			}
			else
			{
				if(head)begin[i]++;
					else end[i]=j-1;
			}
		}
		buffer[bp]=NULL;

		//영문을 한글로 쳤을 때 일치하는 글자 찾기
		while(strlen(buffer)>1)
		{
			bool find=false;
			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];

			for(int j=loop_start; j<loop_end; j++)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					find=true;
					break;
				}

			//찾지 못 했을 때 영역을 줄여서 찾아봄
			if(!find)
			{
				buffer[strlen(buffer)-1]=NULL;
				end[i]--;
			}
			else break;
		}

		//읽지 못하는 한글임을 표시
		if(strlen(buffer)<=1)begin[i]=9999;
	}

	//변환
	unsigned int pp2=0;
	for(int i=0; i<strlen(text); i++)
	{
		while(begin[pp2]==9999)pp2++;

		//한글인 부분
		if(pp2<pp && i==begin[pp2])
		{
			char buffer[10];
			int bp=0;
			for(unsigned int j=begin[pp2]; j<=end[pp2]; j++)
			{
				buffer[bp]=text[j];
				bp++;
			}
			buffer[bp]=NULL;

			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];
			for(int j=loop_start; j<loop_end; j++)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					strcat(han, han1[j*2]);
					break;
				}

			i=end[pp2];
			pp2++;
		}
		//아닌 부분
		else
		{			
			//한글 일때는 끊어진 모음 자음으로 표현
			if(text[i]>='a' && text[i]<='z' || text[i]>='A' && text[i]<='Z')
			{
				for(int j=0; j<33; j++)
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

CHan m_han;

/////////////////////////////////////////////////////
//대화 처리 클래스
class CDlg
{
protected:
	//선택지를 위한 변수
	char* select_point[300];
	bool row;
	int n_of_e;
	int x, y, gap, line;
	int col, xmax, ymax, width;
	char dlg_buffer[1024];
	int select;

	//대사 출력을 위한 변수
	int tp, sp;
	bool ani_end;
	int length;
	RECT text, shadow;

	char han_name[161];	

	//대화창 보이기
	void ShowDlg();

public:
	//대화창 모양만 출력
	void ShowDlg(int vx, int vy, int vwidth, int vline);

	//대화 관련 메소드
	void MakeSelection(char* input_dlg, int vx, int vy, int vgap, int vline, int start=0);
	int Selecting();
	int Select(char* input_dlg, int vx, int vy, int vgap, int vline, int vstart=0, bool cancel=false);
	void MakeText(char* content, int vx, int vy, int vwidth, int vline);
	void Print(char* content, int vx, int vy, int vwidth, int vline);
	int Printing();
	char* InputHan(int vx, int vy, int str_max);
};

#define SNR_START 4

//대화 만들기
void CDlg::MakeText(char* content, int vx, int vy, int vwidth, int vline)
{
	x=vx; y=vy; width=Max(3,vwidth); line=vline;

	length=strlen(content);
	strcpy(dlg_buffer,content);
	ani_end=false;
	row=false;

	//대화를 출력하기 알맞게 처리
	sp=SNR_START;	//처리되고 있는 포인트
	int space=-1;	//이전까지 처리한 포인트, space가 있던 곳
	int senlen=0;	//문장의 길이, 한글 포인트
	
	while(sp<length)
	{
		if(dlg_buffer[sp]&0x80)//한글
		{
			senlen+=21;
			sp+=2;
		}
		else
		{
			senlen+=11;

			if(dlg_buffer[sp]==' ')space=sp;
			else if(dlg_buffer[sp]=='\\')
			{
				dlg_buffer[sp]='\n';
				space=-1;
				senlen=0;
			}
			sp++;
		}

		int limit;
		if(dlg_buffer[sp]&0x80)limit=width*10-21;
			else limit=width*10-11;
		if(senlen>limit)
		{
			if(space<0)	//스페이스가 하나도 없을때 강제 줄 변환
			{
				//사이에 CR을 넣어줌
				char* tmp_text;
				tmp_text = new char[length+2];

				for(int i=0; i<sp; i++)tmp_text[i]=dlg_buffer[i];
				tmp_text[sp]='\n';
				for(int i=sp+1; i<length+1; i++)tmp_text[i]=dlg_buffer[i-1];
				tmp_text[length+1]=NULL;
				strcpy(dlg_buffer,tmp_text);
				delete[] tmp_text;

				length++;
				sp++;
			}
			else dlg_buffer[space]='\n';

			space=-1;
			senlen=0;
		}		
	}

	SetRect(&text,x,y,640,480);
	SetRect(&shadow,x+1,y+1,640,480);

	sp=SNR_START;
	n_of_e=0;		//줄바꿈 횟수
	tp=0;			//출력하는 단어
	//대화창을 위한 계산
	xmax=x+width*10;
	ymax=y+line*20;
}

//대화 출력(정지된 상태에서)
void CDlg::Print(char* content, int vx, int vy, int vwidth, int vline)
{
	MakeText(content, vx, vy, vwidth, vline);
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(Printing()==999)break;
		jdd->Render();
		if(!ani_end && !_GetKeyState(VK_RETURN))Sleep(10);
	}
}

//대화출력(실시간으로)
int CDlg::Printing()
{
	static char text_buffer[1024];

	if(!ani_end)
	{
		if(sp<length && n_of_e<line)
		{
			if(dlg_buffer[sp]=='\n')n_of_e++;
			text_buffer[tp]=dlg_buffer[sp];
			if(dlg_buffer[sp]&0x80)	//한글일 경우 2칸 전진
			{
				sp++; tp++;
				text_buffer[tp]=dlg_buffer[sp];
			}
			text_buffer[tp+1]=NULL;
			//포인터 증가
			sp++; tp++;
		}
		else ani_end=true;
	}
	else
	{
		//다음으로
		if(GetKey(4))
		{
			if(sp<length)
			{
				ani_end=false;
				n_of_e=0;
				tp=0;
			}
			else return 999;
		}
	}
	//대화 출력
	Process();
	Screen();
	ShowDlg();
	jdd->DrawText(backbuffer,text_buffer,font20,&shadow,JColor(0,0,0));
	jdd->DrawText(backbuffer,text_buffer,font20,&text,JColor(255,255,255));

	return 0;
}

int CDlg::Select(char* input_dlg, int vx, int vy, int vgap, int vline, int vstart, bool cancel)
{
	MakeSelection(input_dlg,vx,vy,vgap,vline,vstart);
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		int selected=Selecting();
		if(selected>=0 || (cancel && selected==-999))break;

		jdd->Render();
	}

	return select;
}

void CDlg::MakeSelection(char* input_dlg, int vx, int vy, int vgap, int vline, int start)
{
	strcpy(dlg_buffer,input_dlg);
	int length=strlen(dlg_buffer);
	x=vx; y=vy; gap=vgap; line=vline;

	//가로선택지인지 세로 선택지인지 확인 (마이너스는 가로 선택지)
	if(line<0)
	{
		line*=-1;
		row=true;
	}
	else if(line==0)
	{
		line=1;
		row=false;
	}
	else row=false;
		

	//대화를 출력하기 알맞게 처리
	n_of_e=0;
	select=start;
	int bp=1;
	select_point[0]=dlg_buffer;
	int i;
	for(i=0; i<length; i++)
	{
		if(dlg_buffer[i]=='\\')
		{
			dlg_buffer[i]=NULL;
			select_point[bp]=dlg_buffer+i+1;
			n_of_e++;
			bp++;
			continue;
		}
		if(dlg_buffer[i]==13)
		{
			continue;
		}
	}
	//스크립트에서 읽어 온 문장의 끝에 음표가 생기는 것 방지
	if(dlg_buffer[i-1]==13)
	{
		dlg_buffer[i-1]=NULL;
	}

	//대화창 표시에 필요한 것들 계산
	if(row)
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		xmax=x+line*gap;
		ymax=y+col*20;
		width=gap/10*line;
	}
	else
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		xmax=x+col*gap;
		ymax=y+line*20;
		width=gap/10*col;
	}

}

int CDlg::Selecting()
{
	Process();
	Screen();
	//대화상자&선택바
	int sx, sy;
	if(row)
	{
		//위 아래 이동
		if(GetKey(2,10))select=Min(n_of_e,select+1);
			else if(GetKey(1,10))select=Max(0,select-1);
		//좌우 이동
		if(GetKey(3,10))
		{
			if(select+line<=n_of_e)select+=line;
		}
		else if(GetKey(0,10))
		{
			if(select-line>=0)select-=line;
		}

		ShowDlg();
		sx=x+select%line*gap;
		sy=y+(select/line)*20;
	}
	else
	{
		//위 아래 이동
		if(GetKey(3,10))select=Min(n_of_e,select+1);
			else if(GetKey(0,10))select=Max(0,select-1);

		//좌우 이동
		if(GetKey(2,10))
		{
			if(select+line<=n_of_e)select+=line;
		}
		else if(GetKey(1,10))
		{
			if(select-line>=0)select-=line;
		}

		ShowDlg();
		sx=x+select/line*gap;
		sy=y+(select%line)*20;
	}

	RECT src_rect;
	SetRect(&src_rect,0,0,gap/2,20);
	jdd->DrawPicture(backbuffer,PIC_SELECTBAR,sx,sy,&src_rect);
	SetRect(&src_rect,320-gap/2,0,320,20);
	jdd->DrawPicture(backbuffer,PIC_SELECTBAR,sx+gap/2,sy,&src_rect);

	//선택지들
	for(int i=0; i<=n_of_e; i++)
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

		jdd->DrawText(backbuffer,select_point[i],font20,tx+1,ty+1,JColor(0,0,0));
		jdd->DrawText(backbuffer,select_point[i],font20,tx,ty,JColor(255,255,255));
	}

	if(GetKey(4))return select;
		else if(GetKey(5))return -999;
		else return -1;
}

void CDlg::ShowDlg(int vx, int vy, int vwidth, int vline)
{
	x=vx; y=vy; width=Max(3,vwidth); line=vline;
	row=false;
	xmax=x+width*10;
	ymax=y+line*20;

	ShowDlg();
}

void CDlg::ShowDlg()
{
	//잘못된 값 보정(편의상 변수를 전역으로 해서 고치기 난감하므로 이렇게 디버그 장치를 넣어둠)
	width=MaxMin(width, 3, 100);
	col=MaxMin(col, 1, 30);
	line=MaxMin(line, 1, 30);

	//위
	jdd->DrawPicture(backbuffer,"Dlg1",x-5,y-5,NULL);
	for(int i=0; i<width; i++)jdd->DrawPicture(backbuffer,"Dlg2",x+(i*10),y-5,NULL);
	jdd->DrawPicture(backbuffer,"Dlg3",xmax,y-5,NULL);
	//중간
	int ysize=(row)?col:line;

	for(int i=0; i<ysize; i++)
	{
		int tmp_y=y+i*20;
		jdd->DrawPicture(backbuffer,"Dlg4",x-5,tmp_y,NULL);
		for(int j=0; j<width; j++)jdd->DrawPicture(backbuffer,"Dlg5",x+(j*10),tmp_y,NULL);
		jdd->DrawPicture(backbuffer,"Dlg4",xmax,tmp_y,NULL);
	}
	//아래
	jdd->DrawPicture(backbuffer,"Dlg7",x-5,ymax,NULL);
	for(int i=0; i<width; i++)jdd->DrawPicture(backbuffer,"Dlg2",x+(i*10),ymax,NULL);
	jdd->DrawPicture(backbuffer,"Dlg9",xmax,ymax,NULL);
}

//한글입력
char* CDlg::InputHan(int vx, int vy, int str_max)
{
	int old_len=0;
	char eng_buffer[81];
	strcpy(han_name,"");
	strcpy(eng_buffer,"");
	x=vx;
	y=vy;

	while(1)
	{
		Clear();
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//입력된 것 표시
		Process();
		Screen();
		ShowDlg(vx, vy, str_max*11/10, 1);
		jdd->DrawText(backbuffer,han_name,font20,x+1,y+1,JColor(0,0,0));
		jdd->DrawText(backbuffer,han_name,font20,x,y,JColor(255,255,255));

		int eng_len=(int)strlen(eng_buffer);
		//입력
		if(eng_len<80)
		{
			//문자키
			if(GetKey(31))
			{
				strcat(eng_buffer, "a");
			}
			else if(GetKey(32))
			{
				strcat(eng_buffer, "b");
			}
			else if(GetKey(33))
			{
				strcat(eng_buffer, "c");
			}
			else if(GetKey(34))
			{
				strcat(eng_buffer, "d");
			}
			else if(GetKey(35))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "E");
					else strcat(eng_buffer, "e");
			}
			else if(GetKey(36))
			{
				strcat(eng_buffer, "f");
			}
			else if(GetKey(37))
			{
				strcat(eng_buffer, "g");
			}
			else if(GetKey(38))
			{
				strcat(eng_buffer, "h");
			}
			else if(GetKey(39))
			{
				strcat(eng_buffer, "i");
			}
			else if(GetKey(40))
			{
				strcat(eng_buffer, "j");
			}
			else if(GetKey(41))
			{
				strcat(eng_buffer, "k");
			}
			else if(GetKey(42))
			{
				strcat(eng_buffer, "l");
			}
			else if(GetKey(43))
			{
				strcat(eng_buffer, "m");
			}
			else if(GetKey(44))
			{
				strcat(eng_buffer, "n");
			}
			else if(GetKey(45))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "O");
					else strcat(eng_buffer, "o");
			}
			else if(GetKey(46))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "P");
					else strcat(eng_buffer, "p");
			}
			else if(GetKey(47))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "Q");
					else strcat(eng_buffer, "q");
			}
			else if(GetKey(48))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "R");
					else strcat(eng_buffer, "r");
			}
			else if(GetKey(49))
			{
				strcat(eng_buffer, "s");
			}
			else if(GetKey(50))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "T");
					else strcat(eng_buffer, "t");
			}
			else if(GetKey(51))
			{
				strcat(eng_buffer, "u");
			}
			else if(GetKey(52))
			{
				strcat(eng_buffer, "v");
			}
			else if(GetKey(53))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "W");
					else strcat(eng_buffer, "w");
			}
			else if(GetKey(54))
			{
				strcat(eng_buffer, "x");
			}
			else if(GetKey(55))
			{
				strcat(eng_buffer, "y");
			}
			else if(GetKey(56))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "Z");
					else strcat(eng_buffer, "z");
			}
			//스페이스
			else if(GetKey(6))
			{
				strcat(eng_buffer, " ");
			}
			//숫자, 특수 문자
			else if(GetKey(21))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, ")");
					else strcat(eng_buffer, "0");
			}
			else if(GetKey(22))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "!");
					else strcat(eng_buffer, "1");
			}
			else if(GetKey(23))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "@");
					else strcat(eng_buffer, "2");
			}
			else if(GetKey(24))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "#");
					else strcat(eng_buffer, "3");
			}
			else if(GetKey(25))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "$");
					else strcat(eng_buffer, "4");
			}
			else if(GetKey(26))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "%");
					else strcat(eng_buffer, "5");
			}
			else if(GetKey(27))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "^");
					else strcat(eng_buffer, "6");
			}
			else if(GetKey(28))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "&");
					else strcat(eng_buffer, "7");
			}
			else if(GetKey(29))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "*");
					else strcat(eng_buffer, "8");
			}
			else if(GetKey(30))
			{
				if(_GetKeyState(VK_SHIFT))strcat(eng_buffer, "(");
					else strcat(eng_buffer, "9");
			}
		}
		//특수 키
		if(GetKey(4))break;
		else if(GetKey(58, 10))
		{
			if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
		}

		//입력 정보 갱신
		if(old_len!=eng_len)
		{
			strcpy(han_name, m_han.EngToHan(eng_buffer));
			//입력 한계
			while(str_max<(int)strlen(han_name) && eng_len>0)
			{
				eng_buffer[strlen(eng_buffer)-1]=NULL;
				strcpy(han_name, m_han.EngToHan(eng_buffer));
			}
			old_len=eng_len;
		}

		jdd->Render();
	}

	return han_name;
}

/////////////////////////////////////////////////////
//대화 처리 클래스+스크립트 읽기 기능
class CHugeDlg:public CDlg
{
	int snrs;			//시나리오 한계
	char** snr;			//시나리오 포인터
	int mark_no;		//책갈피 숫자
	int bookmark[1000];	//책갈피

	int process_snr;	//처리 중인 시나리오
	int answer;			//처리 중 얻는 결과값

	bool IsBookMark(int no);
	void Function(int command);

public:
	void Script(int script_no);

	CHugeDlg(char* dialog_file);
	~CHugeDlg();
};

#define KEY1 8
#define KEY2 4

//대화 파일 불러오기
CHugeDlg::CHugeDlg(char* dialog_file)
{	
	if(fp=fopen(dialog_file,"rb"))
	{
		//대사 수 읽기
		fread(&snrs,sizeof(int),1,fp);
		//책갈피 읽기
		fread(&mark_no,sizeof(int),1,fp);
		fread(&bookmark,sizeof(int),mark_no,fp);
		//대사내용 읽기
		snr=new char*[snrs];
		for(int snr_no=0; snr_no<snrs; snr_no++)
		{			
			//파일에서 읽기
			int text_size;
			fread(&text_size,sizeof(int),1,fp);
			snr[snr_no]=new char[text_size/2+1];

			char* buffer=new char[text_size+1];
			fread(buffer,text_size,1,fp);
			buffer[text_size]=NULL;

			//해독
			for(int i=0; i<text_size/2; i++)
			{
				char b1, b2;
				b1=buffer[i*2];
				b2=buffer[i*2+1];
				b1=get_char_num(b1)-KEY1;
				b2=get_char_num(b2)-30-KEY2;

				snr[snr_no][i]=b1*16+b2;
			}
			snr[snr_no][text_size/2]=NULL;
			//printf("%d:%s\n",snr_no,snr[snr_no]);
			delete[] buffer;
		}
		
		fclose(fp);
	}
}

//대화내용 비우기
CHugeDlg::~CHugeDlg()
{
	for(int i=0; i<snrs; i++)
		if(snr[i]!=NULL)
		{
			delete[] snr[i];
			snr[i]=NULL;
		}
}

//스크립트 처리
void CHugeDlg::Script(int script_no)
{
	int command;
	char buffer[5];
	script_no=bookmark[script_no];

	do{
		//명령을 읽어 들이기
		strncpy(buffer, snr[script_no], 3);
		buffer[3]=NULL;
		command=atoi(buffer);
		process_snr=script_no;

		if(command==999)break;
			else Function(command);
		script_no++;
	}while(script_no<snrs && !IsBookMark(script_no));
}

bool CHugeDlg::IsBookMark(int no)
{
	for(int i=0; i<mark_no; i++)
		if(no==bookmark[i])return true;

	return false;
}

/////////////////////////////////
//명령 처리 클래스
class CCommand
{
	int com_id[64];
	int command_max;
	char** source;
	char commands[1024];

	int GetComID(int count);
	CDlg comdlg;

public:
	int count;

	void Init(char* vsource[], int max=64);
	void AddComs(int n, ...);
	void AddCom(int id);
	void AddComStr(int id, char* str);
	void MakeCommand(int x, int y, int gap, int vline=0);
	int CommandSelect(int x, int y, int gap, int vline=0);
	int CommandSelecting();
	bool IsFull();
	bool Empty();

	CCommand(char* vsource[], int max=64);
};

CCommand::CCommand(char* vsource[], int max)	//초기화
{
	Init(vsource,max);
}

void CCommand::Init(char* vsource[], int max)
{
	count=0;
	source=vsource;
	command_max=max;
	strcpy(commands,"");
}

void CCommand::AddComs(int n, ...)
{
	va_list ap;
	va_start(ap,n);
	for(int i=0; i<n; i++)AddCom(va_arg(ap,int));
	va_end(ap);
}

void CCommand::AddCom(int id)	//사용할 수 있는 명령 추가
{
	if(count>=command_max)return;	//명령어가 꽉 참
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	com_id[count]=id;
	count++;
}

void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//명령어가 꽉 참
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,str);
	com_id[count]=id;
	count++;
}

int CCommand::GetComID(int count)
{
	if(count>=0 && count<command_max)return com_id[count];
		else return -1;
}

bool CCommand::IsFull()
{
	if(count>=command_max)return true;
		else return false;
}

void CCommand::MakeCommand(int x, int y, int gap, int vline)
{
	if(vline==0)vline=Min(10,count);
	comdlg.MakeSelection(commands,x,y,gap,vline);
}

int CCommand::CommandSelecting()
{
	if(count<1)return -1;		//사용 가능 명령이 없으면 실행하지 않음

	int selected;
	selected=comdlg.Selecting();

	if(selected>=0)return GetComID(selected);
		else return -1;
}

int CCommand::CommandSelect(int x, int y, int gap, int vline)
{
	if(vline==0)vline=Min(10,count);

	comdlg.MakeSelection(commands,x,y,gap,vline);
	int selected;
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		selected=comdlg.Selecting();
		if(selected>=0)break;

		jdd->Render();
	}

	return GetComID(selected);
}

bool CCommand::Empty()
{
	if(count==0)return true;
		else return false;
}

//////////////////////////////////////
//파일 처리 클래스
#define FILEMAX 35

class CFiles
{
	void AddFile(char* name);
public:
	char filename[FILEMAX][40];
	int count;
	void SearchFile(char* dir, char* filename);

	CFiles();
};

CFiles::CFiles()
{
	count=0;
}

void CFiles::AddFile(char* name)
{
	if(count>=FILEMAX)return;
	strcpy(filename[count],name);
	count++;
}

void CFiles::SearchFile(char* dir, char* filetype)
{
	HANDLE hFind;
    WIN32_FIND_DATA FindData;
    int ErrorCode;
    BOOL Continue = TRUE;

	char search_for[30];
	strcpy(search_for,dir);
	strcat(search_for,"\\*.");
	strcat(search_for,filetype);

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
//애니메이션 클래스
#define LOOP_TYPE_ANI		1
#define EXCHANGE_TYPE_ANI	2

class CAnimation
{
private:
	int x_size, y_size;
	bool walk;							//보행 여부(기본값: false)
	int top, bottom, left, right;		//보행 그래픽을 위한 상하좌우 애니메이션 그림의 위치(Y좌표를 증가)
	int frame, frame_max;
	int delay, delay_max;

public:
	char type;
	RECT ani_rect;

	CAnimation();
	CAnimation(int x, int y, char vtype=0, int vframe_max=0, int vdelay_max=0);
	void Set(int x, int y, char vtype=0, int vframe_max=0, int vdelay_max=0);
	void Process();
	void SetWalk(int t, int b, int l, int r);
	void SetFrame(int vframe);
	int GetXSize();
	int GetYSize();
};

//CAnimation 메소드
CAnimation::CAnimation()
{
	frame=delay=0;
	walk=false;
}

CAnimation::CAnimation(int x, int y, char vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	CAnimation();
}

void CAnimation::Set(int x, int y, char vtype, int vframe_max, int vdelay_max)
{
	x_size=x;
	y_size=y;
	SetRect(&ani_rect, 0, 0, x, y);
	type=vtype;
	if(type==2)frame_max=vframe_max*2-1;
		else frame_max=vframe_max;
	delay_max=vdelay_max;
}

void CAnimation::Process()
{
	if(type==1)	//일직선 방향 애니메이션
	{
		delay++;
		if(delay>=delay_max)
		{
			delay=0;
			frame++;
			if(frame>=frame_max)frame=0;
		}
		
		SetRect(&ani_rect, x_size*frame, 0, x_size*(frame+1), y_size);
	}
	else if(type==2)	//반복 애니메이션
	{
		delay++;
		if(delay>=delay_max)
		{
			delay=0;
			frame++;
			if(frame>=frame_max)frame=0;
		}
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, 0, x_size*(frame2+1), y_size);
	}
}

void CAnimation::SetWalk(int t, int b, int l, int r)
{
	top=t;
	bottom=b;
	left=l;
	right=r;
}

void CAnimation::SetFrame(int vframe)
{
	frame=vframe;
	SetRect(&ani_rect, x_size*vframe, 0, x_size*(vframe+1), y_size);
}

int CAnimation::GetXSize()
{
	return x_size;
}

int CAnimation::GetYSize()
{
	return y_size;
}

void DrawPicture(char* src, int x, int y, int frame=0, CAnimation* ani=NULL)
{
	if(ani!=NULL)
	{
		if(frame>=0)ani->SetFrame(frame);
		jdd->DrawPicture(backbuffer, src, x, y, &ani->ani_rect);
	}
	else jdd->DrawPicture(backbuffer, src, x, y, NULL);
}

//////////////////////////////////////////////////////
//큐 클래스
#define QUE_MAX	100

class QueData
{
public:
	unsigned char command;
	int* value;
	char* content;

	void Delete();
};

void QueData::Delete()
{
	command=-1;

	if(content!=NULL)
	{
		delete[] content;
		content=NULL;
	}

	if(value!=NULL)
	{
		delete[] value;
		value=NULL;
	}
}

class Que
{
	QueData *que;

	int que_max;
	bool empty;

public:
	//생성, 소멸, 삽입, 삭제
	void Init();
	void Destroy();
	void Insert(unsigned char command, char* content, unsigned char values, ...);
	bool Delete(int id);

	//값 얻기
	int GetMax();
	QueData* GetQue(int id);
};

void Que::Init()
{
	//큐 공간 확보
	que=new QueData[QUE_MAX];

	//데이터 초기화
	for(int i=0; i<QUE_MAX; i++)
	{
		que[i].command=-1;
		que[i].value=NULL;
		que[i].content=NULL;
	}

	//한계 초기화
	que_max=0;
}

void Que::Destroy()
{
	for(int i=0; i<que_max; i++)
		if(que[i].command>=0)que[i].Delete();

	if(que!=NULL)
	{
		delete [] que;
		que=NULL;
	}
}

void Que::Insert(unsigned char command, char* content, unsigned char values, ...)
{
	int id;
	bool empty=false;

	//빈 큐에 삽입
	for(int i=0; i<que_max; i++)
	{
		if(que[i].command<0)
		{
			id=i;
			empty=true;			
			break;
		}
	}

	//새로운 자리 만들기
	if(!empty && que_max<QUE_MAX-1)
	{
		id=que_max;
		que_max++;
	}

	//명령
	que[id].command=command;

	//문자열
	if(content!=NULL)
	{
		que[id].content=new char[strlen(content)+1];
		strcpy(que[id].content, content);
	}

	//숫자열
	if(values>0)
	{
		que[id].value=new int[values];
		va_list ap;
		va_start(ap,values);
		for(int i=0; i<values; i++)que[id].value[i]=va_arg(ap,int);
		va_end(ap);
	}

}

bool Que::Delete(int id)
{
	que[id].Delete();
	while(id==que_max-1)
	{
		que_max--;
		if(id>0)
		{
			if(que[id-1].command<0)id--;
		}
		else return true;
	}
	return false;
}

int Que::GetMax()
{
	return que_max;
}

QueData* Que::GetQue(int id)
{
	return &que[id];
}