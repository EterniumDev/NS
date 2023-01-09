//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHGrenadeGun.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHGrenadeGun.cpp,v $
// Revision 1.17  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.16  2002/10/16 20:53:09  Flayra
// - Removed weapon upgrade sounds
//
// Revision 1.15  2002/10/03 18:46:26  Flayra
// - Added heavy view model
//
// Revision 1.14  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.13  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.12  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.11  2002/06/03 16:37:56  Flayra
// - Added different deploy times (this should be refactored a bit more), refactored grenades
//
// Revision 1.10  2002/05/28 17:44:58  Flayra
// - Tweak weapon deploy volume, as Valve's sounds weren't normalized
//
// Revision 1.9  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "AvHPlayer.h"

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
#include "AvHMarineWeapons.h"
#include "AvHMarineWeaponConstants.h"
#include "AvHServerUtil.h"
#include "AvHPlayerUpgrade.h"

LINK_ENTITY_TO_CLASS(kwGrenadeGun, AvHGrenadeGun);
void V_PunchAxis( int axis, float punch );

void AvHGrenadeGun::Init()
{
	this->mRange = kGGRange;
	this->mDamage = BALANCE_VAR(kGrenadeDamage);
}

int	AvHGrenadeGun::GetBarrelLength() const
{
	return kGGBarrelLength;
}

float AvHGrenadeGun::GetRateOfFire() const
{
	return BALANCE_VAR(kGGROF);
}

int	AvHGrenadeGun::GetDeployAnimation() const
{
	int theAnimation = -1;
	
	int theShotsInClip = this->GetShotsInClip();
	
	switch(theShotsInClip)
	{
	case 4:
	case 0:
		theAnimation = 13;
		break;
	case 3:
		theAnimation = 14;
		break;
	case 2:
		theAnimation = 15;
		break;
	case 1:
		theAnimation = 16;
		break;
	}
	
	return theAnimation;
}

char* AvHGrenadeGun::GetDeploySound() const
{
	return kGGDeploySound;
}

float AvHGrenadeGun::GetDeployTime() const
{
	int theUser4 = this->m_pPlayer->pev->iuser4;

	// Speed attack if in range of primal scream
	if (GetHasUpgrade(theUser4, MASK_BUFFED))
	{
		return 0.2f;
	}
	else {
		return 0.3f;
	}
}

float AvHGrenadeGun::GetReloadTime(void) const
{
	int theShotsToLoad = BALANCE_VAR(kGGMaxClip) - this->GetShotsInClip();

	float theBaseReloadTime = BALANCE_VAR(kGrenadeLauncherBaseReloadTime);
	float theGrenadeReloadTime = BALANCE_VAR(kGrenadeLauncherGrenadeReloadTime);
	float theEndReloadTime = BALANCE_VAR(kGrenadeLauncherEndReloadTime);

	int theUser4 = this->m_pPlayer->pev->iuser4;

	
	//fix by alien for grenade launcher reload time to take into account the number of nades in your ammo reserves
	// Translate iAmount, szName and iMax into the ammo for our current weapon
	int theSpareShots = GetPrimaryAmmoAmount();
	

	if (theShotsToLoad > theSpareShots) {
		theShotsToLoad = theSpareShots;
	}

	//catpack now reduces grenadegun reload time
	if (GetHasUpgrade(theUser4, MASK_BUFFED))
	{
		return (theBaseReloadTime + theShotsToLoad * theGrenadeReloadTime + theEndReloadTime)*0.5f;
	}
	else {
		return theBaseReloadTime + theShotsToLoad * theGrenadeReloadTime + theEndReloadTime;
	}


	//return theBaseReloadTime + theShotsToLoad*theGrenadeReloadTime + theEndReloadTime;
}

bool AvHGrenadeGun::GetHasMuzzleFlash() const
{
	return true;
}

void AvHGrenadeGun::GetEventOrigin(Vector& outOrigin) const
{
	Vector theGunPosition = this->m_pPlayer->GetGunPosition();
	VectorCopy(theGunPosition, outOrigin);
}

void AvHGrenadeGun::GetEventAngles(Vector& outAngles) const
{
	float theGrenadeForce = BALANCE_VAR(kGrenadeForce);

	Vector theAiming = this->m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	Vector theVelocity = theAiming*theGrenadeForce + this->m_pPlayer->pev->velocity;

	VectorCopy(theVelocity, outAngles);
}

char* AvHGrenadeGun::GetHeavyViewModel() const
{
	return kGGHVVModel;
}

