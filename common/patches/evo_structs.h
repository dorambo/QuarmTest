#ifndef EVO_STRUCTS_H_
#define EVO_STRUCTS_H_

#include <string>

namespace Evo {
	namespace structs {

/*
** Compiler override to ensure
** byte aligned structures
*/
#pragma pack(1)

/*
** Color_Struct
** Size: 4 bytes
** Used for convenience
** Merth: Gave struct a name so gcc 2.96 would compile
**
*/

struct Texture_Struct
{
	uint32 Material;
};

struct TextureProfile
{
	union {
		struct {
			Texture_Struct Head;
			Texture_Struct Chest;
			Texture_Struct Arms;
			Texture_Struct Wrist;
			Texture_Struct Hands;
			Texture_Struct Legs;
			Texture_Struct Feet;
			Texture_Struct Primary;
			Texture_Struct Secondary;
		};
		Texture_Struct Slot[EQ::textures::materialCount];
	};
};

struct Tint_Struct
{
	union
	{
		struct
		{
			uint8	blue;
			uint8	green;
			uint8	red;
			uint8	use_tint;	// if there's a tint this is FF
		};
		uint32 Color;
	};
};

struct TintProfile {
	union {
		struct {
			Tint_Struct Head;
			Tint_Struct Chest;
			Tint_Struct Arms;
			Tint_Struct Wrist;
			Tint_Struct Hands;
			Tint_Struct Legs;
			Tint_Struct Feet;
			Tint_Struct Primary;
			Tint_Struct Secondary;
		};
		Tint_Struct Slot[EQ::textures::materialCount];
	};
};

struct NewZone_Struct
{
	/*0000*/	char	char_name[64];			// Character Name
	/*0064*/	char	zone_short_name[32];	// Zone Short Name
	/*0096*/	char	zone_long_name[278];	// Zone Long Name
	/*0374*/	uint8	ztype;
	/*0375*/	uint8	fog_red[4];				// Red Fog 0-255 repeated over 4 bytes (confirmed)
	/*0379*/	uint8	fog_green[4];			// Green Fog 0-255 repeated over 4 bytes (confirmed)
	/*0383*/	uint8	fog_blue[4];			// Blue Fog 0-255 repeated over 4 bytes (confirmed)
	/*0387*/	uint8	unknown387;
	/*0388*/	float	fog_minclip[4];			// Where the fog begins (lowest clip setting). Repeated over 4 floats. (confirmed)
	/*0404*/	float	fog_maxclip[4];			// Where the fog ends (highest clip setting). Repeated over 4 floats. (confirmed)
	/*0420*/	float	gravity;
	/*0424*/	uint8	time_type;
	/*0425*/    uint8   rain_chance[4];
	/*0429*/    uint8   rain_duration[4];
	/*0433*/    uint8   snow_chance[4];
	/*0437*/    uint8   snow_duration[4];
	/*0441*/	uint8	specialdates[16];
	/*0457*/	uint8	specialcodes[16];
	/*0473*/	int8	timezone;
	/*0474*/	uint8	sky;					// Sky Type
	/*0475*/	uint8   unknown0475;
	/*0476*/	int16  water_music;
	/*0478*/	int16  normal_music_day;
	/*0480*/	int16  normal_music_night;
	/*0482*/	uint8	unknown0482[6];
	/*0484*/	float	zone_exp_multiplier;	// Experience Multiplier
	/*0488*/	float	safe_y;					// Zone Safe Y
	/*0492*/	float	safe_x;					// Zone Safe X
	/*0496*/	float	safe_z;					// Zone Safe Z
	/*0500*/	float	max_z;					// Guessed
	/*0504*/	float	underworld;				// Underworld, min z (Not Sure?)
	/*0508*/	float	minclip;				// Minimum View Distance
	/*0512*/	float	maxclip;				// Maximum View DIstance
	/*0516*/	uint32	forage_novice;
	/*0520*/	uint32	forage_medium;
	/*0524*/	uint32	forage_advanced;
	/*0528*/	uint32	fishing_novice;
	/*0532*/	uint32	fishing_medium;
	/*0536*/	uint32	fishing_advanced;
	/*0540*/	uint32	skylock;
	/*0544*/	uint16	graveyard_time;
	/*0546*/	uint32	scriptPeriodicHour;
	/*0550*/	uint32	scriptPeriodicMinute;
	/*0554*/	uint32	scriptPeriodicFast;
	/*0558*/	uint32	scriptPlayerDead;
	/*0562*/	uint32	scriptNpcDead;
	/*0566*/	uint32  scriptPlayerEntering;
	/*0570*/	uint16	unknown570;		// ***Placeholder
	/*0572*/
};

struct Spawn_Struct
{
	/*0000*/	uint32  random_dontuse;
	/*0004*/	int8	accel;
	/*0005*/	uint8	heading;			// Current Heading
	/*0006*/	int8	deltaHeading;		// Delta Heading
	/*0007*/	int16	y_pos;				// Y Position
	/*0009*/	int16	x_pos;				// X Position
	/*0011*/	int16	z_pos;				// Z Position
	/*0013*/	uint32	deltaY:11,			// Velocity Y
						deltaZ:11,			// Velocity Z
						deltaX:10;			// Velocity X
	/*0017*/	uint8	void1;
	/*0018*/	uint16	petOwnerId;		// Id of pet owner (0 if not a pet)
	/*0020*/	uint8	animation;
	/*0021*/    uint8	haircolor;
	/*0022*/	uint8	beardcolor;
	/*0023*/	uint8	eyecolor1;
	/*0024*/	uint8	eyecolor2;
	/*0025*/	uint8	hairstyle;
	/*0026*/	uint8	beard;
	/*0027*/    uint8   title; //0xff
	/*0028*/	float	size;
	/*0032*/	float	walkspeed;
	/*0036*/	float	runspeed;
	/*0040*/	TintProfile	equipcolors;
	/*0076*/	uint16	spawn_id;			// Id of new spawn
	/*0078*/	int16	bodytype;			// 65 is disarmable trap, 66 and 67 are invis triggers/traps
	/*0080*/	int16	cur_hp;				// Current hp's of Spawn
	/*0082*/	int16	GuildID;			// GuildID - previously Current hp's of Spawn
	/*0084*/	uint16	race;				// Race
	/*0086*/	uint8	NPC;				// NPC type: 0=Player, 1=NPC, 2=Player Corpse, 3=Monster Corpse, 4=???, 5=Unknown Spawn,10=Self
	/*0087*/	uint8	class_;				// Class
	/*0088*/	uint8	gender;				// Gender Flag, 0 = Male, 1 = Female, 2 = Other
	/*0089*/	uint8	level;				// Level of spawn (might be one int8)
	/*0090*/	uint8	invis;				// 0=visable, 1=invisable
	/*0091*/	uint8	sneaking;
	/*0092*/	uint8	pvp;
	/*0093*/	uint8	anim_type;
	/*0094*/	uint8	light;				// Light emitting
	/*0095*/	int8	anon;				// 0=normal, 1=anon, 2=RP
	/*0096*/	int8	AFK;				// 0=off, 1=on
	/*0097*/	int8	summoned_pc;
	/*0098*/	int8	LD;					// 0=NotLD, 1=LD
	/*0099*/	int8	GM;					// 0=NotGM, 1=GM
	/*0100*/	int8	flymode;
	/*0101*/	int8	bodytexture;
	/*0102*/	int8	helm;
	/*0103*/	uint8	face;
	/*0104*/	uint16	equipment[9];		// Equipment worn: 0=helm, 1=chest, 2=arm, 3=bracer, 4=hand, 5=leg, 6=boot, 7=melee1, 8=melee2
	/*0122*/	int16	guildrank;			// ***Placeholder
	/*0124*/	int16	deity;				// Deity.
	/*0126*/	int8	temporaryPet;
	/*0127*/	char	name[64];			// Name of spawn (len is 30 or less)
	/*0191*/	char	Surname[32];		// Last Name of player
	/*0223*/	uint8	void_;
	/*0224*/
};

//New ChannelMessage_Struct struct
struct ChannelMessage_Struct
{
	/*000*/	char	targetname[64];		// Tell recipient
	/*064*/	char	sender[64];			// The senders name (len might be wrong)
	/*128*/	uint32	language;			// Language
	/*132*/	uint32	chan_num;			// Channel
	/*136*/	uint32	cm_unknown4;		// ***Placeholder
	/*140*/	uint32	skill_in_language;	// The players skill in this language? might be wrong
	/*144*/	char	message[0];			// Variable length message
};

struct SpawnHPUpdate_Struct
{
	/*000*/ uint32  spawn_id;		// Comment: Id of spawn to update
	/*004*/ int32 cur_hp;		// Comment:  Current hp of spawn
	/*008*/ int32 max_hp;		// Comment: Maximum hp of spawn
	/*012*/
};

struct SpecialMesg_Struct
{
	/*0000*/ uint32 msg_type;		// Comment: Type of message
	/*0004*/ char  message[0];		// Comment: Message, followed by four bytes?
};

#define ITEM_STRUCT_SIZE 360
#define SHORT_BOOK_ITEM_STRUCT_SIZE	264
#define SHORT_CONTAINER_ITEM_STRUCT_SIZE 276
struct Item_Struct
{
	/*0000*/ char		Name[64];			// Name of item
	/*0064*/ char		Lore[80];			// Lore text
	/*0144*/ char		IDFile[30];			// This is the filename of the item graphic when held/worn.
	/*0174*/ uint8		Weight;				// Weight of item
	/*0175*/ uint8		NoRent;				// Nosave flag 1=normal, 0=nosave, -1=spell?
	/*0176*/ uint8		NoDrop;				// Nodrop flag 1=normal, 0=nodrop, -1=??
	/*0177*/ uint8		Size;				// Size of item
	/*0178*/ int16		ItemClass;
	/*0180*/ int16		ID;					// Record number. Confirmed to be signed.
	/*0182*/ uint16		Icon;				// Icon Number
	/*0184*/ int16		equipSlot;			// Current slot location of item
	/*0186*/ uint8		unknown0186[2];		// Client dump has equipSlot/location as a short so this is still unknown
	/*0188*/ int32		Slots;				// Slots where this item is allowed
	/*0192*/ int32		Price;				// Item cost in copper
	/*0196*/ float		cur_x;				//Here to 227 are named from client struct dump.
	/*0200*/ float		cur_y;
	/*0204*/ float		cur_z;
	/*0208*/ float		heading;
	/*0212*/ uint32		inv_refnum;			// Unique serial. This is required by apply poison.
	/*0216*/ int16		log;
	/*0218*/ int16		loot_log;
	/*0220*/ int16		avatar_level;		//Usually 01, sometimes seen as FFFF, once as 0.
	/*0222*/ int16		bottom_feed;
	/*0224*/ uint32		poof_item;
	union
	{
		struct
		{
			// 0228- have different meanings depending on flags
			/*0228*/ int8		AStr;				// Strength
			/*0229*/ int8		ASta;				// Stamina
			/*0230*/ int8		ACha;				// Charisma
			/*0231*/ int8		ADex;				// Dexterity
			/*0232*/ int8		AInt;				// Intelligence
			/*0233*/ int8		AAgi;				// Agility
			/*0234*/ int8		AWis;				// Wisdom
			/*0235*/ int8		MR;					// Magic Resistance
			/*0236*/ int8		FR;					// Fire Resistance
			/*0237*/ int8		CR;					// Cold Resistance
			/*0238*/ int8		DR;					// Disease Resistance
			/*0239*/ int8		PR;					// Poison Resistance
			/*0240*/ int16		HP;					// Hitpoints
			/*0242*/ int16		Mana;				// Mana
			/*0244*/ int16		AC;					// Armor Class
			/*0246*/ int8		MaxCharges;			// Maximum number of charges, for rechargable? (Sept 25, 2002)
			/*0247*/ uint8      GMFlag;				// GM flag 0  - normal item, -1 - gm item (Sept 25, 2002)
			/*0248*/ uint8		Light;				// Light effect of this item
			/*0249*/ uint8		Delay;				// Weapon Delay
			/*0250*/ uint8		Damage;				// Weapon Damage
			/*0251*/ int8		EffectType1;		// 0=combat, 1=click anywhere w/o class check, 2=latent/worn, 3=click anywhere EXPENDABLE, 4=click worn, 5=click anywhere w/ class check, -1=no effect
			/*0252*/ uint8		Range;				// Range of weapon
			/*0253*/ uint8		ItemType;			// Skill of this weapon, refer to weaponskill chart
			/*0254*/ uint8      Magic;				// Magic flag
			/*0255*/ uint8      EffectLevel1;		// Casting level
			/*0256*/ uint32		Material;			// Material
			/*0260*/ uint32		Color;				// Amounts of RGB in original color
			/*0264*/ int16		Faction;			// Structs dumped from client has this as Faction
			/*0266*/ int16		Effect1;			// SpellID of special effect
			/*0268*/ int32		Classes;			// Classes that can use this item
			/*0272*/ int32		Races;				// Races that can use this item
			/*0276*/ int8		Stackable;			//  1= stackable, 3 = normal, 0 = ? (not stackable)
		} common;
		struct
		{
			/*0228*/ int16		BookType;			// Type of book (scroll, note, etc)
			/*0230*/ int8		Book;				// Are we a book
			/*0231*/ char		Filename[30];		// Filename of book text on server
			/*0261*/ int32		buffer1[4];			// Not used, fills out space in the packet so ShowEQ doesn't complain.
		} book;
		struct
		{
			/*0228*/ int32		buffer2[10];		// Not used, fills out space in the packet so ShowEQ doesn't complain.
			/*0268*/ uint8		BagType;			//Bag type (obviously)
			/*0269*/ uint8		BagSlots;			// number of slots in container
			/*0270*/ uint8		IsBagOpen;			// 1 if bag is open, 0 if not.
			/*0271*/ uint8		BagSize;			// Maximum size item container can hold
			/*0272*/ uint8		BagWR;				// % weight reduction of container
			/*0273*/ uint32		buffer3;			// Not used, fills out space in the packet so ShowEQ doesn't complain.
		} container;
	};
	/*0277*/ uint8		EffectLevel2;				// Casting level
	/*0278*/ int8		Charges;					// Number of charges (-1 = unlimited)
	/*0279*/ int8		EffectType2;				// 0=combat, 1=click anywhere w/o class check, 2=latent/worn, 3=click anywhere EXPENDABLE, 4=click worn, 5=click anywhere w/ class check, -1=no effect
	/*0280*/ uint16		Effect2;					// spellId of special effect
	/*0282*/ int16		Effect2Duration;			// seen in client decompile being set to duration of Effect2 spell but purpose unknown
	/*0284*/ int16		HouseLockID;				// MSG_REQ_HOUSELOCK
	/*0286*/ uint8		unknown0286[2];
	/*0288*/ float		SellRate;
	/*0292*/ int32		CastTime;					// Cast time of clicky item in miliseconds
	/*0296*/ uint8		unknown0296[12];			// ***Placeholder
	/*0308*/ int32		RecastTime;					// Recast time of clicky item in milliseconds
	/*0312*/ uint16		SkillModType;
	/*0314*/ int16		SkillModValue;
	/*0316*/ int16		BaneDmgRace;
	/*0318*/ int16		BaneDmgBody;
	/*0320*/ uint8		BaneDmgAmt;
	/*0321*/ uint8		unknown0321;
	/*0322*/ uint16		title;
	/*0324*/ uint8		RecLevel;					// max should be 65
	/*0325*/ uint8		RecSkill;					// Max should be 252
	/*0326*/ int16		ProcRate;
	/*0328*/ uint8		ElemDmgType;
	/*0329*/ uint8		ElemDmgAmt;
	/*0330*/ int16		FactionMod1;
	/*0332*/ int16		FactionMod2;
	/*0334*/ int16		FactionMod3;
	/*0336*/ int16		FactionMod4;
	/*0338*/ int16		FactionAmt1;
	/*0340*/ int16		FactionAmt2;
	/*0342*/ int16		FactionAmt3;
	/*0344*/ int16		FactionAmt4;
	/*0346*/ uint16		Void346;
	/*0348*/ int32		Deity;						// Bitmask of Deities that can equip this item
	/*0352*/ int16		ReqLevel;					// Required level
	/*0354*/ int16		BardType;
	/*0356*/ int16		BardValue;
	/*0358*/ int16		FocusEffect;				//Confirmed
	/*0360*/
};

struct PlayerItemsPacket_Struct
{
	/*000*/	int16		opcode;		// OP_ItemTradeIn
	/*002*/	struct Item_Struct	item;
};

struct PlayerItems_Struct
{
	/*000*/	int16		count;
	/*002*/	struct PlayerItemsPacket_Struct	packets[0];
};

struct MerchantItemsPacket_Struct
{
	/*000*/	uint16 itemtype;
	/*002*/	struct Item_Struct	item;
};

struct TradeItemsPacket_Struct
{
	/*000*/	uint16 fromid;
	/*002*/	uint16 slotid;
	/*004*/	uint8  unknown;
	/*005*/	struct Item_Struct	item;
	/*000*/	uint8 unknown1[5];
	/*000*/
};

struct PickPocket_Struct
{
// Size 18
    uint16 to;
    uint16 from;
    uint16 myskill;
    uint16 type; // -1 you are being picked, 0 failed , 1 = plat, 2 = gold, 3 = silver, 4 = copper, 5 = item
    uint32 coin;
    uint8 data[6];
};

struct PickPocketItemPacket_Struct
{
    uint16 to;
    uint16 from;
    uint16 myskill;
    uint16 type; // -1 you are being picked, 0 failed , 1 = plat, 2 = gold, 3 = silver, 4 = copper, 5 = item
    uint32 coin;
	struct Item_Struct	item;
};

struct MerchantItems_Struct
{
	/*000*/	int16		count;
	/*002*/	struct MerchantItemsPacket_Struct packets[0];
};

struct MoveItem_Struct
{
	/*000*/ uint32 from_slot;
	/*004*/ uint32 to_slot;
	/*008*/ uint32 number_in_stack;
	/*012*/
};

struct CancelTrade_Struct
{
	/*000*/	uint16 fromid;
	/*002*/	uint16 action;
	/*004*/
};

struct Merchant_Click_Struct
{
	/*000*/ uint16	npcid;			// Merchant NPC's entity id
	/*002*/ uint16	playerid;
	/*004*/	uint8  command;
	/*005*/ uint8	unknown[3];
	/*008*/ float   rate;
	/*012*/
};

struct Merchant_Sell_Struct
{
	/*000*/	uint16	npcid;			// Merchant NPC's entity id
	/*002*/	uint16	playerid;		// Player's entity id
	/*004*/	uint16	itemslot;
	/*006*/	uint8	IsSold;		// Already sold
	/*007*/	uint8	unknown001;
	/*008*/	uint8	quantity;	// Qty - when used in Merchant_Purchase_Struct
	/*009*/	uint8	unknown004[3];
	/*012*/	uint32	price;
	/*016*/
};

struct Merchant_Purchase_Struct
{
	/*000*/	uint16	npcid;			// Merchant NPC's entity id
	/*002*/ uint16  playerid;
	/*004*/	uint16	itemslot;		// Player's entity id
	/*006*/ uint16  price;
	/*008*/	uint8	quantity;
	/*009*/ uint8   unknown_void[7];
	/*016*/
};

struct Merchant_DelItem_Struct
{
	/*000*/	uint16	npcid;			// Merchant NPC's entity id
	/*002*/	uint16	playerid;		// Player's entity id
	/*004*/	uint8	itemslot;       // Slot of the item you want to remove
	/*005*/	uint8	type;     // 0x40
	/*006*/
};

struct SetDataRate_Struct
{
	/*000*/	float newdatarate;	// Comment:
	/*004*/
};

// Added this struct for eqemu and started eimplimentation ProcessOP_SendLoginInfo
//TODO: confirm everything in this struct
// PPC Mac version of this is 196 bytes, Intel Mac version is 200 bytes
struct LoginInfo_Struct
{
	/*000*/	char	AccountName[127];
	/*127*/	char	Password[24];
	/*151*/ uint8	unknown189[41];
	/*192*/ uint8   zoning;
	/*193*/ uint8   unknown193[3];
	/*196*/
};

// Not sure about this, blindly changed it
struct SpellBuff_Struct
{
	/*000*/uint8  bufftype;        // Comment: 0 = Buff not visible, 1 = Visible and permanent buff, 2 = Visible and timer on, 4 = reverse effect values, used for lifetap type things
	/*001*/uint8  level;			// Comment: Level of person who casted buff
	/*002*/uint8  bard_modifier;	// Comment: this seems to be the bard modifier, it is normally 0x0A because we set in in the CastOn_Struct when its not a bard, else its the instrument mod
	/*003*/uint8  activated;    // Copied from spell data to buff struct.  Only a few spells have this set to 1, the rest are 0
	/*004*/uint32 spellid;        // spell id
	/*008*/uint32 duration;        // Duration in ticks
	/*012*/uint32 counters;        // rune amount, poison/disease/curse counters
	/*016*/
};

// Length: 10
struct ItemProperties_Struct
{

