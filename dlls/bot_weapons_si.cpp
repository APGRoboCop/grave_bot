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

bot_weapon_select_t si_weapon_select[] =
{
	// briefcase
	{
		SI_WEAPON_BRIEFCASE,	// id
		"weapon_briefcase",		// classname
		"briefcase",			// third person model
		{						// primary ammo pickup classnmes
			"",
			"",
		},
		{						// secondary ammo pickup classnmes
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
	// vomit 
	{
		SI_WEAPON_VOMIT,	// id
		"weapon_vomit",		// classname
		"gi",			// third person model
		{						// primary ammo pickup classnmes
			"ammo_gaussclip",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		1,						// priority
		0.0,					// min primary distance
		170.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		10,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// mindray
	{
		SI_WEAPON_MINDRAY,		// id
		"weapon_mindray",			// classname
		"mindray",					// third person model
		{						// primary ammo pickup classnmes
			"weapon_generic",
			"",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		2,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
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
		SI_WEAPON_GAUSS,		// id
		"weapon_gauss",			// classname
		"gauss",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_gaussclip",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		4,						// priority
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
		TRUE,					// charge weapon using primary fire?
		TRUE,					// charge weapon using secondary fire?
		1.0,					// time to charge weapon (primary)
		3.0						// time to charge weapon (secondary)
	},
	// crossbow
	{
		SI_WEAPON_CROSSBOW,	// id
		"weapon_crossbow",		// classname
		"crossbow",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_crossbow",
			"weapon_generic"
		},
		{						// secondary ammo pickup classnmes
			"",
			""
		},
		5,						// skill level
		8,						// priority
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
	// shotgun
	{
		SI_WEAPON_SHOTGUN,	// id
		"weapon_shotgun",		// classname
		"shotgun",	// third person model
		{						// primary ammo pickup classnmes
			"ammo_buckshot",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		5,						// priority
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
	// EMP Cannon
	{
		SI_WEAPON_EMPCANNON,	// id
		"weapon_empcannon",		// classname
		"emp",					// third person model
		{						// primary ammo pickup classnmes
			"weapon_generic",
			"",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		3,						// priority
		0.0,					// min primary distance
		512.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		2.6,					// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		4,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// tommygun
	{
		SI_WEAPON_TOMMYGUN,		// id
		"weapon_tommygun",		// classname
		"tommygun",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_tommygun",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		6,						// priority
		300.0,					// min primary distance
		600.0,					// max primary distance
		0.0,					// min secondary distance
		300.0,					// max secondary distance
		FALSE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
		0,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// dual uzis
	{
		SI_WEAPON_SNUZI,		// id
		"weapon_snUZI",			// classname
		"snUZI",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_snUZI",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		7,						// priority
		0.0,					// min primary distance
		1024.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
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
	// colt
	{
		SI_WEAPON_COLT,			// id
		"weapon_colt",			// classname
		"colt",					// third person model
		{						// primary ammo pickup classnmes
			"ammo_357",
			"weapon_generic"
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		9,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		256.0,					// max secondary distance
		TRUE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
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
	// terminator
	{
		0,
		"",
		"",
		{						// primary ammo pickup classnmes
			"",
			"",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		0,
		MAX_WEAPONS,
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

bot_fire_delay_t si_fire_delay[] =
{
	{
		SI_WEAPON_BRIEFCASE,
		0.25,
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
		SI_WEAPON_VOMIT,
		1.8,
		{
			0.0,
			0.0,
			0.4,
			0.8,
			1.1
		},
		{
			0.0,
			0.2,
			0.7,
			1.2,
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
		SI_WEAPON_MINDRAY,
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
		SI_WEAPON_GAUSS,
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
		0.1,
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
		SI_WEAPON_CROSSBOW,
		0.95,
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
		SI_WEAPON_SHOTGUN,
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
		1.4,
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
		SI_WEAPON_EMPCANNON,
		0.25,
		{
			0.0,
			0.0,
			0.1,
			0.3,
			0.6
		},
		{
			0.0,
			0.2,
			0.4,
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
		SI_WEAPON_TOMMYGUN,
		0.14,
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
		0.09,
		{
			0.0,
			0.0,
			0.0,
			0.1,
			0.2
		},
		{
			0.0,
			0.03,
			0.06,
			0.1,
			0.3
		}
	},
	{
		SI_WEAPON_SNUZI,
		0.1575,
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
		0.1575,
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
		}
	},
	{
		SI_WEAPON_COLT,
		0.5,
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
};
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

bot_weapon_select_t si_weapon_select[] =
{
	// briefcase
	{
		SI_WEAPON_BRIEFCASE,	// id
		"weapon_briefcase",		// classname
		"briefcase",			// third person model
		{						// primary ammo pickup classnmes
			"",
			"",
		},
		{						// secondary ammo pickup classnmes
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
	// vomit 
	{
		SI_WEAPON_VOMIT,	// id
		"weapon_vomit",		// classname
		"gi",			// third person model
		{						// primary ammo pickup classnmes
			"ammo_gaussclip",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		1,						// priority
		0.0,					// min primary distance
		170.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		0,						// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		10,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// mindray
	{
		SI_WEAPON_MINDRAY,		// id
		"weapon_mindray",			// classname
		"mindray",					// third person model
		{						// primary ammo pickup classnmes
			"weapon_generic",
			"",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		2,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
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
		SI_WEAPON_GAUSS,		// id
		"weapon_gauss",			// classname
		"gauss",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_gaussclip",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		4,						// priority
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
		TRUE,					// charge weapon using primary fire?
		TRUE,					// charge weapon using secondary fire?
		1.0,					// time to charge weapon (primary)
		3.0						// time to charge weapon (secondary)
	},
	// crossbow
	{
		SI_WEAPON_CROSSBOW,	// id
		"weapon_crossbow",		// classname
		"crossbow",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_crossbow",
			"weapon_generic"
		},
		{						// secondary ammo pickup classnmes
			"",
			""
		},
		5,						// skill level
		8,						// priority
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
	// shotgun
	{
		SI_WEAPON_SHOTGUN,	// id
		"weapon_shotgun",		// classname
		"shotgun",	// third person model
		{						// primary ammo pickup classnmes
			"ammo_buckshot",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		5,						// priority
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
	// EMP Cannon
	{
		SI_WEAPON_EMPCANNON,	// id
		"weapon_empcannon",		// classname
		"emp",					// third person model
		{						// primary ammo pickup classnmes
			"weapon_generic",
			"",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		3,						// priority
		0.0,					// min primary distance
		512.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		2.6,					// how long does this weapon take to reload?
		100,					// times out of 100 to use primary fire
		4,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// tommygun
	{
		SI_WEAPON_TOMMYGUN,		// id
		"weapon_tommygun",		// classname
		"tommygun",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_tommygun",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		6,						// priority
		300.0,					// min primary distance
		600.0,					// max primary distance
		0.0,					// min secondary distance
		300.0,					// max secondary distance
		FALSE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
		0,					// times out of 100 to use primary fire
		1,						// minimum ammout of primary ammo needed to fire
		0,						// minimum ammout of seconday ammo needed to fire
		FALSE,					// hold down primary fire button to use?
		FALSE,					// hold down secondary fire button to use?
		FALSE,					// charge weapon using primary fire?
		FALSE,					// charge weapon using secondary fire?
		0.0,					// time to charge weapon (primary)
		0.0						// time to charge weapon (secondary)
	},
	// dual uzis
	{
		SI_WEAPON_SNUZI,		// id
		"weapon_snUZI",			// classname
		"snUZI",				// third person model
		{						// primary ammo pickup classnmes
			"ammo_snUZI",
			"weapon_generic",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		7,						// priority
		0.0,					// min primary distance
		1024.0,					// max primary distance
		0.0,					// min secondary distance
		0.0,					// max secondary distance
		FALSE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
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
	// colt
	{
		SI_WEAPON_COLT,			// id
		"weapon_colt",			// classname
		"colt",					// third person model
		{						// primary ammo pickup classnmes
			"ammo_357",
			"weapon_generic"
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		5,						// skill level
		9,						// priority
		0.0,					// min primary distance
		9999.0,					// max primary distance
		0.0,					// min secondary distance
		256.0,					// max secondary distance
		TRUE,					// can use underwater?
		1.5,					// how long does this weapon take to reload?
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
	// terminator
	{
		0,
		"",
		"",
		{						// primary ammo pickup classnmes
			"",
			"",
		},
		{						// secondary ammo pickup classnmes
			"",
			"",
		},
		0,
		MAX_WEAPONS,
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

bot_fire_delay_t si_fire_delay[] =
{
	{
		SI_WEAPON_BRIEFCASE,
		0.25,
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
		SI_WEAPON_VOMIT,
		1.8,
		{
			0.0,
			0.0,
			0.4,
			0.8,
			1.1
		},
		{
			0.0,
			0.2,
			0.7,
			1.2,
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
		SI_WEAPON_MINDRAY,
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
		SI_WEAPON_GAUSS,
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
		0.1,
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
		SI_WEAPON_CROSSBOW,
		0.95,
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
		SI_WEAPON_SHOTGUN,
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
		1.4,
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
		SI_WEAPON_EMPCANNON,
		0.25,
		{
			0.0,
			0.0,
			0.1,
			0.3,
			0.6
		},
		{
			0.0,
			0.2,
			0.4,
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
		SI_WEAPON_TOMMYGUN,
		0.14,
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
		0.09,
		{
			0.0,
			0.0,
			0.0,
			0.1,
			0.2
		},
		{
			0.0,
			0.03,
			0.06,
			0.1,
			0.3
		}
	},
	{
		SI_WEAPON_SNUZI,
		0.1575,
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
		0.1575,
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
		}
	},
	{
		SI_WEAPON_COLT,
		0.5,
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
};
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
>>>>>>> 594320fd6db9c6f9b3b7d743a0270568ddbba6bd
