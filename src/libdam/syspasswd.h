/* syspasswd INCLUDE */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSPASSWD_INCLUDE
#define	SYSPASSWD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<pwd.h>			/* <- money shot */


#define	SYSPASSWD		struct syspasswd_head
#define	SYSPASSWD_MAGIC		0x88776281
#define	SYSPASSWD_FNAME		"/sysdb/passwd"


struct syspasswd_head {
	uint		magic ;
	filemap		b ;
} ;

typedef struct syspasswd_head	syspasswd ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int syspasswd_open(SYSPASSWD *,const char *) noex ;
extern int syspasswd_close(SYSPASSWD *) noex ;
extern int syspasswd_readent(SYSPASSWD *,struct passwd *,char *,int) noex ;
extern int syspasswd_reset(SYSPASSWD *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSPASSWD_INCLUDE */