	/*000*/	uint8	unknown01[2];
	/*002*/	int8	charges;				// Comment: signed int because unlimited charges are -1
	/*003*/	uint8	unknown02[7];
	/*010*/
};

/*
	*Used in PlayerProfile
	*/
struct AA_Array
{
	uint8 AA;
	uint8 value;
};

static const uint32  MAX_PP_AA_ARRAY		= 120;
static const uint32 MAX_PP_SKILL		= 100; // _SkillPacketArraySize;	// 100 - actual skills buffer size
// Length 4308
struct PlayerProfile_Struct
{
	#define pp_inventory_size 30
	#define pp_containerinv_size 80
	#define pp_cursorbaginventory_size 10
	#define pp_bank_inv_size 8
	/* ***************** */
	/*0000*/	uint32  checksum;		    // Checksum
	/*0004*/	char	name[64];			// Player First Name
	/*0068*/	char	last_name[32];		// Surname OR title.
	/*0100*/	uint8	gender;				// Player Gender
	/*0101*/	char	genderchar[3];		// ***Placeholder
	/*0104*/	uint32 race;				// Player Race (Lyenu: Changed to an int16, since races can be over 255)
	/*0108*/	uint32	class_;				// Player Class
	/*0112*/	uint32 bodytype;	// guess
	/*0116*/	uint8	level;				// Player Level
	/*0117*/	char	levelchar[3];		// ***Placeholder
	/*0120*/	uint32	bind_point_zone;
	/*0124*/	float	bind_y;
	/*0128*/	float	bind_x;
	/*0132*/	float	bind_z;
	/*0136*/	float	bind_heading;
	/*0140*/	uint32	deity; // guess
	/*0144*/	uint32	expAA; // guess might be guild_id
	/*0148*/	uint32	birthday;
	/*0152*/	uint32	lastlogin;
	/*0156*/	uint32	timePlayedMin;
	/*0160*/	int8	fatigue;
	/*0161*/	uint8	pvp;				// Player PVP Flag
	/*0162*/	uint8	level2;
	/*0163*/	uint8	anon;				// Player Anon. Flag
	/*0164*/	uint8	gm;					// Player GM Flag

