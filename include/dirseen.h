/* dirseen INCLUDE */
/* lang=C20 */


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
#define	DIRSEEN_CUR	struct dirseen_c
#define	DIRSEEN_MAGIC	0x09854123
#define	DIRSEEN_NDEF	10


struct dirseen_c {
	int		i ;
} ;

struct dirseen_head {
	uint		magic ;
	VECOBJ		list ;
	int		strsize ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int dirseen_start(DIRSEEN *) noex ;
extern int dirseen_add(DIRSEEN *,cchar *,int,USTAT *) noex ;
extern int dirseen_havename(DIRSEEN *,cchar *,int) noex ;
extern int dirseen_havedevino(DIRSEEN *,USTAT *) noex ;
extern int dirseen_count(DIRSEEN *) noex ;
extern int dirseen_curbegin(DIRSEEN *,DIRSEEN_CUR *) noex ;
extern int dirseen_curend(DIRSEEN *,DIRSEEN_CUR *) noex ;
extern int dirseen_enum(DIRSEEN *,DIRSEEN_CUR *,char *,int) noex ;
extern int dirseen_finish(DIRSEEN *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* DIRSEEN_INCLUDE */


