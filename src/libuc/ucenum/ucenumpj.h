/* ucenumpj HEADER */
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

#ifndef	UCENUMPJ_INCLUDE
#define	UCENUMPJ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentpj.h>		/* <- money shot */


#define	UCENUMPJ		struct ucenumpj_head
#define	UCENUMPJ_MAGIC		0x88776216
#define	UCENUMPJ_FNAME		"/sysdb/project"


typedef struct ucenumxx_head	ucenumpj ;
typedef ucentpj			ucenumpj_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int ucenumpj_open(ucenumpj *,cchar *) noex ;
extern int ucenumpj_close(ucenumpj *) noex ;
extern int ucenumpj_readent(ucenumpj *,ucenumpj_ent *,char *,int) noex ;
extern int ucenumpj_reset(ucenumpj *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCENUMPJ_INCLUDE */


