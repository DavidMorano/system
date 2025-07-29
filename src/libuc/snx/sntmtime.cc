/* sntmtime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string of time-component values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sntmtime

	Description:
	Ths subroutine is similar to |sncpy1(3uc)| but it takes a
	broken-out time-specification (a la object TMTIME) and
	creates the corresponding string in the destination buffer.
	A format specifiction is supplied to determine what the
	resulting string looks like.  A format string is a string
	of characters but with format-codes embedded.  The format-codes
	select for elements of the TMTIME object.  See below for a
	list of the format-codes.

	Synopsis:
	int sntmtime(char *dbuf,int dlen,TMTIME *tmp,cchar *fmt) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	tmp		pointer to TMTIME object
	fmt		format string

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	Format codes:
	- A		full weekday name (not yet coded)
	- a		abbreviated weekday name
	- B		full month name (not yet coded)
	- b		abbreviated month name
	- C		century 00-99
	- d		day of month 01-31
	- D		short for '%m/%d/%y'
	- e		day of month 1-31 (leading space as appropriate)
	- h		abbreviated month name
	- H		24-hour 00-23
	- I		12-hour 01-24
	- j		day of year 001-366
	- m		month of year 01-12
	- M		minute 00-61 (for leap-seconds)
	- n		insert a new-line (NL) character
	- p		'am' or 'pm'
	- R		same as '%H:%M'
	- r		a 12-hour time specification w/ am-pm following
	- T		same as %H:%M:%S
	- t		insert a TAB character
	- S		seconds 00-61
	- u		day of week 1-7
	- w		day of week 0-6
	- y		year within century 00-99
	- Y		year 0000-9999
	- Z		time zone abbreviation
	- O		zone-offset ±HHMM
	- Ð		yyyy-mm-dd
	- x		dd mmm yyyy
	- X		HH:MM:SS
	- :		blinking ':' character

	Usage note:
	Provision is made in this subroutine to handle years less
	than 1900, but still greater than or equal to 0000.  In
	order to get those years, one has to supply the 'year'
	member of the TMTIME-object with a negative number (not
	less than -1900).

	Years greater than 9999 seem to be allowed by standards,
	so we (begrudgingly) allow them also.

	All other TMTIME-object values must be within (and assumed
	to be within) proper range.

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>
#include	<cstdlib>		/* for |abs(3c)| */
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<calstrs.h>
#include	<sbuf.h>
#include	<zoffparts.h>
#include	<ctdec.h>
#include	<ctdecp.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |NYEARS_CENTURY| */

#include	"sntmtime.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	sbuf_fmtstrs(sbuf *,tmtime *,cchar *) noex ;
static int	sbuf_twodig(sbuf *,int) noex ;
static int	sbuf_digs(sbuf *,int,int,int) noex ;
static int	sbuf_coder(sbuf *,tmtime *,int) noex ;
static int	sbuf_year(sbuf *,tmtime *) noex ;
static int	sbuf_zoff(sbuf *,tmtime *) noex ;
static int	sbuf_dated(sbuf *,tmtime *) noex ;
static int	sbuf_dater(sbuf *,tmtime *) noex ;
static int	sbuf_datex(sbuf *,tmtime *) noex ;


/* local variables */

static constexpr cchar	blinker[] = "\033[5m:\033[0m" ;

constexpr int		nyears = NYEARS_CENTURY ;


/* exported variables */


/* exported subroutines */

