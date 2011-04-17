#include "srpg.h"
#include "donglib.h"

//�� ����
#define UNIT_Y		40
#define UNIT_PY		8	//������ ���ΰ� 40�̹Ƿ� 8�ȼ� ���� ����

//�׸� �̸�
#define PIC_MAP		"Map"
#define PIC_TILE1	"tile1"
#define PIC_TILE2	"tile2"

//�̺�Ʈ ����
#define EVENT_NONE		-1
#define EVENT_GO_OUT	999

//�ִ� ��ȣ
#define HERO			0
#define ANI_TILE		128

//�ִϸ��̼�
CAnimation ani;

//�̵� ���ʽ�
#define NORMAL_MOVE		0	//����
#define WATER_MOVE		1	//��
#define FIRE_MOVE		2	//��
#define SPECIAL_MOVE	3	//���ݴ�
#define SKY_MOVE		4	//����
#define GHOST_MOVE		5	//����

//����
char* battle_commands[] = {"����", "����", "�޽�", "���"};
CCommand command(battle_commands);


////////////////////////////////////////////////////////////
// CUnit �޼ҵ�

//���� �ɷ�ġ ������
int unit_data[][20] = {
//   HP		��	��ø�̵���	�̵����ʽ�	
	{30,	0,	12,	12,		GHOST_MOVE},	//�ް���
	{99,	0,	8,	12,		GHOST_MOVE},	//��ù�
	{30,	0,	9,	12,		FIRE_MOVE},		//���̾�
	{20,	0,	10,	12,		WATER_MOVE},	//��ũ��
	{20,	0,	15,	12,		SKY_MOVE},		//�̹���
	{30,	5,	5,	10,		NORMAL_MOVE},	//������
	{99,	3,	12,	12,		NORMAL_MOVE},	//�޷��
	{10,	1,	10,	12,		NORMAL_MOVE},	//����
	{10,	1,	10,	12,		NORMAL_MOVE},	//����
	{10,	0,	8,	12,		NORMAL_MOVE},	//������
	{15,	3,	10,	12,		SPECIAL_MOVE},	//���ݴ�
	{12,	2,	15,	14,		SPECIAL_MOVE}	//����
};

CUnit::CUnit()
{
	life=false;
	moving=false;
	SetPos(0,0);
}

//����
void CUnit::Set(int id)
{
	life = true;
	this->id = id;
	pic_id = jdd->GetHash(StrAdd("Unit%d", id));

	if(id<6)team = 0;
	else team = 1;

	//�ɷ�ġ �ʱⰪ ����
	hp_max = unit_data[id][0];
	power = unit_data[id][1];
	quickness = unit_data[id][2];
	move = unit_data[id][3];
	move_bonus = unit_data[id][4];

	//���� �ʱⰪ
	ap = 100;
	hp = hp_max;
}

//���
void CUnit::Die()
{
	life = false;
}

//���� ���� ���
bool CUnit::GetLife()
{
	return life;
}

//ID ���
int CUnit::GetID()
{
	return id;
}

int CUnit::GetPictureID()
{
	return pic_id;
}

//��ǥ ���
int CUnit::GetX()
{
	return x;
}

int CUnit::GetY()
{
	return y;
}

int CUnit::GetPX()
{
	return px;
}

int CUnit::GetPY()
{
	return py;
}

int CUnit::GetRealX()
{
	return x*TILESIZE+px;
}

int CUnit::GetRealY()
{
	return y*TILESIZE+py;
}

//���� ���, ����
int CUnit::GetDir()
{
	return dir;
}

void CUnit::SetDir(int dir)
{
	this->dir = dir;
}

//�ٶ󺸰� �ִ� �� ���ϱ�
int CUnit::GetDirX()
{
	if(dir==DIR_LEFT)return x-1;
		else if(dir==DIR_RIGHT)return x+1;
		else return x;
}

int CUnit::GetDirY()
{
	if(dir==DIR_UP)return y-1;
		else if(dir==DIR_DOWN)return y+1;
		else return y;
}

