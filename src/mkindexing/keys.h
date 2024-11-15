/* keys */


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

#include	"proginfo.h"


#ifdef	__cplusplus
extern "C" {
#endif

extern int keys_begin(PROGINFO *,HDB *,int) noex ;
extern int keys_add(PROGINFO *,HDB *,cchar *,int) noex ;
extern int keys_end(PROGINFO *,HDB *,bfile *,PTM *,cchar *,off_t,int) noex ;

#ifdef	__cplusplus
}
#endif

#endif /* KEYS_INCLUDE */


