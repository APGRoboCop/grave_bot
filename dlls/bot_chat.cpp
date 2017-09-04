//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_combat.cpp
//

#ifndef _WIN32
#include <string.h>
#endif

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


#define NUM_TAGS 22

char *tag1[NUM_TAGS]={
"-=","-[","-]","-}","-{","<[","<]","[-","]-","{-","}-","[[","[","{","]","}","<",">","-","|","=","+"};
char *tag2[NUM_TAGS]={
"=-","]-","[-","{-","}-","]>","[>","-]","-[","-}","-{","]]","]","}","[","{",">","<","-","|","=","+"};

bot_chat_t bot_chat[BOT_TOTAL_CHAT_TYPES][MAX_BOT_CHAT];
int recent_bot_chat[BOT_TOTAL_CHAT_TYPES][5];
int bot_chat_count[BOT_TOTAL_CHAT_TYPES];

int player_count;
char player_names[32][33];  // 32 players max, 32 chars + null

extern int bot_chat_tag_percent;
extern int bot_chat_drop_percent;
extern int bot_chat_swap_percent;
extern int bot_chat_lower_percent;

int BotChatTypeToIndex( const char *pszType )
{
	int type = BOT_CHAT_UNKNOWN;

	if (strcmp(pszType, "idle") == 0)
		type = BOT_CHAT_IDLE;
	else if (strcmp(pszType, "combat") == 0)
		type = BOT_CHAT_COMBAT;
	else if (strcmp(pszType, "frag") == 0)
		type = BOT_CHAT_FRAG;
	else if (strcmp(pszType, "fragged") == 0)
		type = BOT_CHAT_FRAGGED;
	else if (strcmp(pszType, "suicide") == 0)
		type = BOT_CHAT_SUICIDE;
	else if (strcmp(pszType, "killsci") == 0)
		type = BOT_CHAT_KILLSCI;	
	else if (strcmp(pszType, "getsci") == 0)
		type = BOT_CHAT_GETSCI;
	else if (strcmp(pszType, "returnsci") == 0)
		type = BOT_CHAT_RETURNSCI;
	else if (strcmp(pszType, "getrsrc") == 0)
		type = BOT_CHAT_GETRSRC;
	else if (strcmp(pszType, "returnrsrc") == 0)
		type = BOT_CHAT_RETURNRSRC;
	else if (strcmp(pszType, "breaktech") == 0)
		type = BOT_CHAT_BREAKTECH;

	return type;
}

char *BotChatTypeToString( int iType )
{
	switch (iType)
	{
		case BOT_CHAT_IDLE:			return "idle";
		case BOT_CHAT_COMBAT:		return "combat";
		case BOT_CHAT_FRAG:			return "frag";
		case BOT_CHAT_FRAGGED:		return "fragged";
		case BOT_CHAT_SUICIDE:		return "suicide";
		case BOT_CHAT_KILLSCI:		return "killsci";
		case BOT_CHAT_GETSCI:		return "getsci";
		case BOT_CHAT_RETURNSCI:	return "returnsci";
		case BOT_CHAT_GETRSRC:		return "getrsrc";
		case BOT_CHAT_RETURNRSRC:	return "returnrsrc";
		case BOT_CHAT_BREAKTECH:	return "breaktech";
	}

	return "unknown";
}

void LoadBotChat(void)
{
	FILE *bfp;
	char filename[256];
	char buffer[256];
	char temp[128];
	char *stat;
	int section = -1;
	int i, j, length;
	
	for (i = 0; i < BOT_TOTAL_CHAT_TYPES; i++)
	{	// no count
		bot_chat_count[i] = 0;
		// no recent chatting either
		for (j = 0; j < 5; j++)
			recent_bot_chat[i][j] = -1;
	}
	
	UTIL_BuildFileName(filename, "grave_bot_chat.txt", NULL);
	
	bfp = fopen(filename, "r");
	
	while (bfp != NULL)
	{
		stat = fgets(buffer, 128, bfp);
		
		if (stat == NULL)
		{
			fclose(bfp);
			bfp = NULL;
			continue;
		}
		
		buffer[127] = 0;  // truncate lines longer than 128 characters
		
		length = strlen(buffer);
		
		if (buffer[length-1] == '\n')
		{
			buffer[length-1] = 0;  // remove '\n'
			length--;
		}
		
		// chat type section begin
		if (buffer[0] == '[' && buffer[length-1] == ']')
		{
			BotChatTrimTag(buffer, temp);
			section = BotChatTypeToIndex(temp);
			continue;
		}

		if ((length > 0) && (bot_chat_count[section] < MAX_BOT_CHAT))
		{
			if (buffer[0] == '!')
			{
				strcpy(bot_chat[section][bot_chat_count[section]].text, &buffer[1]);
				bot_chat[section][bot_chat_count[section]].can_modify = FALSE;
			}
			else
			{
				strcpy(bot_chat[section][bot_chat_count[section]].text, buffer);
				bot_chat[section][bot_chat_count[section]].can_modify = TRUE;
			}
			
			bot_chat_count[section]++;
		}
	}

	SERVER_PRINT( "BotLoadChat complete...\n");

	for (i = 0; i < BOT_TOTAL_CHAT_TYPES; i++)
		SERVER_PRINT( "%i %s lines...\n", bot_chat_count[i], BotChatTypeToString(i));
}