int	AvHGrenadeGun::GetIdleAnimation() const
{
	int theAnimation = -1;

	int theShotsInClip = this->GetShotsInClip();

	switch(theShotsInClip)
	{
	case 0:
	case 4:
		theAnimation = 0;
		break;

	case 1:
		theAnimation = 3;
		break;

	case 2:
		theAnimation = 2;
		break;

	case 3:
		theAnimation = 1;
		break;
	}

	return theAnimation;
}

char* AvHGrenadeGun::GetPlayerModel() const
{
	return kGGPModel;
}

int	AvHGrenadeGun::GetReloadAnimation() const
{
	int theAnimation = -1;
	
	int theShotsInClip = this->GetShotsInClip();
	int theSpareShots = GetPrimaryAmmoAmount();

	//if the number of missing shots is less than the ammo available
	//this is not a perfect solution because the actual animations for the grenade launcher
	//do not currently have reload animations for reloading 1 shell with only 1 shell in the gun, etc
	//this means when reloading only 1 shell with only 1 shell in the gun already
	//we pretend that we have 3 shells loaded for the purposes of the reload animation
	//this will show the player the wrong number of shells in their gun but the animation will line up correctly now - by alien

	if (theSpareShots < (4 - theShotsInClip)) {
		theShotsInClip = 4 - theSpareShots;
	}

	switch(theShotsInClip)
	{
	case 0: //reload 4 times
		theAnimation = 7;
		break;
		
	case 1: //reload 3 times
		theAnimation = 6;
		break;
		
	case 2: //reload 2 times
		theAnimation = 5;
		break;
		
	case 3: //reload 1 time
		theAnimation = 4;
		break;
	}
	
	return theAnimation;
}

int	AvHGrenadeGun::GetEmptyShootAnimation() const
{
	return 12;
}

int	AvHGrenadeGun::GetShootAnimation() const
{
	int theAnimation = -1;
	
	int theShotsInClip = this->GetShotsInClip();
	
	switch(theShotsInClip)
	{
	case 4:
		theAnimation = 8;
		break;
	case 3:
		theAnimation = 9;
		break;
	case 2:
		theAnimation = 10;
		break;
	case 1:
		theAnimation = 11;
		break;
	case 0:
		theAnimation = 12;
		break;
	}
	
	return theAnimation;
}

char* AvHGrenadeGun::GetViewModel() const
{
	return kGGVModel;
}

char* AvHGrenadeGun::GetWorldModel() const
{
	return kGGWModel;
}

void AvHGrenadeGun::FireProjectiles(void)
{
	#ifdef AVH_SERVER

	float theDamageMultiplier;
	AvHPlayerUpgrade::GetWeaponUpgrade(this->m_pPlayer->pev->iuser3, this->m_pPlayer->pev->iuser4, &theDamageMultiplier);
	//float theDamage = this->mDamage*((2.0f*(theDamageMultiplier - 1.0f)) + 1.0f);

	Vector theOrigin;
	this->GetEventOrigin(theOrigin);

	// Grenade gun uses velocity here instead of angles, assumes angles are the player angles (for both the server grenade and the client temp entity)
	Vector theVelocity;
	this->GetEventAngles(theVelocity);

	// How to handle this?  Only generate entity on server, but we should do SOMETHING on the client, no?
	CGrenade* theGrenade = AvHSUShootServerGrenade(this->m_pPlayer->pev, theOrigin, theVelocity, BALANCE_VAR(kGrenDetonateTime), false);
    ASSERT(theGrenade);
	theGrenade->pev->dmg = this->mDamage*theDamageMultiplier;

	#endif
}

void AvHGrenadeGun::Precache()
{
	AvHMarineWeapon::Precache();

	PRECACHE_UNMODIFIED_MODEL(kGGEjectModel);
	PRECACHE_UNMODIFIED_MODEL(kGGAmmoModel);

	PRECACHE_UNMODIFIED_SOUND(kGrenadeBounceSound1);
	PRECACHE_UNMODIFIED_SOUND(kGrenadeBounceSound2);
	PRECACHE_UNMODIFIED_SOUND(kGrenadeBounceSound3);

	PRECACHE_UNMODIFIED_SOUND(kGGFireSound1);
	
	PRECACHE_UNMODIFIED_SOUND(kGGReloadSound);
	
	this->mEvent = PRECACHE_EVENT(1, kGGEventName);
}

void AvHGrenadeGun::Spawn() 
{ 
    AvHMarineWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_GRENADE_GUN;
	m_iDefaultAmmo = BALANCE_VAR(kGGMaxClip);

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsGrenadeGun);

	SET_MODEL(ENT(this->pev), kGGWModel);

	FallInit();// get ready to fall down.
} 
