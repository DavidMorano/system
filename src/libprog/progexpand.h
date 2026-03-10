/* progexpand HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle the keys while processing a file */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGEXPAND_INCLUDE
#define	PROGEXPAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	"proginfo.hh"


EXTERNC_begin

extern int progexpand(proginfo *,char *,int,cc *,int) noex ;

EXTERNC_end


#endif /* PROGEXPAND_INCLUDE */


