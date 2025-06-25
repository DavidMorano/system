/* sysvar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* SYSVAR management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSVAR_INCLUDE
#define	SYSVAR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>
#include	<modload.h>
#include	<sysvars.h>


#define	SYSVAR_MAGIC	0x99447243
#define	SYSVAR		struct sysvar_head
#define	SYSVAR_FL	struct sysvar_flags
#define	SYSVAR_CUR	struct sysvar_cursor
#define	SYSVAR_CALLS	struct sysvar_callsubs

#define	SYSVAR_OPREFIX	SYSVARS_OPREFIX		/* prefix match */


struct sysvar_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magic ;
} ;

struct sysvar_flags {
    	uint		modload:1 ;
	uint		defaults:1 ;
} ;

struct sysvar_head {
	modload		*mlp ;			/* mod-load-pointer */
	void		*callp ;		/* call-struct pointer */
	void		*obj ;			/* object pointer */
	vecstr		*dlp ;			/* default-list-pointer */
	SYSVAR_FL	fl ;
	uint		magic ;
	int		objsize ;		/* object size */
	int		cursize ;		/* cursor size */
} ;

typedef SYSVAR		sysvar ;
typedef SYSVAR_FL	sysvar_fl ;
typedef SYSVAR_CUR	sysvar_cur ;

EXTERNC_begin

extern int sysvar_open(sysvar *,cchar *,cchar *) noex ;
extern int sysvar_count(sysvar *) noex ;
extern int sysvar_curbegin(sysvar *,sysvar_cur *) noex ;
extern int sysvar_curend(sysvar *,sysvar_cur *) noex ;
extern int sysvar_curenum(sysvar *,sysvar_cur *,char *,int,char *,int) noex ;
extern int sysvar_fetch(sysvar *,cchar *,int,sysvar_cur *,char *,int) noex ;
extern int sysvar_audit(sysvar *) noex ;
extern int sysvar_close(sysvar *) noex ;

EXTERNC_end


#endif /* SYSVAR_INCLUDE */


