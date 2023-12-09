/* ucenumnw INCLUDE */
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

#ifndef	UCENUMNW_INCLUDE
#define	UCENUMNW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentnw.h>		/* <- money shot */


#define	UCENUMNW		struct ucenumnw_head
#define	UCENUMNW_MAGIC		0x88776281
#define	UCENUMNW_FNAME		"/sysdb/network"


typedef struct ucenumxx_head	ucenumnw ;
typedef ucentnw			ucenumnw_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int ucenumnw_open(ucenumnw *,cchar *) noex ;
extern int ucenumnw_close(ucenumnw *) noex ;
extern int ucenumnw_readent(ucenumnw *,ucenumnw_ent *,char *,int) noex ;
extern int ucenumnw_reset(ucenumnw *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCENUMNW_INCLUDE */


