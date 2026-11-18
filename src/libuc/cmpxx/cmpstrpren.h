/* cmpstrpren HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRPREN_INCLUDE
#define	CMPSTRPREN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern int	cmpstrpren	(cchar *,cchar *,int) noex ;
EXTERNC_end

#ifdef	__cplusplus
inline int	cmpstrpren	(cchar *s1,cchar *s2) noex {
    	return cmpstrpren(s1,s2,-1) ;
} /* end */
#endif /* __cplusplus */


#endif /* CMPSTRPREN_INCLUDE */


