/* strxcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRXCMP_INCLUDE
#define	STRXCMP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<strings.h>		/* |strcasecmp(3c)| + |strcmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	<stremacmp.h>
#include	<strkeycmp.h>
#include	<strvalcmp.h>
#include	<strfoldcmp.h>
#include	<strleadcmp.h>
#include	<strpcmp.h>
#include	<strnxcmp.h>		/* |strnndictcmp(3uc)| */


EXTERNC_begin

local inline int strbasecmp(cchar *s1,cchar *s2) noex {
    	return strcmp(s1,s2) ;
}

local inline int strdictcmp(cchar *s1,cchar *s2) noex {
	return strnndictcmp(s1,-1,s2,-1) ;
}

EXTERNC_end


#endif /* STRXCMP_INCLUDE */


