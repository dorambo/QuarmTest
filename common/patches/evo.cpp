#include "../global_define.h"
#include "../eqemu_config.h"
#include "../eqemu_logsys.h"
#include "evo.h"
#include "../opcodemgr.h"
#include "../eq_stream_ident.h"
#include "../crc32.h"

#include "../eq_packet_structs.h"
#include "../packet_dump_file.h"
#include "../misc_functions.h"
#include "../packet_functions.h"
#include "../strings.h"
#include "../inventory_profile.h"
#include "evo_structs.h"
#include "../rulesys.h"
#include "../eq_constants.h"

namespace Evo {

	static const char *name = "Evo";
	static OpcodeManager *opcodes = nullptr;
	static Strategy struct_strategy;

	structs::Item_Struct* EvoItem(const EQ::ItemInstance *inst, int16 slot_id_in, int type = 0);

	static inline int16 ServerToEvoSlot(uint32 ServerSlot);
	static inline int16 ServerToEvoCorpseSlot(uint32 ServerCorpse);

	static inline uint32 EvoToServerSlot(int16 EvoSlot);
	static inline uint32 EvoToServerCorpseSlot(int16 EvoCorpse);
	static inline int32 EvoToServerStartZone(int32 EvoStartZone);

	void Register(EQStreamIdentifier &into)
	{
		auto Config = EQEmuConfig::get();
		//create our opcode manager if we havent already
		if(opcodes == nullptr)
		{
			std::string opfile = Config->PatchDir;
			opfile += "patch_";
			opfile += name;
			opfile += ".conf";
			//load up the opcode manager.
			//TODO: figure out how to support shared memory with multiple patches...
			opcodes = new RegularOpcodeManager();
			if(!opcodes->LoadOpcodes(opfile.c_str()))
			{
				Log(Logs::General, Logs::Netcode, "[OPCODES] Error loading opcodes file %s. Not registering patch %s.", opfile.c_str(), name);
				return;
			}
		}

		//ok, now we have what we need to register.

		EQStream::Signature signature;
		std::string pname;

		signature.ignore_eq_opcode = 0;

		pname = std::string(name) + "_world";
		//register our world signature.
		signature.first_length = sizeof(structs::LoginInfo_Struct);
		signature.first_eq_opcode = opcodes->EmuToEQ(OP_SendLoginInfo);
		into.RegisterOldPatch(signature, pname.c_str(), &opcodes, &struct_strategy);

		pname = std::string(name) + "_zone";
		//register our zone signature.
		signature.first_length = sizeof(structs::SetDataRate_Struct);
		signature.first_eq_opcode = opcodes->EmuToEQ(OP_DataRate);
		into.RegisterOldPatch(signature, pname.c_str(), &opcodes, &struct_strategy);

		Log(Logs::General, Logs::Netcode, "[IDENTIFY] Registered patch %s", name);
	}

	void Reload()
	{

		//we have a big problem to solve here when we switch back to shared memory
		//opcode managers because we need to change the manager pointer, which means
		//we need to go to every stream and replace it's manager.

		if(opcodes != nullptr)
		{
			//TODO: get this file name from the config file
			auto Config = EQEmuConfig::get();
			std::string opfile = Config->PatchDir;
			opfile += "patch_";
			opfile += name;
			opfile += ".conf";
			if(!opcodes->ReloadOpcodes(opfile.c_str()))
			{
				Log(Logs::General, Logs::Netcode, "[OPCODES] Error reloading opcodes file %s for patch %s.", opfile.c_str(), name);
				return;
			}
			Log(Logs::General, Logs::Netcode, "[OPCODES] Reloaded opcodes for patch %s", name);
		}
	}



	Strategy::Strategy()
	: StructStrategy()
	{
		//all opcodes default to passthrough.
		#include "ss_register.h"
		#include "evo_ops.h"
	}

	std::string Strategy::Describe() const
	{
		std::string r;
		r += "Patch ";
		r += name;
		return(r);
	}

	#include "ss_define.h"

	const EQ::versions::ClientVersion Strategy::ClientVersion() const
	{
		return EQ::versions::ClientVersion::Evo;
	}

	DECODE(OP_SendLoginInfo)
	{
		// Intel Mac client and Windows client are 200 bytes, PPC is 196 bytes
		int len = __packet->size;
		DECODE_LENGTH_ATLEAST(structs::LoginInfo_Struct);
		SETUP_DIRECT_DECODE(LoginInfo_Struct, structs::LoginInfo_Struct);
		memcpy(emu->login_info, eq->AccountName, 64);
		emu->macversion = len == 196 ? 8 : 4;
		IN(zoning);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_PlayerProfile)
	{
		SETUP_DIRECT_ENCODE(PlayerProfile_Struct, structs::PlayerProfile_Struct);

		eq->available_slots=0xffff;

		int r = 0;
		OUT(gender);
		OUT(race);
		OUT(class_);
		OUT(level);
		eq->bind_point_zone = emu->binds[0].zoneId;
		eq->bind_x = emu->binds[0].x;
		eq->bind_y = emu->binds[0].y;
		eq->bind_z = emu->binds[0].z;
		eq->bind_heading = emu->binds[0].heading;
		OUT(deity);
		// OUT(intoxication);
		OUT(haircolor);
		OUT(beardcolor);
		OUT(eyecolor1);
		OUT(eyecolor2);
		OUT(hairstyle);
		OUT(beard);
		OUT(points);
		OUT(mana);
		OUT(cur_hp);
		OUT(STR);
		OUT(STA);
		OUT(CHA);
		OUT(DEX);
		OUT(INT);
		OUT(AGI);
		OUT(WIS);
		OUT(face);
		OUT(luclinface);
		OUT_array(spell_book, spells::SPELLBOOK_SIZE);
		OUT_array(mem_spells, spells::SPELL_GEM_COUNT);
		OUT(platinum);
		OUT(gold);
		OUT(silver);
		OUT(copper);
		OUT(platinum_cursor);
		OUT(gold_cursor);
		OUT(silver_cursor);
		OUT(copper_cursor);
		OUT_array(skills, structs::MAX_PP_SKILL);  // 1:1 direct copy (100 dword)
		OUT_array(innate_skills, 25); // why isn't this in the mac pp

		for(r = 0; r < 15; r++)
		{
			eq->buffs[r].bufftype = (emu->buffs[r].spellid == 0xFFFF || emu->buffs[r].spellid == 0) ? 0 : 2; // TODO - don't hardcode this, it can be 4 for reversed effects
			OUT(buffs[r].level);
			OUT(buffs[r].bard_modifier);
			OUT(buffs[r].activated);
			OUT(buffs[r].spellid);
			OUT(buffs[r].duration);
			OUT(buffs[r].counters);
		}
		OUT_str(name);
		OUT_str(last_name);
		OUT(guild_id);
		OUT(birthday);
		OUT(lastlogin);
		OUT(timePlayedMin);
		OUT(pvp);
		OUT(anon);
		OUT(gm);
		// OUT(guildrank);
		// eq->uniqueGuildID = emu->guild_id;
		OUT(exp);
		OUT_array(languages, 26);
		OUT(x);
		OUT(y);
		OUT(z);
		OUT(heading);
		OUT(platinum_bank);
		OUT(gold_bank);
		OUT(silver_bank);
		OUT(copper_bank);
		OUT(level2);
		// OUT(autosplit);
		OUT(zone_id);
		OUT_str(boat);
		// OUT(aapoints);
		OUT(expAA);
		// OUT(perAA);
		// OUT(air_remaining);
		// if(emu->expansions > 15)
		// 	eq->expansions = 15;
		// else
		// 	OUT(expansions);
		// OUT(hunger_level);
		// OUT(thirst_level);
		// eq->thirst_level_unused = 127;
		// eq->hunger_level_unused = 127;
		for(r = 0; r < structs::MAX_PP_AA_ARRAY; r++)
		{
			OUT(aa_array[r].AA);
			OUT(aa_array[r].value);
		}
		for(r = 0; r < 6; r++)
		{
			OUT_str(groupMembers[r]);
		}
		for(r = EQ::textures::textureBegin; r <= EQ::textures::LastTexture; r++)
		{
			OUT(item_material.Slot[r].Material);
		}
		// OUT(abilitySlotRefresh);
		// OUT_array(spellSlotRefresh, spells::SPELL_GEM_COUNT);
		// OUT(eqbackground);
		// OUT(fatigue);
		// OUT(height);
		// OUT(width);
		// OUT(length);
		// OUT(view_height);
		// OUT_array(cursorbaginventory,pp_cursorbaginventory_size);
		// for(r = 0; r < pp_cursorbaginventory_size; r++)
		// {
		// 	OUT(cursorItemProperties[r].charges);
		// }
		// OUT_array(inventory,pp_inventory_size);
		// for(r = 0; r < pp_inventory_size; r++)
		// {
		// 	OUT(invItemProperties[r].charges);
		// }
		// OUT_array(containerinv,pp_containerinv_size);
		// for(r = 0; r < pp_containerinv_size; r++)
		// {
		// 	OUT(bagItemProperties[r].charges);
		// }
		// OUT_array(bank_inv,pp_bank_inv_size);
		// for(r = 0; r < pp_bank_inv_size; r++)
		// {
		// 	OUT(bankinvitemproperties[r].charges);
		// }
		// OUT_array(bank_cont_inv,pp_containerinv_size);
		// for(r = 0; r < pp_containerinv_size; r++)
		// {
		// 	OUT(bankbagitemproperties[r].charges);
		// }
		// OUT(LastModulated);

		CRC32::SetEQChecksum(__packet->pBuffer, sizeof(structs::PlayerProfile_Struct)-4);
		FINISH_ENCODE();
	}

