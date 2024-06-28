#pragma once

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum class SHADER_PASS_VTXANIMMODEL {
		PASS_DEFAULT,
		PASS_DEFAULT_ATOS,
		PASS_DISSOLVE,
		PASS_LIGHTDEPTH,
		PASS_EMISSIVE,
		PASS_END
	};	

	enum UI_ID {
		ORTHOGONAL_UI,
		PERSPECTIVE_UI,
		BUTTON,
		ID_END
	};

	enum UI_OPERRATION{ POP_UP, UI_IDLE, HIDE, STATE_END };

	enum MAP_DIRECTION { DIRECTION_WEST, DIRECTION_EAST, DIRECTION_MID };
	enum PLAYER_DIREECTION { DIRECTION_FRONT = 1, DIRECTION_BACK = 2, DIRECTION_LEFT = 4, DIRECTION_RIGHT = 8, DIRECTION_END };

	enum MONSTER_TYPE { MT_ZOMBIE, MT_DEFAULT };
	enum MONSTER_STATE { MST_IDLE, MST_WALK, MST_BITE, MST_TURN, MST_DEFAULT, MST_DAMAGE, MST_DOWN, MST_STANDUP, MST_END };

	/* Interact Hit Types */
	enum HIT_TYPE { HIT_SMALL, HIT_BIG, HIT_CRITICAL, HIT_EXPLOSION, HIT_END };

	enum ITEM_NUMBER {//���� �̸� �Դϴ� 70������ �߰� �Ǿ� ���� �빮�ڴ� �ӽ���
		emergencyspray01a,
		greenherb01a,
		redherb01a,
		blueherb01a,
		herbsgg01a,
		herbsgr01a,
		herbsgb01a,
		herbsggb01a,
		herbsggg01a,
		herbsgrb01a,
		herbsrb01a,
		greenherbitem01a,
		redherbitem01a,
		blueherbitem01a,
		handgun_bullet01a,
		shotgun_bullet01a,
		submachinegun_bullet01a,
		magnumbulleta,
		biggun_bullet01a,
		inkribbon01a,
		woodbarricade01a,
		blastingfuse01a,
		gunpowder01a,
		gunpowder01b,
		strengtheningyellow01a,
		HandGun,
		ITEM_NUMBER_END 
	};


	//�κ��丮 �ȿ����� ������ Ÿ��
	enum ITEM_TYPE {
		EQUIPABLE, // ���� ���
		CONSUMABLE_EQUIPABLE, //�Ҹ� ���
		USEABLE, // ��� ������
		CONSUMABLE, //�Ҹ� ������
		QUEST, // ����Ʈ������
		DRAG_SHADOW,//�巹�� ������
		INVEN_ITEM_TYPE_END
	};

	enum INVENTORY_EVENT {
		EVENT_IDLE, 
		EQUIP_ITEM,
		UNEQUIP_ITEM,
		USE_ITEM,
		EXAMINE_ITEM,
		COMBINED_ITEM,
		HOTKEY_ASSIGNED_ITEM,
		REARRANGE_ITEM,
		DISCARD_ITEM,
		CONTEXTUI_SELECT, 
		INVEN_EVENT_END
	};

	enum ANIM_BONE_TYPE_COLLIDER_DOOR_SINGLE
	{
		ATC_SINGLE_DOOR_OPEN_L,
		ATC_SINGLE_DOOR_OPEN_R
	};

	enum ANIM_BONE_TYPE_COLLIDER_DOOR_DOUBLE
	{
		ATC_DOUBLE_DOOR_OPEN_L_SIDE_L,
		ATC_DOUBLE_DOOR_OPEN_L_SIDE_R,
		ATC_DOUBLE_DOOR_OPEN_R_SIDE_L,
		ATC_DOUBLE_DOOR_OPEN_R_SIDE_R,
		ATC_ROOT
	};

	enum DOUBLE_DOOR_MESH_TYPE
	{
		DOOR_L,
		DOOR_R
	};

	enum DOUBLE_DOOR_MODEL_TYPE
	{
		NORMAL_DOOR,
		FRONT_DOOR
	};
		
	enum FIRE_WALL_ROTATE_BONE_TYPE
	{
		DOOR,
	};
	
	enum OBJ_TYPE
	{
		OBJ_NOPART,
		OBJ_DOOR,
		OBJ_WINDOW,
		OBJ_CABINET,
		OBJ_SHUTTER,
		OBJ_STATUE,
		OBJ_BIGSTATUE,
		OBJ_HALL_STATUE,
		OBJ_EVENTPROP,
		OBJ_ITEM,
		OBJ_END
	};

	enum ANIM_BONE_TYPE_COLLIDER_CABINET
	{
		ATC_CABINET_DOOR
	};

	enum CABINET_MESH_TYPE
	{
		CABINET_DOOR = 2
	};


	// ���߿� ���� �ʱ� ���� ������ UNDISCOVERED�� ���� �ݿ��ϰڽ��ϴ�
	enum JOMBIE_BEHAVIOR_COLLIDER_TYPE
	{
		JOMBIE_BEHAVIOR_COLLIDER_BOX_100, // ���� �� �ִ� �ڽ� ���� ã������ �� �ڽ��� ��ġ�Ǿ� �ִ� ��ġ�� ã�� ���߽��ϴ�. ��ġ�� �˰� ��ôٸ� ������ �����ּ���.
		JOMBIE_BEHAVIOR_COLLIDER_BOX_50, //  ���� �� �ִ� �ڽ� ���� ã������ �� �ڽ��� ��ġ�Ǿ� �ִ� ��ġ�� ã�� ���߽��ϴ�. ��ġ�� �˰� ��ôٸ� ������ �����ּ���.
		JOMBIE_BEHAVIOR_COLLIDER_RAILING_FALL, // ���� O : em0000_0920_railing_fall
		JOMBIE_BEHAVIOR_COLLIDER_HIGH_RAILING_FALL,  // ����O, �� ������ �� �ִ�(���� �� ��� 3�� ����) : em0000_0855_Crawl_fall_bridge + em0000_0920_railing_fall
		JOMBIE_BEHAVIOR_COLLIDER_FALL, // ����X, �׳� �������ų� �� ������ �� �ִ� :  em0000_0920_railing_fall �Ұ���
		JOMBIE_BEHAVIOR_COLLIDER_STAIR, //��� - ���߿� �ٸ� LAYER�� ���� �� �� ������ ����غ����� (����: ����ݶ��̴��� ����� ���� �ݶ��̴��� �浹 ��ħ �߻��� ���� ó���� ������ �� ����)
		JOMBIE_BEHAVIOR_COLLIDER_WINDOW, //������� ���� ����� �ֽ��ϴ� em0000_window_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_DOOR, //�� ����� ������� ���� �ֽ��ϴ�. em0000_door_em.motlist.85 
		JOMBIE_BEHAVIOR_COLLIDER_END,
	};

	/// <summary>
	/// 
	/// ���� �÷��̾� ������ �غ��� ��� �ʿ��� �� ���� ��� ���
	///<���� - window ��� + �ڸ�ǰ door ���>
	///	natives\x64\sectionroot\animation\enemy\em0000\list\gimmick\em0000_window_em.motlist.85
	/// 
	///  �ݶ��̴��� ����ִ� ����
	///  1. Ÿ�� _int��
	///	  2. ���� �����غ����� ���� : ��ǵ��� ���� ��ġ�� �ݶ��̴��� ������ �ϴ°�?
	/// 
	/// </summary>

	enum PLAYER_BEHAVIOR_COLLIDER_TYPE
	{
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_BOX, // ������ �ڽ�
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WALL, // ������ ��
		PLAYER_BEHAVIOR_COLLIDER_CLIMB_WINDOW, // �����
		PLAYER_BEHAVIOR_COLLIDER_LADDER, //
		PLAYER_BEHAVIOR_COLLIDER_DOOR, //
		PLAYER_BEHAVIOR_COLLIDER_END,
	};


}
