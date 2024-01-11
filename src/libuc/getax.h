/* getax HEADER */
/* lang=C20 */

/* UNIX® System databases (PASSWD, SHADOW, GROUP, and PROJECT) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETAX_INCLUDE
#define	GETAX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<pwd.h>			/* <- standard */
#include	<grp.h>			/* <- standard */
#include	<spwd.h>		/* <- special */
#include	<ucproject.h>		/* <- special */
#include	<clanguage.h>


EXTERNC_begin

extern int getpw_begin() noex ;
extern int getpw_end() noex ;
extern int getpw_ent(PASSWD *,char *,int) noex ;
extern int getpw_name(PASSWD *,char *,int,cchar *) noex ;
extern int getpw_uid(PASSWD *,char *,int,uid_t) noex ;

extern int getsp_begin() noex ;
extern int getsp_end() noex ;
extern int getsp_ent(SPWD *sp,char *,int) noex ;
extern int getsp_name(SPWD *sp,char *,int,cchar *) noex ;

extern int getgr_begin() noex ;
extern int getgr_end() noex ;
extern int getgr_ent(GROUP *,char *,int) noex ;
extern int getgr_name(GROUP *,char *,int,cchar *name) noex ;
extern int getgr_gid(GROUP *,char *,int,gid_t) noex ;

extern int getpj_begin() noex ;
extern int getpj_end() noex ;
extern int getpj_ent(PROJECT *,char *,int) noex ;
extern int getpj_name(PROJECT *,char *,int,cchar *) noex ;
extern int getpj_pid(PROJECT *,char *,int,projid_t) noex ;
extern int getpj_def(PROJECT *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* GETAX_INCLUDE */


