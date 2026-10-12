/* emailaddra_local HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* E-Mail-Address program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/
/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	EMAILADDRLOCAL_INCLUDE
#define	EMAILADDRLOCAL_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	CMD_FL		struct cmd_flags
#define	CMD_AFL		struct cmd_aflags
#define	CMD_LOC		struct cmd_local


struct cmd_flags {
	uint		count:1 ;
	uint		info:1 ;
	uint		expand:1 ;		/* also 'flat' */
} ; /* end struct */

struct cmd_aflags {
	uint		address:1 ;
	uint		route:1 ;
	uint		comment:1 ;
	uint		best:1 ;
	uint		original:1 ;
	uint		anyone:1 ;
} ; /* end struct */

struct cmd_local {
	CMD_FL		fl ;
	CMD_AFL		afl ;
	int		spc ;
} ; /* end struct */

typedef	CMD_FL		cmd_fl ;
typedef	CMD_AFL		cmd_afl ;
typedef	CMD_LOC		cmd_loc ;


#endif /* EMAILADDRLOCAL_INCLUDE */


