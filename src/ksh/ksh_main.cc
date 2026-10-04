/* main */

/* generic front-end for SHELL */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_SHOBJ	0		/* experiment? */


/* revision history:

	= 2002-03-01, David A­D­ Morano

	This was written as a small experiement.


*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Synopsis:

	$ ksh


*******************************************************************************/


#include	<envstandards.h>	/* ordered first to configure */

#include	<shell.h>

#if	defined(SOLARIS) && (SOLARIS >= 8)
#include	<user_attr.h>
#include	<project.h>
#endif

#include	<ctime>

#include	<usystem.h>
#include	<bfile.h>
#include	<vecstr.h>
#include	<sbuf.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	MAXARGINDEX	100
#define	MAXARGGROUPS	(MAXARGINDEX/8 + 1)

#ifndef	LOGNAMELEN
#define	LOGNAMELEN	32
#endif

#ifndef	DEBUGLEVEL
#define	DEBUGLEVEL(n)	(pip->debuglevel >= (n))
#endif


/* external subroutines */

extern int	cfdeci(cchar *,int,int *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	lastlogin(char *,uid_t,time_t *,char *,char *) ;

extern int	printhelp(bfile *,cchar *,cchar *,cchar *) ;

extern char	*strwcpy(char *,cchar *,int) ;


/* external variables */


/* external variables */

struct gprog	g ;


/* local structures */


/* forward references */

local void	shmark(int) ;


/* local variables */


/* exported subroutines */


int main(argc,argv,envv)
int		argc ;
cchar	*argv[] ;
cchar	*envv[] ;
{
	int	rs = SR_OK ;
	int	ex = EX_INFO ;


	memset(&g,0,sizeof(struct gprog)) ;
	g.envv = envv ;

#if	CF_SHOBJ
	rs = shobj_init(&g.shobjs,&g) ;
#endif


	ex = sh_main(argc,argv,shmark) ;


#if	CF_SHOBJ
	shobj_free(&g.shobjs) ;
#endif

	return ex ;
}
/* end subroutine (main) */


/* local subroutines */


local void shmark(n)
int	n ;
{
	time_t	daytime = time(NULL) ;


#if	CF_SHOBJ
	shobj_check(&g.shobjs) ;
#endif



}
/* end subroutine (shmark) */


