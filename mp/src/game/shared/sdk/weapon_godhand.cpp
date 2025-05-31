//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//


#include "cbase.h"
#include "weapon_sdkbase.h"
#include "sdk_weapon_melee.h"
#include "igamemovement.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
#define CWeaponGodhand C_WeaponGodhand
#include "c_sdk_player.h"
#else
#include "sdk_player.h"
#include "ilagcompensationmanager.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CMoveData* g_pMoveData;

//extern ConVar sk_godhand_charge_time;

//-----------------------------------------------------------------------------
// C_GodHand
//-----------------------------------------------------------------------------
class CWeaponGodhand : public CWeaponSDKMelee
{
public:
	DECLARE_CLASS(CWeaponGodhand, CWeaponSDKMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();


	void			Precache();
	void			ItemPostFrame();
	void			Swing();
	void            HandleAttackMovement();
	void            PrimaryAttack();
	//Activity		GetPrimaryAttackActivity();


	CWeaponGodhand() {
		m_bIsHoldingPrimaryAttack = false;
		AddEffects(EF_NODRAW);
	}

private:
	bool m_bIsHoldingPrimaryAttack;
	CWeaponGodhand(const CWeaponGodhand&);
};


IMPLEMENT_NETWORKCLASS_ALIASED(WeaponGodhand, DT_GodHand)

BEGIN_NETWORK_TABLE(CWeaponGodhand, DT_GodHand)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponGodhand)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_godhand, CWeaponGodhand);
PRECACHE_WEAPON_REGISTER(weapon_godhand);

acttable_t CWeaponGodhand::m_acttable[] =
{
	{ ACT_DA_STAND_IDLE,				ACT_DA_STAND_IDLE,				false },
	{ ACT_DA_WALK_IDLE,				ACT_DA_WALK_IDLE,				false },
	{ ACT_DA_RUN_IDLE,					ACT_DA_RUN_IDLE,				false },
	{ ACT_DA_CROUCH_IDLE,				ACT_DA_CROUCH_IDLE,			false },
	{ ACT_DA_CROUCHWALK_IDLE,			ACT_DA_CROUCHWALK_IDLE,		false },
	{ ACT_DA_STAND_READY,              ACT_DA_STAND_IDLE,             false },
	{ ACT_DA_WALK_READY,               ACT_DA_WALK_IDLE,              false },
	{ ACT_DA_RUN_READY,                ACT_DA_RUN_IDLE,               false },
	{ ACT_DA_CROUCH_READY,             ACT_DA_CROUCH_IDLE,            false },
	{ ACT_DA_CROUCHWALK_READY,         ACT_DA_CROUCHWALK_IDLE,        false },
	{ ACT_DA_PRONECHEST_IDLE,			ACT_DA_PRONECHEST_IDLE,		false },
	{ ACT_DA_PRONEBACK_IDLE,			ACT_DA_PRONEBACK_IDLE,			false },
	{ ACT_DA_PRIMARYATTACK,			ACT_DA_PRIMARYATTACK,			false },
	{ ACT_DA_PRIMARYATTACK_CROUCH,		ACT_DA_PRIMARYATTACK_CROUCH,	false },
	{ ACT_DA_PRIMARYATTACK_PRONE,		ACT_DA_PRIMARYATTACK_PRONE,	false },
	{ ACT_DA_PRIMARYATTACK_SLIDE,		ACT_DA_PRIMARYATTACK_SLIDE,	false },
	{ ACT_DA_PRIMARYATTACK_DIVE,		ACT_DA_PRIMARYATTACK_DIVE,		false },
	{ ACT_DA_PRIMARYATTACK_ROLL,		ACT_DA_PRIMARYATTACK_ROLL,		false },
	{ ACT_DA_RELOAD,					ACT_DA_RELOAD,					false },
	{ ACT_DA_RELOAD_CROUCH,			ACT_DA_RELOAD_CROUCH,			false },
	{ ACT_DA_RELOAD_PRONE,				ACT_DA_RELOAD_PRONE,			false },
	{ ACT_DA_RELOAD_SLIDE,				ACT_DA_RELOAD_SLIDE,			false },
	{ ACT_DA_BRAWL,                    ACT_DA_BRAWL,                  false },
	{ ACT_DA_BRAWL_CROUCH,             ACT_DA_BRAWL_CROUCH,           false },
	{ ACT_DA_BRAWL_PRONE,              ACT_DA_BRAWL_PRONE,            false },
	{ ACT_DA_BRAWL_SLIDE,              ACT_DA_BRAWL_SLIDE,            false },
	{ ACT_DA_BRAWL_DIVE,               ACT_DA_BRAWL_DIVE,             false },
	{ ACT_DA_BRAWL_ROLL,               ACT_DA_BRAWL_ROLL,             false },
	{ ACT_DA_JUMP_START,				ACT_DA_JUMP_START,				false },
	{ ACT_DA_JUMP_FLOAT,				ACT_DA_JUMP_FLOAT,				false },
	{ ACT_DA_JUMP_LAND,				ACT_DA_JUMP_LAND,				false },
	{ ACT_DA_DIVE,						ACT_DA_DIVE,					false },
	{ ACT_DA_DIVEFALL,					ACT_DA_DIVEFALL,				false },
	{ ACT_DA_DIVEROLL,					ACT_DA_DIVEROLL,				false },
	{ ACT_DA_ROLL,						ACT_DA_ROLL,					false },
	{ ACT_DA_SLIDESTART,				ACT_DA_SLIDESTART,				false },
	{ ACT_DA_SLIDE,					ACT_DA_SLIDE,					false },
	{ ACT_DA_DIVESLIDE,                ACT_DA_DIVESLIDE,              false },
	{ ACT_DA_PRONE_TO_STAND,           ACT_DA_PRONE_TO_STAND,         false },
	{ ACT_DA_THROW_GRENADE,            ACT_DA_THROW_GRENADE,          false },
	{ ACT_DA_WALLFLIP,                 ACT_DA_WALLFLIP,               false },
};

IMPLEMENT_ACTTABLE(CWeaponGodhand);

void CWeaponGodhand::Precache() {
	BaseClass::Precache();
}

void CWeaponGodhand::PrimaryAttack() {
	if (!m_bIsHoldingPrimaryAttack) {
		BaseClass::Swing();
		HandleAttackMovement();
	}
}

void CWeaponGodhand::ItemPostFrame(void)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	if (m_bIsHoldingPrimaryAttack)
	{
		if (pOwner->m_afButtonPressed & IN_USE)
		{
			m_bIsHoldingPrimaryAttack = false;
		}
	}
	else
	{
		if (pOwner->m_afButtonPressed & IN_USE)
		{
			m_bIsHoldingPrimaryAttack = true;
		}
	}

	BaseClass::ItemPostFrame();
}

void CWeaponGodhand::HandleAttackMovement() {
	CSDKPlayer* pOwner = ToSDKPlayer(GetOwner());

	if (pOwner == NULL)
		return;

	//Vector forward;
	//pOwner->EyeVectors(&forward);

	g_pMoveData->m_vecVelocity = pOwner->m_Shared.StartDiving();
	pOwner->SetGravity(0.2);
}