	ENCODE(OP_NewZone)
	{
		SETUP_DIRECT_ENCODE(NewZone_Struct, structs::NewZone_Struct);
		OUT_str(char_name);
		OUT_str(zone_short_name);
		OUT_str(zone_long_name);
		OUT(ztype);
		OUT_array(fog_red, 4);
		OUT_array(fog_green, 4);
		OUT_array(fog_blue, 4);
		OUT_array(fog_minclip, 4);
		OUT_array(fog_maxclip, 4);
		OUT(gravity);
		OUT(time_type);
		OUT(sky);
		OUT(zone_exp_multiplier);
		OUT(safe_y);
		OUT(safe_x);
		OUT(safe_z);
		OUT(max_z);
		OUT(underworld);
		OUT(minclip);
		OUT(maxclip);
		OUT(skylock);
		OUT(graveyard_time);
		OUT(timezone);
		OUT_array(snow_chance, 4);
		OUT_array(snow_duration, 4);
		OUT_array(rain_chance, 4);
		OUT_array(rain_duration, 4);
		OUT(normal_music_day);
		OUT(water_music);
		OUT(normal_music_night);
		FINISH_ENCODE();
	}

	ENCODE(OP_SpecialMesg)
	{
		EQApplicationPacket *__packet = *p;
		*p = nullptr;
		unsigned char *__emu_buffer = __packet->pBuffer;
		SpecialMesg_Struct *emu = (SpecialMesg_Struct *) __emu_buffer;
		uint32 __i = 0;
		__i++; /* to shut up compiler */

		int msglen = __packet->size - sizeof(structs::SpecialMesg_Struct);
		int len = sizeof(structs::SpecialMesg_Struct) + msglen + 1;
		__packet->pBuffer = new unsigned char[len];
		__packet->size = len;
		memset(__packet->pBuffer, 0, len);
		structs::SpecialMesg_Struct *eq = (structs::SpecialMesg_Struct *) __packet->pBuffer;
		eq->msg_type = emu->msg_type;
		strcpy(eq->message, emu->message);
		FINISH_ENCODE();
	}

	ENCODE(OP_NewSpawn) { ENCODE_FORWARD(OP_ZoneSpawns); }

	ENCODE(OP_ZoneSpawns)
	{

		//consume the packet
		EQApplicationPacket *in = *p;
		*p = nullptr;

		//store away the emu struct
		unsigned char *__emu_buffer = in->pBuffer;
		Spawn_Struct *emu = (Spawn_Struct *) __emu_buffer;

		//determine and verify length
		int entrycount = in->size / sizeof(Spawn_Struct);
		if(entrycount == 0 || (in->size % sizeof(Spawn_Struct)) != 0)
		{
			Log(Logs::General, Logs::Netcode, "[STRUCTS] Wrong size on outbound %s: Got %d, expected multiple of %d", opcodes->EmuToName(in->GetOpcode()), in->size, sizeof(Spawn_Struct));
			delete in;
			return;
		}
		EQApplicationPacket* out = new EQApplicationPacket();
		out->SetOpcode(OP_ZoneSpawns);
		//make the EQ struct.
		out->size = sizeof(structs::Spawn_Struct)*entrycount;
		out->pBuffer = new unsigned char[out->size];
		structs::Spawn_Struct *eq = (structs::Spawn_Struct *) out->pBuffer;

		//zero out the packet. We could avoid this memset by setting all fields (including unknowns)
		memset(out->pBuffer, 0, out->size);

		//do the transform...
		for(int r = 0; r < entrycount; r++, eq++, emu++)
		{

			eq->GM = emu->gm;
			eq->title = emu->aa_title;
			eq->anon = emu->anon;
			memcpy(eq->name, emu->name, 64);
			eq->deity = emu->deity;
			eq->size = emu->size;
			eq->NPC = emu->NPC;
			eq->invis = emu->invis;
			//eq->sneaking = 0;
			eq->pvp = emu->pvp;
			eq->cur_hp = emu->curHp;
			eq->x_pos = emu->x;
			eq->y_pos = emu->y;
			eq->animation = emu->animation;
			eq->z_pos = emu->z * 10;
			eq->deltaY = 0;
			eq->deltaX = 0;
			eq->deltaHeading = emu->deltaHeading;
			eq->heading = emu->heading;
			eq->deltaZ = 0;
			eq->anim_type = emu->StandState;
			eq->level = emu->level;
			eq->petOwnerId = emu->petOwnerId;
			eq->temporaryPet = emu->temporaryPet;
			eq->guildrank = emu->guildrank;
			if (emu->NPC == 1)
			{
				eq->guildrank = 0;
				eq->LD = 1;
			}

			eq->bodytexture = emu->bodytexture;
			for (int k = 0; k < 9; k++)
			{
				eq->equipment[k] = emu->equipment[k];
				eq->equipcolors.Slot[k].Color = emu->colors.Slot[k].Color;
			}
			eq->runspeed = emu->runspeed;
			eq->AFK = emu->afk;
			eq->GuildID = emu->guildID;
			if (eq->GuildID == 0)
				eq->GuildID = 0xFFFF;
			eq->helm = emu->helm;
			eq->race = emu->race;
			strncpy(eq->Surname, emu->lastName, 32);
			eq->walkspeed = emu->walkspeed;
			eq->light = emu->light;
			if (emu->class_ > 19 && emu->class_ < 35)
				eq->class_ = emu->class_ - 3;
			else if (emu->class_ == 40)
				eq->class_ = 16;
			else if (emu->class_ == 41)
				eq->class_ = 32;
			else
				eq->class_ = emu->class_;
			eq->haircolor = emu->haircolor;
			eq->beardcolor = emu->beardcolor;
			eq->eyecolor1 = emu->eyecolor1;
			eq->eyecolor2 = emu->eyecolor2;
			eq->hairstyle = emu->hairstyle;
			eq->beard = emu->beard;
			eq->face = emu->face;
			eq->gender = emu->gender;
			eq->bodytype = emu->bodytype;
			eq->spawn_id = emu->spawnId;
			eq->flymode = emu->flymode;

		}

		auto outapp = new EQApplicationPacket(OP_ZoneSpawns, 8192);
		outapp->size = DeflatePacket((unsigned char*)out->pBuffer, out->size, outapp->pBuffer, 8192);
		EncryptZoneSpawnPacket(outapp->pBuffer, outapp->size);
		delete in;
		delete out;
		dest->FastQueuePacket(&outapp, ack_req);
	}