//�̺�Ʈ ��ȣ ����/���ϱ�
void CUnit::SetEventNo(int event_no)
{
	this->event_no = event_no;
}

int CUnit::GetEventNo()
{
	return event_no;
}

//��ġ ����
void CUnit::SetPos(int x, int y)
{
	this->x = x;
	this->y = y;
	px = py = 0;
	moving = false;
}

//�̵� �ӵ� ����
void CUnit::SetMoveSpeed(int move_speed)
{
	this->move_speed = move_speed;
}

//�̵�
bool CUnit::Move(int dir)
{
	if(moving)return false;
	this->dir = dir;

	if(dir == DIR_LEFT)
	{
		--x;
		px=TILESIZE;
	}
	else if(dir == DIR_UP)
	{
		--y;
		py=TILESIZE;
	}
	else if(dir == DIR_RIGHT)
	{
		++x;
		px=-TILESIZE;
	}
	else if(dir == DIR_DOWN)
	{
		++y;
		py=-TILESIZE;
	}

	moving = true;
	return true;
}

void CUnit::Action()
{
	if(!moving)return;

	//�·� �̵�
	if(px>0)
	{
		px -= move_speed;
	}
	//��� �̵�
	else if(px<0)
	{
		px += move_speed;
	}
	//���� �̵�
	else if(py>0)
	{
		py -= move_speed;
	}
	//�Ʒ��� �̵�
	else if(py<0)
	{
		py += move_speed;
	}
	//�̵��� ������ Ű �Է� ����
	else moving = false;
}

////////////////////////////////////////////////////////////
// CSRPG �޼ҵ�

CSRPG::CSRPG()
{
	//�ִϸ��̼� �ʱ�ȭ
	ani.CreateAnimation(ANI_TILE, 48, 48);

	//Ÿ��, ������Ʈ, ����
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, "User\\tile1.gif", &jpi, true);
	jdd->LoadPicture(PIC_TILE2, "User\\tile2.gif", &jpi, true);	//2�� Ÿ��
	CreateUnit();

	//���ɿ� â
	command.comdlg.SetDlg(0, 0, 100, 4);
	command.comdlg.MakeDlgBox("command_dlg");
	command.comdlg.SetOpacity(0.7f);
}

