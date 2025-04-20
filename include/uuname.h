/* uuname HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* caller interface to a UUNAME database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UUNAME_INCLUDE
#define	UUNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<uunames.h>


#define	UUNAME_MAGIC	0x99447243
#define	UUNAME		struct uuname_head
#define	UUNAME_FL	struct uuname_flags
#define	UUNAME_CUR	struct uuname_cursor
#define	UUNAME_MODBNAME	"uunames"
#define	UUNAME_OBJNAME	"uunames"


struct uuname_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magic ;
} ;

struct uuname_flags {
    	uint		modload:1 ;
} ;

struct uuname_head {
	modload		*mlp ;
	void		*obj ;		/* object pointer */
	void		*callp ;
	UUNAME_FL	fl ;
	uint		magic ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ;

typedef	UUNAME		uuname ;
typedef	UUNAME_CUR	uuname_cur ;

EXTERNC_begin

extern int uuname_open(uuname *,cchar *,cchar *) noex ;
extern int uuname_count(uuname *) noex  ;
extern int uuname_exists(uuname *,cchar *,int) noex ;
extern int uuname_curbegin(uuname *,uuname_cur *) noex ;
extern int uuname_enum(uuname *,uuname_cur *,char *,int) noex ;
extern int uuname_curend(uuname *,uuname_cur *) noex ;
extern int uuname_audit(uuname *) noex ;
extern int uuname_close(uuname *) noex ;

EXTERNC_end


#endif /* UUNAME_INCLUDE */


