#pragma once

#include "../../../sdk/tf2/c_tf_player.h"

#include <map>
#include <ranges>

enum class EEntGroup
{
	PLAYERS_ALL,
	PLAYERS_ENEMIES,
	PLAYERS_TEAMMATES,
	PLAYERS_OBSERVER,

	BUILDINGS_ALL,
	BUILDINGS_ENEMIES,
	BUILDINGS_TEAMMATES,

	PROJECTILES_ALL,
	PROJECTILES_ENEMIES,
	PROJECTILES_TEAMMATES,
	PROJECTILES_LOCAL_STICKIES,

	HEALTHPACKS,
	AMMOPACKS,
	HALLOWEEN_GIFT,
	MVM_MONEY
};

class c_entityhelper
{
public:
	C_TFPlayer* get_local();
	C_TFWeaponBase* get_weapon();

	std::map<int, bool> m_mapHealthPacks = {};
	std::map<int, bool> m_mapAmmoPacks = {};

	bool ishp(C_BaseEntity* pEntity)
	{
		return m_mapHealthPacks.contains(pEntity->m_nModelIndex());
	}

	bool isap(C_BaseEntity* pEntity)
	{
		return m_mapAmmoPacks.contains(pEntity->m_nModelIndex());
	}

	void updatecache();
	void updatemodelindex();
	void clearcache();

	void clearmodelindex()
	{
		m_mapHealthPacks.clear();
		m_mapAmmoPacks.clear();
	}

	std::map<EEntGroup, std::vector<C_BaseEntity*>> m_mapGroups = {};
    std::vector<C_BaseEntity*>& get_group(const EEntGroup group) { return m_mapGroups[group]; }
};

MAKE_SINGLETON_SCOPED(c_entityhelper, entities, f);