	ENCODE(OP_CancelTrade)
	{
		ENCODE_LENGTH_EXACT(CancelTrade_Struct);
		SETUP_DIRECT_ENCODE(CancelTrade_Struct, structs::CancelTrade_Struct);
		OUT(fromid);
		eq->action=1665;
		FINISH_ENCODE();
	}

	ENCODE(OP_ItemLinkResponse) {  ENCODE_FORWARD(OP_ItemPacket); }
	ENCODE(OP_ItemPacket)
	{
		//consume the packet
		EQApplicationPacket *in = *p;
		*p = nullptr;

		//store away the emu struct
		unsigned char *__emu_buffer = in->pBuffer;
		ItemPacket_Struct *old_item_pkt=(ItemPacket_Struct *)__emu_buffer;
		EQ::InternalSerializedItem_Struct *int_struct=(EQ::InternalSerializedItem_Struct *)(old_item_pkt->SerializedItem);

		const EQ::ItemInstance * item = (const EQ::ItemInstance *)int_struct->inst;

		if(item)
		{
			uint8 type = 0;
			if(old_item_pkt->PacketType == ItemPacketViewLink)
				type = 2;

			structs::Item_Struct* evo_item = EvoItem((EQ::ItemInstance*)int_struct->inst,int_struct->slot_id, type);

			if(evo_item == 0)
			{
				delete in;
				return;
			}

			auto outapp = new EQApplicationPacket(OP_ItemPacket,sizeof(structs::Item_Struct));
			memcpy(outapp->pBuffer,evo_item,sizeof(structs::Item_Struct));
			outapp->SetOpcode(OP_Unknown);

			if(old_item_pkt->PacketType == ItemPacketSummonItem)
				outapp->SetOpcode(OP_SummonedItem);
			else if(old_item_pkt->PacketType == ItemPacketViewLink)
				outapp->SetOpcode(OP_ItemLinkResponse);
			else if(old_item_pkt->PacketType == ItemPacketTrade || old_item_pkt->PacketType == ItemPacketMerchant)
				outapp->SetOpcode(OP_MerchantItemPacket);
			else if(old_item_pkt->PacketType == ItemPacketLoot)
				outapp->SetOpcode(OP_LootItemPacket);
			else if(old_item_pkt->PacketType == ItemPacketWorldContainer)
				outapp->SetOpcode(OP_ObjectItemPacket);
			else if(item->GetItem()->ItemClass == EQ::item::ItemClassBag)
				outapp->SetOpcode(OP_ContainerPacket);
			else if(item->GetItem()->ItemClass == EQ::item::ItemClassBook)
				outapp->SetOpcode(OP_BookPacket);
			else if(int_struct->slot_id == EQ::invslot::slotCursor)
				outapp->SetOpcode(OP_SummonedItem);
			else
				outapp->SetOpcode(OP_ItemPacket);

			if(outapp->size != sizeof(structs::Item_Struct))
				Log(Logs::Detail, Logs::ZoneServer, "Invalid size on OP_ItemPacket packet. Expected: %i, Got: %i", sizeof(structs::Item_Struct), outapp->size);

			dest->FastQueuePacket(&outapp);
			delete evo_item;
		}
		delete in;
	}

	ENCODE(OP_TradeItemPacket)
	{
			//consume the packet
		EQApplicationPacket *in = *p;
		*p = nullptr;

		//store away the emu struct
		unsigned char *__emu_buffer = in->pBuffer;
		ItemPacket_Struct *old_item_pkt=(ItemPacket_Struct *)__emu_buffer;
		EQ::InternalSerializedItem_Struct *int_struct=(EQ::InternalSerializedItem_Struct *)(old_item_pkt->SerializedItem);

		const EQ::ItemInstance * item = (const EQ::ItemInstance *)int_struct->inst;

		if(item)
		{
			structs::Item_Struct* evo_item = EvoItem((EQ::ItemInstance*)int_struct->inst,int_struct->slot_id);

			if(evo_item == 0)
			{
				delete in;
				return;
			}

			auto outapp = new EQApplicationPacket(OP_TradeItemPacket,sizeof(structs::TradeItemsPacket_Struct));
			structs::TradeItemsPacket_Struct* myitem = (structs::TradeItemsPacket_Struct*) outapp->pBuffer;
			myitem->fromid = old_item_pkt->fromid;
			myitem->slotid = int_struct->slot_id;
			memcpy(&myitem->item,evo_item,sizeof(structs::Item_Struct));
			dest->FastQueuePacket(&outapp);
			delete evo_item;
		}
		delete in;
	}

