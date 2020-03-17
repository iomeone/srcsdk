#define ABSOLUTE_PLAYER_LIMIT 255  // not 256, so we can send the limit as a byte  

#define NUM_ENT_ENTRY_BITS		(MAX_EDICT_BITS + 1)
#define ENT_ENTRY_MASK			(NUM_ENT_ENTRIES - 1)
#define NUM_SERIAL_NUM_BITS		(32 - NUM_ENT_ENTRY_BITS)