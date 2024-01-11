/* mkcmds SUPPORT */
/* lang=C20 */

/* names string storiage: purpose? */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	MKTXTINDEXCMDS_INCLUDE
#define	MKTXTINDEXCMDS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


enum mkcmds {
	mkcmd_minwordlen,
	mkcmd_maxwordlen,
	mkcmd_eigenwords,
	mkcmd_nkeys,
	mkcmd_tablen,
	mkcmd_sdn,
	mkcmd_sfn,
	mkcmd_lang,
	mkcmd_overlast
} ;


extern cpcchar		mkcmds[] ;


#endif /* MKTXTINDEXCMDS_INCLUDE */


