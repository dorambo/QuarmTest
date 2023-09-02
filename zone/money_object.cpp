/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2003 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "../common/global_define.h"
#include "../common/strings.h"

#include "client.h"
#include "entity.h"
#include "mob.h"
#include "money_object.h"

#include "quest_parser_collection.h"
#include "zonedb.h"
#include "string_ids.h"
#include "queryserv.h"

#include <iostream>

const char DEFAULT_MONEY_OBJECT_NAME[] = "PLAT_ACTORDEF";
const char GOLD_OBJECT_NAME[] = "GOLD_ACTORDEF";
const char SILVER_OBJECT_NAME[] = "SILVER_ACTORDEF";
const char COPPER_OBJECT_NAME[] = "COPPER_ACTORDEF";
const char DEFAULT_MONEY_OBJECT_NAME_SUFFIX[] = "_ACTORDEF";


extern Zone* zone;
extern EntityList entity_list;
extern QueryServ* QServ;

// Loading object from client dropping item on ground
MoneyObject::MoneyObject(Client* client, uint32 copper, uint32 silver, uint32 gold, uint32 platinum)
 : decay_timer(RuleI(Groundspawns, DecayTime))
{
	// Set as much struct data as we can
	memset(&m_data, 0, sizeof(Object_Struct));
	m_data.heading = client->GetHeading();
	m_data.x = client->GetX();
	m_data.y = client->GetY();
	m_data.z = client->GetZ();
	if (zone)
		strncpy(m_data.zone_name, database.GetZoneName(zone->GetZoneID()), 16);

	m_data.pitch = 0.0f;
	m_data.roll = 0.0f;
	m_data.copper = copper;
	m_data.silver = silver;
	m_data.gold = gold;
	m_data.platinum = platinum;

	m_data.money_total = copper + silver + gold + platinum;
	SetObjectNameFromMoney();
}

MoneyObject::~MoneyObject()
{
}

void MoneyObject::SetObjectNameFromMoney()
{
	strcpy(m_data.object_name, DEFAULT_MONEY_OBJECT_NAME);
}

void MoneyObject::SetID(uint16 set_id)
{
	// Invoke base class
	Entity::SetID(set_id);

	// Store new id as drop_id
	m_data.drop_id = (uint32)this->GetID();
}

// Reset state of object back to zero
void MoneyObject::ResetState()
{
	memset(&m_data, 0, sizeof(MoneyObject_Struct));
}

void MoneyObject::CreateSpawnPacket(EQApplicationPacket* app)
{
	app->SetOpcode(OP_MoneySpawn);
	safe_delete_array(app->pBuffer);
	app->pBuffer = new uchar[sizeof(MoneyObject_Struct)];
	app->size = sizeof(MoneyObject_Struct);
	memcpy(app->pBuffer, &m_data, app->size);
}

void MoneyObject::CreateDeSpawnPacket(EQApplicationPacket* app)
{
	app->SetOpcode(OP_ClickMoney);
	safe_delete_array(app->pBuffer);
	app->pBuffer = new uchar[sizeof(ClickObject_Struct)];
	app->size = sizeof(ClickObject_Struct);
	memset(app->pBuffer, 0, app->size);
	ClickObject_Struct* co = (ClickObject_Struct*) app->pBuffer;
	co->drop_id = m_data.drop_id;
	co->player_id = 0;
}

bool MoneyObject::Process(){
	if(decay_timer.Enabled() && decay_timer.Check()) {
		// Send click to all clients (removes entity on client)
		auto outapp = new EQApplicationPacket(OP_ClickObject, sizeof(ClickObject_Struct));
		ClickObject_Struct* click_object = (ClickObject_Struct*)outapp->pBuffer;
		click_object->drop_id = GetID();
		click_object->player_id = 0;
		entity_list.QueueClients(nullptr, outapp, false);
		safe_delete(outapp);
		return false;
	}

	return true;
}

bool MoneyObject::HandleClick(Client* sender, const ClickObject_Struct* click_object)
{
	if (!sender)
		return false;

	sender->AddMoneyToPP(m_data.copper, m_data.silver, m_data.gold, m_data.platinum, false);

	// Send click to all clients (removes entity on client)
	auto outapp = new EQApplicationPacket(OP_ClickMoney, sizeof(ClickObject_Struct));
	memcpy(outapp->pBuffer, click_object, sizeof(ClickObject_Struct));
	entity_list.QueueClients(nullptr, outapp, false);
	safe_delete(outapp);
	entity_list.RemoveEntity(this->GetID());
	
	return true;
}

float MoneyObject::GetX()
{
	return this->m_data.x;
}