//�ε�
bool CSRPG::Load(char* map_name, bool battle)
{
	this->battle = battle;
	strcpy(this->map_name, map_name);

	FILE *fp;
	char map_file[80];
	sprintf(map_file, "%s.map",map_name);
	if(!(fp=fopen(map_file, "rb")))return false;

	char data_type[12];
	fread(&data_type,sizeof(char),11,fp);
	data_type[11]=NULL;

	//�ް������ �ƴ�
	if(strcmp(data_type,"HYUGARU2MAP")!=0)
	{
		fclose(fp);
		return false;
	}

	//�� ũ��
	fread(&x_size, sizeof(char), 1, fp);
	fread(&y_size, sizeof(char), 1, fp);

	//�� ������
	for(int k=0; k<3; ++k)
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	{
		if(k==0)fread(&map[i][j].ground, sizeof(char), 1, fp);
			else if(k==1)fread(&map[i][j].object, sizeof(char), 1, fp);
			else if(k==2)fread(&map[i][j].unit, sizeof(char), 1, fp);
	}

	//�̺�Ʈ �� ������
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
		fread(&map[i][j].evnt, sizeof(char), 1, fp);

	//���ΰ� ������ �ʱ�ȭ
	unit[HERO].Set(HERO);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(40, 120, 0, 80);

	//���� ������ �ʱ�ȭ
	unit_max=1;

	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[i][j].unit!=0xff)
	{
		//������ ������ ��ҿ� ���� ��ġ
		unit[unit_max].Set(map[i][j].unit);
		unit[unit_max].SetPos(i, j);
		unit[unit_max].SetDir(/*rand()%4*/DIR_DOWN);
		
		//���� ������ �Ѱ��� �� ������ ID�� ��ü
		map[i][j].unit = unit_max;
		unit[unit_max].SetEventNo(map[i][j].evnt);
		map[i][j].evnt = 0xff;

		//�ִϸ��̼� �ʱ�ȭ
		ani.CreateAnimation(unit_max, 32, 40, ani_exchange, 3, 20);
		ani.GetAni(unit_max)->SetWalk(40, 120, 0, 80);
		ani.GetAni(unit_max)->SetDirection((WalkDirection)unit[unit_max].GetDir());

		++unit_max;
	}

	//�� �̸� �׸���
	DrawGround();
	
	fclose(fp);

	//�� �Ӽ� �ҷ�����
	bool read_success = false;
	if(fp=fopen("MAP\\map_attr.sav", "rb"))
	{
		//�ĺ���
		char head[9];
		fread(head, sizeof(char), 8, fp);
		head[8] = NULL;
		if(strcmp(head, "MAPATTR0")==0)
		{
			//Ÿ�� �Ӽ���
			for(int i=0; i<2; ++i)
			{
				fread(tile_mov[i], sizeof(int), TILE_MAX, fp);
				fread(tile_attr[i], sizeof(int), TILE_MAX, fp);
				fread(tile_damage[i], sizeof(int), TILE_MAX, fp);
			}
			//���� �Ӽ���
			fread(unitdata, sizeof(CUnitData), TILE_MAX, fp);

			read_success = true;
		}

		//�������� �̵������� ��ȯ
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
		{
			if(tile_mov[j][i]==0)tile_mov[j][i]=99;
			else tile_mov[j][i]=tile_mov[j][i];
		}

		fclose(fp);
	}

	//�б� �����ϸ� �ʱ�ȭ
	if(!read_success)
	{
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
			tile_mov[j][i]=tile_attr[j][i]=tile_damage[j][i]=99;
	}

	//�̵��� �� ����
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	{
		if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
		else map[i][j].move = tile_mov[0][map[i][j].ground];
		
		//�̵��� 0�̸� �̵� �Ұ� ����
		if(map[i][j].move == 99)map[i][j].move = MOVE_DISABLE;
	}

	//��Ÿ �ʱ�ȭ
	active_unit = -1;

	//���ΰ� ��ġ �ʱ�ȭ
	unit[HERO].SetPos(0, 0);
	InitScroll(HERO);
	event_no=-1;

	return true;
}

CSRPG::~CSRPG()
{
}

//���� ����
void CSRPG::CreateUnit()
{
	//���Ͽ��� �б�
	int file_no = 0;

	JPictureInfo pi;
	pi.SetColorKey(JColor(0,0,255));

	FILE *fp;
	while(1)
	{
		char src_file[80];
		strcpy(src_file, StrAdd("User\\Unit%d.png", file_no));

		if(fp = fopen(src_file, "rb"))
		{
			fclose(fp);

			//�ش� ���� ���Ͽ��� ���� ��� �ϳ��� ������ ����
			jdd->LoadPicture(StrAdd("Unit%d", file_no), src_file, &pi, true);

			++file_no;
		}
		else break;
	}
}

//���� �̸� �׸��� �Լ�
void CSRPG::DrawGround()
{
	//������ �����
	jdd->DeleteSurface(PIC_MAP);

	//���ο� �� ����
	JPictureInfo mpi;
	mpi.SetWidth(x_size * TILESIZE);
	mpi.SetHeight(y_size * TILESIZE);
	jdd->CreateSurface(PIC_MAP, &mpi, true);

	//Ÿ��
	RECT tile_rect;
	for(int k=0; k<18; ++k)
	{
		SetRect(&tile_rect, k*48, 0, (k+1)*48, 48);

		for(int j=0; j<y_size; ++j)
		for(int i=0; i<x_size; ++i)
			if(map[i][j].ground==k)ani.GetAni(ANI_TILE)->Draw(PIC_MAP, PIC_TILE1, i*TILESIZE-8, j*TILESIZE-8, k);
	}

	//������Ʈ
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[i][j].object!=0xff)
	{
		SetRect(&tile_rect, map[i][j].object*TILESIZE, 8, (map[i][j].object+1)*TILESIZE, UNIT_Y);
		jdd->DrawPicture(PIC_MAP, PIC_TILE2, i*TILESIZE, j*TILESIZE, &tile_rect);
	}
}

