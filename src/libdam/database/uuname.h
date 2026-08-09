/* uuname HEADER */
/* charset=ISO8859-1 */
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
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBDAM */
#include	<uunames.h>		/* LIBDAM */


#define	UUNAME		struct uuname_head
#define	UUNAME_FL	struct uuname_flags
#define	UUNAME_CUR	struct uuname_cursor
#define	UUNAME_MAGIC	0x99447243
#define	UUNAME_MODBNAME	"uunames"
#define	UUNAME_OBJNAME	"uunames"


struct uuname_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magval ;
} ; /* end struct */

struct uuname_flags {
    	uint		modload:1 ;
} ; /* end struct */

struct uuname_head {
	modload		*mlp ;
	void		*obj ;		/* object pointer */
	void		*callp ;
	UUNAME_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef	UUNAME		uuname ;
typedef	UUNAME_CUR	uuname_cur ;

EXTERNC_begin

extern int uuname_open(uuname *,cchar *,cchar *) noex ;
extern int uuname_count(uuname *) noex  ;
extern int uuname_exists(uuname *,cchar *,int) noex ;
extern int uuname_curbegin(uuname *,uuname_cur *) noex ;
extern int uuname_curenum(uuname *,uuname_cur *,char *,int) noex ;
extern int uuname_curend(uuname *,uuname_cur *) noex ;
extern int uuname_audit(uuname *) noex ;
extern int uuname_close(uuname *) noex ;

EXTERNC_end


#endif /* UUNAME_INCLUDE */


