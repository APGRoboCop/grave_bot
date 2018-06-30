<<<<<<< HEAD
=======
<<<<<<< HEAD
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
#ifndef METAMOD_BUILD
   #include "extdll.h"
   #include "enginecallback.h"
   #include "util.h"
   #include "cbase.h"
   #include "entity_state.h"
#else
   #include <extdll.h>
   #include <dllapi.h>
   #include <h_export.h>
   #include <meta_api.h>
#endif

#include "bot.h"
#include "bot_weapons.h"

bot_weapon_select_t valve_weapon_select[] =
{
	// crowbar
	{
		VALVE_WEAPON_CROWBAR,	// id
		"weapon_crowbar",		// classname
		"crowbar",				// third person model
		{						// primary ammo pickup classnames
			"",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		0,						// priority
		0.0,					// min primary distance
		50.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		0,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// egon
	{
		VALVE_WEAPON_EGON,		// id
		"weapon_egon",			// classname
		"egon",					// third person model
		{						// primary ammo pickup classnames
			"ammo_gaussclip",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		1,						// priority
		32.0,					// min primary distance
		1024.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		TRUE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// gauss
	{
		VALVE_WEAPON_GAUSS,		// id
		"weapon_gauss",			// classname
		"gauss",				// third person model
		{						// primary ammo pickup classnames
			"ammo_gaussclip",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		2,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		9999.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		75,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		TRUE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.8						// time to charge weapon (secondary)
	},
	// rpg
	{
		VALVE_WEAPON_RPG,		// id
		"weapon_rpg",			// classname
		"rpg",					// third person model
		{						// primary ammo pickup classnames
			"ammo_rpgclip",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		3,						// priority
		300.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		2.0,					// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// shotgun
	{
		VALVE_WEAPON_SHOTGUN,	// id
		"weapon_shotgun",		// classname
		"shotgun",	// third person model
		{						// primary ammo pickup classnames
			"ammo_buckshot",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		4,						// priority
		0.0,					// min primary distance
		300.0,					// max primary distance
		0.0,					// min secondary distance
		300.0,					// max secondary distance
		FALSE,					// can use underwater?
		2.0,					// how long does this weapon take to reload?
		60,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		2,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// crossbow
	{
		VALVE_WEAPON_CROSSBOW,	// id
		"weapon_crossbow",		// classname
		"crossbow",				// third person model
		{						// primary ammo pickup classnames
			"ammo_crossbow",
			""
		},
		{						// secondary ammo pickup classnames
			"",
			""
		},
		5,						// skill level
		5,						// priority
		50.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		4.5,					// how long does this weapon take to reload?
		80,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// .357 python
	{
		VALVE_WEAPON_PYTHON,	// id
		"weapon_357",			// classname
		"357",					// third person model
		{						// primary ammo pickup classnames
			"ammo_357",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		6,						// priority
		30.0,					// min primary distance
		2048.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		2.0,					// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// assault rifle (mp5)
	{
		VALVE_WEAPON_MP5,		// id
		"weapon_9mmAR",			// classname
		"9mmAR",				// third person model
		{						// primary ammo pickup classnames
			"ammo_9mmclip",
			"ammo_9mmAR",
		},
		{						// secondary ammo pickup classnames
			"ammo_ARgrenades",
			"",
		},
		5,						// skill level
		7,						// priority
		0.0,					// min primary distance
		786.0,					// max primary distance
		300.0,					// min secondary distance
		600.0,					// max secondary distance
		FALSE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
		90,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// 9mm pistol
	{
		VALVE_WEAPON_GLOCK,		// id
		"weapon_9mmhandgun",	// classname
		"9mmhandgun",// third person model
		{						// primary ammo pickup classnames
			"ammo_9mmAR",
			"ammo_9mmclip"
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		8,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		256.0,					// max secondary distance
		TRUE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
		70,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// hornet gun
	{
		VALVE_WEAPON_HORNETGUN,	// id
		"weapon_hornetgun",		// classname
		"hgun",					// third person model
		{						// primary ammo pickup classnames
			"",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		9,						// priority
		30.0,					// min primary distance
		1000.0,					// max primary distance
		30.0,					// min secondary distance
		1000.0,					// max secondary distance
		TRUE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		90,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		4,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		TRUE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// hand grenades
	{
		VALVE_WEAPON_HANDGRENADE,// id
		"weapon_handgrenade",	// classname
		"grenade",				// third person model
		{						// primary ammo pickup classnames
			"weapon_handgrenade",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		10,						// priority
		250.0,					// min primary distance
		750.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// snarks
	{
		VALVE_WEAPON_SNARK,		// id
		"weapon_snark",			// classname
		"squeak",	// third person model
		{						// primary ammo pickup classnames
			"weapon_snark",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		11,						// priority
		150.0,					// min primary distance
		500.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	/* terminator */
	{
		0,
		"",
		"",
		{						// primary ammo pickup classnames
			"",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		0,
		12,
		0.0,
		0.0,
		0.0,
		0.0,
		TRUE,
		0,
		0,
		1,
		1,
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		0.0,
		0.0
	}
};

// weapon firing delay based on skill (min and max delay for each weapon)
// THESE MUST MATCH THE SAME ORDER AS THE WEAPON SELECT ARRAY!!!

bot_fire_delay_t valve_fire_delay[] =
{
	{
		VALVE_WEAPON_CROWBAR,
		0.0,
		{
			0.0,
			0.2,
			0.3,
			0.4,
			0.6
		},
		{
			0.0,
			0.3,
			0.5,
			0.7,
			1.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_EGON,
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_GAUSS,
		0.2,
		{
			0.0,
			0.0,
			0.3,
			0.5,
			1.0
		},
		{
			0.0,
			0.1,
			0.5,
			0.8,
			1.2
		},
		1.0,
		{
			0.0,
			0.0,
			0.5,
			0.8,
			1.2
		},
		{
			0.0,
			0.7,
			1.0,
			1.5,
			2.0
		}
	},
	{
		VALVE_WEAPON_RPG,
		1.5,
		{
			0.0,
			0.0,
			1.0,
			2.0,
			3.0
		},
		{
			0.0,
			1.0,
			2.0,
			4.0,
			5.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_SHOTGUN,
		0.75,
		{
			0.0,
			0.0,
			0.1,
			0.4,
			0.6
		},
		{
			0.0,
			0.3,
			0.5,
			0.7,
			1.0
		},
		1.5,
		{
			0.0,
			0.0,
			0.4,
			0.6,
			0.8
		},
		{
			0.0,
			0.2,
			0.5,
			0.8,
			1.2
		}
	},
	{
		VALVE_WEAPON_CROSSBOW,
		0.75,
		{
			0.0,
			0.0,
			0.5,
			0.8,
			1.0
		},
		{
			0.0,
			0.4,
			0.7,
			1.0,
			1.3
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_PYTHON,
		0.75,
		{
			0.0,
			0.0,
			0.2,
			0.4,
			0.75
		},
		{
			0.0,
			0.2,
			0.4,
			0.8,
			1.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_MP5,
		0.1,
		{
			0.0,
			0.0,
			0.0,
			0.1,
			0.2
		},
		{
			0.0,
			0.05,
			0.1,
			0.3,
			0.5
		},
		1.0,
		{
			0.0,
			0.0,
			0.7,
			1.0,
			1.4
		},
		{
			0.0,
			0.7,
			1.0,
			1.6,
			2.0
		}
	},
	{
		VALVE_WEAPON_GLOCK,
		0.2,
		{
			0.0,
			0.0,
			0.2,
			0.3,
			0.4
		},
		{
			0.0,
			0.1,
			0.3,
			0.4,
			0.5
		},
		0.2,
		{
			0.0,
			0.0,
			0.1,
			0.1,
			0.2
		},
		{
			0.0,
			0.1,
			0.2,
			0.2,
			0.4
		}
	},
	{
		VALVE_WEAPON_HORNETGUN,
		0.25,
		{
			0.0,
			0.0,
			0.4,
			0.6,
			1.0
		},
		{
			0.0,
			0.0,
			0.7,
			1.0,
			1.5
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_HANDGRENADE,
		0.5,
		{
			0.0,
			0.0,
			1.0,
			2.0,
			3.0
		},
		{
			0.0,
			1.0,
			2.0,
			3.0,
			4.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_SNARK,
		0.1,
		{
			0.0,
			0.0,
			0.2,
			0.4,
			0.6
		},
		{
			0.0,
			0.2,
			0.5,
			0.7,
			1.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	// terminator 
	{
		0,
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	}
<<<<<<< HEAD
=======
=======
#ifndef METAMOD_BUILD
   #include "extdll.h"
   #include "enginecallback.h"
   #include "util.h"
   #include "cbase.h"
   #include "entity_state.h"
#else
   #include <extdll.h>
   #include <dllapi.h>
   #include <h_export.h>
   #include <meta_api.h>
#endif

#include "bot.h"
#include "bot_weapons.h"

bot_weapon_select_t valve_weapon_select[] =
{
	// crowbar
	{
		VALVE_WEAPON_CROWBAR,	// id
		"weapon_crowbar",		// classname
		"crowbar",				// third person model
		{						// primary ammo pickup classnames
			"",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		0,						// priority
		0.0,					// min primary distance
		50.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		0,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// egon
	{
		VALVE_WEAPON_EGON,		// id
		"weapon_egon",			// classname
		"egon",					// third person model
		{						// primary ammo pickup classnames
			"ammo_gaussclip",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		1,						// priority
		32.0,					// min primary distance
		1024.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		TRUE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// gauss
	{
		VALVE_WEAPON_GAUSS,		// id
		"weapon_gauss",			// classname
		"gauss",				// third person model
		{						// primary ammo pickup classnames
			"ammo_gaussclip",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		2,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		9999.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		75,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		TRUE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.8						// time to charge weapon (secondary)
	},
	// rpg
	{
		VALVE_WEAPON_RPG,		// id
		"weapon_rpg",			// classname
		"rpg",					// third person model
		{						// primary ammo pickup classnames
			"ammo_rpgclip",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		3,						// priority
		300.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		2.0,					// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// shotgun
	{
		VALVE_WEAPON_SHOTGUN,	// id
		"weapon_shotgun",		// classname
		"shotgun",	// third person model
		{						// primary ammo pickup classnames
			"ammo_buckshot",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		4,						// priority
		0.0,					// min primary distance
		300.0,					// max primary distance
		0.0,					// min secondary distance
		300.0,					// max secondary distance
		FALSE,					// can use underwater?
		2.0,					// how long does this weapon take to reload?
		60,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		2,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// crossbow
	{
		VALVE_WEAPON_CROSSBOW,	// id
		"weapon_crossbow",		// classname
		"crossbow",				// third person model
		{						// primary ammo pickup classnames
			"ammo_crossbow",
			""
		},
		{						// secondary ammo pickup classnames
			"",
			""
		},
		5,						// skill level
		5,						// priority
		50.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		4.5,					// how long does this weapon take to reload?
		80,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// .357 python
	{
		VALVE_WEAPON_PYTHON,	// id
		"weapon_357",			// classname
		"357",					// third person model
		{						// primary ammo pickup classnames
			"ammo_357",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		6,						// priority
		30.0,					// min primary distance
		2048.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		2.0,					// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// assault rifle (mp5)
	{
		VALVE_WEAPON_MP5,		// id
		"weapon_9mmAR",			// classname
		"9mmAR",				// third person model
		{						// primary ammo pickup classnames
			"ammo_9mmclip",
			"ammo_9mmAR",
		},
		{						// secondary ammo pickup classnames
			"ammo_ARgrenades",
			"",
		},
		5,						// skill level
		7,						// priority
		0.0,					// min primary distance
		786.0,					// max primary distance
		300.0,					// min secondary distance
		600.0,					// max secondary distance
		FALSE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
		90,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// 9mm pistol
	{
		VALVE_WEAPON_GLOCK,		// id
		"weapon_9mmhandgun",	// classname
		"9mmhandgun",// third person model
		{						// primary ammo pickup classnames
			"ammo_9mmAR",
			"ammo_9mmclip"
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		8,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		256.0,					// max secondary distance
		TRUE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
		70,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		1,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// hornet gun
	{
		VALVE_WEAPON_HORNETGUN,	// id
		"weapon_hornetgun",		// classname
		"hgun",					// third person model
		{						// primary ammo pickup classnames
			"",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		9,						// priority
		30.0,					// min primary distance
		1000.0,					// max primary distance
		30.0,					// min secondary distance
		1000.0,					// max secondary distance
		TRUE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		90,						// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		4,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		TRUE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// hand grenades
	{
		VALVE_WEAPON_HANDGRENADE,// id
		"weapon_handgrenade",	// classname
		"grenade",				// third person model
		{						// primary ammo pickup classnames
			"weapon_handgrenade",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		10,						// priority
		250.0,					// min primary distance
		750.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		TRUE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// snarks
	{
		VALVE_WEAPON_SNARK,		// id
		"weapon_snark",			// classname
		"squeak",	// third person model
		{						// primary ammo pickup classnames
			"weapon_snark",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		5,						// skill level
		11,						// priority
		150.0,					// min primary distance
		500.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	/* terminator */
	{
		0,
		"",
		"",
		{						// primary ammo pickup classnames
			"",
			"",
		},
		{						// secondary ammo pickup classnames
			"",
			"",
		},
		0,
		12,
		0.0,
		0.0,
		0.0,
		0.0,
		TRUE,
		0,
		0,
		1,
		1,
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		0.0,
		0.0
	}
};

// weapon firing delay based on skill (min and max delay for each weapon)
// THESE MUST MATCH THE SAME ORDER AS THE WEAPON SELECT ARRAY!!!

bot_fire_delay_t valve_fire_delay[] =
{
	{
		VALVE_WEAPON_CROWBAR,
		0.0,
		{
			0.0,
			0.2,
			0.3,
			0.4,
			0.6
		},
		{
			0.0,
			0.3,
			0.5,
			0.7,
			1.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_EGON,
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_GAUSS,
		0.2,
		{
			0.0,
			0.0,
			0.3,
			0.5,
			1.0
		},
		{
			0.0,
			0.1,
			0.5,
			0.8,
			1.2
		},
		1.0,
		{
			0.0,
			0.0,
			0.5,
			0.8,
			1.2
		},
		{
			0.0,
			0.7,
			1.0,
			1.5,
			2.0
		}
	},
	{
		VALVE_WEAPON_RPG,
		1.5,
		{
			0.0,
			0.0,
			1.0,
			2.0,
			3.0
		},
		{
			0.0,
			1.0,
			2.0,
			4.0,
			5.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_SHOTGUN,
		0.75,
		{
			0.0,
			0.0,
			0.1,
			0.4,
			0.6
		},
		{
			0.0,
			0.3,
			0.5,
			0.7,
			1.0
		},
		1.5,
		{
			0.0,
			0.0,
			0.4,
			0.6,
			0.8
		},
		{
			0.0,
			0.2,
			0.5,
			0.8,
			1.2
		}
	},
	{
		VALVE_WEAPON_CROSSBOW,
		0.75,
		{
			0.0,
			0.0,
			0.5,
			0.8,
			1.0
		},
		{
			0.0,
			0.4,
			0.7,
			1.0,
			1.3
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_PYTHON,
		0.75,
		{
			0.0,
			0.0,
			0.2,
			0.4,
			0.75
		},
		{
			0.0,
			0.2,
			0.4,
			0.8,
			1.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_MP5,
		0.1,
		{
			0.0,
			0.0,
			0.0,
			0.1,
			0.2
		},
		{
			0.0,
			0.05,
			0.1,
			0.3,
			0.5
		},
		1.0,
		{
			0.0,
			0.0,
			0.7,
			1.0,
			1.4
		},
		{
			0.0,
			0.7,
			1.0,
			1.6,
			2.0
		}
	},
	{
		VALVE_WEAPON_GLOCK,
		0.2,
		{
			0.0,
			0.0,
			0.2,
			0.3,
			0.4
		},
		{
			0.0,
			0.1,
			0.3,
			0.4,
			0.5
		},
		0.2,
		{
			0.0,
			0.0,
			0.1,
			0.1,
			0.2
		},
		{
			0.0,
			0.1,
			0.2,
			0.2,
			0.4
		}
	},
	{
		VALVE_WEAPON_HORNETGUN,
		0.25,
		{
			0.0,
			0.0,
			0.4,
			0.6,
			1.0
		},
		{
			0.0,
			0.0,
			0.7,
			1.0,
			1.5
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_HANDGRENADE,
		0.5,
		{
			0.0,
			0.0,
			1.0,
			2.0,
			3.0
		},
		{
			0.0,
			1.0,
			2.0,
			3.0,
			4.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	{
		VALVE_WEAPON_SNARK,
		0.1,
		{
			0.0,
			0.0,
			0.2,
			0.4,
			0.6
		},
		{
			0.0,
			0.2,
			0.5,
			0.7,
			1.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	},
	// terminator 
	{
		0,
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		0.0,
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		},
		{
			0.0,
			0.0,
			0.0,
			0.0,
			0.0
		}
	}
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
};