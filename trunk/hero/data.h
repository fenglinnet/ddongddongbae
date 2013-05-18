#define WHITE JColor(255,255,255)
#define BLACK JColor(0,0,0)
#define RED JColor(255,0,0)
#define GREEN JColor(0,255,0)
#define BLUE JColor(128,128,255)
#define YELLOW JColor(255,255,0)
#define PRC MSG msg; if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ){if ( !GetMessage(&msg, NULL, 0, 0) ) return msg.wParam;TranslateMessage( &msg );DispatchMessage( &msg );}else if(Act)
#define SCENE SetRect(&BackRect, 0, 0, 800, 600); _DrawBmp(BackRect, 0, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT)
#define SMAX 100
#define ENSPR int i2 = 0; for(i2=8;i2<SMAX;i2++)if(!spr[i2].life)break; if(i2<SMAX)
#define MONSTERS 30
#define ENEMYS 30

BOOL LeftButton, RightButton, ReplayFlag, Quit=false, key, key2, Nsave;
extern HSNDOBJ Sound[];
int Frame=0, RFrame=0, Count=0;
int doing[2]={0,0};
RECT BackRect;

char *stage[]={"흙의 나라 <풀풀>","물의 나라 <잘잘>","숲의 나라 <쑥쑥>",
				"잊혀진 나라 <휭휭>","불의 나라 <활활>","철의 나라 <땡땡>",
				"바람의 나라 <솔솔>"};
char *scr[]={"DATA//stage1.bmp","DATA//stage2.bmp","DATA//stage3.bmp",
			"DATA//stage4.bmp","DATA//stage5.bmp","DATA//stage6.bmp",
			"DATA//stage7.bmp"};
char *bgm[]={"music//stage1.mid","music//stage2.mid","music//stage3.mid",
			"music//stage4.mid","music//stage5.mid","music//stage6.mid",
			"music//stage7.mid"};
char *stk[]={"애정의 눈짓","사랑의 매","가혹한 채찍","애정의 철퇴","섬김의 쇠사슬"};
char *swd[]={"목검","동검","철검","장검","영웅의 검"};
char *shoes[]={"구두","운동화","똥똥택스","똥똥택스II","바람의 신발"};

