/* cyimk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Calendar-Year-Index DB-make (make a CYI database) */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CYIMK_INCLUDE
#define	CYIMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	CYIMK_MAGIC	0x88773422
#define	CYIMK		struct cyimk_head
#define	CYIMK_FL	struct cyimk_flags
#define	CYIMK_LN	struct cyimk_liner
#define	CYIMK_OBJ	struct cyimk_object
#define	CYIMK_ENT	struct cyimk_entry
#define	CYIMK_INTOPEN	(10*60)
#define	CYIMK_INTSTALE	(5*60)


struct cyimk_object {
	cchar		*name ;
	uint		objsize ;
} ;

struct cyimk_liner {
	uint		loff ;
	uint		llen ;
} ;

struct cyimk_entry {
	CYIMK_LN	*lines ;
	uint		voff ;
	uint		vlen ;
	uint		hash ;
	uchar		nlines, m, d ;
} ;

struct cyimk_flags {
	uint		notsorted:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		none:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
	uint		abort:1 ;
} ;

struct cyimk_head {
	cchar		*idname ;
	cchar		*cname ;
	char		*nidxfname ;
	vecobj		verses ;
	vecobj		lines ;
	gid_t		gid ;
	uid_t		uid ;
	CYIMK_FL	fl ;
	uint		magic ;
	uint		pcitation ;
	int		nentries ;
	int		nfd ;
	int		year ;
	mode_t		om ;
} ;

typedef	CYIMK		cyimk ;
typedef	CYIMK_FL	cyimk_fl ;
typedef	CYIMK_LN	cyimk_ln ;
typedef	CYIMK_OBJ	cyimk_obj ;
typedef	CYIMK_ENT	cyimk_ent ;

EXTERNC_begin

extern int	cyimk_open(cyimk *,int,cchar *,cchar *,int,mode_t) noex ;
extern int	cyimk_add(cyimk *,cyimk_ent *) noex ;
extern int	cyimk_abort(cyimk *,int) noex ;
extern int	cyimk_close(cyimk *) noex ;

EXTERNC_end


#endif /* CYIMK_INCLUDE */


