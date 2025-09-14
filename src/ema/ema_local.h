/* ema_local HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	EMALOCAL_INCLUDE
#define	EMALOCAL_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


#define	CMD_FL		struct cmd_flags
#define	CMD_AFL		struct cmd_aflags
#define	CMD_LOC		struct cmd_local


struct cmd_flags {
	uint		count:1 ;
	uint		info:1 ;
	uint		expand:1 ;		/* also 'flat' */
} ;

struct cmd_aflags {
	uint		address:1 ;
	uint		route:1 ;
	uint		comment:1 ;
	uint		best:1 ;
	uint		original:1 ;
	uint		anyone:1 ;
} ;

struct cmd_local {
	CMD_FL		fl ;
	CMD_AFL		afl ;
	int		spc ;
} ;

typedef	CMD_FL		cmd_fl ;
typedef	CMD_AFL		cmd_afl ;
typedef	CMD_LOC		cmd_loc ;


#endif /* EMALOCAL_INCLUDE */