char *snr1[]={
	"0","오고 있다...","오고 있어...","",//0
	"1","응? 아버지에게 편지인가?","음, 음... 호오? 크크큭~ 아하하핫!!!","이거 정말 멋지군! 크하하핫!!!",
	"1","이봐, 아들내미...","너희 할애비에게 편지가 왔더구만...","",
	"2","뭐야? 이 빌어먹을 아버지...","그 영감 아직 살아 있었어?","아무튼 무슨 내용인데?",
	"1","훗... 악의 기운이 성장하고 있다는군...","훗날 일어날 환란을 대비해 널 용사로 키우라고 적혀있군.","어때? 재밌겠지?",
	"2","용... 사... ?","쳇! 그 영감 드디어 망령났군.","산에 갖다 버려놨더니 헛소리를 지껄이는군.",//5
	"1","이 자식, 말버릇이 그게 뭐야?","아버진 예언가이시다. 충분히 믿을만해!","그러니 내가 오늘부터 용사수련을 시켜주지~!",
	"2","뭐? 맨날 그 늙은이 헛소리나 한다고 해놓고는...","그렇군, 수련을 빙자해서 날 괴롭힐 수작이군.","이 빌어먹을 아버지!!!",
	"1","허허헛, 젊다는 것은 좋은 것이군. 음, 음.","좋아, 그럼 오늘부터 용사수련이다! 크하하핫!","우켈켈켈~",
	"2","끼야야야야야악~~~~~~~~~!!!","","",
	"1","아무튼 결론은 널 용사로 키우겠다.","하지만 기존의 용사육성방식은 글러 먹었어.","집근처나 얼쩡대면서 수련이라니...",//10
	"1","그래서 나는 이 대륙의 7개의 나라를 돌면서 견문도 넓히고","수련과 모험을 하면서 진정한 용사로 만들어 주겠다.","",
	"2","제법 괜찮은걸?","이봐, 아버지? 그거 당신 머리에서 나온 생각아니지?","어느 책에서 본거야?",
	"1","크크큭... (나중에 죽었다, 넌.)","우선 가장 가까운 흙의 나라 <풀풀>로 출발이다!","",
	"1","자~ 여기가 흙의 나라 풀풀이다!","음~ 이 맑은 공기!","앗! 저길 봐라! 풀풀의 상징이라 할 수 있는 농부다.",
	"3","으아아아아아~!!!","고무고무 모심기!!!","",//15
	"2","뜨억~ 과연 농경국가로군.","농부부터 틀려.","",
	"1","이 나라가 세계 식량의 3분의 1이나 생산하고 있지.","세계의 식량공급원이라고도 할 수 있다.","",
	"2","헤이~ 헤이~ 아버지.","무식한 거 다 아니까 어디서 보고 아는체 좀 하지 말라고...","",
	"1","크크큭... (진짜로 나중에 죽었다, 넌.)","","",
	"2","헥헥... 아이구, 힘들어...","","",//20
	"1","크헤헤헷~ 더욱 더욱...","더욱 고통스러워 해라~!!!","",
	"2","(벼... 변태다...)","","",
	"1","앗싸리삐야~","돈 주웠다!","",
	"2","그럼 나눠가져야죠~","후훗!","",
	"1","웃기고 있네.","너한테 줄 바엔 바다에 던지겠다!","",//25
	"2","(나중에 잘 때 훔쳐가야지... 크크큭...)","","",
	"1","음... 이것은...","","",
	"2","뭐야? 적인가?","","",
	"1","똥 마려!!!","(후다다닥~)","",
	"2","... ...","응? 헉! 빌어먹을 아버지! 적이다!","",//30
	"1","하필 이런때에... 제길...","","",
	"1","억!!!","","",
	"2","(후다닥~~~)","","",
	"1","설마... 저놈이... 으윽...","","",
	"1","돈 훔쳐갔드아~~~","","",//35
	"1","헛둘, 헛둘...","후후, 이 아버진 아직 지지 않는다.","",
	"1","음... 적들인가...","크크큭...","몬스터들이여... 악몽을 꿀 시간이다!",
	"1","어디 다녀오냐?","","",
	"2","몬스터들을 만났어.","","",
	"1","그래서?","","",//40
	"2","이겼어...","","",
	"1","어, 그래.","","",
	"2","크윽... 죽을 뻔 했잖아! 이 바보 아버지...","쳇... 우윽...","하필 몬스터를 만나다니...",
	"1","몬스터에게도 지다니...","한심하군...","",
	"2","이길 상황에 보내야지!!!","","",//45
	"1","이상하군...","우리는 그냥 나들이 하는 중인데...","",
	"2","정말 그래요...","왜 우리를 피하는 눈치일까요?","",
	"2","우후훗, 옷호~","","",
	"1","이것이 맛이 갔나?","왜 그래?","",
	"2","큭큭큭...","처음 왔을때부터 신경 쓰였는데, 근처의 괴물모양 동굴이 있죠.","마을사람들 이야기로는 거기 보물이 있다는군요.",//50
	"1","호오? 그으래~ 당장 가자!!!","","",
	"1","여기인가? 과연... 저것이 보물상자인가?","후후후...","",
	"12","크어어어... 누구냐? 이 보물에 손대는 자는...","죽음으로 죄를 사죄하라!!!","",
	"1","이 자식아! 이게 뭐야?","보물상자는 텅 비어 있고...","쓸데없이 고생만 했잖아!!!",
	"2","괜찮아유~","이런 일들이 조각조각 우리 마음에 아름다운 보석이 될 거예유.","",//55
	"1","뒈져랏!!!","","",
	"2","끼야야야야야악~~~~~~~~~!!!","","",
	"3","이봐!!! 너희들 이리로 와서 농사 좀 거들어!!!","","",
	"1","네가 뭔데 나한테 명령이야? 어엉?","","",
	"3","난 농부왕이 될 사나이다!!!","","",//60
	"1","......","도와주지,농부왕...","",
	"1","푸하핫! 이걸 봐라! 대회다. 잘잘의 명물, 보트대회.","1등에서 3등까지 상이 있다는군.","",
	"2","좋았으! 아버지!","아버지가 노를 열심히 젓고, 난 방향을 잡지!","",
	"1","아, 그래 그래. 노를 이리 다오.","","",
	"2","이상하군."," 보통때 같으면 '이 늙은이에게 힘든일을 시키다니!!!'할텐데...","아무튼, 자요, 노. 응?",//65
	"2","끼야야야야야악~~~~~~~~~!!!","","",
	"99","이렇게 되어 아들내미가 노를 젓고 당신이 조종을 하게 되었습니다.","방법은 간단합니다. 소용돌이를 잘 피하면 됩니다.","그리고 보트의 속도는 소용돌이에 안 걸리는 한 계속 빨라집니다.",
	"99","자, 그럼 시작합니다.","","",
	"2","음냐음냐...","억!!!","",
	"1","일어나! 이 자식아!","이제 한 주가 지났다구. 다음 나라인 물의 나라 <잘잘>로 가자!","",//70
	"2","쳇, 움직이기 싫은데... 물의 나라라...","후후후... 혹시 인어공주와 사랑에 빠질지도...","쿡쿡쿡",
	"1","(저것이 맛이 갔나???)","됐다! 도착이다! 금방왔군.","",
	"1","음... 이제 입상자를 발표하는군...","","",
	"1","네가 듣고 나한테 알려줘. 가슴이 떨려서...","","",
	"2","킥킥... 생긴것 답지 않게...","좋아, 보고와 주지.","",//75
	"2","이얏호~! 1등이야! 1등이라구!","상으로 영웅의 검을 받았어!","",
	"1","아닐 껄...","분명 상이 하나 더 있을텐데...","내가 안내문도 안 보는 사람인 줄 알았나 보지?",
	"2","크윽... 치잇! 자! 여기, 섬김의 쇠사슬!","","",
	"1","당연하지. 내가 강해지는 것은 너한테 공포일테니까...","큭큭큭... 좋았어. 돌아갈까?","",
	"2","이얏호~! 2등이야!","그런데 상은 없다는군.","참가하는데 의의가 있대.",//80
	"1","웃기지 마! 상품없는 대회가 어딨냐?","빨리 주시지...","내가 안내문도 안 보는 사람인 줄 알았나 보지?",
	"2","크윽... 치잇! 자! 여기, 섬김의 쇠사슬!","","",
	"1","당연하지. 내가 강해지는 것은 너한테 공포일테니까...","큭큭큭... 좋았어. 돌아갈까?","",
	"2","3등이래...","상금으로 10K를 받았어.","",
	"1","20K도 같이 줘야 되지않겠냐?","빨리 주시지...","내가 안내문도 안 보는 사람인 줄 알았나 보지?",//85
	"2","크윽... 치잇! 자! 여기, 30K!","","",
	"1","치사하다 욕하지마라.","돈은 대갈에게로! 대갈은 돈으로!","큭큭큭... 좋았어. 돌아갈까?",
	"2","에라이! 조종도 못하는 것이 키는 왜 잡아?","쳇! 가자!","",
	"1","네가 노를 엉터리로 저어서 그래!","불꺼!","잠이나 자자!",
	"2","아아...","","",//90
	"1","뭐 하는거야? 맛이 갔냐?","","",
	"2","후우~ 얼마 전 근처 바다에서 인어공주를 보았어.","희미한 실루엣이었지만...","정말 아름다웠어...",
	"1","그으래? 그럼 다시 보러 가자구. 용기를 내서...","올 때부터 인어공주 타령이더만...","",
	"2","그래, 용기를 내서 만나러 가겠어!!!","","",
	"2","아! 저기야, 저기!","저 아름다운 실루엣. 아름다워...","다가갈 수 없어...",//95
	"1","웃기고 있네~","좋아. 그럼 내가 가보지.","",
	"1","응?","허억...","",
	"2","왜 그래? 어버지?","커어어어억!!!","",
	"2","포세이도오오오온!!!!!!","","",
	"99","그날 아들내미는 쇼크로 기절해 버렸다.","","",//100
	"2","헤헷... 헤헤헤...","아버지 혼자 가...","난 이제 이 나라를 돌아 다니고 싶지않아. 무서워...",
	"1","그럼 나 혼자 놀고 오지 뭐...","크하하핫~!!!","",
	"1","자, 출발이다! 다음의 목적지는 숲의 나라 <쑥쑥>이다!","응? 누구야? 저건...","",
	"5","히죽히죽... 크크크...","네 놈의 아들이 용사가 될 녀석이란 말이군...","후후, 언젠가 다시 보겠지. 이만...",
	"1","뭐야? 저건...","네가 아는 녀석이냐?","",//105
	"2","아뇨. 처음보는 녀석인데...","미쳤나 보죠.","",
	"99","아무튼 그들은 숲의 나라 <쑥쑥>으로 향했다.","하지만...","",
	"1","으하하핫! 길을 잃었어!","","",
	"2","킥킥킥! 계속 똑같은 길이야!","","",
	"1","우린 이제 죽었어!","하하하핫!","",//110
	"2","평생 여기서 헤멜거야!","킥킥킥...","",
	"6","길을 헤메는 것이요?","당신들...","",
	"1","앗! 사람이다!","길을 알고 있소? 좀 가르쳐 주시오.","",
	"6","여기 길은 간단해요.","당신들은 단지 저 이상한 나무에게 홀린 거요.","",
	"1","나무라고...?","그러고 보니... 저 나무들, 몬스터다!","제길. 우릴 헤메게 하다니 모두 죽여주지!!!",//115
	"1","고맙소. 덕분에 길을 찾게 되었군요.","성함이?","",
	"6","난 나불나불이라고 하오.","언젠가 또 만날 날이 있겠지요.","그럼 이만...",
	"2","용사의 일기","하루에 이상한 마법사 둘을 만났다...","하지만 그 만남이 나의 훗날을 바꿀 줄은 그때 나는 모르고 있었다.",
	"1","이봐, 헛소리 하지 말고 빨리 가자!","해가 지겠다.","",
	"1","이 자식아! 들어봐!","마을사람들에게서 들었는데, 이 숲에 신비의 종족인 엘프가 산다는군.","굉장히 아름답다고도 하고...",//120
	"2","후후후, 그거 좋지.","미인이라. 용사가 미인을 얻는 법.","갑시다!!!",
	"1","......","","",
	"2","......","","",
	"7","오홋홋홋. 과연...","저의 아름다움이 그렇게 소문이 나다니...","음, 두분도 꽤 멋진데 그래요?",
	"7","우훗~","","", //125
	"1","해치우자!","","",
	"2","OK!","","",
	"1","헉헉, 해치웠다.","끔찍한 몬스터 녀석.","",
	"2","이런 몬스터가 아름답다니...","이 마을 녀석들은 미친게 분명해요.","",
	"2","앗, 숲의 성자 아저씨!","","",//130
	"4","음... 부자끼리의 산책인가?","그것도 좋지.","",
	"1","누구야? 이 놈은...","","",
	"2","옛날에 만난 적이 있잖아요.","좋은 것도 가르쳐 주고.","",
	"4","그렇다면 오늘은 또 인생에 대해서 얘기해 줄까?","","",
	"1","드디어 이곳도 떠날 때군.","3주라... 꽤 시간이 많이 흘렀군.","이번 가는 나라가 중간이 되는 거군.",//135
	"2","이번엔 어디로 가는데?","","",
	"1","잊혀진 나라 <휭휭>이다.","고대의 도시이지. 지금은 황폐했지만...","",
	"1","휭휭을 아들내미와 누비니","산천은 의구한데 인걸은 간 데 없다.","어즈버 태평연월 꿈이런가 하노라...",
	"2","카악~ 투엣~","재수 없군.","",
	"1","히죽~","","",//140
	"2","끼야야야야야악~~~~~~~~~!!!","","",
	"1","고대유적에 왔으니 사진이나 찍고 갈까?","","",
	"2","그거 좋죠.","전 괜찮으니 아버지 한 장 찍으시죠.","",
	"1","크윽~ 너 이녀석~ 그런 착한 말을...","과연 내 아들...","좋아 찍어라!",
	"99","훗날 이 사진은 아들내미의 방의 다트판에 붙여졌다.","","",//145
	"2","우후훗, 옷호~","소문을 듣자 하니 여기 고대의 비보가 있다는 군요.","역시 고대의 나라.",
	"1","왠지 불길한데...","보물을 찾으러 가며 언제나 허탕을 하던데...","",
	"2","한번 가보자구!","운 좋으면 고대유물이라도...","",
	"1","흐음... 여기군.","응? 땅이 울리는 데...","... 역시 ... 이런 패턴인가?",
	"13","누가 고대의 비보를 노리는가?","크어어어...","",//150
	"1","결국엔 이렇게 허무하게 끝나는 거군.","쳇~","",
	"2","앗! 아냐!","저길 좀 봐!","금이다!",
	"1","앗! 정말... 하지만...","작군... 쳇. 한 10K쯤 되겠어.","뭐... 이걸로 만족해야 겠지?",
	"1","멍~","할일없군...","",
	"2","하암~ 따분해.","아, 근데 언젠가 한번 물어볼께 있었는데 말이야!","",//155
	"1","뭐가?","","",
	"2","아버진 정의감이 언제나 0인데, 상태창에 정의감이 있는 거지?","","",
	"1","쳇, 정의따윈 네놈이나 지켜.","난 준데도 안 받아!","그리고 상태창은 제작자가 귀찮아서 일괄처리 한 것 뿐이야...",
	"2","그런가?","","",
	"1","후~ 세월 빠르군.","벌써 5번째 나라로 갈 시간이다.","",//160
	"2","그런가, 헤이, 아버지.","이번에 갈 나라는 또 어떤 곳이야?","",
	"1","이번 마을은 불의 나라 <활활>이다.","","",
	"2","우왓! 멋지다! 마구 불타오른다!","","",
	"1","정말 멋진데...","활활에는 집을 불로 짓는다는 얘기는 들었지만...","이건 꼭 대형화재같군.",
	"5","여기요, 이 사람들이오.","후후후, 내 말대로이지 않소?","",//165
	"14","과연 그렇군.","화재를 보고서는 즐기고 있다니!","당신들이 방화범이 틀림없군.",
	"2","허억~ 진짜 화재였어?","","",
	"5","자, 빨리 이들을 체포하시오.","큭큭큭","",
	"6","어이, 히죽히죽. 또 이런 일이나 벌이고 있냐?","","",
	"5","큭, 나불나불. 네놈이 어떻게 여기에?","","",//170
	"6","불을 지른 것은 보나마나 네 놈이겠지.","뭐, 상관없어.","이 악당은 내가 맡을테니 당신들이 불을 꺼 주시오.",
	"1","OK!","","",
	"5","큭, 두고보자! 이 녀석들!!!","","",
	"14","이거 죄송합니다.","우리가 오해를 해서...","",
	"1","죄송하다면 다야? 어엉?","","",//175
	"2","똑바로 살아! 이 사람아!","어디 평범한 시민을 방화범으로 몰아!","",
	"14","(뿌드득...)","네, 네, 정말 죄송합니다.","",
	"1","이 사람 영~ 말귀를 못 알아 듣는군.","죄송하다면 다냐구?","",
	"14","그럼...?","앗, 안돼! 그 돈은...","",
	"99","돈 10K를 뜯었다.","","",//180
	"1","앗! 저 녀석은!!!","","",
	"5","잘 만났다. 이 녀석들...","그땐 내 계획을 수포로 만들었겠다.","이번엔 확실히 없애주지!",
	"2","도대체 왜 우리한테 이러는 거야?","무슨 원수를 졌다고?","",
	"5","이유없이 그냥 재수없어! 네 놈들은...","원래 사람이 화내는 것은 논리적인 사고 과정을 거치기 보다는","감정적으로 일어나는 거야! 이유따윈 필요없어!",
	"2","유식하군...","","",//185
	"5","큭... 강하군...","두고보자! 마왕놀이2에서 끝장을 내 주겠어!","",
	"2","마왕놀이2가 뭐야?","","",
	"5","2002년 4,5월쯤에 완성 될 것이다.","많은 성원 부탁... 컥...","다음에 보자!",
	"5","쉬리릭~~~","","",
	"2","뭐야? 저 녀석...","어쩐지 훗날 다시 만나게 될 것 같은데...","",//190
	"1","아까 마왕놀이2에서 다시 만날 거라고 했잖아.","후후후, 나도 출연해야지.","",
	"1","헤이, 가자가자!","","",
	"2","뭐야? 어디를?","","",
	"1","마을 사람들에게 들었는데, 이곳에는 불이 많이 난다는군.","화재 한건 해결하면 5K를 준대.","경험치도 올리니까 좋지.",
	"2","목숨수당 5K면 절대 많은게 아닌데...","","",//195
	"1", "헤이, 가자가자!", "", "",
	"2", "뭐야? 어디를?", "", "",
	"1", "전쟁에서 용병으로 뛰면 10K 준대.", "", "",
	"2", "올 때는 분명히 전쟁에 관여 안 한다고 했잖아!", "돈이 그렇게 좋은가!", "",
	"1", "훗, 돈 때문이 아니야...", "정의를 지키기 위해서다!", "",	//200
	"2", "정의감 0인 주제에 그딴 소리를...", "", "",
	"1", "드디어 떠나는군.", "올때부터 사건이더니 덥기만 덥고...", "다신 오고 싶지 않은 나라야.",
	"2", "또 그 히죽히죽 같은 녀석...", "다신 만나고 싶지 않아!", "",
	"1", "기운내고 다 잊어버리자.", "다음 나라는 평화의 나라일꺼다.", "쇠의 나라 <땡땡>으로 출발이다!",
	"2", "평화의 나라...?", "", "",	//205
	"1", "하하하...", "", "",
	"2", "이런 전쟁 중인 나라는 그냥 지나쳐 가는 것이 낫지 않을까?", "", "",
	"1", "이놈아! 교육의 목적을 잊었냐?", "이런 저런 나라를 돌아다니며", "갖가지 체험을 하는 것이었잖아.",
	"2", "하지만 전쟁에 휩쓸려 죽기라도 하면!", "교육의 의미가 없잖아!", "",
	"1", "괜찮아. 관여하지만 않으면 돼.", "", "",	//210
	"2", "이얏호! 마지막 나라다!", "1주만 지나면 이 지겨운 수련도 끝나는군!", "",
	"1", "훗, 그런데 내 생각엔 넌 전혀 용사다워지지 못했어.", "아마 대악당이 될지도...", "현상금 붙으면 내가 잡아주지.",
	"2", "그게 아버지가 할 소리인가?", "악담도 그런 악담을...", "",
	"2", "우와와~ 대륙이 날고 있다.", "굉장해.", "활활에서 처럼 사실은 폭발되어 날아가는 중은 아니겠지.",
	"1", "재수없는 소리하지마.", "", "",	//215
	"1", "이번 주로 훈련은 끝이군...", "", "",
	"2", "그립군. 모험의 나날이여...", "", "",
	"1", "역시...", "", "",
	"2", "이런 것은 우리에게 안 어울려...", "", "",
	"1", "케케케켓! 빨리 가자!", "망할 아들 놈!", "훈련에 훈련이다!",	//220
	"2", "좋았어! 빌어먹을 아버지!", "", "",
	"2", "앗, 숲의 성자 아저씨!", "이런 곳에는 왜?", "",
	"4", "이벤트 떼우기랄까...?", "앗, 아니다. 쓸데없는 얘기말고...", "오늘은 정의에 대하여 얘기해 주지.",
	"1", "끝났군. 드디어.", "자, 이제 네 갈길을 가라.", "난 집에 가서 쉴란다.",
	"2", "헤이, 아버지, 어디가시나?", "아직 안 끝났어.", "우리 둘의 결판이 안 났잖아.",	//225
	"1", "뭐야?", "나한테 덤비겠다는 거냐?", "어리석은...",
	"2", "보통때의 나로 보면 안 되지~!", "난 지금 분노로 타오른다.", "이때까지 원한을 갚아주마!!!",
	"2", "쳇, 아버지... 역시 강하군.", "우억... 빨리 죽여라...", "",
	"1", "내가 얼마나 힘들게 훈련시켰는데 널 죽이냐?", "빨리 사라져.", "",
	"1", "난 네가 나같이 살지 말고 보다 강하게 살기 바래서 그렇게 키운거야.", "하지만 결과적으로 이렇게 삐둘어 지고 말았구나. 불쌍한...", "",	//230
	"2", "아버진... 충분히 강해...", "", "",
	"1", "그 강함과는 다른거야.", "단어는 동음이의어의 원인이나 문맥에 따라 뜻이 달라지지.", "게다가 사람의 말로는 정확한 생각을 전달하지 못 해.",
	"1", "그건 상호대화 중에 나누는 것은 생각이 아니고 메세지이기 때문이지.", "대화는 경험을 공유하는 것이라 할 수 있는데,", "서로 대상에 대한 같은 경험을 가지고 있지 않으면 오해를 낳게 되지.",
	"2", "아버지...", "", "",
	"2", "그거 어느 책에 나오는 거야?", "", "",	//235
	"1", "빠직~ 빠직~", "", "",
	"1", "이 자식이 봐줄라 했더니만!!!", "38연타 사랑의 펀치!!!", "",
	"2", "끄아아아악~~~", "", "",
	"99", "그렇게 아들내미와의 7주간의 훈련은 끝났다.", "", "",
	"99", "그리고 어느덧 5년이 지났다.", "", "",	//240
	"1", "세월 참 빠르구먼... 벌써 5년이...", "뭐라고? 아들내미 이야기가 궁금하다고?", "",
	"1", "귀찮아. 말 안할래...", "-끝-", "",
	"1", "뭐... 뭐야?", "알았어. 알았다고. 얘기해 주면 될거 아냐!", "",
	"1", "그러니까...", "", "",
	"1", "놈은 마왕을 쓰러뜨렸고...", "그 후에 자신이 새로운 마왕으로 등극하였지...", "",	//245
	"1", "네가 녀석의 목에 현상금이 걸리면 없애겠다고 했는데...", "후후후, 이건 예상외의 최고의 현상금인걸!", "",
	"1", "아무튼 난 놈을 처치하기 위해서 간다.", "우리 가문의 명예도 있으니까...", "그럼 이만...",
	"1", "녀석은 용감하게 싸웠지.", "그리고 마왕군을 제법 많이 격퇴했어.", "",
	"1", "그래서 용사로 인정받았지.", "", "",
	"1", "하지만... 놈은 그렇게 잘 나가는데...", "놈은 나를 외면하고...", "아무도 용사를 키운 날 인정해 주지 않아!!!",	//250
	"1", "두고보자! 이 불효막심한 놈!", "불효의 죄를 삼강오륜으로 묻겠다!", "크하하핫!",
	"99", "참고로 말하자면 이 엔딩이 마왕놀이2로 이어진다.", "", "",
	"1", "크하하하하핫!!!", "그럼 이만!", "",
	"1", "놈은... 죽었어...", "마왕을 무찌르러 갔지만...", "힘이 없는 정의였지...",
	"1", "뭐 별 할말이 없군.", "지금 놈의 무덤에 가는 중인데...", "그럼 이만...",	//255
	"1", "놈은 실력이 후져서 병사로 마왕을 무찌르러 갔지.", "하지만 정의감없고 불만많던 녀석은 마왕편에 붙었어.", "",
	"1", "덕분에 난 아들을 잘못 키웠다고 쫓기는 신세야...", "", "",
	"1", "쳇, 여기까지 쫓아 왔나?", "급해서 이만 실례!!!", "",
	"1", "음, 뭐 돈대는 대로 이것저것 사볼까?", "", "",
	"1", "크하핫! 장비는 다 갖췄으니...", "술집에나 가자!", "",	//260
	"16", "안녕하십까? 여러분.", "오늘은 저희 술집의 술먹기 대회가 있습니다.", "상품으로는 바람의 신발 한켤레!",
	"1", "좋았어!", "참가하자!", "",
	"16", "방법은 간단합니다.", "술 8병을 10초 이내에 모두 마시면 됩니다.", "단 한병을 마실 때마다 방향키가 바뀌니 주의하십시오!",
	"16", "그럼 시작합니다!!!", "", "",
	"16", "축하합니다. 여기 상인 <바람의 신발>입니다!!!", "", "",	//265
	"16", "안 됐군요. 실패입니다.", "다음기회를 이용해 주십시오.", "",
	"2", "크윽, 엘프때문에 버린 눈을 어디가서 정화해야 겠는데.", "", "",
	"8", "찾았다.", "", "",
	"2", "앗! 나도 찾았어요!", "당신 같은 엄청난 미인을!", "",
	"8", "헛소리 그만하고, 너 용사 되는 거 그만 둬.", "", "",	//270
	"1", "뭣이 어쩌고 저째?", "이 녀석은 용사가 될 거다!", "이 녀석은 나의 소중한...",
	"2", "아버지...", "", "",
	"1", "노후 대책이니까.", "", "",
	"2", "빌어먹을...", "", "",
	"8", "하긴 쉽지 않을 거라고 생각했어.", "", "",	//275
	"8", "할 수 없지.", "용사가 될 수 없는 불구의 몸으로 만들어 줄게.", "",
	"2", "이봐, 도대체 왜 이러는 거야!", "", "",
	"8", "쳇! 오늘은 이만 물러가 주지!", "", "",
	"2", "휴, 살았다.", "", "",
	"1", "쟤 누구야?", "너한테 원한 있는 거 같은데?", "",	//280
	"2", "몰라! 처음 보는 얼굴이야!", "", "",
	"1", "그래도 엘프보다 낫다.", "", "",
	"2", "100배 낫지.", "", "",
	"2", "앗! 너는!", "", "",
	"8", "내가 용사 훈련은 그만 두라고 했지?", "왜 그렇게 말을 안 듣는 거야?", "",	//285
	"2", "시끄러!!", "네가 내 애인이라도 돼!", "무슨 상관이야!",
	"8", "...", "", "",
	"8", "물론 애인은 아니지만...", "", "",
	"8", "어쨌든 그만 둬!!", "", "",
	"2", "우아아아아!!", "", "",	//290
	"8", "큭, 이번에도 물러나야 겠군.", "", "",
	"99", "툭", "", "",
	"2", "응? 뭔가 떨어뜨리고 갔는데?", "반지?", "",
	"1", "돈 되겠다!", "팔아치우자!", "",
	"2", "아무튼 돈이라면 환장해서...", "잠시만 있어봐.", "응? 이건?",	//295
	"1", "왜 그래?", "", "",
	"2", "안 쪽에 '아들내미가 벨라에게'라고 적혀있어.", "", "",
	"1", "뭐야? 네가 선물한 거야?", "아는 여자였는데 왜 능청 떨었어?", "",
	"2", "아니, 진짜 모른다니까.", "누구지?", "",
	"2", "앗! 숲의 성자 아저씨!", "", "",	//300
	"4", "무슨 고민이라도 있어?", "얼굴이 안 좋은데...", "",
	"2", "이 반지 때문에 그래요.", "제가 어떤 여자에게 선물한 건데", "아무리 생각해도 누구였는지 기억이 나질 않아요.",
	"4", "어디 보자.", "음, 이거 흥미롭군.", "",
	"2", "네? 뭐가요?", "", "",
	"4", "여기 자세히 보거라.", "숫자가 적혀있지?", "아마도 제작된 년도인거 같다.",	//305
	"2", "어? 진짜네.", "이건???", "년도가 5년 뒤로 되어 있는데요?",
	"4", "그래, 즉...", "", "",
	"2", "유통기한?", "", "",
	"4", "...", "", "",
	"4", "그게 아니라 이건 미래에서 온 물건이라는 거다.", "아마 이걸 가지고 있었던 사람도 미래인이 아니었을까?", "",	//310
	"2", "미래인?", "", "",
	"4", "자세한 건 본인에게 물어보는 게 어떠냐?", "", "",
	"2", "예?", "", "",
	"9", "안녕?", "", "",
	"2", "으아아악! 언제!", "또 싸울테냐?", "",	//315
	"9", "아니, 힘으론 널 이길 수 없다는 걸 깨달았어.", "그래서 마지막으로 대화를 해보려고.", "",
	"2", "대화라면...", "좋아!", "왜 내가 용사가 되는 걸 방해하는 거지?",
	"9", "넌 용사가 되니까.", "", "",
	"2", "???", "무슨 소리야?", "",
	"9", "넌 용사가 돼.", "마왕을 무찌르고 부귀영화를 손에 넣지.", "",	//320
	"2", "...", "넌 누구지?", "",
	"10", "미래의 네 아내야.", "", "",
	"2", "에에에엑!", "아내??", "하긴 없으리란 법은 없지.",
	"2", "그런데 왜 아내가 방해하는 거야!", "부귀영화도 누린다잖아!", "",
	"9", "그 부귀영화로 넌 인간 쓰레기가 되버려.", "가정을 돌보지 않고 매일 술과 도박에 빠지게 된다구.", "",	//325
	"2", "도박과 술!", "", "",
	"2", "크으~ 좋지!", "", "",
	"9", "쓸데없는 소리하지 말고 용사 되는 건 포기해.", "넌 다른 일을 해도 성공할 수 있을 거야.", "",
	"2", "이 멍청아!", "", "",
	"9", "!!!!!", "", "",	//330
	"2", "나 한 명 잘 되자고 그런 길을 택할 수는 있어!", "하지만 용사가 없어지면 인간들의 미래는 어떻게 되지?", "아무 죄 없이 마왕군에게 고통 당할 양민들은 어떻게 하냐고!",
	"9", "그.. 그건...", "", "",
	"2", "벨라, 날 믿어줘.", "사람은 약하기 때문에 쓰러지기도 하고 넘어지기도 해.", "하지만 분명 다시 일어설 거야.",
	"2", "아마 내가 당신과 만났을 때도 그랬을 거야.", "분명 나라면 그랬을 거야.", "",
	"10", "내미...", "", "",	//335
	"10", "알았어. 난 미래에 가서 기다릴게.", "그때는 꼭 잘해 줘야 해.", "",
	"2", "응, 약속할게.", "", "",
	"10", "그... 그럼...", "난 이만.", "",
	"2", "...", "", "",
	"2", "크으~ 내가 미래에 저런 죽이는 여자랑 결혼하다니!!", "크윽! 좋다! 좋아!", "",	//340
	"2", "크크크~ 죄없는 양민들이라고?", "크하하하핫! 그딴 거 알게 뭐야!", "저런 미인과 부와 명예가 기다리고 있는데! 난 용사가 된다!",
	"2", "용사야 말로 나의 운명!", "하늘의 뜻이다!", "",
	"1", "훗, 좋은 이야기로군.", "", "",
	"2", "엉? 아버지, 보고 있었어?", "", "",
	"1", "후후후...", "아들아, 너 말고 용사될 사람은 많단다.", "",	//345
	"2", "?", "", "",
	"1", "바로 나!", "", "",
	"1", "헉헉! 나... 나도 용사할 거야!", "용사가 되어서 저 여자를 내가.", "헉헉헉!",
	"2", "이봐! 정신차려!", "", "",
	"2", "안 갔어?", "", "",	//350
	"10", "조금만 쳐다보고.", "", "",
	"1", "녀석은 용사가 되었어.", "부와 명예를 손에 넣었지.", "그리고 아내 벨라도.",
	"1", "하지만 역시 인간성이 쓰레기다 보니 타락했어.", "결국 술과 도박에 빠졌고.", "",
	"1", "지금은 이혼 조정 중이야.", "", "",
	"1", "역시 미래는 바꿀 수 없나봐.", "", "",	//355
	"1", "그 놈은 병사될 실력도 안 되서 결국 백수가 되었어.", "집에 얹혀 살고 있지.", "",
	"2", "아버지, 전 아버지가 좋아요.", "아버지에게 평생 효도하고 살 게요.", "",
	"1", "그만해. 미친 놈아.", "", "",
	"2", "꺄르르륵... 깔깔깔", "", "",
	"1", "아아악!", "내가 집을 나갈 거야!", "",	//360
	"2", "", "", "",
	"2", "", "", "",
	"2", "", "", "",
	"2", "", "", "",
	};