	ENCODE(OP_CharInventory)
	{

		//consume the packet
		EQApplicationPacket *in = *p;
		*p = nullptr;

		if(in->size == 0) {
			in->size = 2;
			in->pBuffer = new uchar[in->size];
			*((uint16 *) in->pBuffer) = 0;
			dest->FastQueuePacket(&in);
			return;
		}

		//store away the emu struct
		unsigned char *__emu_buffer = in->pBuffer;

		int16 itemcount = in->size / sizeof(EQ::InternalSerializedItem_Struct);
		if(itemcount == 0 || (in->size % sizeof(EQ::InternalSerializedItem_Struct)) != 0)
		{
			Log(Logs::General, Logs::Netcode, "[STRUCTS] Wrong size on outbound %s: Got %d, expected multiple of %d", opcodes->EmuToName(in->GetOpcode()), in->size, sizeof(EQ::InternalSerializedItem_Struct));
			delete in;
			return;
		}

		EQ::InternalSerializedItem_Struct *eq = (EQ::InternalSerializedItem_Struct *) in->pBuffer;
		//do the transform...
		std::string evo_item_string;
		int r;
		//std::string evo_item_string;
		for(r = 0; r < itemcount; r++, eq++)
		{
			structs::Item_Struct* evo_item = EvoItem((EQ::ItemInstance*)eq->inst,eq->slot_id);

			if(evo_item != 0)
			{
				structs::PlayerItemsPacket_Struct *evoitem = new structs::PlayerItemsPacket_Struct;
				memcpy(&evoitem->item, evo_item, sizeof(structs::Item_Struct));
				EQ::ItemInstance *item = (EQ::ItemInstance*)eq->inst;
				if (item->IsType(EQ::item::ItemClassCommon))
					evoitem->opcode = 16740; // OP_ItemPacket
				else if (item->IsType(EQ::item::ItemClassBag))
					evoitem->opcode = 16742; // OP_ContainerPacket
				else
					evoitem->opcode = 16741; // OP_BookPacket

				char *evo_item_char = reinterpret_cast<char*>(evoitem);
				evo_item_string.append(evo_item_char,sizeof(structs::PlayerItemsPacket_Struct));

				safe_delete(evoitem);
				safe_delete(evo_item);
			}
		}
		int buffer = 2;

		auto outapp = new EQApplicationPacket(OP_CharInventory, 16384);
		outapp->size = buffer + DeflatePacket((uchar*)evo_item_string.c_str(), evo_item_string.length(), &outapp->pBuffer[buffer], 16382);
		outapp->pBuffer[0] = itemcount;

		dest->FastQueuePacket(&outapp);
		delete in;
	}

	ENCODE(OP_ShopInventoryPacket)
	{
		//consume the packet
		EQApplicationPacket *in = *p;
		*p = nullptr;

		//store away the emu struct
		unsigned char *__emu_buffer = in->pBuffer;

		int16 itemcount = in->size / sizeof(EQ::InternalSerializedItem_Struct);
		if(itemcount == 0 || (in->size % sizeof(EQ::InternalSerializedItem_Struct)) != 0)
		{
			Log(Logs::Detail, Logs::ZoneServer, "Wrong size on outbound %s: Got %d, expected multiple of %d", opcodes->EmuToName(in->GetOpcode()), in->size, sizeof(EQ::InternalSerializedItem_Struct));
			delete in;
			return;
		}

		if(itemcount > 80)
			itemcount = 80;

		EQ::InternalSerializedItem_Struct *eq = (EQ::InternalSerializedItem_Struct *) in->pBuffer;
		//do the transform...
		std::string evo_item_string;
		int r = 0;
		for(r = 0; r < itemcount; r++, eq++)
		{
			EQ::ItemInstance *cur = (EQ::ItemInstance*)eq->inst;
			structs::Item_Struct* evo_item = EvoItem((EQ::ItemInstance*)eq->inst,eq->slot_id,1);
			if(evo_item != 0)
			{
				structs::MerchantItemsPacket_Struct* merchant = new structs::MerchantItemsPacket_Struct;
				memset(merchant,0,sizeof(structs::MerchantItemsPacket_Struct));
				memcpy(&merchant->item,evo_item,sizeof(structs::Item_Struct));
				merchant->itemtype = evo_item->ItemClass;

				char *evo_item_char = reinterpret_cast<char*>(merchant);
				evo_item_string.append(evo_item_char,sizeof(structs::MerchantItemsPacket_Struct));
				safe_delete(evo_item);
				safe_delete(merchant);
			}
			safe_delete(cur);
		}

		int buffer = 2;

		auto outapp = new EQApplicationPacket(OP_ShopInventoryPacket, 5000);
		outapp->size = buffer + DeflatePacket((uchar*)evo_item_string.c_str(), evo_item_string.length(), &outapp->pBuffer[buffer], 4998);
		outapp->pBuffer[0] = itemcount;
		dest->FastQueuePacket(&outapp);
		delete in;
	}

	ENCODE(OP_PickPocket)
	{
		if((*p)->size == sizeof(PickPocket_Struct))
		{
			ENCODE_LENGTH_EXACT(PickPocket_Struct);
			SETUP_DIRECT_ENCODE(PickPocket_Struct, structs::PickPocket_Struct);
			OUT(to);
			OUT(from);
			OUT(myskill);
			OUT(type);
			OUT(coin);
			FINISH_ENCODE();
		}
		else
		{
			//consume the packet
			EQApplicationPacket *in = *p;
			*p = nullptr;

			//store away the emu struct
			unsigned char *__emu_buffer = in->pBuffer;
			ItemPacket_Struct *old_item_pkt=(ItemPacket_Struct *)__emu_buffer;
			EQ::InternalSerializedItem_Struct *int_struct=(EQ::InternalSerializedItem_Struct *)(old_item_pkt->SerializedItem);

			const EQ::ItemInstance * item = (const EQ::ItemInstance *)int_struct->inst;

			if(item)
			{
				structs::Item_Struct* evo_item = EvoItem((EQ::ItemInstance*)int_struct->inst,int_struct->slot_id);

				if(evo_item == 0)
				{
					delete in;
					return;
				}

				auto outapp = new EQApplicationPacket(OP_PickPocket,sizeof(structs::PickPocketItemPacket_Struct));
				structs::PickPocketItemPacket_Struct* myitem = (structs::PickPocketItemPacket_Struct*) outapp->pBuffer;
				myitem->from = old_item_pkt->fromid;
				myitem->to = old_item_pkt->toid;
				myitem->myskill = old_item_pkt->skill;
				myitem->coin = 0;
				myitem->type = 5;
				memcpy(&myitem->item,evo_item,sizeof(structs::Item_Struct));

				dest->FastQueuePacket(&outapp);
				delete evo_item;
			}
			delete in;
		}
	}

