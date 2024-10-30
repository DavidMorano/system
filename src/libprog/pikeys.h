/* keys HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	KEYS_INCLUDE
#define	KEYS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bfile.h>
#include	<hdb.h>
#include	<ptm.h>


EXTERNC_begin

extern int keys_begin(proginfo *,hdb *,int) noex ;
extern int keys_add(proginfo *,hdb *,cchar *,int) noex ;
extern int keys_end(proginfo *,hdb *,bfile *,ptm *,cchar *,off_t,int) noex ;

EXTERNC_end


#endif /* KEYS_INCLUDE */