typedef struct{
	unsigned char day, out;
	unsigned char vella_flag;
	int money;
}GAME;
GAME gm;

typedef struct{
	unsigned char exp;
	char hp, pow, just, att, shoes;
}MAN;
MAN mn[2];

class CBattle{
	//공용변수
	int direct, x2, y2, win, vic, temp;
	//개인변수
	int x, y, sizeX, sizeY;
	int kind, speed, dir, dam;
	int hp, att;
	int master;	//소환체인 경우
	bool life;

	//기본 데이터
	int dirX[9], dirY[9];
	int mondat[MONSTERS][5];
	int monHP[MONSTERS];
	int monAT[MONSTERS];
	char* name[MONSTERS];
	int enemy[ENEMYS][6];

	void SetData(int num, char* Name, int X, int Y, int SizeX, int SizeY, int Speed, int Hp, int At);
	void PreBattle();
	int Key();
	bool Crush(int x, int y);
	int CenterX(int s1, int s2);
	int CenterY(int s1, int s2);
	bool Bump(int i1);
	void PutSpr(int num);
	void NewSpr(int n, int kind, int x, int y, int hp, int at, int dr, int master = -1);
	void CtrSpr();

public:
	int Battle(int num, bool party);
	int Boat();
	int Drink();
};
CBattle spr[SMAX];
