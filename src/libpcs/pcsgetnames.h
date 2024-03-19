/* pcsgetnames HEADER */
/* lang=C++20 */

/* get various information elements related to the PCS environment */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSGETNAMES_INCLUDE
#define	PCSGETNAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


enum pcsnametypes {
	pcsnametype_name,
	pcsnametype_fullname,
	pcsnametype_projinfo,
	pcsnametype_org,
	pcsnametype_overlast
} ;

EXTERNC_begin

extern int pcsgetnames(cchar *,char *,int,cchar *,int) noex ;
extern int pcsgetname(cchar *,char *,int,cchar *) noex ;
extern int pcsgetfullname(cchar *,char *,int,cchar *) noex ;
extern int pcsgetprojinfo(cchar *,char *,int,cchar *) noex ;

extern int pcsnames(cchar *,char *,int,cchar *,int) noex ;
extern int pcsname(cchar *,char *,int,cchar *) noex ;
extern int pcsfullname(cchar *,char *,int,cchar *) noex ;
extern int pcsprojectinfo(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* PCSGETNAMES_INCLUDE */


