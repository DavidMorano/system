/* pcsnsc HEADER */
/* lang=C20 */

/* PCS Name-Server-Client */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-12-18, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSNSC_INCLUDE
#define	PCSNSC_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<realname.h>
#include	<pcsnsreq.h>
#include	<localmisc.h>


#define	PCSNSC_MAGIC	0x58261221
#define	PCSNSC		struct pcsnsc_head
#define	PCSNSC_OBJ	struct pcsnsc_object
#define	PCSNSC_FL	struct pcsnsc_flags
#define	PCSNSC_STAT	struct pcsnsc_status


struct pcsnsc_object {
	char		*name ;
	uint		objsize ;
} ;

struct pcsnsc_status {
	pid_t		pid ;
	uint		queries ;		/* server is present */
} ;

struct pcsnsc_flags {
	uint		srv:1 ;		/* server is present */
} ;

struct pcsnsc_head {
	cchar		*pr ;
	cchar		*srcfname ;
	cchar		*srvfname ;
	char		*mbuf ;
	PCSNSC_FL	f ;
	pid_t		pid ;
	uint		magic ;
	int		mlen ;
	int		fd ;
	int		to ;
} ;

EXTERNC_begin

extern int pcsnsc_open(PCSNSC *,cchar *,int) noex ;
extern int pcsnsc_status(PCSNSC *,PCSNSC_STAT *) noex ;
extern int pcsnsc_help(PCSNSC *,char *,int,int) noex ;
extern int pcsnsc_getval(PCSNSC *,char *,int,cchar *,int) noex ;
extern int pcsnsc_getname(PCSNSC *,char *,int,cchar *) noex ;
extern int pcsnsc_mark(PCSNSC *) noex ;
extern int pcsnsc_exit(PCSNSC *,cchar *) noex ;
extern int pcsnsc_close(PCSNSC *) noex ;

EXTERNC_end


#endif /* PCSNSC_INCLUDE */