	DECODE(OP_DeleteCharge) {  DECODE_FORWARD(OP_MoveItem); }
	DECODE(OP_MoveItem)
	{
		SETUP_DIRECT_DECODE(MoveItem_Struct, structs::MoveItem_Struct);

		emu->from_slot = EvoToServerSlot(eq->from_slot);
		emu->to_slot = EvoToServerSlot(eq->to_slot);
		IN(number_in_stack);

		Log(Logs::Detail, Logs::Inventory, "EVO DECODE OUTPUT to_slot: %i, from_slot: %i, number_in_stack: %i", emu->to_slot, emu->from_slot, emu->number_in_stack);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_DeleteCharge) {  ENCODE_FORWARD(OP_MoveItem); }
	ENCODE(OP_MoveItem)
	{
		ENCODE_LENGTH_EXACT(MoveItem_Struct);
		SETUP_DIRECT_ENCODE(MoveItem_Struct, structs::MoveItem_Struct);

		eq->from_slot = ServerToEvoSlot(emu->from_slot);
		eq->to_slot = ServerToEvoSlot(emu->to_slot);
		OUT(to_slot);
		OUT(number_in_stack);
		Log(Logs::Detail, Logs::Inventory, "EVO ENCODE OUTPUT to_slot: %i, from_slot: %i, number_in_stack: %i", eq->to_slot, eq->from_slot, eq->number_in_stack);

		FINISH_ENCODE();
	}

	ENCODE(OP_HPUpdate)
	{
		ENCODE_LENGTH_EXACT(SpawnHPUpdate_Struct);
		SETUP_DIRECT_ENCODE(SpawnHPUpdate_Struct, structs::SpawnHPUpdate_Struct);
		OUT(spawn_id);
		OUT(cur_hp);
		OUT(max_hp);
		FINISH_ENCODE();
	}

	ENCODE(OP_MobHealth)
	{
		ENCODE_LENGTH_EXACT(SpawnHPUpdate_Struct2);
		SETUP_DIRECT_ENCODE(SpawnHPUpdate_Struct2, structs::SpawnHPUpdate_Struct);
		OUT(spawn_id);
		eq->cur_hp=emu->hp;
		eq->max_hp=100;
		FINISH_ENCODE();
	}

	ENCODE(OP_ShopRequest)
	{
		ENCODE_LENGTH_EXACT(Merchant_Click_Struct);
		SETUP_DIRECT_ENCODE(Merchant_Click_Struct, structs::Merchant_Click_Struct);
		eq->npcid=emu->npcid;
		OUT(playerid);
		OUT(command);
		eq->unknown[0] = 0x71;
		eq->unknown[1] = 0x54;
		eq->unknown[2] = 0x00;
		OUT(rate);
		FINISH_ENCODE();
	}

	DECODE(OP_ShopRequest)
	{
		DECODE_LENGTH_EXACT(structs::Merchant_Click_Struct);
		SETUP_DIRECT_DECODE(Merchant_Click_Struct, structs::Merchant_Click_Struct);
		emu->npcid=eq->npcid;
		IN(playerid);
		IN(command);
		IN(rate);
		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_ShopPlayerBuy)
	{
		DECODE_LENGTH_EXACT(structs::Merchant_Sell_Struct);
		SETUP_DIRECT_DECODE(Merchant_Sell_Struct, structs::Merchant_Sell_Struct);
		emu->npcid=eq->npcid;
		IN(playerid);
		emu->itemslot = EvoToServerSlot(eq->itemslot);
		IN(quantity);
		IN(price);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ShopPlayerBuy)
	{
		ENCODE_LENGTH_EXACT(Merchant_Sell_Struct);
		SETUP_DIRECT_ENCODE(Merchant_Sell_Struct, structs::Merchant_Sell_Struct);
		eq->npcid=emu->npcid;
		eq->playerid=emu->playerid;
		eq->itemslot = ServerToEvoSlot(emu->itemslot);
		OUT(quantity);
		OUT(price);
		FINISH_ENCODE();
	}

	DECODE(OP_ShopPlayerSell)
	{
		DECODE_LENGTH_EXACT(structs::Merchant_Purchase_Struct);
		SETUP_DIRECT_DECODE(Merchant_Purchase_Struct, structs::Merchant_Purchase_Struct);
		emu->npcid=eq->npcid;
		//IN(playerid);
		emu->itemslot = EvoToServerSlot(eq->itemslot);
		IN(quantity);
		IN(price);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_ShopPlayerSell)
	{
		ENCODE_LENGTH_EXACT(Merchant_Purchase_Struct);
		SETUP_DIRECT_ENCODE(Merchant_Purchase_Struct, structs::Merchant_Purchase_Struct);
		eq->npcid=emu->npcid;
		//eq->playerid=emu->playerid;
		eq->itemslot = ServerToEvoSlot(emu->itemslot);
		OUT(quantity);
		OUT(price);
		FINISH_ENCODE();
	}

	ENCODE(OP_ShopDelItem)
	{
		ENCODE_LENGTH_EXACT(Merchant_DelItem_Struct);
		SETUP_DIRECT_ENCODE(Merchant_DelItem_Struct, structs::Merchant_DelItem_Struct);
		eq->npcid=emu->npcid;
		OUT(playerid);
		eq->itemslot = ServerToEvoSlot(emu->itemslot);
		if(emu->type == 0)
			eq->type=64;
		else
			OUT(type);
		FINISH_ENCODE();
	}

	ENCODE(OP_AAAction)
	{
		ENCODE_LENGTH_EXACT(UseAA_Struct);
		SETUP_DIRECT_ENCODE(UseAA_Struct, structs::UseAA_Struct);
		OUT(end);
		OUT(ability);
		OUT(begin);
		eq->unknown_void=2154;

		FINISH_ENCODE();
	}

