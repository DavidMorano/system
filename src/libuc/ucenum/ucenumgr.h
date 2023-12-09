/* ucenumgr INCLUDE */
/* lang=C20 */

/* enumerate system SYSDB-XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMGR_INCLUDE
#define	UCENUMGR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentgr.h>		/* <- money shot */


#define	UCENUMGR		struct ucenumgr_head
#define	UCENUMGR_MAGIC		0x88776281
#define	UCENUMGR_FNAME		"/sysdb/group"


typedef struct ucenumxx_head	ucenumgr ;
typedef ucentgr			ucenumgr_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int ucenumgr_open(ucenumgr *,cchar *) noex ;
extern int ucenumgr_close(ucenumgr *) noex ;
extern int ucenumgr_readent(ucenumgr *,ucenumgr_ent *,char *,int) noex ;
extern int ucenumgr_reset(ucenumgr *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCENUMGR_INCLUDE */