//�̵� ���ɼ�
int CSRPG::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	//������
	if(battle)
	{		
		if(map[x][y].distance == MOVE_DISABLE || map[x][y].move == MOVE_DISABLE)return 0;	//�̵� �Ұ�
		else if(map[x][y].object!=0xff)return Max(TILESIZE/(2<<map[x][y].move), 1);			//2�� Ÿ�Ͽ� ���� �̵�
		else return Max(TILESIZE/(2<<map[x][y].move), 1);									//�Ϲ� 1�� Ÿ�� �̵�
	}
	//����
	else
	{
		if(map[x][y].unit!=0xff)return 0;									//������ ������ �̵� �Ұ���
		else if(map[x][y].object!=0xff)return TILESIZE/(2<<map[x][y].move);	//2�� Ÿ�Ͽ� ���� �̵�
		else if(tile_attr[0][map[x][y].ground]>0)return 0;					//���ÿ� �Ϲ� �� �ܿ��� �̵� �Ұ���
		else return TILESIZE/(2<<map[x][y].move);							//�Ϲ� 1�� Ÿ�� �̵�
	}

}

//�浹
int CSRPG::Crush(int x, int y)
{
	//ȭ�� ������ ����
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_GO_OUT;

	//���� �̺�Ʈ
	if(map[x][y].unit!=0xff)//return GetUnit(map[x][y].unit)->GetEventNo();
	{
		CUnit* unit;
		unit = GetUnit(map[x][y].unit);
		int ev_no = unit->GetEventNo();
		return ev_no;
	}

	//���� �̺�Ʈ
	if(map[x][y].evnt!=0xff)return map[x][y].evnt;

	return EVENT_NONE;
}

//��ȭ
int CSRPG::Talk(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_NONE;

	//���� �̺�Ʈ
	if(map[x][y].unit!=0xff)return GetUnit(map[x][y].unit)->GetEventNo();

	//���� �̺�Ʈ
	if(map[x][y].evnt!=0xff)return map[x][y].evnt;

	return EVENT_NONE;
}

//���� �̵�
void CSRPG::UnitMove(int id,  int dir)
{
	//���� ��ġ ���
	int old_x = unit[id].GetX();
	int old_y = unit[id].GetY();

	//�̵����̸� ó�� ���� �ʰ� ����
	if(!unit[id].Move(dir))return;

	//���ο� ��ġ ���
	int new_x = unit[id].GetX();
	int new_y = unit[id].GetY();

	//�浹 �̺�Ʈ
	if(id==HERO)
	{
		event_no = Crush(new_x, new_y);
		if(event_no == 0)event_no = -1;	//0�� �̺�Ʈ�� ����(Ư�� �̺�Ʈ)
	}

	//�̵� �ӵ� ���ϱ�, �̵� �� �� ������ ���� ��ġ��
	int move_speed = GetMoveSpeed(new_x, new_y);
	unit[id].SetMoveSpeed(move_speed);
	
	//�̵� �Ұ��̸� ����ġ
	if(move_speed <= 0)
	{
		unit[id].SetPos(old_x, old_y);
	}
	//������ġ�� �Բ� �ʿ� ǥ�õǴ� ���� ��ġ�� ����
	else if(id != active_unit)
	{
		map[old_x][old_y].unit = 0xff;
		map[unit[id].GetX()][unit[id].GetY()].unit = id;
	}
}

//��ũ�� �ʱ�ȭ
void CSRPG::InitScroll(int unit_id)
{
	next_scroll_x=scroll_x=MaxMin(unit[unit_id].GetRealX()-320, 0, x_size*TILESIZE-640);
	next_scroll_y=scroll_y=MaxMin(unit[unit_id].GetRealY()-240, 0, y_size*TILESIZE-480);
}

