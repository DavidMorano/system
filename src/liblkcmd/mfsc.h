/* mfsc HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* PCS Name-Server-Client */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-12-18, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	MFSC_INCLUDE
#define	MFSC_INCLUDE


#include	<envstandards.h>	/* must be fist to configure */
#include	<sys/types.h>		/* system types */


#define	MFSC_MAGIC	0x58261222
#define	MFSC		struct mfsc_head
#define	MFSC_OBJ	struct mfsc_object
#define	MFSC_FL		struct mfsc_flags
#define	MFSC_STAT	struct mfsc_status


struct mfsc_object {
	char		*name ;
	uint		objsz ;
	uint		cursz ;
} ;

struct mfsc_status {
	pid_t		pid ;
	uint		queries ;		/* server is present */
} ;

struct mfsc_flags {
	uint		srv:1 ;		/* server is present */
} ;

struct mfsc_head {
	cchar		*pr ;
	cchar		*srcfname ;
	cchar		*srvfname ;
	char		*mbuf ;
	MFSC_FL		fl ;
	pid_t		pid ;
	uint		magic ;
	int		mlen ;
	int		fd ;
	int		to ;
} ;

typedef	MFSC		mfsc ;
typedef	MFSC_OBJ	mfsc_obj ;
typedef	MFSC_FL		mfsc_fl ;
typedef	MFSC_STAT	mfsc_stat ;

EXTERNC_begin

extern int mfsc_open(mfsc *,cchar *,int) noex ;
extern int mfsc_getstat(mfsc *,mfsc_stat *) noex ;
extern int mfsc_help(mfsc *,char *,int,int) noex ;
extern int mfsc_getval(mfsc *,char *,int,cchar *,int) noex ;
extern int mfsc_getname(mfsc *,char *,int,cchar *) noex ;
extern int mfsc_mark(mfsc *) noex ;
extern int mfsc_exit(mfsc *,cchar *) noex ;
extern int mfsc_listener(mfsc *,char *,int,int) noex ;
extern int mfsc_close(mfsc *) noex ;

EXTERNC_end


#endif /* MFSC_INCLUDE */


