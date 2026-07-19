/* removename HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* remove a named file-system object (and its descendants) */
/* version %I% last-modified %G% */


/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

#ifndef	REMOVENAME_INCLUDE
#define	REMOVENAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<randomvar.h>		/* LIBUC */


enum removenameos {
    	removenameo_burn,			/* "burn" the file */
    	removenameo_follow,			/* follow symbolic links */
    	removenameo_overflow
} ; /* end enum */

#ifdef	__cplusplus
struct removenameoms {
    	static constexpr int removenameom_burn	 = (1 << removenameo_burn) ;
    	static constexpr int removenameom_follow = (1 << removenameo_follow) ;
} ; /* end struct */
#endif /* __cplusplus */

/* object defines (options) */
#define	REMOVENAME_MDEFAULT	0
#define	REMOVENAME_MBURN	(1 << removenameo_burn)
#define	REMOVENAME_MFOLLOW	(1 << removenameo_follow)


EXTERNC_begin

extern int	removename(cchar *,int,randomvar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int removename(cchar *n,int o = 0,randomvar *p = nullptr) noex {
	cint		bc = (o & REMOVENAME_MBURN) ? 1 : 0 ;
	return removename(n,o,p,bc) ;
} /* end subroutine */

#endif /* __cplusplus */

#ifdef	__cplusplus
extern const removenameoms	removenamem ;
#endif /* __cplusplus */


#endif /* REMOVENAME_INCLUDE */


