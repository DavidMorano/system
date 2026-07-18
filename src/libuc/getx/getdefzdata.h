/* getdefzdata HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* this is supposed to provide an OS-independent time management operation */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETDEFZDATA_INCLUDE
#define	GETDEFZDATA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	DEFZDATA		struct defzdata_head


struct defzdata_head {
	int	zoff  ;		/* minutes west of GMT */
	int	isdst ;		/* is-dst flag */
} ; /* end struct (defzdata_head) */

#ifdef	__cplusplus
struct defzdata : defzdata_head {
    	defzdata() = default ;
	defzdata(const defzdata &) = delete ;
	defzdata &operator = (const defzdata &) = delete ;
	int get(char *,int,int) noex ;
} ; /* end struct (defzdata) */
#else	/* __cplusplus */
typedef ZDEFINFO	defzdata ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	getdefzdata(defzdata *,char *,int,int) noex ;

EXTERNC_end


#endif /* GETDEFZDATA_INCLUDE */