int sntmtime(char *dbuf,int dlen,tmtime *tmp,cchar *fmt) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf && tmp && fmt) ylikely {
	    if (sbuf ss ; (rs = ss.start(dbuf,dlen)) >= 0) ylikely {
	        {
	            rs = sbuf_fmtstrs(&ss,tmp,fmt) ;
	        }
	        rs1 = ss.finish ; /* <- return value */
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sntmtime) */


/* local subroutines */

static int sbuf_fmtstrs(sbuf *ssp,tmtime *tmp,cchar *fmt) noex {
	int		rs = SR_FAULT ;
	if (ssp && tmp) ylikely {
	    mainv	monp = calstrs_months ;
	    mainv	dayp = calstrs_days ;
	    rs = SR_OK ;
	    while ((rs >= 0) && *fmt) {
	        int	ch = mkchar(*fmt++) ;
	        if (ch == '%') {
	            ch = mkchar(*fmt++) ;
	            switch (ch) {
	            case '%':
	                rs = sbuf_chr(ssp,ch) ;
	                break ;
	            case 'a':
	                rs = sbuf_strw(ssp,dayp[tmp->wday],3) ;
	                break ;
	            case 'b':
	            case 'h':
	                rs = sbuf_strw(ssp,monp[tmp->mon],3) ;
	                break ;
	            case 'C':
	                {
	                    cint y = ((tmp->year+TM_YEAR_BASE)/nyears) ;
	                    rs = sbuf_twodig(ssp,y) ;
	                }
	                break ;
	            case 'd':
	                rs = sbuf_twodig(ssp,tmp->mday) ;
	                break ;
	            case 'e':
	                rs = sbuf_digs(ssp,tmp->mday,2,1) ;
	                break ;
	            case 'H':
	                rs = sbuf_twodig(ssp,tmp->hour) ;
	                break ;
	            case 'I':
	                {
	                    int		h = (tmp->hour % 12) ;
	                    if (h == 0) h = 12 ;
	                    rs = sbuf_twodig(ssp,h) ;
	                }
	                break ;
	            case 'j':
	                rs = sbuf_digs(ssp,(tmp->yday + 1),3,0) ;
	                break ;
	            case 'k':
	                rs = sbuf_digs(ssp,tmp->hour,2,1) ;
	                break ;
	            case 'l':
	                {
	                    int		h = (tmp->hour % 12) ;
	                    if (h == 0) h = 12 ;
	                    rs = sbuf_digs(ssp,h,2,1) ;
	                }
	                break ;
	            case 'm':
	                rs = sbuf_twodig(ssp,(tmp->mon + 1)) ;
	                break ;
	            case 'M':
	                rs = sbuf_twodig(ssp,tmp->min) ;
	                break ;
	            case 'n':
	                rs = sbuf_chr(ssp,'\n') ;
	                break ;
	            case 't':
	                rs = sbuf_chr(ssp,'\t') ;
	                break ;
	            case 'r': /* this is what Free-BSD does */
	                rs = sbuf_fmtstrs(ssp,tmp,"%I:%M:%S ") ;
			fallthrough ;
    			/* FALLTHROUGH */
	            case 'p':
	                if (rs >= 0) {
	                    cchar *cp = (tmp->hour < 12) ? "am" : "pm" ;
	                    rs = sbuf_strw(ssp,cp,2) ;
	                }
	                break ;
	            case 'S':
	                rs = sbuf_twodig(ssp,tmp->sec) ;
	                break ;
	            case 'U':
	                {
	                    cint w = ((tmp->yday + 7 - tmp->wday) / 7) ;
	                    rs = sbuf_twodig(ssp,w) ;
	                }
	                break ;
	            case 'u':
	                {
	                    int		d = tmp->wday ;
	                    if (d == 0) d = 7 ;
	                    rs = sbuf_digs(ssp,d,1,0) ;
	                }
	                break ;
	            case 'V':
	                {
	                    int		w ;
	                    w = (tmp->yday+10-(tmp->wday?(tmp->wday-1):6))/7 ;
	                    if (w == 0) w = 53 ;
	                    rs = sbuf_twodig(ssp,w) ;
	                }
	                break ;
	            case 'W':
	                {
	                    int		w ;
			    w = (tmp->yday+7-(tmp->wday?(tmp->wday-1):6))/7 ;
	                    rs = sbuf_twodig(ssp,w) ;
	                }
	                break ;
	            case 'w':
	                rs = sbuf_digs(ssp,tmp->wday,1,0) ;
	                break ;
	            case 'y':
	                {
	                    cint y = ((tmp->year+TM_YEAR_BASE)%nyears) ;
	                    rs = sbuf_twodig(ssp,y) ;
	                }
	                break ;
	            case 'Y':
	                rs = sbuf_year(ssp,tmp) ;
	                break ;
	            case 'Z':
	                rs = sbuf_strw(ssp,tmp->zname,8) ;
	                break ;
	            case 'R':
	                rs = sbuf_coder(ssp,tmp,0) ;
	                break ;
	            case 'X':
	            case 'T':
	                rs = sbuf_coder(ssp,tmp,1) ;
	                break ;
	            case 'O':
	                rs = sbuf_zoff(ssp,tmp) ;
	                break ;
	            case 'D':
	                rs = sbuf_dated(ssp,tmp) ;
	                break ;
	            case MKCHAR('Ð'):
	                rs = sbuf_dater(ssp,tmp) ;
	                break ;
	            case 'x':
	                rs = sbuf_datex(ssp,tmp) ;
	                break ;
	            case ':':
	                rs = sbuf_strw(ssp,blinker,-1) ;
	                break ;
	            default:
	                rs = SR_ILSEQ ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = sbuf_chr(ssp,ch) ;
	        }
	    } /* end while */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sbuf_fmtstrs) */

static int sbuf_twodig(sbuf *ssp,int v) noex {
	char		dbuf[2+1] ;
	dbuf[0] = charconv((v/10) + '0') ;
	dbuf[1] = charconv((v%10) + '0') ;
	return sbuf_strw(ssp,dbuf,2) ;
}
/* end subroutine (sbuf_twodig) */

static int sbuf_digs(sbuf *ssp,int v,int n,int f_space) noex {
	int		rs = SR_OK ;
	int		ch ;
	char		dbuf[3+1] ;
	switch (n) {
	case 1:
	    dbuf[0] = charconv((v % 10) + '0') ;
	    break ;
	case 2:
	    ch = ((v / 10) + '0') ;
	    dbuf[0] = charconv(ch) ;
	    if ((ch == '0') && f_space) dbuf[0] = ' ' ;
	    dbuf[1] = charconv((v % 10) + '0') ;
	    break ;
	case 3:
	    rs = ctdecpi(dbuf,n,n,v) ;
	    if (f_space) {
	        for (int i = 0 ; (i < (n - 1)) && (dbuf[i] == '0')  ; i += 1) {
		    dbuf[i] = ' ' ;
	        } /* end for */
	    } /* end if (space) */
	    break ;
	} /* end switch */
	if (rs >= 0) ylikely {
	    rs = sbuf_strw(ssp,dbuf,n) ;
	}
	return rs ;
}
/* end subroutine (sbuf_digs) */

static int sbuf_year(sbuf *ssp,tmtime *tmp) noex {
	cint		y = ((tmp->year + TM_YEAR_BASE)%10000) ;
	int		rs ;
	char		dbuf[4+1] ;
	if ((rs = ctdecpi(dbuf,4,4,y)) >= 0) ylikely {
	    rs = sbuf_strw(ssp,dbuf,4) ;
	}
	return rs ;
}
/* end subroutine (sbuf_year) */

static int sbuf_coder(sbuf *ssp,tmtime *tmp,int f_sec) noex {
	int		rs ;
	if ((rs = sbuf_twodig(ssp,tmp->hour)) >= 0) ylikely {
	    if ((rs = sbuf_chr(ssp,':')) >= 0) ylikely {
	        if ((rs = sbuf_twodig(ssp,tmp->min)) >= 0) ylikely {
		    if (f_sec) {
			if ((rs = sbuf_chr(ssp,':')) >= 0) ylikely {
	        	    rs = sbuf_twodig(ssp,tmp->sec) ;
			}
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_coder) */

static int sbuf_zoff(sbuf *ssp,tmtime *tmp) noex {
	cint		zo = (tmp->gmtoff / 60) ; /* minutes west of GMT */
	int		rs ;
	{
	    cint	ch = ((zo >= 0) ? '-' : '+') ;
	    if ((rs = sbuf_chr(ssp,ch)) >= 0) ylikely {
	        cint	zh = abs(zo / 60) % 100 ;
	        cint	zm = abs(zo % 60) ;
	        if ((rs = sbuf_twodig(ssp,zh)) >= 0) ylikely {
	    	    rs = sbuf_twodig(ssp,zm) ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_zoff) */

static int sbuf_dated(sbuf *ssp,tmtime *tmp) noex {
	int		rs ;
	if ((rs = sbuf_twodig(ssp,(tmp->mon+1))) >= 0) ylikely {
	    if ((rs = sbuf_chr(ssp,'/')) >= 0) ylikely {
	        if ((rs = sbuf_twodig(ssp,tmp->mday)) >= 0) ylikely {
	    	    if ((rs = sbuf_chr(ssp,'/')) >= 0) ylikely {
	        	int	y ;
	        	y = ((tmp->year+TM_YEAR_BASE)%nyears) ;
	        	rs = sbuf_twodig(ssp,y) ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_dated) */

static int sbuf_dater(sbuf *ssp,tmtime *tmp) noex {
	int		rs ;
	if ((rs = sbuf_year(ssp,tmp)) >= 0) ylikely {
	    if ((rs = sbuf_chr(ssp,'-')) >= 0) ylikely {
	        if ((rs = sbuf_twodig(ssp,(tmp->mon+1))) >= 0) ylikely {
	    	    if ((rs = sbuf_chr(ssp,'-')) >= 0) ylikely {
			rs = sbuf_twodig(ssp,tmp->mday) ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_dater) */

static int sbuf_datex(sbuf *ssp,tmtime *tmp) noex {
	int		rs ;
	if ((rs = sbuf_twodig(ssp,tmp->mday)) >= 0) ylikely {
	    if ((rs = sbuf_chr(ssp,' ')) >= 0) ylikely {
		cchar	*const *m = calstrs_months ;
	        if ((rs = sbuf_strw(ssp,m[tmp->mon],3)) >= 0) ylikely {
	    	    if ((rs = sbuf_chr(ssp,' ')) >= 0) ylikely {
	        	rs = sbuf_year(ssp,tmp) ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (sbuf_datex) */