void BotTrimBlanks(char *in_string, char *out_string)
{
	int i, pos;
	char *dest;
	
	pos=0;
	while ((pos < 80) && (in_string[pos] == ' '))  // skip leading blanks
		pos++;
	
	dest=&out_string[0];
	
	while ((pos < 80) && (in_string[pos]))
	{
		*dest++ = in_string[pos];
		pos++;
	}
	*dest = 0;  // store the null
	
	i = strlen(out_string) - 1;
	while ((i > 0) && (out_string[i] == ' '))  // remove trailing blanks
	{
		out_string[i] = 0;
		i--;
	}
}


int BotChatTrimTag(char *original_name, char *out_name)
{
	int i;
	char *pos1, *pos2, *src, *dest;
	char in_name[80];
	int result = 0;
	
	strncpy(in_name, original_name, 31);
	in_name[32] = 0;
	
	for (i=0; i < NUM_TAGS; i++)
	{
		pos1=strstr(in_name, tag1[i]);
		if (pos1)
			pos2=strstr(pos1+strlen(tag1[i]), tag2[i]);
		else
			pos2 = NULL;
		
		if (pos1 && pos2 && pos1 < pos2)
		{
			src = pos2+strlen(tag2[i]);
			dest = pos1;
			while (*src)
				*dest++ = *src++;
			*dest = *src;  // copy the null;
			
			result = 1;
		}
	}
	
	strcpy(out_name, in_name);
	
	BotTrimBlanks(out_name, in_name);
	
	if (strlen(in_name) == 0)  // is name just a tag?
	{
		strncpy(in_name, original_name, 31);
		in_name[32] = 0;
		
		// strip just the tag part...
		for (i=0; i < NUM_TAGS; i++)
		{
			pos1=strstr(in_name, tag1[i]);
			if (pos1)
				pos2=strstr(pos1+strlen(tag1[i]), tag2[i]);
			else
				pos2 = NULL;
			
			if (pos1 && pos2 && pos1 < pos2)
			{
				src = pos1 + strlen(tag1[i]);
				dest = pos1;
				while (*src)
					*dest++ = *src++;
				*dest = *src;  // copy the null;
				
				src = pos2 - strlen(tag2[i]);
				*src = 0; // null out the rest of the string
			}
		}
	}
	
	BotTrimBlanks(in_name, out_name);
	
	out_name[31] = 0;
	
	return (result);
}


void BotDropCharacter(char *in_string, char *out_string)
{
	int len, pos;
	int count = 0;
	char *src, *dest;
	bool is_bad;
	
	strcpy(out_string, in_string);
	
	len = strlen(out_string);
	pos = RANDOM_LONG(1, len-1);  // don't drop position zero
	
	is_bad = !isalpha(out_string[pos]) || (out_string[pos-1] == '%');
	
	while ((is_bad) && (count < 20))
	{
		pos = RANDOM_LONG(1, len-1);
		is_bad = !isalpha(out_string[pos]) || (out_string[pos-1] == '%');
		count++;
	}
	
	if (count < 20)
	{
		src = &out_string[pos+1];
		dest = &out_string[pos];
		while (*src)
			*dest++ = *src++;
		*dest = *src;  // copy the null;
	}
}


void BotSwapCharacter(char *in_string, char *out_string)
{
	int len, pos;
	int count = 0;
	char temp;
	bool is_bad;
	
	strcpy(out_string, in_string);
	
	len = strlen(out_string);
	pos = RANDOM_LONG(1, len-2);  // don't swap position zero
	
	is_bad = !isalpha(out_string[pos]) || !isalpha(out_string[pos+1]) ||
		(out_string[pos-1] == '%');
	
	while ((is_bad) && (count < 20))
	{
		pos = RANDOM_LONG(1, len-2);
		is_bad = !isalpha(out_string[pos]) || !isalpha(out_string[pos+1]) ||
			(out_string[pos-1] == '%');
		count++;
	}
	
	if (count < 20)
	{
		temp = out_string[pos];
		out_string[pos] = out_string[pos+1];
		out_string[pos+1] = temp;
	}
}


void BotChatName(char *original_name, char *out_name)
{
	int pos;
	
	if (RANDOM_LONG(1, 100) <= bot_chat_tag_percent)
	{
		char temp_name[80];
		
		strncpy(temp_name, original_name, 31);
		temp_name[31] = 0;
		
		while (BotChatTrimTag(temp_name, out_name))
		{
			strcpy(temp_name, out_name);
		}
	}
	else
	{
		strncpy(out_name, original_name, 31);
		out_name[31] = 0;
	}
	
	if (RANDOM_LONG(1, 100) <= bot_chat_lower_percent)
	{
		pos=0;
		while ((pos < 80) && (out_name[pos]))
		{
			out_name[pos] = tolower(out_name[pos]);
			pos++;
		}
	}
}