//��Ŀ��
void CSRPG::Focus(int unit_id)
{
	//X��
	int ux = MaxMin(unit[unit_id].GetRealX()-320, 0, x_size*TILESIZE-640);
	if(scroll_x < ux)
	{
		scroll_x = Min(scroll_x+4, ux);
	}
	else if(scroll_x > ux)
	{
		scroll_x = Max(scroll_x-4, ux);
	}

	//Y��
	int uy = MaxMin(unit[unit_id].GetRealY()-240, 0, y_size*TILESIZE-480);
	if(scroll_y < uy)
	{
		scroll_y = Min(scroll_y+4, uy);
	}
	else if(scroll_y > uy)
	{
		scroll_y = Max(scroll_y-4, uy);
	}
}

//����
void CSRPG::Control()
{
	if(battle)BattleControl();
		else NormalControl();
}

//���� ����
void CSRPG::NormalControl()
{
	//�̵�
	if(GetKey(vkey_up,0))
	{
		UnitMove(HERO, DIR_UP);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_down,0))
	{
		UnitMove(HERO, DIR_DOWN);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_left,0))
	{
		UnitMove(HERO, DIR_LEFT);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_right,0))
	{
		UnitMove(HERO, DIR_RIGHT);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}

	//NPC �̵�
	for(int i=0; i<unit_max; ++i)
	{
		if(i != HERO)
		{
			static int delay;
			++delay;
			if(delay>100)
			{
				int dir = rand()%4;
				UnitMove(i, dir);
				ani.GetAni(i)->SetDirection((WalkDirection)dir);
				delay=0;
			}			
		}
	}
}

//������ ����
void CSRPG::BattleControl()
{
	if(active_unit >=0)
	{
		//������ �� �ִ� �Ʊ�
		if(mode==0)
		{
			if(unit[active_unit].team==0)
			{
				//�̵�
				if(GetKey(vkey_up,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_UP);
						else unit[active_unit].SetDir(DIR_UP);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_down,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_DOWN);
						else unit[active_unit].SetDir(DIR_DOWN);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_left,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_LEFT);
						else unit[active_unit].SetDir(DIR_LEFT);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_right,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_RIGHT);
						else unit[active_unit].SetDir(DIR_RIGHT);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_enter))
				{
					//����â ����
					int px = unit[active_unit].GetRealX()+TILESIZE-scroll_x;
					int py = unit[active_unit].GetRealY()+TILESIZE-scroll_y;
					if(px+100 > SCREEN_X)px-=100+TILESIZE;
					if(py+80 > SCREEN_Y)py-=100+TILESIZE;
					command.comdlg.SetDlg(px, py, 100, 4);
					command.Init(battle_commands);
					command.AddCom(1);
					command.AddCom(2);
					command.AddCom(3);

					mode = 1;
				}
			}
		}
		//���� ����
		else if(mode==1)
		{
			if(result != -1)mode=2;
		}
		else if(mode==2)
		{
			//�ٸ� ���ֿ��� ���ʸ� �ѱ�
			map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = active_unit;
			unit[active_unit].ap = 0;
			active_unit = -1;
		}

		return;
	}

	//�̵��ϰ� �ִ� ������ ���� �� ���� �̵��� ���� ã��
	bool moveable = false;
	for(int i=0; i<unit_max; ++i)
	if(unit[i].ap >= 100 && unit[i].GetLife())
	{
		if(unit[i].team>0)continue;
		SetActiveUnit(i);
		SetStartPoint(unit[i].GetX(), unit[i].GetY(), unit[i].move);
		moveable = true;
		mode = 0;
		break;
	}

	//AP ȸ��
	if(!moveable)
	{
		for(int i=0; i<unit_max; ++i)unit[i].ap += unit[i].quickness;
	}
}

void CSRPG::Process()
{
	//���ֵ� �ൿ
	for(int i=0; i<unit_max; ++i)unit[i].Action();

	//��Ŀ��
	if(battle)
	{
		if(active_unit>=0)Focus(active_unit);
	}
	else InitScroll(HERO);

	ani.Process();
}

