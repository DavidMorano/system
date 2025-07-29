/* calyear HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	CALYEAR_INCLUDE
#define	CALYEAR_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<modload.h>
#include	<calyears.h>
#include	<localmisc.h>


#define	CALYEAR_MAGIC	0x99447246
#define	CALYEAR		struct calyear_head
#define	CALYEAR_Q	struct calyear_q
#define	CALYEAR_QUERY	struct calyear_q
#define	CALYEAR_CITE	struct calyear_q
#define	CALYEAR_CUR	struct calyear_c
#define	CALYEAR_CALLS	struct calyear_calls


struct calyear_q {
	ushort	y ;
	uchar	m, d ;
} ;

struct calyear_c {
	uint	magic ;
	void	*scp ;
} ;

struct calyear_calls {
	int	(*open)(void *,cchar *,cchar **,cchar **) ;
	int	(*count)(void *) ;
	int	(*curbegin)(void *,CALYEARS_CUR *) ;
	int	(*lookcite)(void *,CALYEARS_CUR *,CALYEARS_Q *) ;
	int	(*read)(void *,CALYEARS_CUR *,CALYEARS_Q *,char *,int) ;
	int	(*curend)(void *,CALYEARS_CUR *) ;
	int	(*check)(void *,time_t) ;
	int	(*audit)(void *) ;
	int	(*close)(void *) ;
} ;

struct calyear_head {
	uint		magic ;
	modload		loader ;
	void		*obj ;		/* object pointer */
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
	CALYEAR_CALLS	call ;
} ;


#if	(! defined(CALYEAR_MASTER)) || (CALYEAR_MASTER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern int calyear_open(CALYEAR *,cchar *,cchar **,cchar **) ;
extern int calyear_count(CALYEAR *) ;
extern int calyear_curbegin(CALYEAR *,CALYEAR_CUR *) ;
extern int calyear_lookcite(CALYEAR *,CALYEAR_CUR *,CALYEAR_Q *) ;
extern int calyear_read(CALYEAR *,CALYEAR_CUR *,CALYEAR_Q *,char *,int) ;
extern int calyear_curend(CALYEAR *,CALYEAR_CUR *) ;
extern int calyear_check(CALYEAR *,time_t) ;
extern int calyear_audit(CALYEAR *) ;
extern int calyear_close(CALYEAR *) ;

#ifdef	__cplusplus
}
#endif

#endif /* CALYEAR_MASTER */

#endif /* CALYEAR_INCLUDE */


