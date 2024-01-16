/* dirseen HEADER */
/* lang=C20 */

/* directory list manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2006-09-10, David A­D­ Morano
	I created this from hacking something that was similar that was
	originally created for a PCS program.

*/

/* Copyright © 2006 David A­D­ Morano.  All rights reserved. */

#ifndef	DIRSEEN_INCLUDE
#define	DIRSEEN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>
#include	<vecobj.h>
#include	<localmisc.h>


#define	DIRSEEN		struct dirseen_head
#define	DIRSEEN_CUR	struct dirseen_cursor
#define	DIRSEEN_MAGIC	0x09854123
#define	DIRSEEN_NDEF	10


struct dirseen_cursor {
	int		i ;
} ;

struct dirseen_head {
	vecobj		*dlistp ;
	uint		magic ;
	int		strsize ;
} ;

typedef DIRSEEN		dirseen ;
typedef DIRSEEN_CUR	dirseen_cur ;

EXTERNC_begin

extern int dirseen_start(dirseen *) noex ;
extern int dirseen_add(dirseen *,cchar *,int,USTAT *) noex ;
extern int dirseen_havename(dirseen *,cchar *,int) noex ;
extern int dirseen_havedevino(dirseen *,USTAT *) noex ;
extern int dirseen_count(dirseen *) noex ;
extern int dirseen_curbegin(dirseen *,dirseen_cur *) noex ;
extern int dirseen_curend(dirseen *,dirseen_cur *) noex ;
extern int dirseen_curenum(dirseen *,dirseen_cur *,char *,int) noex ;
extern int dirseen_finish(dirseen *) noex ;

EXTERNC_end


#endif /* DIRSEEN_INCLUDE */


