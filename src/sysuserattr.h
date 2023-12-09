/* ucenumua INCLUDE */
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

#ifndef	UCENUMUA_INCLUDE
#define	UCENUMUA_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<userattr.h>		/* <- ?? */

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentua.h>		/* <- money shot */


#define	UCENUMUA		struct ucenumua_head
#define	UCENUMUA_MAGIC		0x88776217
#define	UCENUMUA_FNAME		"/sysdb/userattr"


typedef struct ucenumxx_head	ucenumua ;
typedef ucentua			ucenumua_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int ucenumua_open(ucenumua *,cchar *) noex ;
extern int ucenumua_close(ucenumua *) noex ;
extern int ucenumua_readent(ucenumua *,ucenumua_ent *,char *,int) noex ;
extern int ucenumua_reset(ucenumua *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCENUMUA_INCLUDE */


