/* getfname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a file name according to rules */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETFNAME_INCLUDE
#define	GETFNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum getfnames {
    	getfname_local,
    	getfname_root,
    	getfname_overlast
} ; /* end enum (getfnames) */

#ifndef	GETFNAME_TYPELOCAL
#define	GETFNAME_TYPEUNKNOWN	-1
#define	GETFNAME_TYPELOCAL	getfname_local
#define	GETFNAME_TYPEROOT	getfname_root
#endif

#ifdef	__cplusplus

extern "C" {
    extern int getfname(cc *,char *,getfnames,cc *,int = 0) noex ;
}

#else /* __cplusplus */

extern int getfname(cchar *,char *,int,cchar *,int) noex ;

#endif /* __cplusplus */


#endif /* GETFNAME_INCLUDE */


