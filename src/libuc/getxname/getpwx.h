/* getpwx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a password entry (either straight or cached) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPWX_INCLUDE
#define	GETPWX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


EXTERNC_begin

extern int getpwx_name(ucentpw *,char *,int,cchar *) noex ;
extern int getpwx_uid(ucentpw *,char *,int,uid_t) noex ;
extern int getpwx_control(int) noex ;

EXTERNC_end

#ifdef	__cplusplus

struct ucentpwx : ucentpw {
    	int nam(char *, int,cchar *) noex ;
    	int uid(char *, int,uid_t) noex ;
	int control(int) noex ;
} ; /* end struct (ucentpwx) */

#endif /* __cplusplus */


#endif /* GETPWX_INCLUDE */


