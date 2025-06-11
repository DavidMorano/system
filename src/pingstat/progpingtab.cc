/* progpingtab */

/* add a ping-tab name to the list */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debugging */


/* revision history:

	= 2001-09-01, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 2001 David A�D� Morano.  All rights reserved. */

/*****************************************************************************

	This subroutine adds a ping-tab name to the list of them.


*****************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctype.h>

#include	<usystem.h>
#include	<field.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"pingtab.h"
#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	snsds(char *,int,const char *,const char *) ;
extern int	snwcpy(char *,int,const char *,int) ;
extern int	mkpath1w(char *,const char *,int) ;
extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	mkpath3(char *,const char *,const char *,const char *) ;
extern int	matstr(const char **,const char *,int) ;
extern int	matostr(const char **,int,const char *,int) ;
extern int	vecstr_envadd(vecstr *,const char *,const char *,int) ;
extern int	vecstr_envset(vecstr *,const char *,const char *,int) ;
extern int	vecstr_adduniq(vecstr *,const char *,int) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern char	*strwcpy(char *,const char *,int) ;
extern char	*strnchr(const char *,int,int) ;


/* externals variables */


/* local global variabes */


/* local structures */


/* forward references */


/* local variables */

static const unsigned char 	pterms[32] = {
	0x00, 0x3E, 0x00, 0x00,
	0x01, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported subroutines */


int progpingtabadd(PROGINFO *pip,cchar *abuf,int alen)
{
	FIELD		fsb ;
	vecstr		*ptp = &pip->pingtabs ;
	int		rs ;
	int		fl ;
	int		c = 0 ;
	const char	*fp ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	debugprintf("progpingtab: ent >%r<\n",abuf,alen) ;
#endif

	if ((rs = field_start(&fsb,abuf,alen)) >= 0) {

	    while ((fl = field_get(&fsb,pterms,&fp)) >= 0) {
	        if (fl > 0) {
	            c += 1 ;
	            rs = vecstr_adduniq(ptp,fp,fl) ;
		}
	        if (rs < 0) break ;
	    } /* end while */

	    field_finish(&fsb) ;
	} /* end if (field) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	debugprintf("progpingtab: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (progpingtabadd) */