	/*0165*/	uint8	unknown0165[7];
	/*0172*/	uint8	unknown0172[40];	// from ghidra

	/*0212*/	uint8	haircolor;			// Player hair color
	/*0213*/	uint8	beardcolor;			// Player beard color
	/*0214*/	uint8	eyecolor1;			// Player left eye color
	/*0215*/	uint8	eyecolor2;			// Player right eye color
	/*0216*/	uint8	hairstyle;			// Player hair style
	/*0217*/	uint8	beard;				// Beard type
	/*0218*/	uint8	luclinface;		// guess
	/*0219*/	uint8	unknown0219;

	// ghidra shows 88 bytes then 36 bytes, 5.0 shows the opposite, might need to swap them:
	/*0220*/	TextureProfile item_material;	// Item texture/material of worn/held items
	/*0256*/	uint8	unknown0256[88];

	/*0344*/	AA_Array	aa_array[MAX_PP_AA_ARRAY];		// Length may not be right but i assume no class has more than this
	/*0584*/	uint8	unknown0584[4];

	/*0588*/	char	servername[32];
	/*0620*/	char unknown00620[32];	// complete guess
	/*0652*/	uint32				guild_id;		// complete guess
	/*0656*/	uint32				exp;				// Current Experience
	/*0660*/	uint32				points;				// Unspent Practice points
	/*0664*/	uint32				mana;				// current mana
	/*0668*/	uint32				cur_hp;				// current hp
	/*0672*/	uint32				status;		// 0x05
	/*0676*/	uint32				STR;				// Strength
	/*0680*/	uint32				STA;				// Stamina
	/*0684*/	uint32				CHA;				// Charisma
	/*0688*/	uint32				DEX;				// Dexterity
	/*0692*/	uint32				INT;				// Intelligence
	/*0696*/	uint32				AGI;				// Agility
	/*0700*/	uint32				WIS;				// Wisdom
	/*0704*/	uint8				face;				// Player face

