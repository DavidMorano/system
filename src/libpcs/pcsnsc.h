/* pcsnsc HEADER */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<pcsnsreq.h>


#define	PCSNSC		struct pcsnsc_head
#define	PCSNSC_FL	struct pcsnsc_flags
#define	PCSNSC_OBJ	struct pcsnsc_object
#define	PCSNSC_ST	struct pcsnsc_status
#define	PCSNSC_MAGIC	0x58261221


struct pcsnsc_object {
	char		*name ;
	uint		objsze ;
} ; /* end struct */

struct pcsnsc_status {
	pid_t		pid ;
	uint		queries ;		/* server is present */
} ; /* end struct */

struct pcsnsc_flags {
	uint		srv:1 ;		/* server is present */
} ; /* end struct */

struct pcsnsc_head {
	cchar		*pr ;
	cchar		*srcfname ;
	cchar		*srvfname ;
	char		*mbuf ;
	PCSNSC_FL	fl ;
	pid_t		pid ;
	uint		magic ;
	int		mlen ;
	int		fd ;
	int		to ;
} ; /* end struct */

typedef	PCSNSC		pcsnsc ;
typedef	PCSNSC_FL	pcsnsc_fl ;
typedef	PCSNSC_OBJ	pcsnsc_obj ;
typedef	PCSNSC_ST	pcsnsc_st ;

EXTERNC_begin

extern int pcsnsc_open(pcsnsc *,cchar *,int) noex ;
extern int pcsnsc_getstat(pcsnsc *,pcsnsc_st *) noex ;
extern int pcsnsc_help(pcsnsc *,char *,int,int) noex ;
extern int pcsnsc_getval(pcsnsc *,char *,int,cchar *,int) noex ;
extern int pcsnsc_getname(pcsnsc *,char *,int,cchar *) noex ;
extern int pcsnsc_mark(pcsnsc *) noex ;
extern int pcsnsc_exit(pcsnsc *,cchar *) noex ;
extern int pcsnsc_close(pcsnsc *) noex ;

EXTERNC_end


#endif /* PCSNSC_INCLUDE */


