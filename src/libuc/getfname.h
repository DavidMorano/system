/* getfname INCLUDE */
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


#ifndef	GETFNAME_TYPELOCAL
#define	GETFNAME_TYPEUNKNOWN	-1
#define	GETFNAME_TYPELOCAL	0
#define	GETFNAME_TYPEROOT	1
#endif


#ifdef	__cplusplus
extern "C" {
#endif

extern int getfname(cchar *,char *,int,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETFNAME_INCLUDE */


