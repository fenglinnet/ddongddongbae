#define WHITE RGB2(255,255,255)
#define BLACK RGB2(0,0,0)
#define RED RGB2(255,0,0)
#define GREEN RGB2(0,255,0)
#define BLUE RGB2(0,0,255)
#define YELLOW RGB2(255,255,0)
#define LOCK if(_LockScreen())_UnlockScreen()
#define PRC MSG msg; if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ){if ( !GetMessage(&msg, NULL, 0, 0) ) return msg.wParam;TranslateMessage( &msg );DispatchMessage( &msg );}else
#define BOX1 SetRect(&BackRect, 100, 60, 600, 160); _DrawBmp(BackRect, 70, 190, BmpScreen[9], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT)
#define SCENE SetRect(&BackRect, 0, 0, 640, 480); _DrawBmp(BackRect, 0, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT)
#define MSND if(!sndd)_Play(movesnd[cara[0].kind])
#define MSND2 if(!sndd)_Play(movesnd[cara[1].kind])
#define SMAX 50
#define ENSHOT for(int i2=5;i2<15;i2++)if(!spr[i2].life)break; if(i2<15)
#define ENSPR for(int i2=15;i2<SMAX;i2++)if(!spr[i2].life)break; if(i2<SMAX)
#define MONSTERS 23
#define ENEMYS 9

int Nstory[]={8,13,17,21,23,26,28,35,38};
int Help[]={99,0,1,99,0,2,2,0};
char *general[]={"과묵","초쨉실","100프로","왕미모","은마녀","의문의 바보"};
char *BattleBack[]={
	"DATA//back6.bmp","DATA//back5.bmp","DATA//back4.bmp",
	"DATA//back3.bmp","DATA//back2.bmp","DATA//back1.bmp",
	"DATA//back1.bmp","DATA//back1.bmp","DATA//back7.bmp"};
char *Carac[]={
	"DATA//Cbang.bmp","DATA//Cbate.bmp","DATA//Cbow.bmp",
	"DATA//Cdark.bmp","DATA//Cknight.bmp","DATA//Cgold.bmp",
	"DATA//Chero.bmp","DATA//Chero2.bmp","DATA//Cmage.bmp",
	"DATA//Cghost.bmp","DATA//Czombie.bmp"};
int BattleMon[]={0,4,4,6,8,8,8,0,
				 4,0,2,4,4,4,10,20,
				 0,2,8,6,6,8,8,0};
int BattleMan[]={0,4,5,5,5,8,10,0,
				 4,0,8,9,8,10,6,7,
				 0,5,4,5,9,5,8,0};
int BattleMal[]={0,0,1,1,2,0,0};
int movesnd[]={4,5,6,7,7,7,4,4,4,7,7},Matt[]={1,2,1,2,2,4,6,8,1,3,3},Mdef[]={1,1,2,2,2,4,6,8,1,4,4};
char base[9][10]={"999999999", "910203049", "900000009",
				 "900000009", "900000009", "900000009",
				 "900000009", "905060709", "999999999"};
char tile[9][10];

