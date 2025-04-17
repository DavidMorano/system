/* cmimk HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a CMI database */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMIMK_INCLUDE
#define	CMIMK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	CMIMK_MAGIC	0x88773425
#define	CMIMK		struct cmimk_head
#define	CMIMK_OBJ	struct cmimk_object
#define	CMIMK_ENT	struct cmimk_entry
#define	CMIMK_LINE	struct cmimk_liner
#define	CMIMK_INFO	struct cmimk_information
#define	CMIMK_FL	struct cmimk_flags
#define	CMIMK_NE	4
#define	CMIMK_INTOPEN	(10*60)
#define	CMIMK_INTSTALE	(5*60)


struct cmimk_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;		/* *unused* */
} ;

struct cmimk_information {
	uint		maxent ;
	uint		nents ;
} ;

struct cmimk_liner {
	uint		loff ;
	uint		llen ;
} ;

struct cmimk_entry {
	CMIMK_LINE	*lines ;
	uint		eoff ;
	uint		elen ;
	ushort		nlines, cn ;
} ;

struct cmimk_flags {
	uint		notsorted:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
	uint		abort:1 ;
} ;

struct cmimk_head {
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nidxfname ;
	vecobj		*elp ;		/* entry-list-pointer */
	vecobj		*llp ;		/* line-list-pointer */
	size_t		size_db ;
	time_t		ti_db ;
	CMIMK_FL	fl ;
	uint		magic ;
	uint		pcn ;		/* previous command-number (PCN) */
	uint		maxent ;
	uint		nents ;
	int		nfd ;
	mode_t		om ;
} ;

typedef CMIMK		cmimk ;
typedef CMIMK_OBJ	cmimk_obj ;
typedef CMIMK_ENT	cmimk_ent ;
typedef CMIMK_LINE	cmimk_line ;
typedef CMIMK_INFO	cmimk_info ;
typedef	CMIMK_FL	cmimk_fl ;

EXTERNC_begin

extern int	cmimk_open(cmimk *,cchar *,int,mode_t) noex ;
extern int	cmimk_setdb(cmimk *,size_t,time_t) noex ;
extern int	cmimk_add(cmimk *,cmimk_ent *) noex ;
extern int	cmimk_abort(cmimk *,int) noex ;
extern int	cmimk_getinfo(cmimk *,cmimk_info *) noex ;
extern int	cmimk_close(cmimk *) noex ;

EXTERNC_end


#endif /* CMIMK_INCLUDE */