float MoneyObject::GetY()
{
	return this->m_data.y;
}


float MoneyObject::GetZ()
{
	return this->m_data.z;
}

float MoneyObject::GetHeadingData()
{
	return this->m_data.heading;
}

void MoneyObject::SetX(float pos)
{
	this->m_data.x = pos;

	EQApplicationPacket app;
	this->CreateDeSpawnPacket(&app);
	entity_list.QueueClients(0, &app);
	safe_delete_array(app.pBuffer);

	this->CreateSpawnPacket(&app);
	entity_list.QueueClients(0, &app);
	safe_delete_array(app.pBuffer);
}

void MoneyObject::SetY(float pos)
{
	this->m_data.y = pos;

	EQApplicationPacket app;

	this->CreateDeSpawnPacket(&app);
	entity_list.QueueClients(0, &app);
	safe_delete_array(app.pBuffer);
	
	this->CreateSpawnPacket(&app);
	entity_list.QueueClients(0, &app);
	safe_delete_array(app.pBuffer);
}

void MoneyObject::Depop()
{
	auto app = new EQApplicationPacket();
	this->CreateDeSpawnPacket(app);
	entity_list.QueueClients(0, app);
	safe_delete(app);
	entity_list.RemoveMoneyObject(this->GetID());
}

void MoneyObject::DepopWithTimer()
{
	
	auto app = new EQApplicationPacket();
	this->CreateDeSpawnPacket(app);
	entity_list.QueueClients(0, app);
	safe_delete(app);
	entity_list.RemoveEntity(this->GetID());
}

void MoneyObject::Repop()
{
	auto app = new EQApplicationPacket();
	auto app2 = new EQApplicationPacket();
	this->CreateDeSpawnPacket(app);
	this->CreateSpawnPacket(app2);
	entity_list.QueueClients(0, app);
	entity_list.QueueClients(0, app2);
	safe_delete(app);
	safe_delete(app2);
}



void MoneyObject::SetZ(float pos)
{
	this->m_data.z = pos;

	auto app = new EQApplicationPacket();
	auto app2 = new EQApplicationPacket();
	this->CreateDeSpawnPacket(app);
	this->CreateSpawnPacket(app2);
	entity_list.QueueClients(0, app);
	entity_list.QueueClients(0, app2);
	safe_delete(app);
	safe_delete(app2);
}

void MoneyObject::SetModelName(const char* modelname)
{
	strn0cpy(m_data.object_name, modelname, sizeof(m_data.object_name)); // 32 is the max for chars in object_name, this should be safe
	auto app = new EQApplicationPacket();
	auto app2 = new EQApplicationPacket();
	this->CreateDeSpawnPacket(app);
	this->CreateSpawnPacket(app2);
	entity_list.QueueClients(0, app);
	entity_list.QueueClients(0, app2);
	safe_delete(app);
	safe_delete(app2);
}

const char* MoneyObject::GetModelName()
{
	return this->m_data.object_name;
}

void MoneyObject::GetObjectData(Object_Struct* Data)
{
	if (Data)
	{
		memcpy(Data, &this->m_data, sizeof(this->m_data));
	}
}

void MoneyObject::SetObjectData(Object_Struct* Data)
{
	if (Data)
	{
		memcpy(&this->m_data, Data, sizeof(this->m_data));
	}
}

void MoneyObject::GetLocation(float* x, float* y, float* z)
{
	if (x)
	{
		*x = this->m_data.x;
	}

	if (y)
	{
		*y = this->m_data.y;
	}

	if (z)
	{
		*z = this->m_data.z;
	}
}

void MoneyObject::SetLocation(float x, float y, float z)
{
	this->m_data.x = x;
	this->m_data.y = y;
	this->m_data.z = z;
	auto app = new EQApplicationPacket();
	auto app2 = new EQApplicationPacket();
	this->CreateDeSpawnPacket(app);
	this->CreateSpawnPacket(app2);
	entity_list.QueueClients(0, app);
	entity_list.QueueClients(0, app2);
	safe_delete(app);
	safe_delete(app2);
}

void MoneyObject::GetHeading(float* heading)
{
	if (heading)
	{
		*heading = this->m_data.heading;
	}
}

void MoneyObject::SetHeading(float heading)
{
	this->m_data.heading = heading;
	auto app = new EQApplicationPacket();
	auto app2 = new EQApplicationPacket();
	this->CreateDeSpawnPacket(app);
	this->CreateSpawnPacket(app2);
	entity_list.QueueClients(0, app);
	entity_list.QueueClients(0, app2);
	safe_delete(app);
	safe_delete(app2);
}