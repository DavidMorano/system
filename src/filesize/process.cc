/* process */

/* process a name */


#define	F_DEBUG		0


/* revision history :

	= 96/03/01, David A­D­ Morano

	The subroutine was adapted from others programs that
	did similar types of functions.


*/


/******************************************************************************

	This module just provides optional expansion of directories.
	The real work is done by the 'checkname' module.



******************************************************************************/


#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<csignal>
#include	<unistd.h>
#include	<time.h>
#include	<cstdlib>
#include	<cstring>

#include	<usystem.h>
#include	<baops.h>
#include	<field.h>
#include	<wdt.h>

#include	"misc.h"
#include	"paramopt.h"
#include	"config.h"
#include	"defs.h"



/* local defines */



/* external subroutines */

extern int	checkname() ;

extern char	*strbasename(char *) ;


/* external variables */


/* external variables */


/* local structures */


/* forward references */


/* local variables */






int process(pip,name,pp)
struct proginfo	*pip ;
char		name[] ;
PARAMOPT	*pp ;
{
	ustat		sb, sb2 ;

	struct checkparams	ck ;

	int	rs ;
	int	wopts = 0 ;


	if (name == NULL) 
		return BAD ;

#if	F_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: entered name=\"%s\"\n",name) ;
#endif

	if (u_lstat(name,&sb) < 0) 
		return BAD ;

#if	F_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: name=\"%s\" mode=%0o\n",
			name,sb.st_mode) ;
#endif

	ck.pip = pip ;
	ck.pp = pp ;

	if (S_ISLNK(sb.st_mode)) {

	    if (pip->fl.follow &&
		(u_stat(name,&sb2) >= 0) && S_ISDIR(sb.st_mode)) {

#if	F_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: calling wdt\n") ;
#endif

		wopts != (pip->fl.follow) ? WDT_MFOLLOW : 1 ;
		rs = wdt(name,WDTM_FOLLOW,checkname,&ck) ;

	    } else
		rs = checkname(name,&sb,&ck) ;

	} else
		rs = checkname(name,&sb,&ck) ;

#if	F_DEBUG
	if (pip->debuglevel > 1)
		eprintf("process: rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (process) */



