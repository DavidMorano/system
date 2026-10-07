


/* local variables */

static cchar	*exts[] = {
	".so",
	".o",
	"",
	NULL
} ;

static cchar	*dirs64[] = {
	"lib/dialers/sparcv9",
	"lib/dialers/sparc",
	"lib/dialers",
	NULL
} ;

static cchar	*dirs32[] = {
	"lib/dialers/sparcv8",
	"lib/dialers/sparcv7",
	"lib/dialers/sparc",
	"lib/dialers",
	NULL
} ;

static cchar	*subs[] = {
	"init",
	"check",
	"free",
	NULL
} ;

enum subs {
	sub_init,
	sub_check,
	sub_free,
} ;




