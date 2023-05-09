//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBileBombGun.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBileBombGun.cpp,v $
// Revision 1.9  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.8  2002/10/16 00:50:04  Flayra
// - Updated effects to get rid of cheesy effect and use nice particle effect
//
// Revision 1.7  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.6  2002/08/16 02:33:12  Flayra
// - Added damage types
//
// Revision 1.5  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.4  2002/07/24 18:55:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.3  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.2  2002/07/01 21:18:01  Flayra
// - Implemented bile bomb
//
// Revision 1.1  2002/06/25 17:23:53  Flayra
// - Level 3 bomb attack
//
//===============================================================================
#include "AvHAlienWeapons.h"
#include "AvHPlayer.h"
#include "AvHMarineEquipment.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#include "../common/hldm.h"
#include "../common/event_api.h"
#include "../common/event_args.h"
#include "../common/vector_util.h"
#include "AvHAlienWeaponConstants.h"
#include "AvHPlayerUpgrade.h"
#include "AvHConstants.h"
#include "AvHServerUtil.h"
#include "AvHParticleConstants.h"
#include "AvHServerVariables.h"

LINK_ENTITY_TO_CLASS(kwBileBombGun, AvHBileBombGun);
void V_PunchAxis( int axis, float punch );


#ifdef AVH_SERVER

LINK_ENTITY_TO_CLASS(kwBileBomb, AvHBileBomb);

#include "AvHGamerules.h"

void AvHBileBomb::SetDamage(float inDamage)
{
	this->mDamage = inDamage;
}

void AvHBileBomb::Precache()
{
	CBaseEntity::Precache();
	//this->mExplodeSprite = PRECACHE_MODEL("sprites/lgtning.spr");
}

void AvHBileBomb::Spawn()
{
	this->Precache();
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_TOSS;
	//this->pev->movetype = MOVETYPE_FLY;//MOVETYPE_BOUNCE;
	this->pev->classname = MAKE_STRING(kwsBileBomb);
	
	this->pev->solid = SOLID_BBOX;
	this->mDamage = 0.0f;

	SET_MODEL(ENT(this->pev), kBileBombProjectileModel);


	// Comment out effects line, uncomment next four, then comment out creation of temp entity in EV_AcidGun to see server side Acid for testing
	if(!GetGameRules()->GetDrawInvisibleEntities())
	{
		this->pev->effects = EF_NODRAW;
	}
	else
	{
		this->pev->frame = 0;
		this->pev->scale = 0.2;
		this->pev->rendermode = kRenderTransAlpha;
		this->pev->renderamt = 255;
	}

	//UTIL_SetSize(this->pev, Vector( 0, 0, 0), Vector(0, 0, 0));
	UTIL_SetSize(this->pev, Vector( -kBileBombSize, -kBileBombSize, -kBileBombSize), Vector(kBileBombSize, kBileBombSize, kBileBombSize));


	// Must use default gravity, so event syncs up (event flags only allow default gravity, or half that)
//	this->pev->gravity = 1.0f;
	this->pev->friction = kBileBombFrictionConstant;
	this->pev->angles = g_vecZero;

	SetTouch(&AvHBileBomb::BileBombTouch);
}

