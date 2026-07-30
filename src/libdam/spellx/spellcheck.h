/* spellcheck */


/* revision history:

	- 2008-10-01, David A­D­ Morano

	This code was originally written.


*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	SPELLCHECK_INCLUDE
#define	SPELLCHECK_INCLUDE	1


#include	<envstandards.h>	/* ordered first to configure */

#include	<sys/types.h>

#include	<localmisc.h>
#include	<modload.h>
#include	<bits.h>

#include	"spellchecks.h"


#define	SPELLCHECK		struct spellcheck_head
#define	SPELLCHECK_CUR		struct spellcheck_cursor
#define	SPELLCHECK_CS		struct spellcheck_calls
#define	SPELLCHECK_MAGIC	0x97677247


struct spellcheck_cursor {
	void		*scp ;
	uint		magval ;
} ; /* end struct */

struct spellcheck_calls {
	int	(*start)(void *,cchar *,cchar *) ;
	int	(*count)(void *) ;
	int	(*look)(void *,cchar *,int) ;
	int	(*looks)(void *,bits *,cchar **,int) ;
	int	(*curbegin)(void *,SPELLCHECKS_CUR *) ;
	int	(*enumerate)(void *,SPELLCHECKS_CUR *,char *,int) ;
	int	(*curend)(void *,SPELLCHECKS_CUR *) ;
	int	(*audit)(void *) ;
	int	(*finish)(void *) ;
} ; /* end struct */

struct spellcheck_head {
	MODLOAD		loader ;
	void		*obj ;		/* object pointer */
	SPELLCHECK_CS	call ;
	uint		magval ;
	int		objsz ;
	int		cursz ;
} ; /* end struct */

typedef SPELLCHECK		spellcheck ;
typedef SPELLCHECK_CUR		spellcheck_cur ;

EXTERNC_begin

extern int spellcheck_start	(spellcheck *,cchar *,cchar *) noex ;
extern int spellcheck_count	(spellcheck *) noex ;
extern int spellcheck_look	(spellcheck *,cchar *,int) noex ;
extern int spellcheck_looks	(spellcheck *,bits *,cchar **,int) noex ;
extern int spellcheck_curbegin	(spellcheck *,spellcheck_cur *) noex ;
extern int spellcheck_curenum	(spellcheck *,spellcheck_cur *,
					char *,int) noex ;
extern int spellcheck_curend	(spellcheck *,spellcheck_cur *) noex ;
extern int spellcheck_audit	(spellcheck *) noex ;
extern int spellcheck_finish	(spellcheck *) noex ;

EXTERNC_end


#endif /* spellcheck */