char *snr1[]={
	"2","용서할 수 없다! 인간 놈들!","감히 나를 봉인하고 마족을 괴롭히다니!","나같은 평화주의자로 하여금 전쟁을 하게 하는군!",//0
	"4","용사의 침입으로 우리의 영토의 저주가 풀렸습니다.","다시 가서 땅에 저주를 걸어 놔야 합니다.","그렇게 하면 다시 각 영토에서 마물이 번성케 될 겁니다.",
	"5","헤헤헷, 인간놈들 이 기회에 다 죽여 버리자고요!","","",
	"5","(앗, 내가 이런 끔찍한 말을... 에잇, 이놈의 입!!!)","","",
	"4","인간들이 잘못한 것은 사실이나 몰살은 안 돼.","마족이 오랜 정권을 누린 것이 다 평화를 사랑했기 때문이다.","우리의 영토만 회수하면 돼.",
	"2","그래, 그 말이 맞다!","나는 평화주의자, 전쟁은 별로 원하지 않는다.","헤멤의 숲까지 차지하는 것이 우리의 목표다!",//5
	"3","게임진행에 대해 말씀드립니다.","모든 것은 회의로 진행됩니다.","부하들이 의견을 내고 그걸 채택하는 방식으로 진행됩니다.",
	"3","그럼 회의를 시작하겠습니다.","마우스 오른쪽버튼을 누르고 있으면 각 항목의 도움말을 볼 수 있습니다.","",
	"16","이봐, 너흰 마족 녀석들이군.","가만히 마왕성에 쳐박혀 있지, 왜 여길 온거지?","",
	"16","엇, 이리로 계속 오는군.","용사님이 정복하신 영토를 다시 뺏을 참인가?","좋아, 공격개시다!!!",
	"3","전투에 대해 말씀드리겠습니다.","기본은 말을 움직이는 택틱스 방식이고,","서로 근접하면 1:1대전을 하게 됩니다.",//10
	"3","대전에서 쓰는 키는 방향키와 엔터입니다.","좌,우는 이동(두번 연속 누르면 돌격), 아래는 방어입니다. 엔터는 공격입니다.","파마배추와 암흑기사의 경우는 엔터를 오래 누르면 마법을 쓸 수 있습니다.",
	"3","그럼 전 마왕님의 승리를 기원하겠습니다.","","",
	"16","이런, 마족녀석들 미쳤군.","쏘갈요새를 너무 허술히 해 놨어.","",
	"16","하지만 더 이상은 못 간다!","이 영토는 선대 용사님의 희생이다!","",
	"18","우리 백마법사들이 보조해주지!","병사들! 힘을 내서 선대 용사님의 유지를 받들자!","", //15
	"16","와~~~~~!!!","","",
	"16","현실인가? 이건...","동화책에나 나오던 마족들과의 전쟁이...","",
	"16","모두들 평화를 지키자!!!","마족따위는 동화속으로 돌려보내 버리는 거다!!!","",
	"17","우리 흑마법사들이 보조해주지!","병사들! 힘을 내서 선대 용사님의 유지를 받들자!","",
	"16","와~~~~~!!!","","",//20
	"16","이젠 우리 인간에게 여유는 없다!","힘을 전부 모아서 마족을 섬멸하자!","총공격이다!!!",
	"16","와~~~~~!!!","","",
	"19","카베사막의 병사들이... 전멸했단 말인가...","이젠 병사는 별로 안 남았군...","",
	"19","하지만 우리들은 정예병! 긍지를 가지고 싸운다!","어중이 떠중이들과 비교했다간 큰일날걸!!!","",
	"19","자! 우리 정예병의 실력을 몸으로 느껴봐라!","","", //25
	"19","결국 놈들이 성까지 오다니...","모두들 목숨을 걸고 조국을 지킨다!!!","",
	"19","와~~","","",
	"0","전 더 이상 참을 수 없습니다!","나가서 싸우겠습니다.","",
	"1","좋을대로 하게.","하지만 우리의 도움은 바라지말게.","우린 이 성이나마 지키고 싶으니까 말이야...",
	"0","알겠습니다, 지혜로우신 폐하.","이 성에 눌러 앉아 나라를 잘 보존 하시옵소서.","", //30
	"1","자네의 용맹과 패기는 훌륭하네.","용사로써 당연히 갖추어야 할 것이지.","하지만 난 왕, 지혜가 더 중요하네.",
	"1","나에게는 수천의 백성의 목숨이 달려있네.","신중하게 행동하지 않으면 안돼.","하지만 자넬 돕지 않을 생각은 아니네.",
	"1","내가 조건을 걸지.","자네가 쏘갈요새까지 공략한다면 우리 성 사람들도 도와주지.","",
	"0","쏘갈요새라면 마왕성 바로 앞...","저의 승리가 거의 확실하다면 돕겠다는 것이군요.","좋습니다. 마왕성 공략은 어려우니까요.",
	"0","마왕!! 이제 끝이다! 수련의 성과를 보여주마!","너를 없애고 이 세계에 평화를 되찾겠다!","",//35
	"2","이런, 뻔뻔한 놈이 뻔한 대사를 하다니!","그 대사는 지겹게 들었다.","겨우 1개월 수련으로 뭐가 변한단 말이냐!",
	"0","그건 보면 알겠지!","훗!","",
	"4","마왕님 이걸로 우리의 영토를 다 회복했군요.","전쟁을 끝내고 이제 평화를 누릴 수 있겠군요.","",//38
	"2","무슨 소리냐? 여기까지 왔으니 인간을 전멸시킬테다!","난...","",
	"2","난... 처음으로 마왕다운 일을 했어. 그래 이거야. 이게 마왕이야.","마왕이 평화주의자라고? 웃기지마!","자, 세계를 다 지배하는 거다!!!",//40
	"4","마왕님...","","",
	"5","우히히힛! 잘 됐군요. 마왕님, 저도 돕지요.","(에이... 또 싸워야 하나? 인간들과 싸우다 보니 정들었는데...)","",
	"4","알겠습니다. 마왕님의 뜻이라면...","저도 받들죠...","",
	"2","우하하핫! 용사란게 고작 이거냐!","자, 당장 숨통을 끊어 놔라!","",
	"12","잠깐!!!","","",//45
	"2","응?","","",
	"12","쉬리릭","","",
	"2","앗, 저 녀석이 용사를 이런! 젠장!","쉬리릭 3글자로 모든 상황이 전개되다니...","",
	"2","그 녀석은 도대체 누구지?","","",
	"4","이 세계에서 못 보던 녀석인데...","","",//50
	"5","크크큭, 천을 뒤집어 쓰고 있는지 누군지 어째 알어?","(혹시 과묵은 투시도 가능한가? 꺄~ 멋있어!)","",
	"9","헤헤... 난... 안다. 그 아찌...","","",
	"5","바보야... 절루 가거래이~","(어머, 얘가 상처받았으면 어쩌지?)","",
	"3","1개월후...","","",
	"3","인간들은 마왕의 통치속에 노예처럼 지내고 있을 때였습니다.","용사가 의문의 사나이에게서 특훈을 받고 다시 성으로 돌아 왔습니다.","",//55
	"3","그리고 성 사람들에게 용기를 불어넣어주어 마왕과 대항하게 합니다.","그래서 우리는 다시 인간과 싸우게 됐습니다.","",
	"4","너 지금 어디다가 얘기하고 있는거야?","","",
	"2","우하핫! 드디어 용사녀석...","저 하늘의 별이 되었당~","",
	"2","크하하하하핫!","","",
	"2","킥킥킥킥킥!!","","",//60
	"2","푸하하하하핫!","","",
	"2","헥헥... 아이고, 숨차...","","",
	"2","후훗, 마왕다운 웃음이었다.","","",
	"2","이렇게 마왕은 용사를 물리치고 멋진 세계를 만들었답니다.","-끝-","",
	"3","마왕님, 큰일 났습니다.","헤멤의 숲을 뺏겼습니다.","",//65
	"2","뭐야? 도대체 누가?","","",
	"3","용사가 나타나서 공격했다고 합니다.","","",
	"2","뭣이!!! 그럼 우리가 죽인 용사는 뭐란 말이냐?","","",
	"5","으아~ 따분해~ 정말 재미없군. 이 게임!!!","(제작자님, 죄송해요...)","",
	"6","이런 따분한 날에는 재미있는 이벤트를 벌여야지!!!","씨익~","",//70
	"5","무슨 이벤트?","","",
	"4","무투회가 좋겠군.","","",
	"6","이런 삭막한 자식!!!","전쟁의 피로의 지친 우리와 병사들에게 또 전투라니...","에잉, 에잉, 과묵 바봉~",
	"4","(빠직~ 빠직~ 부들 부들...)","그럼... 뭘 하지는 거지... 엉?","",
	"6","미인대회!!!","","",//75
	"4","미, 인, 대, 회?","","",
	"5","너 바보 아니니? 여자가 둘 뿐인데 무슨 미인 대회를 한단 말이야?","","",
	"6","이런, 이런, 언어의 성차별과 언어와 문화의 관계속에서 언어장애를 유발 시켰군.","미인은 말 그대로 하면 아름다운 사람.","미녀도 미남도 아니다.",
	"4","즉... 남녀모두 참여 하자는 거냐?","","",
	"6","그렇지!","빨리 준비하자고!","",//80
	"3","대회 당일...","","",
	"3","자~ 그럼 미인대회를 시작하겠습니다!","심사는 마왕님이 하시겠습니다.","전 사회가 아니냐구요? 저는 대회에 참가합니다.",
	"2","후딱후딱 나와라~ 짜식들아!!!","1번 과묵!","",
	"4","훗... 멋있군... 나란 녀석은...","","",
	"2","다음~","2번 초쨉실!","",//85
	"5","마왕님~ 약속 잊으시면 안 됩니다.","킬킬킬킬킬~","",
	"2","다음~","3번 100프로!","",
	"6","위잉~ 기익~ 지잉~","오늘에야 말로 나의 100프로를 보여주지!","",
	"6","구오오오오~~~ 하아아아아~","","",
	"6","콜록, 콜록, 오늘은 몸이 안 좋아서 이만...","","",//90
	"2","다음~","4번 왕미모!","",
	"7","우훗~","","",
	"6","죽여라!!!","","",
	"5","꺼꾸로 매달아!!!","","",
	"7","뭐 하는거야?","내 미모를 질투하는 건가?","어~ 헝~",//95
	"6","죽여라!!!","","",
	"5","음식쓰레기 통에 버려버려!!!","","",
	"2","다음~","5번 은마녀!","",
	"8","(빙긋~)","","",
	"6","오~","","",//100
	"5","우승은 정해진 듯...","","",
	"4","이래선 남자들도 참가한 의미가 없잖아.","바보같은 짓을 해 버렸군. 쳇.","",
	"5","과연 그럴까?","","",
	"2","입 다물어! 씨끄러운 놈들!","자~ 그럼 우승은...","",
	"2","초쨉실!!!","","",//105
	"5","옹쉐~~~!","","",
	"6","우~ 돈 먹었다!","","",
	"4","끌어내라! 오늘부터 내가 새 마왕이다!","","",
	"6","뭣이 어째?","새 마왕은 바로 이 몸이다!!!","",
	"2","이것들이 보자보자 하니까!!!","전략을 세울때도 내 생각대로 못하고 너희 맘대로 하면서!!!","됐어! 나 마왕안해! 너희들이 다 해먹어!!!",//110
	"6","모두들 절하라!","내가 새 마왕이시다.","",
	"4","어이, 상황파악 좀 하라구...","","",
	"3","이런 젠장, 나도 참가한다고 했는데... 날 까먹다니...","아무튼 미인대회는 엉망이 되었군...","",
	"0","끄응... 여긴?","당신들은... 누구?","왜 날 납치한 거지?",
	"13","우후후후후후...","우리는 주시자들...","오랜 세월 이 세계를 지켜 봐 오고 있었다.",//115
	"0","설마... 당신들이 인간과 마족을 싸우게 한건가?","그 뒤에 너희들이 세계를 지배하려고?","",
	"13","흥, 그런 진부한 스토리가 아니다.","우린 주시자들 그냥 세계를 지켜보기만 한다.","하지만... 이번같은 일을 저지른 이유는...",
	"0","이유는...?","","",
	"13","재미로!!!","","",
	"0","에...?","","",//120
	"13","말했듯이 우린 세계를 지켜보기만 하지.","하지만 수십년간 이 땅은 평화로웠고, 그 평화는 계속 될 것 같았다.","구슬만 보고 있는 우리로는 정말 지겨운 일이지.",
	"13","그래서 우리가 발 벗고 나서서 마족과 인간의 전쟁을 일으킨 거다.","재미있는 전쟁관람을 위해...","",
	"0","그런... 미친!!!","","",
	"13","우후후후... 맘대로 생각해라.","하지만 우린 재미가 목적일 뿐... 지금 세계지배에는 관심이 없다.","자, 용사여. 다시 세계로 돌아가서 평화를 찾아 주어라.",
	"13","하지만 그 전에 우리 의문의 세력의 4장수를 이겨야 보내준다.","왜냐면... 여흥이지. 우후후후후...","",//125
	"0","흥, 좋다. 덤벼라! 모두 해치워 주지.","","",
	"13","좋은 자세다. 그럼 첫 상대는 마왕군에서 스파이 노릇을 했던 의문의 바보!","","",
	"9","헤헤... 하하... 큭큭...","좋아. 상대해주지. 용사여.","큭큭큭...",
	"0","좋아, 다음 상대는 누구지?","","",
	"12","나다. 용사여.","","",//130
	"0","당신은 날 구해준...","아니, 결과적으론 날 납치한...","",
	"12","생명의 은인보다 납치범으로 생각하는게 좋을거다.","그래야 제대로 싸울 수 있지.","",
	"0","그렇군. 훗.","","",
	"13","과연 용사. 강하군!","우후후후후, 무척 즐겁다.","그럼 이번 상대는 좀 색다를 것이다.",
	"0","누구길래...? 헉!!!","","",//135
	"14","오랫만이군. 내 아들...","","",
	"0","아버지... 아버지가 왜?","","",
	"14","큭큭큭, 네 놈이 용사가 된 후에 교만해 져서","이 애비는 잊고 영화를 누렸겠다...","그래서 이 세력과 손잡고 널 혼내주러 나왔다.",
	"0","과연... 아버지다운 짓이군.","내가 아버지라고 봐줄 놈이 아니란 것도 잘 알테지?","",
	"14","상관없어. 나도 그런 자세로 나온 거니까...","칼을 들었으면 목숨을 걸어라...","내가 항상 말하지 않았던가?",//140
	"0","언제?","","",
	"14","......부들부들","빠직 빠직...","큭큭큭...",
	"0","이번 녀석을 해치우면 난 돌아갈 수 있는 거군.","","",
	"13","그래 하지만... 서둘러야 할거다.","","",
	"20","맞아. 왜냐면 지금 인간군은 아주 혼란된 상태거든.","용사가 사라졌으니...","가짜 용사 말이지. 켈켈켈...",//145
	"0","뭐야? 네 놈은? 어디 있는거지?","","",
	"20","어디 있냐고? 그런 것은 의미없는 질문이야.","왜냐면 난 고정된 형태도 모양도 없거든...","그래서 용사로 변신할 수도 있었지.",
	"0","그럼 네 녀석이 가짜용사 노릇을 한 녀석이냐!!!","","",
	"20","켈켈켈... 그렇지.","그런데 내가 여기 와 있다는 것은...?","인간군의 형세가 어떤지 알 수 있겠지?",
	"0","이놈! 널 없애고 바로 인간군을 도우러 가겠다!!!","","",//150
	"0","시간이 없어. 서둘러야 돼!","","",
	"13","우후후후... 열나게 달리는군.","하지만 그렇게 쉽게 가진 못 할 것이다.","왜냐면 네가 쉽게 가버리면 우리가 재미가 없거든...",
	"0","헉헉...","마왕성? 그렇군. 주시자들의 영토는 마왕성 너머에 있었던거군.","좋아. 그럼 이대로 인간군이 있는 곳까지...",
	"15","후후후...","오랜만이군. 아들내미(용사의 본명).","기다리고 있었다.",
	"0","네가 왜 여기에? 혹시 너 인간군을 배신한 거냐?","","",//155
	"15","배신이라니? 난 처음부터 인간군이 아니었어.","난 단지 너의 적이었을 뿐이다.","자, 여기서 결판을 내자!",
	"0","이제 다 왔어. 곧 인간군의 영토이다.","","",
	"4","앗! 뭐냐! 넌! 왜 여기 있는거지?","기습이라도 할 모양였나보군.","하지만 내가 없애주겠다.",
	"6","좋아, 과묵. 나도 엄호해 주지!","100프로로 말이야...","",
	"0","젠장, 마왕군 영토 근처였나...","","",//160
	"16","저것봐! 용사님이시다!","","",
	"17","정말이다!","기다리고 있었습니다.","",
	"18","용사님께 만세~!!!","","",
	"13","드디어 우리의 전설이군.","그래. 우리의 할일이 뭐지?","",
	"9","뭐... 할 것이 있나요?","이전처럼 구슬이나 들여봐야죠.","",//165
	"13","그래, 이 땅이 평화로우면 우리는 할 일이 없다는 건가?","","",
	"9","아니, 우선 이 스토리의 결말을 얘기해 줘야죠.","이 주시자의 전설은 에필로그같은 부분이니까...","",
	"13","꼭 말할 필요가 있나?","혹시 이 다음 스토리가 마왕놀이3로 이어질지...","",
	"9","마왕놀이3는 기획되긴 했지만","스토리는 없는 1의 방식이래요.","1이 인기가 더 좋았거든요.",
	"13","그런가... 하지만 뭐 말할 것도 없지.","뻔한 해피엔딩","",//170
	"9","그래도 말해요...","","",
	"13","용사는 인간군에게 진실을 가르쳐 주어서 서로의 싸움을 그쳤습니다.","마왕은 계속 전쟁을 하고 싶어해서 했는데","초쨉실이 만들어 준 장기로 전쟁의 재미를 대신하고 있습니다.",
	"13","내가 말해도 너무 뻔한 스토리야...","","",
	"9","그럼 우리들은 이만...","","",
	"13","무더운 여름날, 시원한 그늘에서 용사가 낮잠을 잡니다.","전 주시자라서 꿈도 볼 수 있죠.","어떤 꿈을 꾸는지 한번 볼까요?",//175
	"0","으윽... 갑갑해...","여긴 어디지?","",
	"21","여기는 게임들간의 공간이 무너져 버린 세계...","","",
	"0","헉, 너는 용사탄생의 엘프!","왜 여기 있는거지?","",
	"21","네가 날 사랑하는 것을 알고 왔지.","자 마음껏 나를 사랑해봐.","",
	"0","꾸에에엑~ 헉헉...","또 너를 죽일 수 밖에 없는거냐?","",//180
	"21","호호홋, 역시 그런 식으로 내숭을...","하지만 저번과는 다르지. 분신술을 써주지.","웃음소리도 4배~~~",
	"0","그건 지옥이야!!!","","",
	"0","헉헉... 끔찍했다. 빨리 여길 빠져나가야...","","",
	"22","그렇게는 안 된다.","위대한 의지의 뜻에 따라 널 없애주지.","",
	"0","넌 <위대한 의지>의 컬렘...","강자들만 등장하는군.","",//185
	"0","이제 끝났나?","","",
	"23","찾았다! 네 놈이 범인이군.","고문실로 같이 가 주실까?","",
	"0","이번엔 <사립탐정 이동헌>인가...","","",
	"0","헉!!! 꿈이었나?","악몽이었어...","",
	"0","그럼...","","",//190
	"0","다시 자자...","쿨~","",
	"13","한 무더운 여름날의 이야기였습니다.","","",
	};

