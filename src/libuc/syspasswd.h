/* syspasswd HEADER */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSPASSWD_INCLUDE
#define	SYSPASSWD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>


#define	SYSPASSWD		struct syspasswd_head
#define	SYSPASSWD_MAGIC		0x88776281
#define	SYSPASSWD_FNAME		"/sysdb/passwd"


struct syspasswd_head {
	uint		magic ;
	filemap		b ;
} ;

typedef SYSPASSWD	syspasswd ;

EXTERNC_begin

extern int syspasswd_open(SYSPASSWD *,cchar *) noex ;
extern int syspasswd_close(SYSPASSWD *) noex ;
extern int syspasswd_readent(SYSPASSWD *,ucentpw *,char *,int) noex ;
extern int syspasswd_reset(SYSPASSWD *) noex ;

EXTERNC_end


#endif /* SYSPASSWD_INCLUDE */


