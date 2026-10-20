/* msent HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* machine status entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	MSENT_INCLUDE
#define	MSENT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/utsname.h>		/* for |SYS_NMLN| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* object defines */
#define	MSENT			struct msent_head
#define	MSENT_LA		struct msent_la
#define	MSENT_ATIME		struct msent_atime
#define	MSENT_UTIME		struct msent_utime
#define	MSENT_DTIME		struct msent_dtime
#define	MSENT_STIME		struct msent_stime

/* entry field lengths */
#define	MSENT_LBOOTTIME		4
#define	MSENT_LATIME		4
#define	MSENT_LUTIME		4
#define	MSENT_LDTIME		4
#define	MSENT_LSTIME		4
#define	MSENT_LLA		(3 * 4)
#define	MSENT_LNPROC		4
#define	MSENT_LNUSER		4
#define	MSENT_LPMTOTAL		4
#define	MSENT_LPMAVAIL		4
#define	MSENT_LSPEED		4
#define	MSENT_LNCPU		4
#define	MSENT_LNODEHASH		4
#define	MSENT_LPID		4
#define	MSENT_LFLAGS		2

#ifdef	NODENAMELEN
#define	MSENT_LNODENAME	NODENAMELEN
#else
#ifdef	SYS_NMLN
#define	MSENT_LNODENAME	SYS_NMLN	/* from <sys/utsname.h> */
#else
#define	MSENT_LNODENAME	256
#endif
#endif

/* entry field offsets */
/* do this carefully! */
/* there is no good automatic way to do this in C language (sigh) */
/* the C-language does not have all of the advantages of assembly language! */

#define	MSENT_OBOOTTIME	0
#define	MSENT_OATIME		(MSENT_OBOOTTIME + MSENT_LBOOTTIME)
#define	MSENT_OUTIME		(MSENT_OATIME + MSENT_LATIME)
#define	MSENT_ODTIME		(MSENT_OUTIME + MSENT_LUTIME)
#define	MSENT_OSTIME		(MSENT_ODTIME + MSENT_LDTIME)
#define	MSENT_OLA		(MSENT_OSTIME + MSENT_LSTIME)
#define	MSENT_ONPROC		(MSENT_OLA + MSENT_LLA)
#define	MSENT_ONUSER		(MSENT_ONPROC + MSENT_LNPROC)
#define	MSENT_OPMTOTAL		(MSENT_ONUSER + MSENT_LNUSER)
#define	MSENT_OPMAVAIL		(MSENT_OPMTOTAL + MSENT_LPMTOTAL)
#define	MSENT_OSPEED		(MSENT_OPMAVAIL + MSENT_LPMAVAIL)
#define	MSENT_ONCPU		(MSENT_OSPEED + MSENT_LSPEED)
#define	MSENT_ONODEHASH		(MSENT_ONCPU + MSENT_LNCPU)
#define	MSENT_OPID		(MSENT_ONODEHASH + MSENT_LNODEHASH)
#define	MSENT_OFLAGS		(MSENT_OPID + MSENT_LPID)
#define	MSENT_ONODENAME		(MSENT_OFLAGS + MSENT_LFLAGS)
#define	MSENT_OVERLAST		(MSENT_ONODENAME + MSENT_LNODENAME)

#define	MSENT_SZ		((MSENT_OVERLAST + 3) & (~ 3))


struct msent_head {
	uint	btime ;			/* time-stamp boot */
	uint	atime ;			/* time-stamp access */
	uint	utime ;			/* time-stamp update */
	uint	dtime ;			/* time-stamp disable */
	uint	stime ;			/* time-stamp speed */
	uint	la[3] ;
	uint	nproc ;			/* current processes */
	uint	nuser ;			/* logged-in users */
	uint	pmtotal ;		/* physical memory total */
	uint	pmavail ;		/* physical memory available */
	uint	speed ;			/* machine speed (relative) */
	uint	ncpu ;
	uint	nodehash ;		/* hash of nodename */
	uint	sysid ;			/* system ID (host-ID) */
	uint	pid ;			/* daemon PID (per system) */
	ushort	flags ;			/* flags */
	char	nodename[MSENT_LNODENAME + 1] ;
} ; /* end struct (msent_head) */

#ifdef	__cplusplus
struct msent : msent_head {
	int	wr	(cchar *,int) noex ;
	int	rd	(char *,int) noex ;
} ; /* end struct (msent) */
#else
typedef	MSFILE		msfile ;
#endif /* __cplusplus */

struct msent_la {
	uint	la[3] ;
} ;

struct msent_atime {
	uint	atime ;
} ;

struct msent_utime {
	uint	utime ;
} ;

struct msent_dtime {
	uint	dtime ;
} ;

struct msent_stime {
	uint	stime ;
} ;

EXTERNC_begin

extern int msent_proc(msent *,int,char *,int) noex ;

EXTERNC_end

#ifdef	COMMENT
extern int msent_atime(struct msent_atime *,int,char *,int) noex ;
extern int msent_utime(struct msent_utime *,int,char *,int) noex ;
extern int msent_dtime(struct msent_dtime *,int,char *,int) noex ;
extern int msent_stime(struct msent_stime *,int,char *,int) noex ;
extern int msent_la(struct msent_la *,int,char *,int) noex ;
#endif /* COMMENT */


#endif /* MSENT_INCLUDE */


