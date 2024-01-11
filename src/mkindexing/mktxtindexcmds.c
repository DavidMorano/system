/* mkcmds HEADER */
/* lang=C20 */

/* names string storiage: purpose? */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKTXTINDEXCMDS_INCLUDE
#define	MKTXTINDEXCMDS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


const cpcchar	mkcmds[] = {
	"minwordlen",
	"maxwordlen",
	"eigenwords",
	"nkeys",
	"tablen",
	"sdn",
	"sfn",
	"lang",
	NULL
} ; /* end if (mkcmds) */


#endif /* MKTXINDEXCMDS_INCLUDE */


