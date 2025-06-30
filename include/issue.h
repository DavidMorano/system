/* issue HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to help and manage "issue" messages */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISSUE_INCLUDE
#define	ISSUE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* |time_t| */
#include	<usystem.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<vechand.h>


#define	ISSUE_MAGIC	0x75648941
#define	ISSUE		struct issue_head
#define	ISSUE_FL	struct issue_flags
#define	ISSUE_MAP	struct issue_mapper
#define	ISSUE_DEFKEYNAME	"login"


struct issue_mapper {
	lockrw		rwm ;
	paramfile	dirsfile ;
	vechand		mapdirs ;
	cchar		*fname ;
	time_t		ti_mtime ;
	time_t		ti_check ;
	uint		magic ;
} ;

struct issue_flags {
	uint		sorted:1 ;
} ;

struct issue_head {
	ISSUE_MAP	mapper ;
	ptm		mx ;		/* this is for all of the data */
	mainv		envv ;
	cchar		*pr ;
	cchar		*fe ;
	time_t		ti_lastcheck ;
	ISSUE_FL	fl ;
	uint		magic ;
	int		nmaps ;
	int		nenv ;
} ;

typedef	ISSUE		issue ;
typedef	ISSUE_FL	issue_fl ;
typedef	ISSUE_MAP	issue_map ;

EXTERNC_begin

extern int	issue_open(issue *,cchar *) noex ;
extern int	issue_check(issue *,time_t) noex ;
extern int	issue_process(issue *,cchar *,mainv,int) noex ;
extern int	issue_audit(issue *) noex ;
extern int	issue_close(issue *) noex ;

EXTERNC_end


#endif /* ISSUE_INCLUDE */