BOOL LeftButton, RightButton, ReplayFlag, Quit=false, key, key2, first;
extern HSNDOBJ Sound[];
int mode=0, Frame=0, RFrame=0, Count=0, Select=0, number, boss, help, title, combo[5];
RECT BackRect;

typedef struct{
	int lv[3], idea[5], monster[7], lv2[5];
	int mp , goal, turn, score, fear;
}Devil;
Devil devil;

typedef struct{
	bool turn, life;
	int hp, x, y, kind;
}Unit;
Unit monster[7];

typedef struct{
	int x, state, dam, kind;
	int num, hp, mp;
	bool ctr, poison, shield;
}Cara;
Cara cara[2];

typedef struct{
	int hp, weapon, money;
}Hero;
Hero hero;

class CBattle{
	//공용변수
	int direct, x2, y2, win, vic, time;
	//개인변수
	int x, y, sizeX, sizeY;
	int kind, speed, dir, dam, temp, laser;
	int hp, att;
	bool life;

	//기본 데이터
	int dirX[9], dirY[9];
	int mondat[MONSTERS][5];
	int monHP[MONSTERS];
	int monAT[MONSTERS];
	char* name[MONSTERS];
	int enemy[ENEMYS][4];

	void SetData(int num, char* Name, int X, int Y, int SizeX, int SizeY, int Speed, int Hp, int At);
	void PreBattle();
	int Key();
	bool Crush(int x, int y);
	int CenterX(int s1, int s2);
	int CenterY(int s1, int s2);
	bool Bump(int i1);
	void PutSpr();
	void NewSpr(int n, int kind, int x, int y, int hp, int at, int dr);
	void CtrSpr();

public:
	int Battle(int num);
};

CBattle spr[SMAX];
