<<<<<<< HEAD
//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// linkfunc.cpp
//

#ifndef METAMOD_BUILD
   #include "extdll.h"
   #include "enginecallback.h"
   #include "util.h"
   #include "cbase.h"
   #include "entity_state.h"

   #include "bot.h"

   #ifdef __BORLANDC__
   extern HINSTANCE _h_Library;
   #elif _WIN32
   extern HINSTANCE h_Library;
   #else
   extern void *h_Library;
   #endif

   #ifdef __BORLANDC__

   #define LINK_ENTITY_TO_FUNC(mapClassName) \
    extern "C" EXPORT void mapClassName( entvars_t *pev ); \
    void mapClassName( entvars_t *pev ) { \
         static LINK_ENTITY_FUNC otherClassName = NULL; \
         static int skip_this = 0; \
         if (skip_this) return; \
         if (otherClassName == NULL) \
            otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(_h_Library, #mapClassName); \
         if (otherClassName == NULL) { \
            skip_this = 1; return; \
         } \
         (*otherClassName)(pev); }

   #else

   #define LINK_ENTITY_TO_FUNC(mapClassName) \
    extern "C" EXPORT void mapClassName( entvars_t *pev ); \
    void mapClassName( entvars_t *pev ) { \
         static LINK_ENTITY_FUNC otherClassName = NULL; \
         static int skip_this = 0; \
         if (skip_this) return; \
         if (otherClassName == NULL) \
            otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, #mapClassName); \
         if (otherClassName == NULL) { \
            skip_this = 1; return; \
         } \
         (*otherClassName)(pev); }

   #endif


   // entities for Valve's hl.dll and Standard SDK...
   LINK_ENTITY_TO_FUNC(aiscripted_sequence);
   LINK_ENTITY_TO_FUNC(ambient_generic);
   LINK_ENTITY_TO_FUNC(ammo_357);
   LINK_ENTITY_TO_FUNC(ammo_9mmAR);
   LINK_ENTITY_TO_FUNC(ammo_9mmbox);
   LINK_ENTITY_TO_FUNC(ammo_9mmclip);
   LINK_ENTITY_TO_FUNC(ammo_ARgrenades);
   LINK_ENTITY_TO_FUNC(ammo_buckshot);
   LINK_ENTITY_TO_FUNC(ammo_crossbow);
   LINK_ENTITY_TO_FUNC(ammo_egonclip);
   LINK_ENTITY_TO_FUNC(ammo_gaussclip);
   LINK_ENTITY_TO_FUNC(ammo_glockclip);
   LINK_ENTITY_TO_FUNC(ammo_mp5clip);
   LINK_ENTITY_TO_FUNC(ammo_mp5grenades);
   LINK_ENTITY_TO_FUNC(ammo_rpgclip);
   LINK_ENTITY_TO_FUNC(beam);
   LINK_ENTITY_TO_FUNC(bmortar);
   LINK_ENTITY_TO_FUNC(bodyque);
   LINK_ENTITY_TO_FUNC(button_target);
   LINK_ENTITY_TO_FUNC(cine_blood);
   LINK_ENTITY_TO_FUNC(controller_energy_ball);
   LINK_ENTITY_TO_FUNC(controller_head_ball);
   LINK_ENTITY_TO_FUNC(crossbow_bolt);
   LINK_ENTITY_TO_FUNC(cycler);
   LINK_ENTITY_TO_FUNC(cycler_prdroid);
   LINK_ENTITY_TO_FUNC(cycler_sprite);
   LINK_ENTITY_TO_FUNC(cycler_weapon);
   LINK_ENTITY_TO_FUNC(cycler_wreckage);
   LINK_ENTITY_TO_FUNC(env_beam);
   LINK_ENTITY_TO_FUNC(env_beverage);
   LINK_ENTITY_TO_FUNC(env_blood);
   LINK_ENTITY_TO_FUNC(env_bubbles);
   LINK_ENTITY_TO_FUNC(env_debris);
   LINK_ENTITY_TO_FUNC(env_explosion);
   LINK_ENTITY_TO_FUNC(env_fade);
   LINK_ENTITY_TO_FUNC(env_funnel);
   LINK_ENTITY_TO_FUNC(env_global);
   LINK_ENTITY_TO_FUNC(env_glow);
   LINK_ENTITY_TO_FUNC(env_laser);
   LINK_ENTITY_TO_FUNC(env_lightning);
   LINK_ENTITY_TO_FUNC(env_message);
   LINK_ENTITY_TO_FUNC(env_render);
   LINK_ENTITY_TO_FUNC(env_shake);
   LINK_ENTITY_TO_FUNC(env_shooter);
   LINK_ENTITY_TO_FUNC(env_smoker);
   LINK_ENTITY_TO_FUNC(env_sound);
   LINK_ENTITY_TO_FUNC(env_spark);
   LINK_ENTITY_TO_FUNC(env_sprite);
   LINK_ENTITY_TO_FUNC(fireanddie);
   LINK_ENTITY_TO_FUNC(func_breakable);
   LINK_ENTITY_TO_FUNC(func_button);
   LINK_ENTITY_TO_FUNC(func_conveyor);
   LINK_ENTITY_TO_FUNC(func_door);
   LINK_ENTITY_TO_FUNC(func_door_rotating);
   LINK_ENTITY_TO_FUNC(func_friction);
   LINK_ENTITY_TO_FUNC(func_guntarget);
   LINK_ENTITY_TO_FUNC(func_healthcharger);
   LINK_ENTITY_TO_FUNC(func_illusionary);
   LINK_ENTITY_TO_FUNC(func_ladder);
   LINK_ENTITY_TO_FUNC(func_monsterclip);
   LINK_ENTITY_TO_FUNC(func_mortar_field);
   LINK_ENTITY_TO_FUNC(func_pendulum);
   LINK_ENTITY_TO_FUNC(func_plat);
   LINK_ENTITY_TO_FUNC(func_platrot);
   LINK_ENTITY_TO_FUNC(func_pushable);
   LINK_ENTITY_TO_FUNC(func_recharge);
   LINK_ENTITY_TO_FUNC(func_rot_button);
   LINK_ENTITY_TO_FUNC(func_rotating);
   LINK_ENTITY_TO_FUNC(func_tank);
   LINK_ENTITY_TO_FUNC(func_tankcontrols);
   LINK_ENTITY_TO_FUNC(func_tanklaser);
   LINK_ENTITY_TO_FUNC(func_tankmortar);
   LINK_ENTITY_TO_FUNC(func_tankrocket);
   LINK_ENTITY_TO_FUNC(func_trackautochange);
   LINK_ENTITY_TO_FUNC(func_trackchange);
   LINK_ENTITY_TO_FUNC(func_tracktrain);
   LINK_ENTITY_TO_FUNC(func_train);
   LINK_ENTITY_TO_FUNC(func_traincontrols);
   LINK_ENTITY_TO_FUNC(func_wall);
   LINK_ENTITY_TO_FUNC(func_wall_toggle);
   LINK_ENTITY_TO_FUNC(func_water);
   LINK_ENTITY_TO_FUNC(game_counter);
   LINK_ENTITY_TO_FUNC(game_counter_set);
   LINK_ENTITY_TO_FUNC(game_end);
   LINK_ENTITY_TO_FUNC(game_player_equip);
   LINK_ENTITY_TO_FUNC(game_player_hurt);
   LINK_ENTITY_TO_FUNC(game_player_team);
   LINK_ENTITY_TO_FUNC(game_score);
   LINK_ENTITY_TO_FUNC(game_team_master);
   LINK_ENTITY_TO_FUNC(game_team_set);
   LINK_ENTITY_TO_FUNC(game_text);
   LINK_ENTITY_TO_FUNC(game_zone_player);
   LINK_ENTITY_TO_FUNC(garg_stomp);
   LINK_ENTITY_TO_FUNC(gibshooter);
   LINK_ENTITY_TO_FUNC(grenade);
   LINK_ENTITY_TO_FUNC(hornet);
   LINK_ENTITY_TO_FUNC(hvr_rocket);
   LINK_ENTITY_TO_FUNC(info_bigmomma);
   LINK_ENTITY_TO_FUNC(info_intermission);
   LINK_ENTITY_TO_FUNC(info_landmark);
   LINK_ENTITY_TO_FUNC(info_node);
   LINK_ENTITY_TO_FUNC(info_node_air);
   LINK_ENTITY_TO_FUNC(info_null);
   LINK_ENTITY_TO_FUNC(info_player_deathmatch);
   LINK_ENTITY_TO_FUNC(info_player_start);
   LINK_ENTITY_TO_FUNC(info_target);
   LINK_ENTITY_TO_FUNC(info_teleport_destination);
   LINK_ENTITY_TO_FUNC(infodecal);
   LINK_ENTITY_TO_FUNC(item_airtank);
   LINK_ENTITY_TO_FUNC(item_antidote);
   LINK_ENTITY_TO_FUNC(item_battery);
   LINK_ENTITY_TO_FUNC(item_healthkit);
   LINK_ENTITY_TO_FUNC(item_longjump);
   LINK_ENTITY_TO_FUNC(item_security);
   LINK_ENTITY_TO_FUNC(item_sodacan);
   LINK_ENTITY_TO_FUNC(item_suit);
   LINK_ENTITY_TO_FUNC(laser_spot);
   LINK_ENTITY_TO_FUNC(light);
   LINK_ENTITY_TO_FUNC(light_environment);
   LINK_ENTITY_TO_FUNC(light_spot);
   LINK_ENTITY_TO_FUNC(momentary_door);
   LINK_ENTITY_TO_FUNC(momentary_rot_button);
   LINK_ENTITY_TO_FUNC(monster_alien_controller);
   LINK_ENTITY_TO_FUNC(monster_alien_grunt);
   LINK_ENTITY_TO_FUNC(monster_alien_slave);
   LINK_ENTITY_TO_FUNC(monster_apache);
   LINK_ENTITY_TO_FUNC(monster_babycrab);
   LINK_ENTITY_TO_FUNC(monster_barnacle);
   LINK_ENTITY_TO_FUNC(monster_barney);
   LINK_ENTITY_TO_FUNC(monster_barney_dead);
   LINK_ENTITY_TO_FUNC(monster_bigmomma);
   LINK_ENTITY_TO_FUNC(monster_bloater);
   LINK_ENTITY_TO_FUNC(monster_bullchicken);
   LINK_ENTITY_TO_FUNC(monster_cine2_hvyweapons);
   LINK_ENTITY_TO_FUNC(monster_cine2_scientist);
   LINK_ENTITY_TO_FUNC(monster_cine2_slave);
   LINK_ENTITY_TO_FUNC(monster_cine3_barney);
   LINK_ENTITY_TO_FUNC(monster_cine3_scientist);
   LINK_ENTITY_TO_FUNC(monster_cine_barney);
   LINK_ENTITY_TO_FUNC(monster_cine_panther);
   LINK_ENTITY_TO_FUNC(monster_cine_scientist);
   LINK_ENTITY_TO_FUNC(monster_cockroach);
   LINK_ENTITY_TO_FUNC(monster_flyer);
   LINK_ENTITY_TO_FUNC(monster_flyer_flock);
   LINK_ENTITY_TO_FUNC(monster_furniture);
   LINK_ENTITY_TO_FUNC(monster_gargantua);
   LINK_ENTITY_TO_FUNC(monster_generic);
   LINK_ENTITY_TO_FUNC(monster_gman);
   LINK_ENTITY_TO_FUNC(monster_grunt_repel);
   LINK_ENTITY_TO_FUNC(monster_headcrab);
   LINK_ENTITY_TO_FUNC(monster_hevsuit_dead);
   LINK_ENTITY_TO_FUNC(monster_hgrunt_dead);
   LINK_ENTITY_TO_FUNC(monster_houndeye);
   LINK_ENTITY_TO_FUNC(monster_human_assassin);
   LINK_ENTITY_TO_FUNC(monster_human_grunt);
   LINK_ENTITY_TO_FUNC(monster_ichthyosaur);
   LINK_ENTITY_TO_FUNC(monster_leech);
   LINK_ENTITY_TO_FUNC(monster_miniturret);
   LINK_ENTITY_TO_FUNC(monster_mortar);
   LINK_ENTITY_TO_FUNC(monster_nihilanth);
   LINK_ENTITY_TO_FUNC(monster_osprey);
   LINK_ENTITY_TO_FUNC(monster_rat);
   LINK_ENTITY_TO_FUNC(monster_satchel);
   LINK_ENTITY_TO_FUNC(monster_scientist);
   LINK_ENTITY_TO_FUNC(monster_scientist_dead);
   LINK_ENTITY_TO_FUNC(monster_sentry);
   LINK_ENTITY_TO_FUNC(monster_sitting_scientist);
   LINK_ENTITY_TO_FUNC(monster_snark);
   LINK_ENTITY_TO_FUNC(monster_tentacle);
   LINK_ENTITY_TO_FUNC(monster_tentaclemaw);
   LINK_ENTITY_TO_FUNC(monster_tripmine);
   LINK_ENTITY_TO_FUNC(monster_turret);
   LINK_ENTITY_TO_FUNC(monster_vortigaunt);
   LINK_ENTITY_TO_FUNC(monster_zombie);
   LINK_ENTITY_TO_FUNC(monstermaker);
   LINK_ENTITY_TO_FUNC(multi_manager);
   LINK_ENTITY_TO_FUNC(multisource);
   LINK_ENTITY_TO_FUNC(nihilanth_energy_ball);
   LINK_ENTITY_TO_FUNC(node_viewer);
   LINK_ENTITY_TO_FUNC(node_viewer_fly);
   LINK_ENTITY_TO_FUNC(node_viewer_human);
   LINK_ENTITY_TO_FUNC(node_viewer_large);
   LINK_ENTITY_TO_FUNC(path_corner);
   LINK_ENTITY_TO_FUNC(path_track);
   LINK_ENTITY_TO_FUNC(player);
   LINK_ENTITY_TO_FUNC(player_loadsaved);
   LINK_ENTITY_TO_FUNC(player_weaponstrip);
   LINK_ENTITY_TO_FUNC(rpg_rocket);
   LINK_ENTITY_TO_FUNC(scripted_sentence);
   LINK_ENTITY_TO_FUNC(scripted_sequence);
   LINK_ENTITY_TO_FUNC(soundent);
   LINK_ENTITY_TO_FUNC(spark_shower);
   LINK_ENTITY_TO_FUNC(speaker);
   LINK_ENTITY_TO_FUNC(squidspit);
   LINK_ENTITY_TO_FUNC(streak_spiral);
   LINK_ENTITY_TO_FUNC(target_cdaudio);
   LINK_ENTITY_TO_FUNC(test_effect);
   LINK_ENTITY_TO_FUNC(testhull);
   LINK_ENTITY_TO_FUNC(trigger);
   LINK_ENTITY_TO_FUNC(trigger_auto);
   LINK_ENTITY_TO_FUNC(trigger_autosave);
   LINK_ENTITY_TO_FUNC(trigger_camera);
   LINK_ENTITY_TO_FUNC(trigger_cdaudio);
   LINK_ENTITY_TO_FUNC(trigger_changelevel);
   LINK_ENTITY_TO_FUNC(trigger_changetarget);
   LINK_ENTITY_TO_FUNC(trigger_counter);
   LINK_ENTITY_TO_FUNC(trigger_endsection);
   LINK_ENTITY_TO_FUNC(trigger_gravity);
   LINK_ENTITY_TO_FUNC(trigger_hurt);
   LINK_ENTITY_TO_FUNC(trigger_monsterjump);
   LINK_ENTITY_TO_FUNC(trigger_multiple);
   LINK_ENTITY_TO_FUNC(trigger_once);
   LINK_ENTITY_TO_FUNC(trigger_push);
   LINK_ENTITY_TO_FUNC(trigger_relay);
   LINK_ENTITY_TO_FUNC(trigger_teleport);
   LINK_ENTITY_TO_FUNC(trigger_transition);
   LINK_ENTITY_TO_FUNC(weapon_357);
   LINK_ENTITY_TO_FUNC(weapon_9mmAR);
   LINK_ENTITY_TO_FUNC(weapon_9mmhandgun);
   LINK_ENTITY_TO_FUNC(weapon_crossbow);
   LINK_ENTITY_TO_FUNC(weapon_crowbar);
   LINK_ENTITY_TO_FUNC(weapon_egon);
   LINK_ENTITY_TO_FUNC(weapon_gauss);
   LINK_ENTITY_TO_FUNC(weapon_glock);
   LINK_ENTITY_TO_FUNC(weapon_handgrenade);
   LINK_ENTITY_TO_FUNC(weapon_hornetgun);
   LINK_ENTITY_TO_FUNC(weapon_mp5);
   LINK_ENTITY_TO_FUNC(weapon_python);
   LINK_ENTITY_TO_FUNC(weapon_rpg);
   LINK_ENTITY_TO_FUNC(weapon_satchel);
   LINK_ENTITY_TO_FUNC(weapon_shotgun);
   LINK_ENTITY_TO_FUNC(weapon_snark);
   LINK_ENTITY_TO_FUNC(weapon_tripmine);
   LINK_ENTITY_TO_FUNC(weaponbox);
   LINK_ENTITY_TO_FUNC(world_items);
   LINK_ENTITY_TO_FUNC(worldspawn);
   LINK_ENTITY_TO_FUNC(xen_hair);
   LINK_ENTITY_TO_FUNC(xen_hull);
   LINK_ENTITY_TO_FUNC(xen_plantlight);
   LINK_ENTITY_TO_FUNC(xen_spore_large);
   LINK_ENTITY_TO_FUNC(xen_spore_medium);
   LINK_ENTITY_TO_FUNC(xen_spore_small);
   LINK_ENTITY_TO_FUNC(xen_tree);
   LINK_ENTITY_TO_FUNC(xen_ttrigger);

   // Science and Industry
   LINK_ENTITY_TO_FUNC(ammo_rocketpistol);
   LINK_ENTITY_TO_FUNC(ammo_snUZI);
   LINK_ENTITY_TO_FUNC(ammo_tommygun);
   LINK_ENTITY_TO_FUNC(carry_resource);
   LINK_ENTITY_TO_FUNC(carry_scientist);
   LINK_ENTITY_TO_FUNC(emp_cannister);
   LINK_ENTITY_TO_FUNC(emp_pulse);
   LINK_ENTITY_TO_FUNC(env_model);
   LINK_ENTITY_TO_FUNC(fev_cloud);
   LINK_ENTITY_TO_FUNC(fev_vial);
   LINK_ENTITY_TO_FUNC(func_tech_breakable);
   LINK_ENTITY_TO_FUNC(info_administrator);
   LINK_ENTITY_TO_FUNC(info_scientist_dead);
   LINK_ENTITY_TO_FUNC(info_scientist_start);
   LINK_ENTITY_TO_FUNC(info_sidetect);
   LINK_ENTITY_TO_FUNC(info_transistor);
   LINK_ENTITY_TO_FUNC(item_resource);
   LINK_ENTITY_TO_FUNC(rocketpistol_rocket);
   LINK_ENTITY_TO_FUNC(timed_grenade);
   LINK_ENTITY_TO_FUNC(trigger_deliveryzone);
   LINK_ENTITY_TO_FUNC(weapon_briefcase);
   LINK_ENTITY_TO_FUNC(weapon_cloak);
   LINK_ENTITY_TO_FUNC(weapon_colt);
   LINK_ENTITY_TO_FUNC(weapon_empcannon);
   LINK_ENTITY_TO_FUNC(weapon_experimental);
   LINK_ENTITY_TO_FUNC(weapon_fleshgrenade);
   LINK_ENTITY_TO_FUNC(weapon_generic);
   LINK_ENTITY_TO_FUNC(weapon_mindray);
   LINK_ENTITY_TO_FUNC(weapon_resource);
   LINK_ENTITY_TO_FUNC(weapon_rocketpistol);
   LINK_ENTITY_TO_FUNC(weapon_snUZI);
   LINK_ENTITY_TO_FUNC(weapon_tommygun);
   LINK_ENTITY_TO_FUNC(weapon_transistor);
   LINK_ENTITY_TO_FUNC(weapon_vomit);
#endif
=======
//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// linkfunc.cpp
//

#ifndef METAMOD_BUILD
   #include "extdll.h"
   #include "enginecallback.h"
   #include "util.h"
   #include "cbase.h"
   #include "entity_state.h"

   #include "bot.h"

   #ifdef __BORLANDC__
   extern HINSTANCE _h_Library;
   #elif _WIN32
   extern HINSTANCE h_Library;
   #else
   extern void *h_Library;
   #endif

   #ifdef __BORLANDC__

   #define LINK_ENTITY_TO_FUNC(mapClassName) \
    extern "C" EXPORT void mapClassName( entvars_t *pev ); \
    void mapClassName( entvars_t *pev ) { \
         static LINK_ENTITY_FUNC otherClassName = NULL; \
         static int skip_this = 0; \
         if (skip_this) return; \
         if (otherClassName == NULL) \
            otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(_h_Library, #mapClassName); \
         if (otherClassName == NULL) { \
            skip_this = 1; return; \
         } \
         (*otherClassName)(pev); }

   #else

   #define LINK_ENTITY_TO_FUNC(mapClassName) \
    extern "C" EXPORT void mapClassName( entvars_t *pev ); \
    void mapClassName( entvars_t *pev ) { \
         static LINK_ENTITY_FUNC otherClassName = NULL; \
         static int skip_this = 0; \
         if (skip_this) return; \
         if (otherClassName == NULL) \
            otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, #mapClassName); \
         if (otherClassName == NULL) { \
            skip_this = 1; return; \
         } \
         (*otherClassName)(pev); }

   #endif


   // entities for Valve's hl.dll and Standard SDK...
   LINK_ENTITY_TO_FUNC(aiscripted_sequence);
   LINK_ENTITY_TO_FUNC(ambient_generic);
   LINK_ENTITY_TO_FUNC(ammo_357);
   LINK_ENTITY_TO_FUNC(ammo_9mmAR);
   LINK_ENTITY_TO_FUNC(ammo_9mmbox);
   LINK_ENTITY_TO_FUNC(ammo_9mmclip);
   LINK_ENTITY_TO_FUNC(ammo_ARgrenades);
   LINK_ENTITY_TO_FUNC(ammo_buckshot);
   LINK_ENTITY_TO_FUNC(ammo_crossbow);
   LINK_ENTITY_TO_FUNC(ammo_egonclip);
   LINK_ENTITY_TO_FUNC(ammo_gaussclip);
   LINK_ENTITY_TO_FUNC(ammo_glockclip);
   LINK_ENTITY_TO_FUNC(ammo_mp5clip);
   LINK_ENTITY_TO_FUNC(ammo_mp5grenades);
   LINK_ENTITY_TO_FUNC(ammo_rpgclip);
   LINK_ENTITY_TO_FUNC(beam);
   LINK_ENTITY_TO_FUNC(bmortar);
   LINK_ENTITY_TO_FUNC(bodyque);
   LINK_ENTITY_TO_FUNC(button_target);
   LINK_ENTITY_TO_FUNC(cine_blood);
   LINK_ENTITY_TO_FUNC(controller_energy_ball);
   LINK_ENTITY_TO_FUNC(controller_head_ball);
   LINK_ENTITY_TO_FUNC(crossbow_bolt);
   LINK_ENTITY_TO_FUNC(cycler);
   LINK_ENTITY_TO_FUNC(cycler_prdroid);
   LINK_ENTITY_TO_FUNC(cycler_sprite);
   LINK_ENTITY_TO_FUNC(cycler_weapon);
   LINK_ENTITY_TO_FUNC(cycler_wreckage);
   LINK_ENTITY_TO_FUNC(env_beam);
   LINK_ENTITY_TO_FUNC(env_beverage);
   LINK_ENTITY_TO_FUNC(env_blood);
   LINK_ENTITY_TO_FUNC(env_bubbles);
   LINK_ENTITY_TO_FUNC(env_debris);
   LINK_ENTITY_TO_FUNC(env_explosion);
   LINK_ENTITY_TO_FUNC(env_fade);
   LINK_ENTITY_TO_FUNC(env_funnel);
   LINK_ENTITY_TO_FUNC(env_global);
   LINK_ENTITY_TO_FUNC(env_glow);
   LINK_ENTITY_TO_FUNC(env_laser);
   LINK_ENTITY_TO_FUNC(env_lightning);
   LINK_ENTITY_TO_FUNC(env_message);
   LINK_ENTITY_TO_FUNC(env_render);
   LINK_ENTITY_TO_FUNC(env_shake);
   LINK_ENTITY_TO_FUNC(env_shooter);
   LINK_ENTITY_TO_FUNC(env_smoker);
   LINK_ENTITY_TO_FUNC(env_sound);
   LINK_ENTITY_TO_FUNC(env_spark);
   LINK_ENTITY_TO_FUNC(env_sprite);
   LINK_ENTITY_TO_FUNC(fireanddie);
   LINK_ENTITY_TO_FUNC(func_breakable);
   LINK_ENTITY_TO_FUNC(func_button);
   LINK_ENTITY_TO_FUNC(func_conveyor);
   LINK_ENTITY_TO_FUNC(func_door);
   LINK_ENTITY_TO_FUNC(func_door_rotating);
   LINK_ENTITY_TO_FUNC(func_friction);
   LINK_ENTITY_TO_FUNC(func_guntarget);
   LINK_ENTITY_TO_FUNC(func_healthcharger);
   LINK_ENTITY_TO_FUNC(func_illusionary);
   LINK_ENTITY_TO_FUNC(func_ladder);
   LINK_ENTITY_TO_FUNC(func_monsterclip);
   LINK_ENTITY_TO_FUNC(func_mortar_field);
   LINK_ENTITY_TO_FUNC(func_pendulum);
   LINK_ENTITY_TO_FUNC(func_plat);
   LINK_ENTITY_TO_FUNC(func_platrot);
   LINK_ENTITY_TO_FUNC(func_pushable);
   LINK_ENTITY_TO_FUNC(func_recharge);
   LINK_ENTITY_TO_FUNC(func_rot_button);
   LINK_ENTITY_TO_FUNC(func_rotating);
   LINK_ENTITY_TO_FUNC(func_tank);
   LINK_ENTITY_TO_FUNC(func_tankcontrols);
   LINK_ENTITY_TO_FUNC(func_tanklaser);
   LINK_ENTITY_TO_FUNC(func_tankmortar);
   LINK_ENTITY_TO_FUNC(func_tankrocket);
   LINK_ENTITY_TO_FUNC(func_trackautochange);
   LINK_ENTITY_TO_FUNC(func_trackchange);
   LINK_ENTITY_TO_FUNC(func_tracktrain);
   LINK_ENTITY_TO_FUNC(func_train);
   LINK_ENTITY_TO_FUNC(func_traincontrols);
   LINK_ENTITY_TO_FUNC(func_wall);
   LINK_ENTITY_TO_FUNC(func_wall_toggle);
   LINK_ENTITY_TO_FUNC(func_water);
   LINK_ENTITY_TO_FUNC(game_counter);
   LINK_ENTITY_TO_FUNC(game_counter_set);
   LINK_ENTITY_TO_FUNC(game_end);
   LINK_ENTITY_TO_FUNC(game_player_equip);
   LINK_ENTITY_TO_FUNC(game_player_hurt);
   LINK_ENTITY_TO_FUNC(game_player_team);
   LINK_ENTITY_TO_FUNC(game_score);
   LINK_ENTITY_TO_FUNC(game_team_master);
   LINK_ENTITY_TO_FUNC(game_team_set);
   LINK_ENTITY_TO_FUNC(game_text);
   LINK_ENTITY_TO_FUNC(game_zone_player);
   LINK_ENTITY_TO_FUNC(garg_stomp);
   LINK_ENTITY_TO_FUNC(gibshooter);
   LINK_ENTITY_TO_FUNC(grenade);
   LINK_ENTITY_TO_FUNC(hornet);
   LINK_ENTITY_TO_FUNC(hvr_rocket);
   LINK_ENTITY_TO_FUNC(info_bigmomma);
   LINK_ENTITY_TO_FUNC(info_intermission);
   LINK_ENTITY_TO_FUNC(info_landmark);
   LINK_ENTITY_TO_FUNC(info_node);
   LINK_ENTITY_TO_FUNC(info_node_air);
   LINK_ENTITY_TO_FUNC(info_null);
   LINK_ENTITY_TO_FUNC(info_player_deathmatch);
   LINK_ENTITY_TO_FUNC(info_player_start);
   LINK_ENTITY_TO_FUNC(info_target);
   LINK_ENTITY_TO_FUNC(info_teleport_destination);
   LINK_ENTITY_TO_FUNC(infodecal);
   LINK_ENTITY_TO_FUNC(item_airtank);
   LINK_ENTITY_TO_FUNC(item_antidote);
   LINK_ENTITY_TO_FUNC(item_battery);
   LINK_ENTITY_TO_FUNC(item_healthkit);
   LINK_ENTITY_TO_FUNC(item_longjump);
   LINK_ENTITY_TO_FUNC(item_security);
   LINK_ENTITY_TO_FUNC(item_sodacan);
   LINK_ENTITY_TO_FUNC(item_suit);
   LINK_ENTITY_TO_FUNC(laser_spot);
   LINK_ENTITY_TO_FUNC(light);
   LINK_ENTITY_TO_FUNC(light_environment);
   LINK_ENTITY_TO_FUNC(light_spot);
   LINK_ENTITY_TO_FUNC(momentary_door);
   LINK_ENTITY_TO_FUNC(momentary_rot_button);
   LINK_ENTITY_TO_FUNC(monster_alien_controller);
   LINK_ENTITY_TO_FUNC(monster_alien_grunt);
   LINK_ENTITY_TO_FUNC(monster_alien_slave);
   LINK_ENTITY_TO_FUNC(monster_apache);
   LINK_ENTITY_TO_FUNC(monster_babycrab);
   LINK_ENTITY_TO_FUNC(monster_barnacle);
   LINK_ENTITY_TO_FUNC(monster_barney);
   LINK_ENTITY_TO_FUNC(monster_barney_dead);
   LINK_ENTITY_TO_FUNC(monster_bigmomma);
   LINK_ENTITY_TO_FUNC(monster_bloater);
   LINK_ENTITY_TO_FUNC(monster_bullchicken);
   LINK_ENTITY_TO_FUNC(monster_cine2_hvyweapons);
   LINK_ENTITY_TO_FUNC(monster_cine2_scientist);
   LINK_ENTITY_TO_FUNC(monster_cine2_slave);
   LINK_ENTITY_TO_FUNC(monster_cine3_barney);
   LINK_ENTITY_TO_FUNC(monster_cine3_scientist);
   LINK_ENTITY_TO_FUNC(monster_cine_barney);
   LINK_ENTITY_TO_FUNC(monster_cine_panther);
   LINK_ENTITY_TO_FUNC(monster_cine_scientist);
   LINK_ENTITY_TO_FUNC(monster_cockroach);
   LINK_ENTITY_TO_FUNC(monster_flyer);
   LINK_ENTITY_TO_FUNC(monster_flyer_flock);
   LINK_ENTITY_TO_FUNC(monster_furniture);
   LINK_ENTITY_TO_FUNC(monster_gargantua);
   LINK_ENTITY_TO_FUNC(monster_generic);
   LINK_ENTITY_TO_FUNC(monster_gman);
   LINK_ENTITY_TO_FUNC(monster_grunt_repel);
   LINK_ENTITY_TO_FUNC(monster_headcrab);
   LINK_ENTITY_TO_FUNC(monster_hevsuit_dead);
   LINK_ENTITY_TO_FUNC(monster_hgrunt_dead);
   LINK_ENTITY_TO_FUNC(monster_houndeye);
   LINK_ENTITY_TO_FUNC(monster_human_assassin);
   LINK_ENTITY_TO_FUNC(monster_human_grunt);
   LINK_ENTITY_TO_FUNC(monster_ichthyosaur);
   LINK_ENTITY_TO_FUNC(monster_leech);
   LINK_ENTITY_TO_FUNC(monster_miniturret);
   LINK_ENTITY_TO_FUNC(monster_mortar);
   LINK_ENTITY_TO_FUNC(monster_nihilanth);
   LINK_ENTITY_TO_FUNC(monster_osprey);
   LINK_ENTITY_TO_FUNC(monster_rat);
   LINK_ENTITY_TO_FUNC(monster_satchel);
   LINK_ENTITY_TO_FUNC(monster_scientist);
   LINK_ENTITY_TO_FUNC(monster_scientist_dead);
   LINK_ENTITY_TO_FUNC(monster_sentry);
   LINK_ENTITY_TO_FUNC(monster_sitting_scientist);
   LINK_ENTITY_TO_FUNC(monster_snark);
   LINK_ENTITY_TO_FUNC(monster_tentacle);
   LINK_ENTITY_TO_FUNC(monster_tentaclemaw);
   LINK_ENTITY_TO_FUNC(monster_tripmine);
   LINK_ENTITY_TO_FUNC(monster_turret);
   LINK_ENTITY_TO_FUNC(monster_vortigaunt);
   LINK_ENTITY_TO_FUNC(monster_zombie);
   LINK_ENTITY_TO_FUNC(monstermaker);
   LINK_ENTITY_TO_FUNC(multi_manager);
   LINK_ENTITY_TO_FUNC(multisource);
   LINK_ENTITY_TO_FUNC(nihilanth_energy_ball);
   LINK_ENTITY_TO_FUNC(node_viewer);
   LINK_ENTITY_TO_FUNC(node_viewer_fly);
   LINK_ENTITY_TO_FUNC(node_viewer_human);
   LINK_ENTITY_TO_FUNC(node_viewer_large);
   LINK_ENTITY_TO_FUNC(path_corner);
   LINK_ENTITY_TO_FUNC(path_track);
   LINK_ENTITY_TO_FUNC(player);
   LINK_ENTITY_TO_FUNC(player_loadsaved);
   LINK_ENTITY_TO_FUNC(player_weaponstrip);
   LINK_ENTITY_TO_FUNC(rpg_rocket);
   LINK_ENTITY_TO_FUNC(scripted_sentence);
   LINK_ENTITY_TO_FUNC(scripted_sequence);
   LINK_ENTITY_TO_FUNC(soundent);
   LINK_ENTITY_TO_FUNC(spark_shower);
   LINK_ENTITY_TO_FUNC(speaker);
   LINK_ENTITY_TO_FUNC(squidspit);
   LINK_ENTITY_TO_FUNC(streak_spiral);
   LINK_ENTITY_TO_FUNC(target_cdaudio);
   LINK_ENTITY_TO_FUNC(test_effect);
   LINK_ENTITY_TO_FUNC(testhull);
   LINK_ENTITY_TO_FUNC(trigger);
   LINK_ENTITY_TO_FUNC(trigger_auto);
   LINK_ENTITY_TO_FUNC(trigger_autosave);
   LINK_ENTITY_TO_FUNC(trigger_camera);
   LINK_ENTITY_TO_FUNC(trigger_cdaudio);
   LINK_ENTITY_TO_FUNC(trigger_changelevel);
   LINK_ENTITY_TO_FUNC(trigger_changetarget);
   LINK_ENTITY_TO_FUNC(trigger_counter);
   LINK_ENTITY_TO_FUNC(trigger_endsection);
   LINK_ENTITY_TO_FUNC(trigger_gravity);
   LINK_ENTITY_TO_FUNC(trigger_hurt);
   LINK_ENTITY_TO_FUNC(trigger_monsterjump);
   LINK_ENTITY_TO_FUNC(trigger_multiple);
   LINK_ENTITY_TO_FUNC(trigger_once);
   LINK_ENTITY_TO_FUNC(trigger_push);
   LINK_ENTITY_TO_FUNC(trigger_relay);
   LINK_ENTITY_TO_FUNC(trigger_teleport);
   LINK_ENTITY_TO_FUNC(trigger_transition);
   LINK_ENTITY_TO_FUNC(weapon_357);
   LINK_ENTITY_TO_FUNC(weapon_9mmAR);
   LINK_ENTITY_TO_FUNC(weapon_9mmhandgun);
   LINK_ENTITY_TO_FUNC(weapon_crossbow);
   LINK_ENTITY_TO_FUNC(weapon_crowbar);
   LINK_ENTITY_TO_FUNC(weapon_egon);
   LINK_ENTITY_TO_FUNC(weapon_gauss);
   LINK_ENTITY_TO_FUNC(weapon_glock);
   LINK_ENTITY_TO_FUNC(weapon_handgrenade);
   LINK_ENTITY_TO_FUNC(weapon_hornetgun);
   LINK_ENTITY_TO_FUNC(weapon_mp5);
   LINK_ENTITY_TO_FUNC(weapon_python);
   LINK_ENTITY_TO_FUNC(weapon_rpg);
   LINK_ENTITY_TO_FUNC(weapon_satchel);
   LINK_ENTITY_TO_FUNC(weapon_shotgun);
   LINK_ENTITY_TO_FUNC(weapon_snark);
   LINK_ENTITY_TO_FUNC(weapon_tripmine);
   LINK_ENTITY_TO_FUNC(weaponbox);
   LINK_ENTITY_TO_FUNC(world_items);
   LINK_ENTITY_TO_FUNC(worldspawn);
   LINK_ENTITY_TO_FUNC(xen_hair);
   LINK_ENTITY_TO_FUNC(xen_hull);
   LINK_ENTITY_TO_FUNC(xen_plantlight);
   LINK_ENTITY_TO_FUNC(xen_spore_large);
   LINK_ENTITY_TO_FUNC(xen_spore_medium);
   LINK_ENTITY_TO_FUNC(xen_spore_small);
   LINK_ENTITY_TO_FUNC(xen_tree);
   LINK_ENTITY_TO_FUNC(xen_ttrigger);

   // Science and Industry
   LINK_ENTITY_TO_FUNC(ammo_rocketpistol);
   LINK_ENTITY_TO_FUNC(ammo_snUZI);
   LINK_ENTITY_TO_FUNC(ammo_tommygun);
   LINK_ENTITY_TO_FUNC(carry_resource);
   LINK_ENTITY_TO_FUNC(carry_scientist);
   LINK_ENTITY_TO_FUNC(emp_cannister);
   LINK_ENTITY_TO_FUNC(emp_pulse);
   LINK_ENTITY_TO_FUNC(env_model);
   LINK_ENTITY_TO_FUNC(fev_cloud);
   LINK_ENTITY_TO_FUNC(fev_vial);
   LINK_ENTITY_TO_FUNC(func_tech_breakable);
   LINK_ENTITY_TO_FUNC(info_administrator);
   LINK_ENTITY_TO_FUNC(info_scientist_dead);
   LINK_ENTITY_TO_FUNC(info_scientist_start);
   LINK_ENTITY_TO_FUNC(info_sidetect);
   LINK_ENTITY_TO_FUNC(info_transistor);
   LINK_ENTITY_TO_FUNC(item_resource);
   LINK_ENTITY_TO_FUNC(rocketpistol_rocket);
   LINK_ENTITY_TO_FUNC(timed_grenade);
   LINK_ENTITY_TO_FUNC(trigger_deliveryzone);
   LINK_ENTITY_TO_FUNC(weapon_briefcase);
   LINK_ENTITY_TO_FUNC(weapon_cloak);
   LINK_ENTITY_TO_FUNC(weapon_colt);
   LINK_ENTITY_TO_FUNC(weapon_empcannon);
   LINK_ENTITY_TO_FUNC(weapon_experimental);
   LINK_ENTITY_TO_FUNC(weapon_fleshgrenade);
   LINK_ENTITY_TO_FUNC(weapon_generic);
   LINK_ENTITY_TO_FUNC(weapon_mindray);
   LINK_ENTITY_TO_FUNC(weapon_resource);
   LINK_ENTITY_TO_FUNC(weapon_rocketpistol);
   LINK_ENTITY_TO_FUNC(weapon_snUZI);
   LINK_ENTITY_TO_FUNC(weapon_tommygun);
   LINK_ENTITY_TO_FUNC(weapon_transistor);
   LINK_ENTITY_TO_FUNC(weapon_vomit);
#endif
>>>>>>> 3f195a42c7b84bf4071b789634ebbe14b6297ebc
