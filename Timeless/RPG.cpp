#include "rpg.h"

////////////////////////////////////////////////////////////
// CRPG �޼ҵ�

CRPG::CRPG()
{
}

CRPG::~CRPG()
{
}

//����
void CRPG::Control()
{
	CGameMap::Control();

	//��ũ��Ʈ ó�� ���� ���� ���� �Ұ�
	if(m_script->IsReady())return;

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
}

void CRPG::Process()
{
	CGameMap::Process();

	//NPC �ൿ
	for(int i = HERO + 1; i < unit_max; ++i)
	{
		static int delay;
		++delay;
		if(delay > 100)
		{
			int dir = rand()%4;
			UnitMove(i, dir);
			ani.GetAni(i)->SetDirection((WalkDirection)dir);
			delay=0;
		}
	}

	//��Ŀ��
	InitScroll(HERO);
}

//�̵� ���ɼ�
int CGameMap::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	if(map[x][y].unit!=0xff)return 0;									//������ ������ �̵� �Ұ���
	else if(map[x][y].object!=0xff)return TILESIZE/(2<<map[x][y].move);	//2�� Ÿ�Ͽ� ���� �̵�
	else if(tile_attr[0][map[x][y].ground]>0)return 0;					//���ÿ� �Ϲ� �� �ܿ��� �̵� �Ұ���
	else return TILESIZE/(2<<map[x][y].move);							//�Ϲ� 1�� Ÿ�� �̵�
}