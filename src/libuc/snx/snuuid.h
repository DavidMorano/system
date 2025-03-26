/* snuuid HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* string-UUID (String-UUID) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNUUID_INCLUDE
#define	SNUUID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkuuid.h>		/* |uuid_dat| below */


EXTERNC_begin

extern int snuuid(char *,int,uuid_dat *) noex ;

EXTERNC_end


#endif /* SNUUID_INCLUDE */


