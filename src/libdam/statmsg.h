/* statmsg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* object to help (manage) STATMSG messages */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STATMSG_INCLUDE
#define	STATMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |uid_t| + |gid_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<lockrw.h>		/* LIBUC */
#include	<paramfile.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<userid.hh>		/* LIBUC */


#define	STATMSG		struct statmsg_head
#define	STATMSG_ID	struct statmsg_ident
#define	STATMSG_MAP	struct statmsg_mapper
#define	STATMSG_FL	struct statmsg_flags
#define	STATMSG_CK	struct statmsg_checkdata
#define	STATMSG_MAGIC	0x75648942


struct statmsg_ident {
	cchar		*groupname ;
	cchar		*username ;
	uid_t		uid ;
	gid_t		gid ;
} ; /* end struct */

struct statmsg_mapper {
	lockrw		rwm ;
	paramfile	dirsfile ;
	vechand		mapdirs ;
	cchar		*username ;
	cchar		*userhome ;
	cchar		*fname ;
	time_t		ti_mtime ;
	time_t		ti_check ;
	uint		magval ;
} ; /* end struct */

struct statmsg_flags {
	uint		sorted:1 ;
} ; /* end struct */

struct statmsg_checkdata {
	ptm		mx ;
	time_t		ti_lastcheck ;	/* needs mutex protection */
	int		nmaps ;
} ; /* end struct */

struct statmsg_head {
	STATMSG_MAP	mapper ;
	ptm		*mxp ;		/* this is for all of the data */
	ccharpp		envv ;
	cchar		*username ;
	cchar		*userhome ;
	cchar		*fe ;		/* file-ending */
	char		*useralloc ;
	time_t		ti_lastcheck ;
	STATMSG_FL	fl ;
	uint		magval ;
	int		nmaps ;
	int		nenv ;
} ; /* end struct */

typedef	STATMSG		statmsg ;
typedef	STATMSG_FL	statmsg_fl ;
typedef	STATMSG_ID	statmsg_id ;
typedef	STATMSG_MAP	statmsg_map ;
typedef	STATMSG_CK	statmsg_ch ;

EXTERNC_begin

extern int statmsg_open(statmsg *,cchar *) noex ;
extern int statmsg_check(statmsg *,time_t) noex ;
extern int statmsg_process(statmsg *,cchar *,cchar **,cchar *,int) noex ;
extern int statmsg_close(statmsg *) noex ;

EXTERNC_end

#ifdef	__cplusplus

extern int statmsg_processid(statmsg *,userid *,cchar **,cchar *,int) noex ;

#endif /* __cplusplus */


#endif /* STATMSG_INCLUDE */


