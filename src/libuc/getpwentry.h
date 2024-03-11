/* getpwentry HEADER */
/* lang=C20 */

/* subroutines to access the 'passwd' and 'group' databases */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	GETPWENTRY_INCLUDE
#define	GETPWENTRY_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<pwfile.h>


EXTERNC_begin

extern int getpwentry_name(PWENTRY *,char *,int,cchar *) noex ;
extern int getpwentry_uid(PWENTRY *,char *,int,uid_t) noex ;

EXTERNC_end


#endif /* GETPWENTRY_INCLUDE */


