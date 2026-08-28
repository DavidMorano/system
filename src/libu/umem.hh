/* umem HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UMEM_INCLUDE
#define	UMEM_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


namespace libu {
    struct umems {
        int strw	(cchar *,int,cchar **) noex ;
        int mall	(int,void *) noex ;
        int vall	(int,void *) noex ;
        int call	(int,int,void *) noex ;
        int rall	(void *,int,void *) noex ;
        int free	(void *) noex ;
	int ps		(char **) noex ;
	int ma		(char **) noex ;
	int ml		(char **) noex ;
	int mn		(char **) noex ;
	int mp		(char **) noex ;
	int nn		(char **) noex ;
	int zn		(char **) noex ;
    } ; /* end struct (umems) */
    extern umems	um ;
    extern umems	umem ;
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* UMEM_INCLUDE */


