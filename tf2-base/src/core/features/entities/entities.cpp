#include "entities.h"

C_TFPlayer* c_entityhelper::get_local()
{
	if (const auto entity = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer()))
		return entity->As<C_TFPlayer>();

	return nullptr;
}

C_TFWeaponBase* c_entityhelper::get_weapon()
{
	if (const auto local = get_local())
	{
		if (const auto entity = local->m_hActiveWeapon().Get())
			return entity->As<C_TFWeaponBase>();
	}

	return nullptr;
}

void c_entityhelper::updatecache()
{
	if (const auto local = get_local())
	{
		int nLocalTeam = 0;

		if (!local->IsInValidTeam(&nLocalTeam))
			return;

		for (int n = 1; n < I::ClientEntityList->GetHighestEntityIndex(); n++)
		{
			IClientEntity* cliententity = I::ClientEntityList->GetClientEntity(n);

			if (!cliententity || cliententity->IsDormant())
				continue;

			auto entity = cliententity->As<C_BaseEntity>();

			switch (entity->GetClassId())
			{
			case ETFClassIds::CTFPlayer:
			{
				int nPlayerTeam = 0;

				const auto pPlayer = entity->As<C_TFPlayer>();
				if (pPlayer->deadflag() && pPlayer->m_iObserverMode() != OBS_MODE_NONE)
				{
					m_mapGroups[EEntGroup::PLAYERS_OBSERVER].push_back(entity);
				}

				if (!entity->IsInValidTeam(&nPlayerTeam))
					continue;

				m_mapGroups[EEntGroup::PLAYERS_ALL].push_back(entity);
				m_mapGroups[nLocalTeam != nPlayerTeam ? EEntGroup::PLAYERS_ENEMIES : EEntGroup::PLAYERS_TEAMMATES].push_back(entity);

				break;
			}

			case ETFClassIds::CObjectSentrygun:
			case ETFClassIds::CObjectDispenser:
			case ETFClassIds::CObjectTeleporter:
			{
				int nObjectTeam = 0;

				if (!entity->IsInValidTeam(&nObjectTeam))
					continue;

				m_mapGroups[EEntGroup::BUILDINGS_ALL].push_back(entity);
				m_mapGroups[nLocalTeam != nObjectTeam ? EEntGroup::BUILDINGS_ENEMIES : EEntGroup::BUILDINGS_TEAMMATES].push_back(entity);

				break;
			}

			case ETFClassIds::CTFProjectile_Rocket:
			case ETFClassIds::CTFProjectile_SentryRocket:
			case ETFClassIds::CTFProjectile_Jar:
			case ETFClassIds::CTFProjectile_JarGas:
			case ETFClassIds::CTFProjectile_JarMilk:
			case ETFClassIds::CTFProjectile_Arrow:
			case ETFClassIds::CTFProjectile_Flare:
			case ETFClassIds::CTFProjectile_Cleaver:
			case ETFClassIds::CTFProjectile_HealingBolt:
			case ETFClassIds::CTFGrenadePipebombProjectile:
			case ETFClassIds::CTFProjectile_BallOfFire:
			case ETFClassIds::CTFProjectile_EnergyRing:
			case ETFClassIds::CTFProjectile_EnergyBall:
			{
				int nProjectileTeam = 0;

				if (!entity->IsInValidTeam(&nProjectileTeam))
					continue;

				if (entity->GetClassId() == ETFClassIds::CTFGrenadePipebombProjectile)
				{
					const auto bomb = entity->As<C_TFGrenadePipebombProjectile>();

					if (bomb->HasStickyEffects() && bomb->As<C_BaseGrenade>()->m_hThrower().Get() == local)
						m_mapGroups[EEntGroup::PROJECTILES_LOCAL_STICKIES].push_back(entity);
				}

				m_mapGroups[EEntGroup::PROJECTILES_ALL].push_back(entity);
				m_mapGroups[nLocalTeam != nProjectileTeam ? EEntGroup::PROJECTILES_ENEMIES : EEntGroup::PROJECTILES_TEAMMATES].push_back(entity);

				break;
			}

			case ETFClassIds::CBaseAnimating:
			{
				if (ishp(entity))
					m_mapGroups[EEntGroup::HEALTHPACKS].push_back(entity);

				if (isap(entity))
					m_mapGroups[EEntGroup::AMMOPACKS].push_back(entity);

				break;
			}

			case ETFClassIds::CTFAmmoPack:
			{
				m_mapGroups[EEntGroup::AMMOPACKS].push_back(entity);
				break;
			}

			case ETFClassIds::CHalloweenGiftPickup:
			{
				m_mapGroups[EEntGroup::HALLOWEEN_GIFT].push_back(entity);

				break;
			}

			case ETFClassIds::CCurrencyPack:
			{
				if (entity->As<C_CurrencyPack>()->m_bDistributed())
				{
					continue;
				}

				m_mapGroups[EEntGroup::MVM_MONEY].push_back(entity);

				break;
			}

			default: break;
			}
		}
	}
}

void c_entityhelper::updatemodelindex()
{
	m_mapHealthPacks.clear();
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_small.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_medium.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_large.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_halloween/halloween_medkit_small.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_halloween/halloween_medkit_medium.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_halloween/halloween_medkit_large.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_small_bday.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_medium_bday.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/medkit_large_bday.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/props_medieval/medieval_meat.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/plate.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/plate_sandwich_xmas.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/plate_robo_sandwich.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/workshop/weapons/c_models/c_fishcake/plate_fishcake.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/workshop/weapons/c_models/c_buffalo_steak/plate_buffalo_steak.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/workshop/weapons/c_models/c_chocolate/plate_chocolate.mdl")] = true;
	m_mapHealthPacks[I::ModelInfoClient->GetModelIndex("models/items/banana/plate_banana.mdl")] = true;

	m_mapAmmoPacks.clear();
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_small.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_medium.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_large.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_small_bday.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_medium_bday.mdl")] = true;
	m_mapAmmoPacks[I::ModelInfoClient->GetModelIndex("models/items/ammopack_large_bday.mdl")] = true;
}

void c_entityhelper::clearcache()
{
	for (auto& group : m_mapGroups | std::views::values)
	{
		group.clear();
	}
}
