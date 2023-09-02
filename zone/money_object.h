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
#ifndef MONEYOBJECT_H
#define MONEYOBJECT_H

// Object Class:
// Represents Zone Money Objects

#include "../common/eq_packet_structs.h"
#include "../common/timer.h"
#include "../common/types.h"
#include "client.h"
#include "entity.h"

class Client;
class EQApplicationPacket;


class MoneyObject : public Entity
{
public:
	// Loading object from client dropping item on ground
	MoneyObject(Client* client, uint32 copper, uint32 silver, uint32 gold, uint32 platinum);
	// Destructor
	~MoneyObject();
	void SetObjectNameFromMoney();
	bool Process();

	// Event handlers
	bool HandleClick(Client* sender, const ClickObject_Struct* click_object);

	// Packet functions
	void CreateSpawnPacket(EQApplicationPacket* app);
	void CreateDeSpawnPacket(EQApplicationPacket* app);
	void Depop();
	void DepopWithTimer();
	void Repop();

	//Decay functions
	void StartDecay() {decay_timer.Start();}

	// Override base class implementations
	virtual bool IsMoneyObject()	const { return true; }
	virtual void SetID(uint16 set_id);
	void GetObjectData(Object_Struct* Data);
	void SetObjectData(Object_Struct* Data);
	void GetLocation(float* x, float* y, float* z);
	void SetLocation(float x, float y, float z);
	void GetHeading(float* heading);
	void SetHeading(float heading);
	float GetX();
	float GetY();
	float GetZ();
	float GetHeadingData();
	void SetX(float pos);
	void SetY(float pos);
	void SetZ(float pos);
	void SetModelName(const char* modelname);
	const char* GetModelName();

	bool IsPlayerDrop() const { return true; }

protected:
	void	ResetState();	// Set state back to original

	MoneyObject_Struct	m_data;		// Packet data

	Timer decay_timer;
};

#endif
