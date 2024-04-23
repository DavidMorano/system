/* uuname HEADER */
/* lang=C20 */

/* caller interface to a UUNAME database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UUNAME_INCLUDE
#define	UUNAME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<modload.h>
#include	<localmisc.h>

#include	"uunames.h"


#define	UUNAME_MAGIC	0x99447243
#define	UUNAME		struct uuname_head
#define	UUNAME_CUR	struct uuname_cursor
#define	UUNAME_CALLS	struct uuname_caller


struct uuname_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magic ;
} ;

struct uuname_caller {
	int	(*open)(void *,cchar *,cchar *) ;
	int	(*count)(void *) ;
	int	(*exists)(void *,cchar *,int) ;
	int	(*curbegin)(void *,void *) ;
	int	(*enumerate)(void *,void *,char *,int) ;
	int	(*curend)(void *,void *) ;
	int	(*audit)(void *) ;
	int	(*close)(void *) ;
} ;

struct uuname_head {
	void		*obj ;		/* object pointer */
	modload		loader ;
	UUNAME_CALLS	call ;
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef	UUNAME		uuname ;
typedef	UUNAME_CUR	uuname_cur ;
typedef	UUNAME_CALLS	uuname_calls ;

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


