/* removename HEADER */
/* lang=C20 */

/* remove a named file-system object (and its descendants) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	REMOVENAME_INCLUDE
#define	REMOVENAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<randomvar.h>


/* object defines (options) */
#define	REMOVENAME_MDEFAULT	0
#define	REMOVENAME_MBURN	(1 << 0)	/* "burn" the file */
#define	REMOVENAME_MFOLLOW	(1 << 1)	/* follow symbolic links */


EXTERNC_begin

extern int	removename(cchar *,int,randomvar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int removename(cchar *n,int o = 0,randomvar *p = nullptr) noex {
	cint		bc = (o & REMOVENAME_MBURN) ? 1 : 0 ;
	return removename(n,o,p,bc) ;
}

#endif /* __cplusplus */


#endif /* REMOVENAME_INCLUDE */


