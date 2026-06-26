/* pikeys HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle the keys while processing a file */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PIKEYS_INCLUDE
#define	PIKEYS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX system types */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<hdb.h>			/* LIBUC */

#include	"proginfo.hh"


EXTERNC_begin

extern int keys_begin(proginfo *,hdb *,int) noex ;
extern int keys_add(proginfo *,hdb *,cchar *,int) noex ;
extern int keys_end(proginfo *,hdb *,bfile *,ptm *,cchar *,off_t,int) noex ;

EXTERNC_end


#endif /* __cplusplus */
#endif /* PIKEYS_INCLUDE */