void BotChatText(char *in_text, char *out_text)
{
	int pos;
	char temp_text[81];
	int count;
	
	strncpy(temp_text, in_text, 79);
	temp_text[80] = 0;
	
	if (RANDOM_LONG(1, 100) <= bot_chat_drop_percent)
	{
		count = RANDOM_LONG(1, 3);
		
		while (count)
		{
			BotDropCharacter(temp_text, out_text);
			strcpy(temp_text, out_text);
			count--;
		}
	}
	
	if (RANDOM_LONG(1, 100) <= bot_chat_swap_percent)
	{
		count = RANDOM_LONG(1, 2);
		
		while (count)
		{
			BotSwapCharacter(temp_text, out_text);
			strcpy(temp_text, out_text);
			count--;
		}
	}
	
	if (RANDOM_LONG(1, 100) <= bot_chat_lower_percent)
	{
		pos=0;
		while (temp_text[pos])
		{
			temp_text[pos] = tolower(temp_text[pos]);
			pos++;
		}
	}
	
	strcpy(out_text, temp_text);
}


void BotChatGetPlayers(void)
{
	int index;
	const char *pName;
	
	player_count = 0;
	
	for (index = 1; index <= gpGlobals->maxClients; index++)
	{
		edict_t *pPlayer = INDEXENT(index);
		
		// skip invalid players
		if ((pPlayer) && (!pPlayer->free))
		{
			if (pPlayer->v.netname)
			{
				pName = STRING(pPlayer->v.netname);
				
				if (*pName != 0)
				{
					strncpy(player_names[player_count], pName, 32);
					
					player_count++;
				}
			}
		}
	}
}


void BotChatFillInName(char *bot_say_msg, char *chat_text,
                       char *chat_name, const char *bot_name)
{
	int chat_index, say_index;
	char *name_pos, *rand_pos;
	char random_name[64];
	int index, name_offset, rand_offset;
	bool is_bad;
	
	chat_index = 0;
	say_index = 0;
	bot_say_msg[0] = 0;
	
	name_pos = strstr(&chat_text[chat_index], "%n");
	rand_pos = strstr(&chat_text[chat_index], "%r");
	
	while ((name_pos != NULL) || (rand_pos != NULL))
	{
		if (name_pos != NULL)
			name_offset = name_pos - chat_text;
		if (rand_pos != NULL)
			rand_offset = rand_pos - chat_text;
		
		if ((rand_pos == NULL) ||
			((name_offset < rand_offset) && (name_pos != NULL)))
		{
			while (&chat_text[chat_index] < name_pos)
				bot_say_msg[say_index++] = chat_text[chat_index++];
			
			bot_say_msg[say_index] = 0;  // add null terminator
			
			chat_index += 2;  // skip the "%n"
			
			strcat(bot_say_msg, chat_name);
			say_index += strlen(chat_name);
			
			bot_say_msg[say_index] = 0;
		}
		else  // use random player name...
		{
			int count = 0;
			
			BotChatGetPlayers();
			
			// pick a name at random from the list of players...
			
			index = RANDOM_LONG(0, player_count-1);
			
			is_bad = (strcmp(player_names[index], chat_name) == 0) ||
				(strcmp(player_names[index], bot_name) == 0);
			
			while ((is_bad) && (count < 20))
			{
				index = RANDOM_LONG(0, player_count-1);
				
				is_bad = (strcmp(player_names[index], chat_name) == 0) ||
					(strcmp(player_names[index], bot_name) == 0);
				
				count++;
			}
			
			BotChatName(player_names[index], random_name);
			
			while (&chat_text[chat_index] < rand_pos)
				bot_say_msg[say_index++] = chat_text[chat_index++];
			
			bot_say_msg[say_index] = 0;  // add null terminator
			
			chat_index += 2;  // skip the "%r"
			
			strcat(bot_say_msg, random_name);
			say_index += strlen(random_name);
			
			bot_say_msg[say_index] = 0;
		}
		
		name_pos = strstr(&chat_text[chat_index], "%n");
		rand_pos = strstr(&chat_text[chat_index], "%r");
	}
	
	// copy the rest of the chat_text into the bot_say_msg...
	
	while (chat_text[chat_index])
		bot_say_msg[say_index++] = chat_text[chat_index++];
	
	bot_say_msg[say_index] = 0;  // add null terminator
}

int BotChatGetRandomIndex( int chat_type )
{
	int recent_index = 0;
	bool used;
	int i, recent_count;
					
	recent_count = 0;
					
	while (recent_count < 5)
	{
		recent_index = RANDOM_LONG(0, bot_chat_count[chat_type]-1);
						
		used = FALSE;
						
		for (i=0; i < 5; i++)
		{
			if (recent_bot_chat[chat_type][i] == recent_index)
				used = TRUE;
		}
						
		if (used)
			recent_count++;
		else
			break;
	}
					
	for (i = 4; i > 0; i--)
		recent_bot_chat[chat_type][i] = recent_bot_chat[chat_type][i-1];
					
	recent_bot_chat[chat_type][0] = recent_index;

	return recent_index;
}