	/*0705*/	uint8	unknown0705[11]; // maybe EquipType[9] ? this is where I was two bytes short.
	/*0716*/ TintProfile EquipColor; // 36 bytes in titanium

	/*0752*/ uint8 languages[32];
	/*0784*/	int32	spell_book[spells::SPELLBOOK_SIZE];    // List of the Spells in spellbook // 400
	/*2384*/	int32	unused_spell_book[112];   // all 0xff after last spell
	/*2832*/	int32	mem_spells[spells::SPELL_GEM_COUNT]; // List of spells memorized
	/*2864*/ int32 unused_mem_spells[8];
	/*2896*/	uint32 available_slots; // guess
	/*2900*/	float	y;					// Player Y
	/*2904*/	float	x;					// Player X
	/*2908*/	float	z;					// Player Z
	/*2912*/	float	heading;			// Player Heading
	/*2916*/	uint32	position;		// ***Placeholder
	/*2920*/	int32	platinum;			// Player Platinum (Character)
	/*2922*/	int32	gold;				// Player Gold (Character)
	/*2928*/	int32	silver;				// Player Silver (Character)
	/*2932*/	int32	copper;				// Player Copper (Character)
	/*2936*/	int32	platinum_bank;		// Player Platinum (Bank)
	/*2940*/	int32	gold_bank;			// Player Gold (Bank)
	/*2944*/	int32	silver_bank;		// Player Silver (Bank)
	/*2948*/	int32	copper_bank;		// Player Copper (Bank)
	/*2952*/	int32	platinum_cursor;
	/*2956*/	int32	gold_cursor;
	/*2960*/	int32	silver_cursor;
	/*2964*/	int32	copper_cursor;
	/*2968*/	int32	skills[MAX_PP_SKILL];		// Player Skills - 100 skills but only the first 74 are used, followed by 25 innates but only the first 14 are used
	/*3368*/	int32	innate_skills[25];	// Like regular skills, these are 255 to indicate that the player doesn't have it and 0 means they do have it
	/*3468*/	uint8 texture;
	/*3469*/	uint8 unknown3469[19];

