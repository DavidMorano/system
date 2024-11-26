/* bibleq SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEQ_INCLUDE
#define	BIBLEQ_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<bibleqs.h>


#define	BIBLEQ_MAGIC	0x99447243
#define	BIBLEQ		struct bibleq_head
#define	BIBLEQ_CUR	struct bibleq_c
#define	BIBLEQ_CA	struct bibleq_calls
#define	BIBLEQ_Q	BIBLEQS_CITE
#define	BIBLEQ_CITE	BIBLEQS_CITE
/* query options */
#define	BIBLEQ_OPREFIX	BIBLEQS_OPREFIX		/* prefix match */


struct bibleq_cursor  {
	uint	magic ;
	void	*scp ;		/* SO-cursor pointer */
} ;

EXTERNC_begin
struct bibleq_calls {
	int	(*open)(void *,cchar *,cchar *) noex ;
	int	(*count)(void *) noex ;
	int	(*curbegin)(void *,void *) noex ;
	int	(*lookup)(void *,void *,int,cchar **) noex ;
	int	(*enumerate)(void *,void *,BIBLEQS_CITE *,char *,int) noex ;
	int	(*curend)(void *,void *) noex ;
	int	(*audit)(void *) noex ;
	int	(*close)(void *) noex ;
} ;
EXTERNC_end

struct bibleq_head {
	uint		magic ;
	modload		loader ;
	BIBLEQ_CA	call ;
	void		*obj ;		/* object pointer */
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;


EXTERNC_begin

extern int bibleq_open(BIBLEQ *,cchar *,cchar *) noex ;
extern int bibleq_count(BIBLEQ *) noex ;
extern int bibleq_curbegin(BIBLEQ *,BIBLEQ_CUR *) noex ;
extern int bibleq_lookup(BIBLEQ *,BIBLEQ_CUR *,int,cchar **) noex ;
extern int bibleq_read(BIBLEQ *,BIBLEQ_CUR *,BIBLEQ_CITE *,char *,int) noex ;
extern int bibleq_curend(BIBLEQ *,BIBLEQ_CUR *) noex ;
extern int bibleq_audit(BIBLEQ *) noex ;
extern int bibleq_close(BIBLEQ *) noex ;

EXTERNC_end


#endif /* BIBLEQ_INCLUDE */


