/* cmpstrnn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRNN_INCLUDE
#define	CMPSTRNN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern int	cmpstrnn	(cchar *,int,cchar *,int) noex ;
EXTERNC_end

#ifdef	__cplusplus
inline int	cmpstrnn	(cchar *s1,int s1l,cchar *s2) noex {
    	return cmpstrnn(s1,s1l,s2,-1) ;
} /* end */
#endif /* __cplusplus */


#endif /* CMPSTRNN_INCLUDE */


