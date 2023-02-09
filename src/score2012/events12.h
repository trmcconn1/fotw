
/* Make sure dmr is always the last event listed */

const char *EventNames[]={"55m","100m","200m","257.627m","400m","515.254m","772.881m","800m","1500m","mile","3k","5k","8k","5M","10k",
	"15k","10M","hmar","mar","400r","800r","4x1manley","1600r","4x2manley","4x3manley","3200r","dmr"};
/* N.B. The order of relay events should be in the order their leg-lengths
occur */
/* Warning: if you ever add an event between standard and manley equivalent events,
all the delicate indexing manoevers  will break! */