/* cmpstrvaln HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMPSTRVALN_INCLUDE
#define	CMPSTRVALN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
    extern int	cmpstrvaln	(cchar *,cchar *,int) noex ;
EXTERNC_end

#ifdef	__cplusplus
    inline int	cmpstrvaln	(cchar *s1,cchar *s2) noex {
    	return cmpstrvaln(s1,s2,-1) ;
    } /* end */
#endif /* __cplusplus */


#endif /* CMPSTRVALN_INCLUDE */


