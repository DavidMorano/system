/* sysgroup HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* system group-entry enumeration */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSGROUP_INCLUDE
#define	SYSGROUP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<grp.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<filemap.h>


/* object defines */
#define	SYSGROUP		struct sysgroup_head
#define	SYSGROUP_MAGIC		0x88776216
#define	SYSGROUP_FNAME		"/sys/group"


struct sysgroup_head {
	filemap		*fp ;
	uint		magic ;
} ;


typedef SYSGROUP	sysgroup ;

EXTERNC_begin

extern int sysgroup_open(sysgroup *,cchar *) noex ;
extern int sysgroup_close(sysgroup *) noex ;
extern int sysgroup_readent(sysgroup *,ucgrent *,char *,int) noex ;
extern int sysgroup_reset(sysgroup *) noex ;

EXTERNC_end


#endif /* SYSGROUP_INCLUDE */


