/* progoff */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */


#ifndef	PROGOFF_INCLUDE
#define	PROGOFF_INCLUDE	1


#include	<envstandards.h>	/* ordered first to configure */

#include	<sys/types.h>

#include	<bfile.h>
#include	<localmisc.h>

#include	"defs.h"


#ifdef	__cplusplus
extern "C" {
#endif

extern int	progoffbegin(struct proginfo *,bfile *) ;
extern int	progoffcomment(struct proginfo *,bfile *,cchar *,int) ;
extern int	progoffdss(struct proginfo *,bfile *,
			cchar *,cchar *) ;
extern int	progoffdsn(struct proginfo *,bfile *,
			cchar *,int) ;
extern int	progoffsrs(struct proginfo *,bfile *,
			cchar *,cchar *,cchar *) ;
extern int	progoffsrn(struct proginfo *,bfile *,
			cchar *,cchar *,int) ;
extern int	progoffhf(struct proginfo *,bfile *,
			cchar *,cchar *,cchar *,cchar *) ;
extern int	progoffsetbasefont(struct proginfo *,bfile *) ;
extern int	progoffwrite(struct proginfo *,bfile *,cchar *,int) ;
extern int	progofftcadd(struct proginfo *,bfile *,int,cchar *) ;
extern int	progofftcmk(struct proginfo *,bfile *,int) ;
extern int	progoffend(struct proginfo *) ;

#ifdef	__cplusplus
}
#endif


#endif /* PROGOFF_INCLUDE */



