/* bibleq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEQ_INCLUDE
#define	BIBLEQ_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBUC */
#include	<bibleqs.h>		/* LIBDAM */


#define	BIBLEQ		struct bibleq_head
#define	BIBLEQ_CUR	struct bibleq_cursor
#define	BIBLEQ_CA	struct bibleq_calls
#define	BIBLEQ_Q	BIBLEQS_CITE
#define	BIBLEQ_CITE	BIBLEQS_CITE
#define	BIBLEQ_MAGIC	0x99447243
/* query options */
#define	BIBLEQ_OPREFIX	BIBLEQS_OPREFIX		/* prefix match */


struct bibleq_cursor  {
	void	*scp ;		/* SO-cursor pointer */
	uint	magval ;
} ; /* end struct */

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
	void		*obj ;		/* object pointer */
	modload		*mlp ;
	BIBLEQ_CA	*callp ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef	BIBLEQ		bibleq ;
typedef	BIBLEQ_CUR	bibleq_cur ;
typedef	BIBLEQ_CA	bibleq_ca ;
typedef	BIBLEQ_Q	bibleq_q ;

EXTERNC_begin

extern int bibleq_open(bibleq *,cchar *,cchar *) noex ;
extern int bibleq_count(bibleq *) noex ;
extern int bibleq_curbegin(bibleq *,bibleq_cur *) noex ;
extern int bibleq_lookup(bibleq *,bibleq_cur *,int,cchar **) noex ;
extern int bibleq_read(bibleq *,bibleq_cur *,bibleq_q *,char *,int) noex ;
extern int bibleq_curend(bibleq *,bibleq_cur *) noex ;
extern int bibleq_audit(bibleq *) noex ;
extern int bibleq_close(bibleq *) noex ;

EXTERNC_end


#endif /* BIBLEQ_INCLUDE */


