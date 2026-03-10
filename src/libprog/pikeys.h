/* pikeys HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle the keys while processing a file */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PIKEYS_INCLUDE
#define	PIKEYS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<hdb.h>
#include	<ptm.h>

#include	"proginfo.h"


EXTERNC_begin

extern int keys_begin(proginfo *,hdb *,int) noex ;
extern int keys_add(proginfo *,hdb *,cchar *,int) noex ;
extern int keys_end(proginfo *,hdb *,bfile *,ptm *,cchar *,off_t,int) noex ;

EXTERNC_end


#endif /* PIKEYS_INCLUDE */