	structs::Item_Struct* EvoItem(const EQ::ItemInstance *inst, int16 slot_id_in, int type)
	{

		if(!inst)
			return 0;

		const EQ::ItemData *item=inst->GetItem();
		int32 serial = inst->GetSerialNumber();

		if(item->ID > 32767)
			return 0;

		structs::Item_Struct *evo_pop_item = new structs::Item_Struct;
		memset(evo_pop_item,0,sizeof(structs::Item_Struct));

		if(item->GMFlag == -1)
			Log(Logs::Detail, Logs::EQMac, "Item %s is flagged for GMs.", item->Name);

		// General items
  		if(type == 0)
  		{
			evo_pop_item->Charges = inst->GetCharges();
  			evo_pop_item->equipSlot = ServerToEvoSlot(slot_id_in);
			if(item->NoDrop == 0)
				evo_pop_item->Price = 0;
			else
				evo_pop_item->Price = item->Price;
			evo_pop_item->SellRate = item->SellRate;
  		}
		// Items on a merchant
  		else if(type == 1)
  		{
  			evo_pop_item->Charges = inst->GetCharges();
  			evo_pop_item->equipSlot = inst->GetMerchantSlot();
			evo_pop_item->Price = inst->GetPrice();  //This handles sellrate, faction, cha, and vendor greed for us.
			evo_pop_item->SellRate = 1;
		}
		// Item links
		else if(type == 2)
		{
			evo_pop_item->Charges = item->MaxCharges;
			evo_pop_item->equipSlot = ServerToEvoSlot(slot_id_in);
			evo_pop_item->Price = item->Price;
			evo_pop_item->SellRate = item->SellRate;
		}

			evo_pop_item->ItemClass = item->ItemClass;
			strcpy(evo_pop_item->Name,item->Name);
			strcpy(evo_pop_item->Lore,item->Lore);
			strcpy(evo_pop_item->IDFile,item->IDFile);
			evo_pop_item->Weight = item->Weight;
			evo_pop_item->NoRent = item->NoRent;
			evo_pop_item->NoDrop = item->NoDrop;
			evo_pop_item->Size = item->Size;
			evo_pop_item->ID = item->ID;
			evo_pop_item->inv_refnum = serial;
			evo_pop_item->Icon = item->Icon;
			evo_pop_item->Slots = item->Slots;
			evo_pop_item->CastTime = item->CastTime;
			evo_pop_item->SkillModType = item->SkillModType;
			evo_pop_item->SkillModValue = item->SkillModValue;
			evo_pop_item->BaneDmgRace = item->BaneDmgRace;
			evo_pop_item->BaneDmgBody = item->BaneDmgBody;
			evo_pop_item->BaneDmgAmt = item->BaneDmgAmt;
			evo_pop_item->RecLevel = item->RecLevel;
			evo_pop_item->RecSkill = item->RecSkill;
			evo_pop_item->ProcRate = item->ProcRate;
			evo_pop_item->ElemDmgType = item->ElemDmgType;
			evo_pop_item->ElemDmgAmt = item->ElemDmgAmt;
			evo_pop_item->FactionMod1 = item->FactionMod1;
			evo_pop_item->FactionMod2 = item->FactionMod2;
			evo_pop_item->FactionMod3 = item->FactionMod3;
			evo_pop_item->FactionMod4 = item->FactionMod4;
			evo_pop_item->FactionAmt1 = item->FactionAmt1;
			evo_pop_item->FactionAmt2 = item->FactionAmt2;
			evo_pop_item->FactionAmt3 = item->FactionAmt3;
			evo_pop_item->FactionAmt4 = item->FactionAmt4;
			evo_pop_item->Deity = item->Deity;
			evo_pop_item->ReqLevel = item->ReqLevel;
			evo_pop_item->BardType = item->BardType;
			evo_pop_item->BardValue = item->BardValue;

			//Focus items did not exist before Planes of Power, so disable them.
			if(item->Focus.Effect < 0)
				evo_pop_item->FocusEffect = 0;
			else if((RuleB(AlKabor, EnableEraItemRules)) && (RuleR(World, CurrentExpansion) < (float)ExpansionEras::LuclinEQEra))
				evo_pop_item->FocusEffect = 0;
			else
				evo_pop_item->FocusEffect = item->Focus.Effect;

			if(item->ItemClass == 1)
			{
				evo_pop_item->container.BagType = item->BagType;
				evo_pop_item->container.BagSlots = item->BagSlots;
				evo_pop_item->container.BagSize = item->BagSize;
				evo_pop_item->container.BagWR = item->BagWR;
				evo_pop_item->container.IsBagOpen = 0;
			}
			else if(item->ItemClass == 2)
			{
				strcpy(evo_pop_item->book.Filename,item->Filename);
				evo_pop_item->book.Book = item->Book;
				evo_pop_item->book.BookType = item->BookType;
			}
			else
			{
			evo_pop_item->common.AStr = item->AStr;
			evo_pop_item->common.ASta = item->ASta;
			evo_pop_item->common.ACha = item->ACha;
			evo_pop_item->common.ADex = item->ADex;
			evo_pop_item->common.AInt = item->AInt;
			evo_pop_item->common.AAgi = item->AAgi;
			evo_pop_item->common.AWis = item->AWis;
			evo_pop_item->common.MR = item->MR;
			evo_pop_item->common.FR = item->FR;
			evo_pop_item->common.CR = item->CR;
			evo_pop_item->common.DR = item->DR;
			evo_pop_item->common.PR = item->PR;
			evo_pop_item->common.HP = item->HP;
			evo_pop_item->common.Mana = item->Mana;
			evo_pop_item->common.AC = item->AC;
			evo_pop_item->common.MaxCharges = item->MaxCharges;
			evo_pop_item->common.GMFlag = item->GMFlag;
			evo_pop_item->common.Light = item->Light;
			evo_pop_item->common.Delay = item->Delay;
			evo_pop_item->common.Damage = item->Damage;
			evo_pop_item->common.Range = item->Range;
			if (item->ItemType == 7) // Throwing
				evo_pop_item->common.ItemType = 19; // casting throwing items to fixed stacking issue.
			else
				evo_pop_item->common.ItemType = item->ItemType;
			evo_pop_item->common.Magic = item->Magic;
			evo_pop_item->common.Material = item->Material;
			evo_pop_item->common.Color = item->Color;
			//evo_pop_item->common.Faction = item->Faction;
			evo_pop_item->common.Classes = item->Classes;
			evo_pop_item->common.Races = item->Races;
			evo_pop_item->common.Stackable = item->Stackable_;
			}

			//FocusEffect and BardEffect is already handled above. Now figure out click, scroll, proc, and worn.

			if(item->Click.Effect > 0)
			{
				evo_pop_item->common.Effect1 = item->Click.Effect;
				evo_pop_item->Effect2 = item->Click.Effect;
				evo_pop_item->EffectType2 = item->Click.Type;
				evo_pop_item->common.EffectType1 = item->Click.Type;
				if(item->Click.Level > 0)
				{
					evo_pop_item->common.EffectLevel1 = item->Click.Level;
					evo_pop_item->EffectLevel2 = item->Click.Level;
				}
				else
				{
					evo_pop_item->common.EffectLevel1 = item->Click.Level2;
					evo_pop_item->EffectLevel2 = item->Click.Level2;
				}
			}
			else if(item->Scroll.Effect > 0)
			{
				evo_pop_item->common.Effect1 = item->Scroll.Effect;
				evo_pop_item->Effect2 = item->Scroll.Effect;
				evo_pop_item->EffectType2 = item->Scroll.Type;
				evo_pop_item->common.EffectType1 = item->Scroll.Type;
				if(item->Scroll.Level > 0)
				{
					evo_pop_item->common.EffectLevel1 = item->Scroll.Level;
					evo_pop_item->EffectLevel2 = item->Scroll.Level;
				}
				else
				{
					evo_pop_item->common.EffectLevel1 = item->Scroll.Level2;
					evo_pop_item->EffectLevel2 = item->Scroll.Level2;
				}
			}
			//We have some worn effect items (Lodizal Shell Shield) as proceffect in db.
			else if(item->Proc.Effect > 0)
			{
				evo_pop_item->common.Effect1 = item->Proc.Effect;
				evo_pop_item->Effect2 = item->Proc.Effect;
				if(item->Worn.Type > 0)
				{
					evo_pop_item->EffectType2 = item->Worn.Type;
					evo_pop_item->common.EffectType1 = item->Worn.Type;
				}
				else
				{
					evo_pop_item->EffectType2 = item->Proc.Type;
					evo_pop_item->common.EffectType1 = item->Proc.Type;
				}
				if(item->Proc.Level > 0)
				{
					evo_pop_item->common.EffectLevel1 = item->Proc.Level;
					evo_pop_item->EffectLevel2 = item->Proc.Level;
				}
				else
				{
					evo_pop_item->common.EffectLevel1 = item->Proc.Level2;
					evo_pop_item->EffectLevel2 = item->Proc.Level2;
				}
			}
			else if(item->Worn.Effect > 0)
			{
				evo_pop_item->common.Effect1 = item->Worn.Effect;
				evo_pop_item->Effect2 = item->Worn.Effect;
				evo_pop_item->EffectType2 = item->Worn.Type;
				evo_pop_item->common.EffectType1 = item->Worn.Type;
				if(item->Worn.Level > 0)
				{
					evo_pop_item->common.EffectLevel1 = item->Worn.Level;
					evo_pop_item->EffectLevel2 = item->Worn.Level;
				}
				else
				{
					evo_pop_item->common.EffectLevel1 = item->Worn.Level2;
					evo_pop_item->EffectLevel2 = item->Worn.Level2;
				}
			}

		return evo_pop_item;
	}

