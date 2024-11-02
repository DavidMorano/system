/* dayofmonth SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* day-of-month operations (determinations) */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dayofmonth

	Description:
	This module implements a day-of-month calculator.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<tzfile.h>		/* for |TM_YEAR_BASE| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<tmtime.h>
#include	<ismisc.h>		/* |isleapyear(3uc)| */
#include	<localmisc.h>

#include	"dayofmonth.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum wdays {
	wday_sunday,
	wday_monday,
	wday_tuesday,
	wday_wednesday,
	wday_thursday,
	wday_friday,
	wday_saturday,
	wday_overlast
} ;

enum odays {
	oday_first,
	oday_second,
	oday_third,
	oday_fourth,
	oday_fifth,
	oday_last,
	oday_overlast
} ;


/* forward references */

template<typename ... Args>
static int dayofmonth_ctor(dayofmonth *op,Args ... args) noex {
    	dayofmonth	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dayofmonth_ctor) */

static int dayofmonth_dtor(dayofmonth *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dayofmonth_dtor) */

template<typename ... Args>
static inline int dayofmonth_magic(dayofmonth *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DAYOFMONTH_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (dayofmonth_magic) */

static int	dayofmonth_mkmonth(dayofmonth *,int) noex ;


/* exported variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	daysmonth[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0
} ;

enum months {
	month_january,
	month_february,
	month_march,
	month_april,
	month_may,
	month_june,
	month_july,
	month_august,
	month_september,
	month_october,
	month_november,
	month_december,
	month_overlast
} ;

constexpr bool		f_braindamaged = false ; /* old ILP32 environments */


/* exported variables */


/* exported subroutines */

int dayofmonth_start(dayofmonth *op,int year) noex {
	int		rs ;
	if ((rs = dayofmonth_ctor(op)) >= 0) {
	    if_constexpr (f_braindamaged) {
	        if (year >= 0) {
	            if ((year < 1970) || (year > 2038)) {
		        rs = SR_INVALID ;
	            }
	        }
	    } /* end if_constexpr (f_braindamaged) */
	    if (rs >= 0) {
	        custime		t = getustime ;
	        if (tmtime ts{} ; (rs = tmtime_localtime(&ts,t)) >= 0) {
	            op->isdst = ts.isdst ;
	            op->gmtoff = ts.gmtoff ;
	            op->year = (year >= 0) ? year : (ts.year + TM_YEAR_BASE) ;
	            op->magic = DAYOFMONTH_MAGIC ;
	        } /* end if (tmtime_localtime) */
	    } /* end if (ok) */
	    if (rs < 0) {
		dayofmonth_dtor(op) ;
	    }
	} /* end if (dayofmonth_ctor) */
	return rs ;
}
/* end subroutine (dayofmonth_start) */

int dayofmonth_finish(dayofmonth *op) noex {
	cint		n = DAYOFMONTH_NMONS ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = dayofmonth_magic(op)) >= 0) {
	    for (int i = 0 ; i < n ; i += 1) {
	        if (op->months[i] != nullptr) {
	            c += 1 ;
	            rs1 = uc_free(op->months[i]) ;
	            if (rs >= 0) rs = rs1 ;
	            op->months[i] = nullptr ;
	        }
	    } /* end for */
	    {
		rs1 = dayofmonth_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dayofmonth_finish) */

int dayofmonth_lookup(dayofmonth *op,int m,int wday,int oday) noex {
	cint		n = DAYOFMONTH_NMONS ;
	int		rs ;
	int		mday = 0 ;
	if ((rs = dayofmonth_magic(op)) >= 0) {
	    bool	fval = true ;
	    rs = SR_INVALID ;
	    fval = fval && ((m >= 0) && (m < n)) ;
	    fval = fval && ((wday >= wday_sunday) && (wday < wday_overlast)) ;
	    fval = fval && ((oday >= oday_first) && (oday < oday_overlast)) ;
	    if (fval) {
		rs = SR_OK ;
	        if (op->months[m] == nullptr) {
	            rs = dayofmonth_mkmonth(op,m) ;
	        }
	        if (rs >= 0) {
	            dayofmonth_mon	*mp = op->months[m] ;
	            int			w = 0 ; /* used-afterwards */
	            if (oday != oday_last) { /* a numbered week */
	                w = 0 ;
	                if (mp->days[w][wday] < 0) {
			    w += 1 ;
			}
	                w += oday ;
	                mday = mp->days[w][wday] ;
	            } else { /* the last week */
	                for (w = 5 ; w >= 3 ; w -= 1) {
	                    if (mp->days[w][wday] >= 0) break ;
	                } /* end for */
	                mday = mp->days[w][wday] ;
	            } /* end if */
	            if (mday < 1) {
	                rs = SR_NOTFOUND ;
		    }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? mday : rs ;
}
/* end subroutine (dayofmonth_lookup) */


/* private subroutines */

static int dayofmonth_mkmonth(dayofmonth *op,int m) noex {
	int		rs = SR_OK ;
	if (op->months[m] == nullptr) {
	    cint	sz = szof(dayofmonth_mon) ;
	    if (dayofmonth_mon *mp ; (rs = uc_malloc(sz,&mp)) >= 0) {
	        int	daymax = daysmonth[m] ;
	        op->months[m] = mp ;
	        if (m == month_february) {
	            daymax = (isleapyear(op->year)) ? 29 : 28 ;
	        } /* end if */
	       	TM	tmo{} ;
	        tmo.tm_isdst = -1 ;
	        tmo.tm_year = (op->year - TM_YEAR_BASE) ;
	        tmo.tm_mon = m ;
	        tmo.tm_mday = 1 ;
	        if ((rs = uc_mktime(&tmo,nullptr)) >= 0) {
	            int		day = 1 ;
	            bool	f = false ;
	            for (int w = 0 ; w < 6 ; w += 1) {
	                for (int wday = 0 ; wday < 7 ; wday += 1) {
	                    if ((! f) && (wday == tmo.tm_wday)) {
	                        f = true ;
			    }
	                    if (f && (day <= daymax)) {
	                        mp->days[w][wday] = day++ ;
	                    } else {
	                        mp->days[w][wday] = -1 ;
	                    }
	                } /* end for */
	            } /* end for */
	        } /* end if (uc_mktime) */
	        if (rs < 0) {
	            if (op->months[m] != nullptr) {
	                uc_free(op->months[m]) ;
	                op->months[m] = nullptr ;
	            }
	        }
	    } /* end if (memory-allocation) */
	} /* end if (work needed) */
	return rs ;
}
/* end subroutine (dayofmonth_mkmonth) */