void CSRPG::Draw()
{
	//��� ���
	RECT screen_rect;
	SetRect(&screen_rect, scroll_x, scroll_y, scroll_x+640, scroll_y+480);
	jdd->DrawPicture(backbuffer, PIC_MAP, 0, 0, &screen_rect);

	//���� ����
	int start_x = scroll_x/32;
	int start_y = scroll_y/32;
	int end_x = Min(start_x+20, x_size-1);
	int end_y = Min(start_y+15, y_size-1);

	//2�� ���̾�
	for(int j=start_y; j<=end_y; ++j)
	for(int i=start_x; i<=end_x; ++i)
	{
		int px = i*TILESIZE-scroll_x;
		int py = j*TILESIZE-scroll_y;

		//2��
		if(map[i][j].object!=0xff)
		{
			RECT tile_rect;
			SetRect(&tile_rect, map[i][j].object*TILESIZE, 0, (map[i][j].object+1)*TILESIZE, 8);
			jdd->DrawPicture(backbuffer, PIC_TILE2, px, py-UNIT_PY, &tile_rect);
		}

		//�Ϲ� ����
		if(map[i][j].unit!=0xff)
		{
			int p = map[i][j].unit;
			if(unit[p].move_bonus < SKY_MOVE)
			{
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
	}

	//�̵� ���� ����
	if(battle)
	{
		static int delay;
		++delay;
		if(delay>=64)delay=0;

		float strlength = float(abs(32-delay))/100;
		JPictureInfo pi;
		pi.SetOpacity(strlength);
		jdd->SetPictureInfo("BLUE", &pi);

		for(int j=start_y; j<=end_y; ++j)
		for(int i=start_x; i<=end_x; ++i)
		{
			int px = i*TILESIZE-scroll_x;
			int py = j*TILESIZE-scroll_y;

			//������ �̵� ���� ����
			if(map[i][j].distance != MOVE_DISABLE)
			{
				RECT tile_rect;
				SetRect(&tile_rect, px, py, px+TILESIZE, py+TILESIZE);
				jdd->DrawPicture(backbuffer, "BLUE", px, py, NULL);
			}
		}
	}

	//3�� ���̾�
	for(int j=start_y; j<=end_y; ++j)
	for(int i=start_x; i<=end_x; ++i)
	{
		int px = i*TILESIZE-scroll_x;
		int py = j*TILESIZE-scroll_y;

		//���� ����
		if(map[i][j].unit!=0xff)
		{
			int p = map[i][j].unit;
			if(unit[p].move_bonus >= SKY_MOVE)
			{
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
		//Ȱ��ȭ�� ����
		if(active_unit>=0 && i == unit[active_unit].GetX() && j == unit[active_unit].GetY())
		{
			int p = active_unit;
			ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
		}
	}

	//���� ����
	if(battle && mode==1)result = command.CommandSelecting();
}

void CSRPG::SetHeroXY(int x, int y)
{
	x = MaxMin(x, 0, x_size-1);
	y = MaxMin(y, 0, y_size-1);
	unit[HERO].SetPos(x, y);	
	map[x][y].unit = HERO;
}

void CSRPG::SetAutoHeroXY(int ev)
{
	//�ϴ� ���ΰ��� �� �ۿ� ��
	unit[HERO].SetPos(-1, 0);

	//�̺�Ʈ �� ������
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	for(int i=0; i<x_size && !quit; ++i)
	if(map[i][j].evnt == ev)	//�ش� �̺�Ʈ ��ġ�� ���ΰ� ��ġ
	{
		unit[HERO].SetPos(i, j);
		map[i][j].unit = HERO;
		quit=true;
	}

	if(unit[HERO].GetX() == -1)unit[HERO].SetPos(0, 0);
}

CUnit* CSRPG::GetUnit(int id)
{
	return &unit[id];
}

//�ش� ��ġ ���� ã��
int CSRPG::FindUnit(int x, int y)
{
	for(int i=0; i<unit_max; ++i)
		if(unit[i].GetX()==x && unit[i].GetY()==y)return i;

	return -1;
}

//���� ����
void CSRPG::SaveGame(FILE *save_fp)
{
	fwrite(map_name, sizeof(int), 80, save_fp);
	fwrite(&x_size, sizeof(char), 1, save_fp);
	fwrite(&y_size, sizeof(char), 1, save_fp);
	fwrite(map, sizeof(TileData), x_size*y_size, save_fp);
	fwrite(&scroll_x, sizeof(int), 1, save_fp);
	fwrite(&scroll_y, sizeof(int), 1, save_fp);
	fwrite(&next_scroll_x, sizeof(int), 1, save_fp);
	fwrite(&next_scroll_y, sizeof(int), 1, save_fp);
	fwrite(&unit_max, sizeof(int), 1, save_fp);
	fwrite(&unit, sizeof(CUnit), UNIT_MAX, save_fp);
}

//���� �ҷ�����
void CSRPG::LoadGame(FILE *load_fp)
{
	//�� �ε�
	fread(map_name, sizeof(int), 80, load_fp);
	Load(map_name, battle);

	//���� �б�
	fread(&x_size, sizeof(char), 1, load_fp);
	fread(&y_size, sizeof(char), 1, load_fp);
	fread(map, sizeof(TileData), x_size*y_size, load_fp);
	fread(&scroll_x, sizeof(int), 1, load_fp);
	fread(&scroll_y, sizeof(int), 1, load_fp);
	fread(&next_scroll_x, sizeof(int), 1, load_fp);
	fread(&next_scroll_y, sizeof(int), 1, load_fp);
	fread(&unit_max, sizeof(int), 1, load_fp);
	fread(&unit, sizeof(CUnit), UNIT_MAX, load_fp);
}

//�̺�Ʈ �ʱ�ȭ
void CSRPG::InitEvent()
{
	event_no = -1;
}

//�̺�Ʈ ��ȣ ���
int CSRPG::GetEventNo()
{
	return event_no;
}

void CSRPG::SetStartPoint(int x, int y, int move_max)
{
	point.x=MaxMin(x, 0, x_size-1);
	point.y=MaxMin(y, 0, y_size-1);

	//�� �ʱ�ȭ
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			map[i][j].road=MOVE_DISABLE;

	//�� �ʱ�ȭ(�ӽ�)
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			map[i][j].distance=MOVE_DISABLE;

	//�� ã��
	FindRoad(point.x, point.y, 0, move_max);
}

void CSRPG::SetEndPoint(int x, int y)
{
	//�� �ʱ�ȭ
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			map[i][j].distance=MOVE_DISABLE;

	//�̵���� �˻�
	FindRoad(x, y, 0);
}

void CSRPG::FindRoad(int x, int y, int expense, int move_max)
{
	if(move_max>=0 && expense>move_max)return;	//�̵��� �Ѱ��� ���� �� �̻� �˻����� ����
	if(map[x][y].unit != 0xff)return;			//������ ������ �̵� �Ұ���
	map[x][y].distance = expense;

	if(x>0 && map[x-1][y].distance>expense+map[x-1][y].move)
		FindRoad(x-1, y, expense+map[x-1][y].move, move_max);	//���� �˻�
	if(x<x_size-1 && map[x+1][y].distance>expense+map[x+1][y].move)
		FindRoad(x+1, y, expense+map[x+1][y].move, move_max);	//������ �˻�
	if(y>0 && map[x][y-1].distance>expense+map[x][y-1].move)
		FindRoad(x, y-1, expense+map[x][y-1].move, move_max);	//�� �˻�
	if(y<y_size-1 && map[x][y+1].distance>expense+map[x][y+1].move)
		FindRoad(x, y+1, expense+map[x][y+1].move, move_max);	//�Ʒ� �˻�
}

bool CSRPG::MoveNextPointFast(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=map[point.x][point.y].distance;

	//���� �˻�
	if(point.x>0 && map[point.x-1][point.y].distance<best_expense && map[point.x-1][point.y].move<=move)
	{
		best_dir=0;
		best_expense=map[point.x-1][point.y].distance;
	}
	//������ �˻�
	if(point.x<x_size-1 && map[point.x+1][point.y].distance<best_expense && map[point.x+1][point.y].move<=move)
	{
		best_dir=1;
		best_expense=map[point.x+1][point.y].distance;
	}
	//�� �˻�
	if(point.y>0 && map[point.x][point.y-1].distance<best_expense && map[point.x][point.y-1].move<=move)
	{
		best_dir=2;
		best_expense=map[point.x][point.y-1].distance;
	}
	//�Ʒ� �˻�
	if(point.y<y_size-1 && map[point.x][point.y+1].distance<best_expense && map[point.x][point.y+1].move<=move)
	{
		best_dir=3;
		best_expense=map[point.x][point.y+1].distance;
	}

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� true, �ƴϸ� false
	if(map[point.x][point.y].distance==0)return true;
		else return false;
}

bool CSRPG::MoveNextPoint(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=map[point.x][point.y].distance;

	//ù°�� �˻��ϴ� ���� �����ϰ� ���ϱ� ���� �÷���
	//(�켱������ ������ ������ ������ �ܼ��ؼ� ���⿡ �� ����.)
	bool left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=false;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//���� �˻�
		if(ran==0)
		{
			if(point.x>0 && map[point.x-1][point.y].distance<best_expense && map[point.x-1][point.y].move<=move)
			{
				best_dir=0;
				best_expense=map[point.x-1][point.y].distance;
			}
			left_ok=true;
		}
		//������ �˻�
		else if(ran==1)
		{
			if(point.x<x_size-1 && map[point.x+1][point.y].distance<best_expense && map[point.x+1][point.y].move<=move)
			{
				best_dir=1;
				best_expense=map[point.x+1][point.y].distance;
			}
			right_ok=true;
		}
		//�� �˻�
		else if(ran==2)
		{
			if(point.y>0 && map[point.x][point.y-1].distance<best_expense && map[point.x][point.y-1].move<=move)
			{
				best_dir=2;
				best_expense=map[point.x][point.y-1].distance;
			}
			up_ok=true;
		}
		//�Ʒ� �˻�
		else if(ran==3)
		{
			if(point.y<y_size-1 && map[point.x][point.y+1].distance<best_expense && map[point.x][point.y+1].move<=move)
			{
				best_dir=3;
				best_expense=map[point.x][point.y+1].distance;
			}
			down_ok=true;
		}
	}

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� true, �ƴϸ� false
	if(map[point.x][point.y].distance==0)return true;
		else return false;
}

//���� Ȱ��ȭ
void CSRPG::SetActiveUnit(int id)
{
	active_unit = id;
	map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = 0xff;	//Ȱ��ȭ ������ Ư�����

	//������ �Ӽ��� ���� �̵��� �� ����
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	{
		if(unit[active_unit].move_bonus == NORMAL_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
			else map[i][j].move = tile_mov[0][map[i][j].ground];
		}
		else if(unit[active_unit].move_bonus == WATER_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object]+1;
			else if(tile_attr[0][map[i][j].ground] == 1)map[i][j].move = 1; //���� ������ �̵� ���ʽ�
			else map[i][j].move = tile_mov[0][map[i][j].ground]+1;
		}
		else if(unit[active_unit].move_bonus == FIRE_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
			else if(tile_attr[0][map[i][j].ground] == 2)map[i][j].move = 1; //�� ������ �̵� ���ʽ�
			else map[i][j].move = tile_mov[0][map[i][j].ground];
		}
		else if(unit[active_unit].move_bonus == SPECIAL_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
			else map[i][j].move = tile_mov[0][map[i][j].ground];

			if(map[i][j].move != 99)map[i][j].move = 2;		//�̵� ������ ��� ������ �Ȱ��� ������ �̵�
		}
		else if(unit[active_unit].move_bonus == SKY_MOVE)
		{
			if(tile_mov[0][map[i][j].ground] != 99)map[i][j].move = 2;	//�̵� �Ұ� ����(��, ��������) ���� ��� ���� �̵���
			else map[i][j].move = 99;
		}
		else if(unit[active_unit].move_bonus == GHOST_MOVE)
		{
			map[i][j].move = 2;		//������  ���
		}

		//�̵��� 0�̸� �̵� �Ұ� ����
		if(map[i][j].move == 99)map[i][j].move = MOVE_DISABLE;
	}
}