	ENCODE(OP_RaidJoin) { ENCODE_FORWARD(OP_Unknown); }
	ENCODE(OP_Unknown)
	{
		EQApplicationPacket *in = *p;
		*p = nullptr;

		Log(Logs::Detail, Logs::PacketClientServer, "Dropped an invalid packet: %s", opcodes->EmuToName(in->GetOpcode()));

		delete in;
		return;
	}

	DECODE(OP_CharacterCreate)
	{
		DECODE_LENGTH_EXACT(structs::CharCreate_Struct);
		SETUP_DIRECT_DECODE(CharCreate_Struct, structs::CharCreate_Struct);

		IN(gender);
		IN(race);
		IN(class_);
		// FIXME: more logic is needed. ex: evo start_zone 12 should map cabwest for class 11
		emu->start_zone = EvoToServerStartZone(eq->start_zone);
		IN(deity);
		IN(STR);
		IN(STA);
		IN(AGI);
		IN(DEX);
		IN(WIS);
		IN(INT);
		IN(CHA);
		IN(face);
		IN(hairstyle);
		IN(eyecolor1);
		IN(eyecolor2);
		IN(haircolor);
		IN(beard);
		IN(beardcolor);

		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_SendCharInfo)
	{
		ENCODE_LENGTH_EXACT(CharacterSelect_Struct);
		SETUP_DIRECT_ENCODE(CharacterSelect_Struct, structs::CharacterSelect_Struct);

		int r;
		for(r = 0; r < 10; r++)
		{
			OUT_str(name[r]);
			OUT(level[r]);
			OUT(class_[r]);
			OUT(race[r]);
			OUT(zone[r]);
			OUT(gender[r]);
			OUT(face[r]);
			OUT(equip[r]);
			OUT(cs_colors[r]);
			OUT(deity[r]);
			OUT(primary[r]);
			OUT(secondary[r]);
			OUT(haircolor[r]);
			OUT(beardcolor[r]);
			OUT(eyecolor1[r]);
			OUT(eyecolor2[r]);
			OUT(hairstyle[r]);
			OUT(beard[r]);
		}

		FINISH_ENCODE();
	}

	ENCODE(OP_ZoneEntry)
	{
		ENCODE_LENGTH_EXACT(ServerZoneEntry_Struct);
		SETUP_DIRECT_ENCODE(ServerZoneEntry_Struct, structs::ServerZoneEntry_Struct);

		OUT(type);
		OUT_str(name);
			// OUT(unknown0x41);
			// OUT(unknown0x42);
			// OUT(unknown0x43);
			// OUT(unknown0x44);
			// OUT(unknown0x45);
			// OUT(unknown0x46);
			// OUT(unknown0x47);
		// OUT(vz);
		OUT(heading);
		OUT(x_pos);
		// OUT(vy);
		OUT(y_pos);
		OUT(z_pos);
		// OUT(pitch);
		// OUT(accel);
		// OUT(vx);
		// OUT(rate_p);
		// OUT(rate_h);
		// OUT(view_pitch);
			// OUT(unknown0x78);
			// OUT(unknown0x7c);
			// OUT(unknown0x80);
		OUT(LocalInfo);
			// OUT(unknown0x88);
			// OUT(unknown0x89);
			// OUT(unknown0x8a);
			// OUT(unknown0x8b);
		OUT(sneaking);
		OUT(LD);
		// OUT(m_bTemporaryPet); // FIXME:
		// OUT(LFG);
			// OUT(unknown0x90);
			// OUT(unknown0x91); // Something boat / sphere related
			// OUT(unknown0x92);
			// OUT(unknown0x93);
		// OUT(equipment); // FIXME: uint32 to uint16
		for (int k = 0; k < 9; k++)
		{
			eq->equipment[k] = emu->equipment[k];
			eq->equipcolors.Slot[k].Color = emu->equipcolors.Slot[k].Color;
		}

		OUT(equipcolors);
		OUT(zoneID);
			// OUT(animation_vtable);
		OUT(next);
		OUT(My_Char);
			// OUT(unknown0xec);
			// OUT(unknown0xed);
			// OUT(unknown0xee);
			// OUT(unknown0xef);
		OUT(prev);
			// OUT(unknown0xf4);
			// OUT(unknown0xf5);
			// OUT(unknown0xf6);
			// OUT(unknown0xf7);
		OUT_str(Surname);
		OUT(width);
		OUT(length);
		OUT(runspeed);
		OUT(view_height);
		OUT(sprite_oheight);
		OUT(size);
		OUT(walkspeed);
		OUT(NPC);
		OUT(haircolor);
		OUT(beardcolor);
		OUT(eyecolor1);
		OUT(eyecolor2);
		OUT(hairstyle);
		OUT(beard);
		OUT(animation);
		OUT(level);
		OUT(face);
		OUT(gender);
		OUT(pvp);
		OUT(invis);
		OUT(anim_type);
		OUT(class_);
		OUT(light);
		OUT(helm);
		OUT(bodytexture);
		OUT(GM);
			// OUT(unknown0x147);
		OUT(sprite_oheights);
			// OUT(unknown0x14a);
			// OUT(unknown0x14b);
		OUT(petOwnerId);
		OUT(race);
		OUT(avatar);
			// OUT(unknown0x158);
			// OUT(unknown0x159);
			// OUT(unknown0x15a);
			// OUT(unknown0x15b);
		OUT(AFK);
		OUT(bodytype);
		OUT(curHP);
		// OUT(aa_title);
		OUT(guildrank);
		OUT(deity);
		OUT(max_hp);
		OUT(GuildID);
		OUT(flymode);
			// OUT(unknown0x180);
			// OUT(unknown0x181);
			// OUT(unknown0x182);
			// OUT(unknown0x183);
			// OUT(unknown0x184);
			// OUT(unknown0x185);
			// OUT(unknown0x186);
			// OUT(unknown0x187);

		CRC32::SetEQChecksum(__packet->pBuffer, sizeof(structs::ServerZoneEntry_Struct));
		FINISH_ENCODE();
	}