void AvHBileBomb::BileBombTouch(CBaseEntity* pOther)
{
	CBaseEntity* theHitEntity = CBaseEntity::Instance(ENT(this->pev->owner));
	if(pOther != theHitEntity)
	{
		//EMIT_SOUND(this->edict(), CHAN_WEAPON, kBileBombHitSound, 1.0f, ATTN_IDLE);
	
		// Explode with splash damage (also change in GetDamageType() above)
		int theRadius = BALANCE_VAR(kBileBombRadius);
		RadiusDamage(this->pev->origin, this->pev, VARS(this->pev->owner), this->mDamage, theRadius, CLASS_NONE, NS_DMG_ACID);

		//clear webs and etc now
		// Scan area for webs, and clear them.  I can't make the webs solid, and it seems like the welder might do this, so why not?  Also
		// adds neat element of specialization where a guy with a welder might be needed to clear an area before an attack, kinda RPS

		//perhaps make it so u can clear enemy team webs with bile bomb in AvAs
		const float kWebClearingRadius = theRadius;
		CBaseEntity* thePotentialWebStrand = NULL;
		while ((thePotentialWebStrand = UTIL_FindEntityInSphere(thePotentialWebStrand, this->pev->origin, kWebClearingRadius)) != NULL)
		{
			AvHWebStrand* theWebStrand = dynamic_cast<AvHWebStrand*>(thePotentialWebStrand);
			if (theWebStrand)
			{
				//theBileBomb->pev->team = this->m_pPlayer->pev->team;
				//only destroy enemy webs
				if (theWebStrand->pev->team != this->pev->team)
				{
					//check web's team
					theWebStrand->Break();
				}

			}
		}

		
		const float kWelderClearingRadius = 30;
		CBaseEntity* thePotentialWelder = NULL;
		while ((thePotentialWelder = UTIL_FindEntityInSphere(thePotentialWelder, this->pev->origin, kWelderClearingRadius)) != NULL)
		{
			AvHPlayerEquipment* thePack = dynamic_cast<AvHPlayerEquipment*>(thePotentialWelder);
			AvHMarineWeapon* theWelder = dynamic_cast<AvHMarineWeapon*>(thePotentialWelder);
			if (theWelder)
			{
				

				if (theWelder->m_pPlayer)
				{
					ALERT(at_console, "WEAPON CANT BE DESTROYED \n");
				}
				else {
					ALERT(at_console, "WEAPON DESTROY PROC \n");


					if (theWelder->pev->nextthink > gpGlobals->time + 1.0f) {
						theWelder->pev->nextthink = max(theWelder->pev->nextthink - 5.0f, gpGlobals->time);
					}
					else
					{
						theWelder->Kill();
					}


					//theWelder->Kill();
					// /*
					//int theLifetime = theWelder->GetLifetime() - 5;
					//if (theLifetime > 0)
					//{
						//SetThink(&AvHMarineWeapon::SUB_Remove);
						
					//}
					// */
				}
				//theWelder->SetGroundLifetime(0.5f);
			}
			else if(thePack)
			{

				if (thePack->m_pPlayer)
				{
					ALERT(at_console, "ITEM CANT BE DESTROYED \n");
				}
				else {
					ALERT(at_console, "ITEM DESTROY PROC \n");
					//thePack->Kill();

					if (thePack->pev->nextthink > gpGlobals->time + 1.0f) {
						thePack->pev->nextthink = max(thePack->pev->nextthink - 5.0f, gpGlobals->time);
					}
					else
					{
						AvHJetpack* theJP = dynamic_cast<AvHJetpack*>(thePotentialWelder);
						AvHHeavyArmor* theHA = dynamic_cast<AvHHeavyArmor*>(thePotentialWelder);
						if (theJP || theHA)
						{
						}
						else
						{
							thePack->Kill();
						}
					}

				}


				

				
				//theWelder->SetGroundLifetime(0.5f);
			}
		}



		SetTouch(NULL);

		UTIL_Remove(this);
	}
}
#endif



void AvHBileBombGun::Init()
{
	this->mRange = kBileBombRange;
	this->mDamage = BALANCE_VAR(kBileBombDamage);
	//this->mFramesSinceMoreAmmo = 0;
}

void AvHBileBombGun::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	
	// Spawn AcidRocket
	AvHBileBomb* theBileBomb = GetClassPtr((AvHBileBomb*)NULL );
	theBileBomb->Spawn();
	
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);

	Vector vecAiming = gpGlobals->v_forward;// + gpGlobals->v_up;
	VectorNormalize(vecAiming);

	Vector vecSrc;
	//VectorMA(this->pev->origin, this->GetBarrelLength(), vecAiming, vecSrc);
	this->GetEventOrigin(vecSrc);
	
	UTIL_SetOrigin(theBileBomb->pev, vecSrc);

	// This needs to be the same as in EV_BileBombGun
	//VectorMA(this->m_pPlayer->pev->velocity, kBileBombVelocity, vecAiming, theBileBomb->pev->velocity);
	VectorMA(Vector(0, 0, 0), kBileBombVelocity, vecAiming, theBileBomb->pev->velocity);

	// Set owner
	theBileBomb->pev->owner = ENT(this->m_pPlayer->pev);
	theBileBomb->pev->gravity = 0.0f;

	// Set BileBomb's team :)
	theBileBomb->pev->team = this->m_pPlayer->pev->team;

	// Set amount of damage it will do
	float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(this->m_pPlayer->pev->iuser4);
	theBileBomb->SetDamage(theDamage);
	#endif	
}

int	AvHBileBombGun::GetBarrelLength() const
{
	return kBileBombBarrelLength;
}

float AvHBileBombGun::GetRateOfFire() const
{
	return BALANCE_VAR(kBileBombROF);
}

int	AvHBileBombGun::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = -1;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
		// Healing spray and web look the same
	case AVH_WEAPON_HEALINGSPRAY:
	case AVH_WEAPON_WEBSPINNER:
		theDeployAnimation = 7;
		break;
	}
	
	return theDeployAnimation;
}

int	AvHBileBombGun::GetShootAnimation() const
{
	return 3;
}

// Also change in RadiusDamage below
int AvHBileBombGun::GetDamageType() const
{
	return DMG_IGNITE;// NS_DMG_ACID;//NS_DMG_STRUCTURAL;
}

char* AvHBileBombGun::GetViewModel() const
{
	return kLevel2ViewModel;
}

void AvHBileBombGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_MODEL(kBileBombProjectileModel);
	PRECACHE_UNMODIFIED_SOUND(kBileBombFireSound);
	PRECACHE_UNMODIFIED_SOUND(kBileBombHitSound);
	this->mEvent = PRECACHE_EVENT(1, kBileBombShootEventName);
}


void AvHBileBombGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_BILEBOMB;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsBileBombGun);

	SET_MODEL(ENT(this->pev), kNullModel);

#ifdef AVH_SERVER
	if (avh_balance_ava.value == 1)
	{
		this->mDamage = 370; //75% bonus damage
	}
#endif

	FallInit();// get ready to fall down.
} 