	/*3488*/	char	boat[32]; // unsure about length
	/*3520*/	char	lazy[40]; // FIXME: fill out packet

	/*3560*/	uint8 unknown3560[116]; // ghidra has a memcpy 116
	/*3676*/	uint32	zone_id;			// Current zone of the player
	/*3680*/	SpellBuff_Struct	buffs[15];	// Player Buffs Currently On - guess
	/*3920*/	char	groupMembers[6][64]; // guess
	/*4304*/	uint8 lazy3[4]; // FIXME: fill out packet
	/*4308*/
};

//Server sends this packet for reuse timers
//Client sends this packet as 256 bytes, and is our equivlent of AA_Action
struct UseAA_Struct
{
	/*000*/ int32 begin;
	/*004*/ int16 ability; // skill_id of a purchased AA.
	/*006*/ int16  unknown_void;
	/*008*/ int32 end;
	/*012*/

};

/*Not Implemented*/

//           No idea what this is used for, but it creates a
//           perminent object that no client may interact with.
//			 It also accepts spell sprites (i.e., GENC00), but
//			 they do not currently display. I guess we could use
//			 this for GM events?
//
//Opcode: 0xF640 MSG_ADD_OBJECT
struct ObjectDisplayOnly_Struct
{
	/*0000*/ char test1[32];
	/*0032*/ char modelName[16];
	/*0048*/ char test2[12];
	/*0060*/ float size;
	/*0064*/ float y;
	/*0068*/ float x;
	/*0072*/ float z;
	/*0076*/ float heading;
	/*0080*/ float tilt;
	/*0084*/ char test4[40];
};

/*
** Character Creation struct
** Length: 140 Bytes
** OpCode: 0x1301
*/
struct CharCreate_Struct
{
	/*0000*/	char	name[64];
	/*0064*/	int32	gender;
	/*0068*/	int32	race;
	/*0072*/	int32	class_;
		// 0 = odus
		// 1 = qeynos
		// 2 = halas
		// 3 = rivervale
		// 4 = freeport
		// 5 = neriak
		// 6 = gukta/grobb
		// 7 = ogguk
		// 8 = kaladim
		// 9 = gfay
		// 10 = felwithe
		// 11 = akanon
		// 12 = cabalis
		// 13 = shar vahl
	/*0076*/	int32	start_zone;
	/*0080*/	int32	deity;
	/*0084*/	int32	STR;
	/*0088*/	int32	STA;
	/*0092*/	int32	AGI;
	/*0096*/	int32	DEX;
	/*0100*/	int32	WIS;
	/*0104*/	int32	INT;
	/*0108*/	int32	CHA;
	/*0112*/	int32	face;
	/*0116*/	int32	hairstyle;
	/*0120*/	int32	eyecolor1;
	/*0124*/	int32	eyecolor2;
	/*0128*/	int32	haircolor;
	/*0132*/	int32	beard;
	/*0136*/	int32	beardcolor;
	/*0140*/
};

/*
** Character Selection Struct
** Length: 1660 Bytes
*/
struct CharacterSelect_Struct {
	/*0000*/	char	name[10][64];			// Characters Names
	/*0640*/	uint8	level[10];				// Characters Levels
	/*0650*/	uint8	class_[10];				// Characters Classes
	/*0660*/	uint32	race[10];				// Characters Race
	/*0700*/	uint32	zone[10];				// Characters Current Zone
	/*0740*/	uint8	gender[10];				// Characters Gender
	/*0750*/	uint8	face[10];				// Characters Face Type
	/*0760*/	EQ::TextureProfile	equip[10];	// Characters texture array
	/*1120*/	EQ::TintProfile cs_colors[10];	// Characters Equipment Colors (RR GG BB 00)
	/*1480*/	uint32	deity[10];				// Characters Deity
	/*1520*/	uint32	primary[10];			// Characters primary and secondary IDFile number
	/*1560*/	uint32	secondary[10];			// Characters primary and secondary IDFile number
	/*1600*/	uint8	haircolor[10];
	/*1610*/	uint8	beardcolor[10];
	/*1620*/	uint8	eyecolor1[10];
	/*1630*/	uint8	eyecolor2[10];
	/*1640*/	uint8	hairstyle[10];
	/*1650*/	uint8	beard[10];
	/*1660*/
};

struct ServerZoneEntry_Struct
{
	uint8	checksum[4];
	uint8	type;		// ***Placeholder
	char	name[64];			// Name
	uint8 unknown0x41;
	uint8 unknown0x42;
	uint8 unknown0x43;
	uint8 unknown0x44;
	uint8 unknown0x45;
	uint8 unknown0x46;
	uint8 unknown0x47;
	float vz;
	float heading;
	float x_pos;
	float vy;
	float y_pos;
	float z_pos;
	float pitch;
	float accel;
	float vx;
	float rate_p;
	float rate_h;
	float view_pitch;
	uint32 unknown0x78;
	uint32 unknown0x7c;
	uint32 unknown0x80;
	int32	LocalInfo;
	uint8 unknown0x88;
	uint8 unknown0x89;
	uint8 unknown0x8a;
	uint8 unknown0x8b;
	uint8	sneaking;
	int8 LD;
	char m_bTemporaryPet;
	int8 LFG;
	uint8 unknown0x90;
	int8 unknown0x91; // Something boat / sphere related
	uint8 unknown0x92;
	uint8 unknown0x93;
	uint32 equipment[9];
	EQ::TintProfile equipcolors;
	uint32 zoneID;
	int32 animation_vtable;
	int32	next;
	int32	My_Char;
	uint8 unknown0xec;
	uint8 unknown0xed;
	uint8 unknown0xee;
	uint8 unknown0xef;
	int32	prev;
	uint8 unknown0xf4;
	uint8 unknown0xf5;
	uint8 unknown0xf6;
	uint8 unknown0xf7;
	char Surname[32];
	float width;
	float length;
	float runspeed;
	float view_height;
	float sprite_oheight;
	float size;
	float walkspeed;
	uint8 NPC;
	uint8 haircolor;
	uint8 beardcolor;
	uint8 eyecolor1;
	uint8 eyecolor2;
	uint8 hairstyle;
	uint8 beard;
	uint8 animation;
	uint8 level;
	uint8 face;
	uint8 gender;
	uint8 pvp;
	uint8 invis;
	uint8 anim_type;
	uint8 class_;
	uint8 light;
	uint8 helm;
	uint8 bodytexture;
	uint8 GM;
	uint8 unknown0x147;
	uint16 sprite_oheights;
	uint8 unknown0x14a;
	uint8 unknown0x14b;
	uint32 petOwnerId;
	uint32 race;
	uint32 avatar;
	uint8 unknown0x158;
	uint8 unknown0x159;
	uint8 unknown0x15a;
	uint8 unknown0x15b;
	uint32 AFK;
	int32 bodytype;
	uint32 curHP;
	uint32 aa_title;
	uint32 guildrank;
	uint32 deity;
	uint32 max_hp;
	uint32 GuildID;
	int32 flymode;
	uint8 unknown0x180;
	uint8 unknown0x181;
	uint8 unknown0x182;
	uint8 unknown0x183;
	uint8 unknown0x184;
	uint8 unknown0x185;
	uint8 unknown0x186;
	uint8 unknown0x187;
};

// FIXME: Taken from emu 5 & is complete wrong still
struct SpawnPositionUpdate_Struct
{
	/*00*/	uint16	spawn_id;
	/*02*/	uint32	heading:12;
	/*02*/	int32	delta_heading:10;
	/*02*/	int32	anim_type:10;
	/*06*/	int32	delta_y:13;
	/*06*/	int32	x_pos:19;
	/*10s*/	int32	y_pos:19;
	/*10*/	int32	delta_z:13;
	/*14*/	int32	delta_x:13;
	/*14*/	int32	z_pos:19;
};

struct ClientTarget_Struct
{
	/*000*/	uint32	new_target;			// Target ID
	/*004*/
};

// Taken from eqemu 5
struct Animation_Struct
{
	/*00*/	int16 spawnid;
	/*02*/	int8 animation_speed;
	/*03*/	int8 action;
	/*04*/
};

// Length: 9
struct WearChange_Struct
{
	/*000*/	uint16 spawn_id;
	/*002*/	uint16 material;
	/*004*/	EQ::textures::Tint_Struct color;
	/*008*/	uint8	wear_slot_id;
	/*009*/
};

	};	//end namespace structs
};	//end namespace EVO

#endif /*EVO_STRUCTS_H_*/
