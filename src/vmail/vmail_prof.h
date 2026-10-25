/* vmail_prof HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* test program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#define DRDMAILOPTS "+all_scan:+long_scan:+confirm:+page"

 /* user profile options */
struct profopts {
	char	name[30] ;
	int	value ;
} ; /* end struct (profopts) */


 /* user profile options  (with defaults, 1=on  0=off) */
static struct profopts	userprof[] = {
	{ "all_scan",	0	},	/* scan mailbox when enter */
	{ "each_scan",	0	},	/* scan current mess before prompt */
	{"long_scan",	1	},	/* 80 column scan (no=50 col) */
	{ "fifo",	1	},	/* mess ordering (no=lifo) */
	{ "sort_all",	0	},	/* presort mess (no=in newbox only) */
	{ "confirm",	0	},	/* ask before deleting mess at quit */
	{ "convert",    0	},	/* convert UNIX message formats */
	{ "page",	0	},	/* display one page at a time*/
	{ "replyinc",	0	},	/* include original message in reply */
	{ "", 0		 	}	/* end senital */
} ; /* end array */



