/* debug HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debug utilities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEBUG_INCLUDE
#define	DEBUG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netdb.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int debugopen(cchar *) noex ;
extern int debugclose(void) noex ;
extern int debugprint(cchar *,int) noex ;
extern int debugprintf(cchar *,...) noex ;
extern int debugprinthexblock(cchar *,int,cvoid *,int) noex ;
extern int nprintf(cchar *,cchar *,...) noex ;
extern int nprinthexblock(cchar *,cchar *,int,cvoid *,int) noex ;
extern int strlinelen(cchar *,int,int) noex ;
extern int strnnlen(cchar *,int,int) noex ;

extern int d_openfiles() noex ;
extern int d_ispath(cchar *) noex ;
extern int mkhexstr(char *,int,cvoid *,int) noex ;
extern int mkhexnstr(char *,int,int,cchar *,int) noex ;
extern int debugprinthex(cchar *,int,cchar *,int) noex ;
extern int debugprinthexblock(cchar *,int,cvoid *,int) noex ;
extern int hexblock(cchar *,cchar *,int) noex ;
extern int heaudit(HOSTENT *,cchar *,int) noex ;
extern char *stroflags(char *,int) noex ;
extern char *d_reventstr(int,char *,int) noex ;
extern void d_whoopen(int *) noex ;

EXTERNC_end


#endif /* DEBUG_INCLUDE */


