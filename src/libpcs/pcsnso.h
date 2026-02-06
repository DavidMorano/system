/* pcsnso HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PCS-NAME-SERVER query database manager */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSNSO_INCLUDE
#define	PCSNSO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucentpw.h>
#include	<ids.h>
#include	<pcsnsc.h>

#include	"pcsnsreq.h"


#define	PCSNSO		struct pcsnso_head
#define	PCSNSO_CUR	struct pcsnso_cursor
#define	PCSNSO_OBJ	struct pcsnso_object
#define	PCSNSO_FL	struct pcsnso_flags
#define	PCSNSO_PWD	struct pcsnso_pwdir
#define	PCSNSO_TO	7
#define	PCSNSO_MAGIC	0x99889298

/* query options */
#define	PCSNSO_ONOSERV	(1<<0)		/* do not call the server */
#define	PCSNSO_OPREFIX	(1<<1)		/* prefix match */


struct pcsnso_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ; /* end struct */

struct pcsnso_cursor {
	uint		*verses ;		/* file-offsets to tags */
	uint		nverses ;
	int		i ;
} ; /* end struct */

struct pcsnso_flags {
	uint		id:1 ;			/* text-index */
	uint		client:1 ;		/* client */
	uint		server:1 ;		/* server */
} ; /* end struct */

struct pcsnso_pwdir {
	char		*pwbuf ;
	ucentpw		pw ;
	int		pwlen ;
} ; /* end struct (pcsnso_pwdir) */

struct pcsnso_head {
	cchar		*a ;			/* memory allocation */
	cchar		*pr ;			/* stored argument */
	PCSNSO_FL	fl, open ;
	PCSNSO_PWD	pwd ;
	ids		id ;
	PCSNSC		client ;		/* the PCS-client object */
	time_t		ti_lastcheck ;		/* server-open check */
	uint		magic ;
	int		ncursors ;
	int		opts ;
} ; /* end struct (pcsnso_head) */

typedef	PCSNSO		pcsnso ;
typedef	PCSNSO_CUR	pcsnso_cur ;
typedef	PCSNSO_OBJ	pcsnso_obj ;
typedef	PCSNSO_FL	pcsnso_fl ;
typedef	PCSNSO_PWD	pcsnso_pwd ;

EXTERNC_begin

extern int pcsnso_open(pcsnso *,cchar *) noex ;
extern int pcsnso_setopts(pcsnso *,int) noex ;
extern int pcsnso_get(pcsnso *,char *,int,cchar *,int) noex ;
extern int pcsnso_curbegin(pcsnso *,pcsnso_cur *) noex ;
extern int pcsnso_read(pcsnso *,pcsnso_cur *,char *,int,int) noex ;
extern int pcsnso_curend(pcsnso *,pcsnso_cur *) noex ;
extern int pcsnso_audit(pcsnso *) noex ;
extern int pcsnso_close(pcsnso *) noex ;

EXTERNC_end


#endif /* PCSNSO_INCLUDE */