	DECODE(OP_ChannelMessage)
	{
		unsigned char *__eq_buffer = __packet->pBuffer;
		char *InBuffer = (char *)__eq_buffer;
		char Target[64];
		char Sender[64];

		strncpy(Target, InBuffer, sizeof(Target));
		InBuffer += sizeof(Target);
		strncpy(Sender, InBuffer, sizeof(Sender));
		InBuffer += sizeof(Sender);

		uint32 Language = VARSTRUCT_DECODE_TYPE(uint32, InBuffer);
		uint32 Channel = VARSTRUCT_DECODE_TYPE(uint32, InBuffer);
		uint32 CmUnknown4 = VARSTRUCT_DECODE_TYPE(uint32, InBuffer);
		uint32 Skill = VARSTRUCT_DECODE_TYPE(uint32, InBuffer);

		__packet->size = sizeof(ChannelMessage_Struct) + strlen(InBuffer) + 1;
		__packet->pBuffer = new unsigned char[__packet->size];
		ChannelMessage_Struct *emu = (ChannelMessage_Struct *) __packet->pBuffer;

		strn0cpy(emu->targetname, Target, sizeof(emu->targetname));
		strn0cpy(emu->sender, Sender, sizeof(emu->sender));
		emu->language = (uint16)Language;
		emu->chan_num = (uint16)Channel;
		emu->cm_unknown4 = (uint16)CmUnknown4;
		emu->skill_in_language = (uint16)Skill;
		strcpy(emu->message, InBuffer);

		delete [] __eq_buffer;
	}

	ENCODE(OP_ChannelMessage)
	{
		EQApplicationPacket *__packet = *p;
		*p = nullptr;
		unsigned char *__emu_buffer = __packet->pBuffer;
		ChannelMessage_Struct *emu = (ChannelMessage_Struct *) __emu_buffer;

		int msglen = strlen(emu->message) + 1;
		__packet->size = sizeof(structs::ChannelMessage_Struct) + msglen ;
		__packet->pBuffer = new unsigned char[__packet->size];
		structs::ChannelMessage_Struct *eq = (structs::ChannelMessage_Struct *) __packet->pBuffer;
		OUT_str(targetname);
		OUT_str(sender);
		OUT(language);
		OUT(chan_num);
		OUT(cm_unknown4);
		OUT(skill_in_language);
		strn0cpy(eq->message, emu->message, msglen);
		FINISH_ENCODE();
	}

	DECODE(OP_ClientUpdate)
	{
		DECODE_LENGTH_EXACT(structs::SpawnPositionUpdate_Struct);
		SETUP_DIRECT_DECODE(SpawnPositionUpdate_Struct, structs::SpawnPositionUpdate_Struct);

		emu->spawn_id = eq->spawn_id;
		emu->heading = eq->heading * 0.25;
		emu->delta_heading = eq->delta_heading * 0.05;
		emu->anim_type = eq->anim_type;
		emu->y_pos = eq->y_pos * 0.125f;
		emu->x_pos = eq->x_pos * 0.125f;
		emu->z_pos = eq->z_pos * 0.125f;

		// FIXME: all of the delta structs & calcs are completely wrong.
		// SetValue uses 1/16 and * 16
		// Evo might be using 0.015625 and 64
		float delta_y = EQ19toFloat(eq->delta_y) * 0.25;
		float delta_x = EQ19toFloat(eq->delta_x) * 0.25;
		float delta_z = EQ19toFloat(eq->delta_z) * 0.25;
		emu->delta_yzx.SetValue(delta_x, delta_y, delta_z);

		// emu->delta_yzx.SetValue(NewEQ13toFloat(eq->delta_x), NewEQ13toFloat(eq->delta_y), NewEQ13toFloat(eq->delta_z));
		// emu->delta_yzx.SetValue(EQ19toFloat(eq->delta_x), EQ19toFloat(eq->delta_y), EQ19toFloat(eq->delta_z));

		FINISH_DIRECT_DECODE();
	}

	DECODE(OP_TargetMouse)
	{
		DECODE_LENGTH_EXACT(structs::ClientTarget_Struct);
		SETUP_DIRECT_DECODE(ClientTarget_Struct, structs::ClientTarget_Struct);
		// FIXME: common struct needs to be converted to uint
		emu->new_target = eq->new_target;
		FINISH_DIRECT_DECODE();
	}

	// FIXME: not working, no anim when dispatched to pop client
	DECODE(OP_Animation)
	{
		DECODE_LENGTH_EXACT(structs::Animation_Struct);
		SETUP_DIRECT_DECODE(Animation_Struct, structs::Animation_Struct);
		IN(spawnid);
		// IN(animation_speed);
		IN(action);
		FINISH_DIRECT_DECODE();
	}

	// FIXME: not working, #doanim will get stuck
	ENCODE(OP_Animation)
	{
		ENCODE_LENGTH_EXACT(Animation_Struct);
		SETUP_DIRECT_ENCODE(Animation_Struct, structs::Animation_Struct);
		OUT(spawnid);
		// OUT(animation_speed);
		OUT(action);
		FINISH_ENCODE();
	}

	DECODE(OP_WearChange)
	{
		DECODE_LENGTH_EXACT(structs::WearChange_Struct);
		SETUP_DIRECT_DECODE(WearChange_Struct, structs::WearChange_Struct);
		IN(spawn_id);
		IN(material);
		IN(color);
		IN(wear_slot_id);
		FINISH_DIRECT_DECODE();
	}

	ENCODE(OP_WearChange)
	{
		ENCODE_LENGTH_EXACT(WearChange_Struct);
		SETUP_DIRECT_ENCODE(WearChange_Struct, structs::WearChange_Struct);
		OUT(spawn_id);
		OUT(material);
		OUT(color);
		OUT(wear_slot_id);
		FINISH_ENCODE();
	}

	static inline int16 ServerToEvoSlot(uint32 ServerSlot)
	{
			 //int16 EvoSlot;
			if (ServerSlot == INVALID_INDEX)
				 return INVALID_INDEX;

			return ServerSlot; // deprecated
	}

	static inline int16 ServerToEvoCorpseSlot(uint32 ServerCorpse)
	{
		return ServerCorpse;
	}

	static inline uint32 EvoToServerSlot(int16 EvoSlot)
	{
		//uint32 ServerSlot;
		if (EvoSlot == INVALID_INDEX)
			 return INVALID_INDEX;

		return EvoSlot; // deprecated
	}

	static inline uint32 EvoToServerCorpseSlot(int16 EvoCorpse)
	{
		return EvoCorpse;
	}

	static inline int32 EvoToServerStartZone(int32 EvoStartZone)
	{
		switch (EvoStartZone)
		{
			case 0:
				return Zones::erudnext;
			case 1:
				return Zones::qeynos2;
			case 2:
				return Zones::halas;
			case 3:
				return Zones::rivervale;
			case 4:
				return Zones::freportw;
			case 5:
				return Zones::neriaka;
			case 6:
				return Zones::grobb;
			case 7:
				return Zones::oggok;
			case 8:
				return Zones::kaladima;
			case 9:
				return Zones::gfaydark;
			case 10:
				return Zones::felwithea;
			case 11:
				return Zones::akanon;
			case 12:
				return Zones::cabeast;
			case 13:
				return Zones::sharvahl;
		}

		return Zones::qeynos;
	}

} //end namespace Evo

