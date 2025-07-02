/* td SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Terminal Display (TD) library */
/* version %I% last-modified %G% */

#define	CF_SAVERESTORE	0		/* implementing cursor-save-restore? */

/* revision history:

	= 1999-08-01, Dave morano
	This object package is finally finished!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	td

	Description:
	This object module facilitates managing a terminal display.
	This object contains routines (not unlike in the UNIX
	'curses' library) to display information in "windows" on a
	terminal display.  The major difference between this terminal
	display library and the standard UNIX 'curses' library is
	that THIS LIBRARY WORKS!

	Other notes:
	An 'xterm' terminal is almost en entire complete 'vt102'
	terminal.  It pretty much only lacks the blinking character
	attribute and the double height-width characters.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |isatty(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |abs(3c)| */
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| * |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecitem.h>
#include	<buffer.h>
#include	<ascii.h>
#include	<ansigr.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<termstr.h>
#include	<termconseq.h>
#include	<tabcols.h>
#include	<bufprintf.h>
#include	<localmisc.h>		/* |HEXBUFLEN| + |NTABCOLS| */

#include	"td.h"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	TD_MAGIC	0x02652103
#define	TD_BUFLEN	(8 * 1024)

#define	TD_RESTORELEN	4
#define	TD_SCROLLBUFLEN	20

/* terminal capabilities */
#define	TD_TCSCROLL	(1 << 0)	/* supports scroll regions */
#define	TD_TCCOLOR	(1 << 1)	/* supports color */
#define	TD_TCEIGHT	(1 << 2)	/* 8-bit controls */
#define	TD_TCIL		(1 << 3)	/* insert line */

/* aid for graphic rendition operations */
#define	TD_GRMASK	(TD_GRBOLD|TD_GRUNDER|TD_GRBLINK|TD_GRREV)

#define	TERMTYPE	struct termtype

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	2048
#endif

#ifndef	TERMCONSEQLEN
#define	TERMCONSEQLEN	84
#endif

#undef	CBUFLEN
#define	CBUFLEN		((4*4)+3)

#ifndef	HEXBUFLEN
#define	HEXBUFLEN	100
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		td_vprintf(td *,int,cchar *,va_list) noex ;
    int		td_vpprintf(td *,int,int,int,cchar *,va_list) noex ;
    int		td_printf(td *,int,cchar *,...) noex ;
    int		td_pprintf(td *,int,int,int,cchar *,...) noex ;
    int		td_pwrite(td *,int,int,int,cchar *,int) noex ;
    int		td_pwritegr(td *,int,int,int,int,cchar *,int) noex ;
    int		td_suspend(td *,int,int) noex ;
}


/* external variables */


/* local structures */

enum erases {
	erase_disp,			/* display */
	erase_line,			/* line */
	erase_char,			/* character */
	erase_overlast
} ;

struct termtype {
	cchar		*term ;
	int		termcap ;	/* capabilities */
} ;


/* forward references */

template<typename ... Args>
static int td_ctor(td *op,Args ... args) noex {
    	TD		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ; /* dangerous */
	    if ((op->tsp = new(nothrow) termstr) != np) {
	        if ((op->wlp = new(nothrow) vecitem) != np) {
		    rs = SR_OK ;
	        } /* end if (new-vecitem) */
		if (rs < 0) {
		    delete op->tsp ;
		    op->tsp = nullptr ;
		}
	    } /* end if (new-termstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (td_ctor) */

static int td_dtor(td *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->wlp) {
		delete op->wlp ;
		op->wlp = nullptr ;
	    }
	    if (op->tsp) {
		delete op->tsp;
		op->tsp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (td_dtor) */

template<typename ... Args>
static int td_magic(td *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (td_magic) */

static int	td_starter(td *,int,cchar *,int,int) noex ;
static int	td_startwin(td *,int,int,int,int) noex ;
static int	td_flushmove(td *,td_win *,int,int) noex ;
static int	td_iflush(td *) noex ;
static int	td_procstr(td *,td_win *,int,cchar *,int) noex ;
static int	td_termstrbegin(td *) noex ;
static int	td_termstrend(td *) noex ;
static int	td_store(td *,cchar *,int) noex ;
static int	td_erase(td *,int,int,int) noex ;

#if	CF_SAVERESTORE
static int	td_isave(td *) noex ;
static int	td_irestore(td *) noex ;
#endif /* CF_SAVERESTORE */

#ifdef	COMMENT
static int	termmatch(const TERMTYPE *,cchar *) noex ;
#endif


/* local variables */

#ifdef	COMMENT
/* 	TYPE		CAPABILITY	*/
static const TERMTYPE	terms[] = {
	{ "vt100",	(TD_TCSCROLL) },
	{ "ansi",	(TD_TCSCROLL | TD_TCIL) },
	{ "xterm",	(TD_TCSCROLL | TD_TCIL | TD_TCCOLOR) },
	{ "screen",	(TD_TCSCROLL | TD_TCIL) },
	{ "vt220",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT) },
	{ "vt240",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT | TD_TCCOLOR) },
	{ "vt320",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT) },
	{ "vt340",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT | TD_TCCOLOR) },
	{ "vt420",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT) },
	{ "vt440",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT | TD_TCCOLOR) },
	{ "vt520",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT) },
	{ "vt540",	(TD_TCSCROLL | TD_TCIL | TD_TCEIGHT | TD_TCCOLOR) },
	{ "vt101",	(TD_TCSCROLL | TD_TCIL) },
	{ "vt102",	(TD_TCSCROLL | TD_TCIL) },
	{ nullptr, 0 }
} ;
#endif /* COMMENT */


/* exported variables */


/* exported subroutines */

int td_start(td *tdp,int tfd,cchar *termname,int r,int c) noex {
	int		rs ;
	if ((rs = td_ctor(tdp,termname)) >= 0) {
	    rs = SR_BADF ;
	    if (tfd >= 0) {
		rs = SR_INVALID ;
	        if (termname[0] && (r >= 0) && (c >= 0)) {
	            USTAT	sb ;
	            if ((rs = u_fstat(tfd,&sb)) >= 0) {
	                if (isatty(tfd)) {
		            rs = td_starter(tdp,tfd,termname,r,c) ;
	                } else {
	                    rs = SR_NOTSOCK ;
	                }
	            } /* end if (stat) */
	        } /* end if (valid) */
	    } /* end if (ok-fd) */
	    if (rs < 0) {
		td_dtor(tdp) ;
	    }
	} /* end if (td_ctor) */
	return rs ;
}
/* end subroutine (td_start) */

int td_finish(td *tdp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = td_magic(tdp)) >= 0) {
/* reset the scroll region if necessary */
	    {
	        rs1 = td_suspend(tdp,-1,-1) ;
	        if (rs >= 0) rs = rs1 ;
	    }
/* do one final flush */
	    {
	        rs1 = td_iflush(tdp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
/* free the internal stuff */
	    {
	        rs1 = vecitem_finish(tdp->wlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = termstr_finish(tdp->tsp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (tdp->buf) {
	        rs1 = uc_free(tdp->buf) ;
	        if (rs >= 0) rs = rs1 ;
	        tdp->buf = nullptr ;
	    }
	    if (tdp->termname) {
	        rs1 = uc_free(tdp->termname) ;
	        if (rs >= 0) rs = rs1 ;
	        tdp->termname = nullptr ;
	    }
	    {
		rs1 = td_dtor(tdp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    tdp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_finish) */

/* suspend the windowing object (like for escape or something like that) */
int td_suspend(td *tdp,int r,int c) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = td_magic(tdp)) >= 0) {
	    td_win	*wp ;
	    if ((rs = vecitem_get(tdp->wlp,0,&wp)) >= 0) {
	        if ((rs = td_termstrbegin(tdp)) >= 0) {
	            if ((rs >= 0) && tdp->f.smallscroll) {
	                tdp->f.smallscroll = false ;
	                rs = termstr_ssr(tdp->tsp,0,tdp->rows) ;
	            }
	            if (rs >= 0) {
	                rs = td_flushmove(tdp,wp,r,c) ;
		    }
	            if (rs >= 0) {
	                rs = td_termstrend(tdp) ;
	                len = rs ;
	            }
	        } /* end if (transaction) */
	        if (rs >= 0) rs = td_iflush(tdp) ;
	    } /* end if (getting window) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_suspend) */

/* create a new subwindow */
int td_subnew(td *tdp,int srow,int scol,int rows,int cols) noex {
	int		rs ;
	if ((rs = td_magic(tdp)) >= 0) {
	    bool	f = true ;
	    rs = SR_INVALID ;
	    f = f && (srow >= 0) && (scol >= 0) ;
	    f = f && (rows >= 0) && (cols >= 0) ;
	    if (f) {
	        rs = td_startwin(tdp,srow,scol,rows,cols) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_subnew) */

/* delete a subwindow */
int td_subdel(td *tdp,int wn) noex {
	int		rs ;
	if ((rs = td_magic(tdp)) >= 0) {
	    rs = SR_INVALID ;	/* cannot delete win==0 */
	    if (wn != 0) {
	        if ((rs = td_iflush(tdp)) >= 0) {
	            if (wn < 0) {
			vecitem	*wlp = tdp->wlp ;
			auto	vg = vecitem_get ;
	                td_win	*wp ;
	                for (int i = 1 ; vg(wlp,i,&wp) >= 0 ; i += 1) {
	                    if (wp) {
	                        rs = vecitem_del(tdp->wlp,i) ;
	                        if (rs < 0) break ;
		            }
	                } /* end for */
	            } else {
	                rs = vecitem_del(tdp->wlp,wn) ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_subdel) */

int td_getlines(td *tdp,int w) noex {
	int		rs ;
	int		lines = 0 ;
	if ((rs = td_magic(tdp)) >= 0) {
	    td_win	*wp ;
	    if ((rs = vecitem_get(tdp->wlp,w,&wp)) >= 0) {
	        lines = wp->rows ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? lines : rs ;
}
/* end subroutine (td_getlines) */

int td_setlines(td *tdp,int w,int nlines) noex {
	int		rs ;
	int		olines = 0 ;
	if ((rs = td_magic(tdp)) >= 0) {
	    td_win	*wp ;
	    if ((rs = vecitem_get(tdp->wlp,w,&wp)) >= 0) {
	        olines = wp->rows ;
	        wp->rows = nlines ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? olines : rs ;
}
/* end subroutine (td_setlines) */

/* execute a control action */
int td_control(td *tdp,int cmd,...) noex {
	int		rs ;
	if ((rs = td_magic(tdp)) >= 0) {
	    switch (cmd) {
	    case TD_CCURSOR:
	    case TD_CEOL:
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_control) */

/* make a cursor screen move */
int td_move(td *tdp,int wn,int r,int c) noex {
	int		rs ;
	int		f = false ;
	if ((rs = td_magic(tdp)) >= 0) {
	    if ((r >= 0) || (c >= 0)) {
	        td_win	*wp ;
	        if ((rs = vecitem_get(tdp->wlp,wn,&wp)) >= 0) {
	            if ((r < wp->rows) && (c < wp->cols)) {
	                if (r >= 0) {
	                    f = true ;
	                    wp->move.row = r ;
	                }
	                if (c >= 0) {
	                    f = true ;
	                    wp->move.col = c ;
	                }
	                if (f) {
	                    wp->move.timecount = tdp->timecounter++ ;
		        }
	            } /* end if (ok) */
	        } /* end if (get-window-pointer) */
	    } /* end if (need something) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (td_move) */

/* scroll a window ( (n>0)==UP, (n<0)==DOWN ) */
int td_scroll(td *tdp,int wn,int n) noex {
	int		rs ;
	if ((rs = td_magic(tdp)) >= 0) {
	    if (n != 0) {
	        td_win	*wp ;
	        if ((rs = vecitem_get(tdp->wlp,wn,&wp)) >= 0) {
	            int		na = abs(n) ;
	            char	index_chars[8] ;
/* prepare a move for after the entire scroll operation -- do it */
	            tdp->cur.row = -1 ; /* unknown cursor location */
	            tdp->cur.col = -1 ; /* unknown cursor location */
	            if (na > wp->rows) {
	                na = wp->rows ;
		    }
	            if ((rs = td_termstrbegin(tdp)) >= 0) {
	                termstr	*tsp = tdp->tsp ;
	                int	index_line ;
	                if ((rs = termstr_ssr(tsp,wp->srow,wp->rows)) >= 0) {
/* move to the boundary of the scroll region for the proper direction */
	                    if (n >= 0) {
				cchar	*ts_ind = "\033D" ; /* index */
	                        index_line = (wp->srow + wp->rows - 1) ;
	                        strcpy(index_chars,ts_ind) ;	/* UP */
	                    } else {
				cchar	*ts_ri = "\033M" ; /* reverse-index */
	                        index_line = wp->srow ;
	                        strcpy(index_chars,ts_ri) ;	/* DOWN */
	                    }
	                } /* end if */
	                if (rs >= 0) {
	                    if ((rs = termstr_curh(tsp,index_line,-1)) >= 0) {
	                        for (int i = 0 ; (i < na) ; i += 1) {
	                            rs = termstr_write(tsp,index_chars,-1) ;
				    if (rs < 0) break ;
	                        } /* end for */
	                        if (rs >= 0) {
				    cint	r = tdp->rows ;
	                            if ((rs = termstr_ssr(tsp,0,r)) >= 0) {
	                                rs = td_termstrend(tdp) ;
	                            }
	                        }
	                    } /* end if (termstr_curh) */
	                } /* end if (ok) */
	            } /* end if (transaction) */
	        } /* end if (vecitem_get) */
	    } /* end if (not-zero) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_scroll) */

/* position print-formatted */
int td_pprintf(td *tdp,int wn,int r,int c,cchar *fmt,...) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = td_magic(tdp,fmt)) >= 0) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = td_vpprintf(tdp,wn,r,c,fmt,ap) ;
	    len = rs ;
	    va_end(ap) ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_pprintf) */

/* regular print-formatted */
int td_printf(td *tdp,int wn,cchar *fmt,...) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = td_magic(tdp,fmt)) >= 0) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = td_vpprintf(tdp,wn,-1,-1,fmt,ap) ;
	    len = rs ;
	    va_end(ap) ;
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_printf) */

int td_vprintf(td *tdp,int wn,cchar *fmt,va_list ap) noex {
	return td_vpprintf(tdp,wn,-1,-1,fmt,ap) ;
}
/* end subroutine (td_vprintf) */

/* vprintf-like thing */
int td_vpprintf(td *tdp,int wn,int r,int c,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = td_magic(tdp,fmt)) >= 0) {
	    char	*lbuf{} ;
	    if ((rs = malloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
	        if ((rs = bufvprintf(lbuf,llen,fmt,ap)) > 0) {
	            rs = td_pwrite(tdp,wn,r,c,lbuf,rs) ;
		    len = rs ;
	        }
		rs1 = uc_free(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_vpprintf) */

int td_write(td *tdp,int wn,cchar *wbuf,int wlen) noex {
	return td_pwrite(tdp,wn,-1,-1,wbuf,wlen) ;
}
/* end subroutine (td_write) */

/* position-write */
int td_pwrite(td *tdp,int wn,int r,int c,cchar *wbuf,int wlen) noex {
	return td_pwritegr(tdp,wn,r,c,0,wbuf,wlen) ;
}
/* end subroutine (td_pwrite) */

/* position-write w/ graphic-rendition */
int td_pwritegr(td *tdp,int wn,int r,int c,int gr,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = td_magic(tdp,wbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (wlen < 0) wlen = lenstr(wbuf) ;
	    if (wn >= 0) {
	        td_win	*wp ;
	        if ((rs = vecitem_get(tdp->wlp,wn,&wp)) >= 0) {
	            cint	nrow = (r >= 0) ? r : wp->move.row ;
	            cint	ncol = (c >= 0) ? c : wp->move.col ;
	            if ((nrow < wp->rows) && (ncol < wp->cols)) {
	                if ((rs = td_termstrbegin(tdp)) >= 0) {
	                    if ((rs = td_flushmove(tdp,wp,nrow,ncol)) >= 0) {
	                        rs = td_procstr(tdp,wp,gr,wbuf,wlen) ;
	                        if (rs >= 0) {
	                            rs = td_termstrend(tdp) ;
	                            len = rs ;
				}
	                    }
	                } /* end if (transaction) */
	            } /* end if (ok) */
	        } /* end if (get-window-pointer) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_pwritegr) */

/* clear a window */
int td_clear(td *tdp,int w) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = td_magic(tdp)) >= 0) {
	    td_win	*wp ;
	    if ((rs = vecitem_get(tdp->wlp,w,&wp)) >= 0) {
	        if ((rs = td_termstrbegin(tdp)) >= 0) {
	            if ((rs = td_flushmove(tdp,wp,-1,-1)) >= 0) {
	                if (w == 0) {
	                    rs = termstr_ed(tdp->tsp,2) ;
	                } else {
	                    for (int i = 0 ; i < wp->rows ; i += 1) {
	                        if ((rs = termstr_el(tdp->tsp,2)) >= 0) {
	                            rs = termstr_char(tdp->tsp,'\n') ;
			        }
			        if (rs < 0) break ;
	                    } /* end for */
	                } /* end if */
	            } /* end if (flushmove) */
	            if (rs >= 0) {
	                rs = td_termstrend(tdp) ;
	                len = rs ;
	            }
	        } /* end if (transaction) */
	    } /* end if (get-window-pointer) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_clear) */

/* erase-window; type: 0=forward, 1=back, 2=whole */
int td_ew(td *tdp,int w,int r,int type) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = td_magic(tdp)) >= 0) {
	    td_win	*wp ;
	    if ((rs = vecitem_get(tdp->wlp,w,&wp)) >= 0) {
	        if ((rs = td_termstrbegin(tdp)) >= 0) {
	            if ((rs = td_flushmove(tdp,wp,r,-1)) >= 0) {
			rs = termstr_ed(tdp->tsp,type) ;
		    } /* end if */
	            if (rs >= 0) {
	                rs = td_termstrend(tdp) ;
	                len = rs ;
	            }
	        } /* end if (transaction) */
	    } /* end if (get-window-pointer) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_ew) */

/* erase-line */
int td_el(td *tdp,int w,int type) noex {
	cint		item = erase_line ;
	return td_erase(tdp,w,item,type) ;
}
/* end subroutine (td_el) */

/* erase-character */
int td_ec(td *tdp,int w,int n) noex {
	cint		item = erase_char ;
	return td_erase(tdp,w,item,n) ;
}
/* end subroutine (td_ec) */

/* flush buffer */
int td_flush(td *tdp) noex {
	int		rs ;
	if ((rs = td_magic(tdp)) >= 0) {
	    rs = td_iflush(tdp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_flush) */

/* terminal window size change */
int td_setsize(td *tdp,int rows,int cols) noex {
	int		rs ;
	if ((rs = td_magic(tdp)) >= 0) {
	    rs = SR_INVALID ;
	    if ((rows >= 0) && (cols >= 0)) {
	        td_win	*wp ;
	        if ((rs = vecitem_get(tdp->wlp,0,&wp)) >= 0) {
	            wp->rows = rows ;
	            wp->cols = cols ;
	        } /* end if (get-window-pointer) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_setsize) */

/* check */
int td_check(td *tdp) noex {
	int		rs ;
	if ((rs = td_magic(tdp)) >= 0) {
	    if ((rs = td_flushmove(tdp,0,-1,-1)) >= 0) {
	        rs = td_iflush(tdp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_check) */

int td_getpos(td *tdp,int wn,td_pos *pp) noex {
	int		rs ;
	if ((rs = td_magic(tdp,pp)) >= 0) {
	    td_win	*wp ;
	    memclear(pp) ;
	    if ((rs = vecitem_get(tdp->wlp,wn,&wp)) >= 0) {
	        pp->timecount = wp->cur.timecount ;
	        pp->row = wp->cur.row ;
	        pp->col = wp->cur.col ;
	    } /* end if (get-window-pointer) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (td_position) */


/* private subroutines */

static int td_starter(td *tdp,int tfd,cchar *termname,int r,int c) noex {
        TERMIOS         termconf{} ;
        int             rs ;
        tdp->tfd = tfd ;
        tdp->termcap = 0 ;
        tdp->buflen = TD_BUFLEN ;
        tdp->curlen = 0 ;
        tdp->rows = r ;
        tdp->cols = c ;
        tdp->f.statusdisplay = false ;
        tdp->f.meol = false ;
        tdp->f.linebuf = false ;
        if ((rs = uc_tcattrget(tfd,&termconf)) >= 0) {
            cchar       *cp ;
            tdp->f.nlcr = (termconf.c_oflag & ONLCR) ? true : false ;
            if ((rs = uc_mallocstrw(termname,-1,&cp)) >= 0) {
                char    *bp ;
                tdp->termname = cp ;
                if ((rs = uc_malloc(tdp->buflen,&bp)) >= 0) {
                    cchar       *tt = termname ;
                    tdp->buf = bp ;
                    if ((rs = termstr_start(tdp->tsp,tt)) >= 0) {
                        vecitem *wlp = tdp->wlp ;
                        cint    vo = 0 ;
                        if ((rs = vecitem_start(wlp,5,vo)) >= 0) {
                            if ((rs = td_startwin(tdp,0,0,r,c)) >= 0) {
                                tdp->cur.row = -1 ;
                                tdp->cur.col = -1 ;
                                tdp->magic = TD_MAGIC ;
                            }
                            if (rs < 0) {
                                vecitem_finish(wlp) ;
                            }
                        } /* end if (vecitem_start) */
                        if (rs < 0) {
                            termstr_finish(tdp->tsp) ;
                        }
                    } /* end if (termstr_start) */
                    if (rs < 0) {
                        uc_free(tdp->buf) ;
                        tdp->buf = nullptr ;
                    }
                } /* end if (m-a) */
                if (rs < 0) {
                    uc_free(tdp->termname) ;
                    tdp->termname = nullptr ;
                }
            } /* end if (m-a) */
        } /* end if (tcgetattr) */
        return rs ;
}
/* end subroutine (td_starter) */

/* create and initialize a new sub-window */
static int td_startwin(td *tdp,int srow,int scol,int rows,int cols) noex {
	int		rs = SR_OK ;
	bool		f = false ;
	f = f || (srow >= tdp->rows) ;
	f = f || ((srow + rows) > tdp->rows) ;
	f = f || (scol >= tdp->cols) ;
	f = f || ((scol + cols) > tdp->cols) ;
	if (f) {
	    rs = SR_DOM ;
	} else {
	    cint	esz = szof(td_win) ;
	    td_win	win{} ;
	    win.srow = min(srow,(tdp->rows-1)) ;
	    win.scol = min(scol,(tdp->cols-1)) ;
	    win.rows = min(rows,tdp->rows) ;
	    win.cols = min(cols,tdp->cols) ;
	    win.move.row = -1 ;
	    win.move.col = -1 ;
	    rs = vecitem_add(tdp->wlp,&win,esz) ;
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (td_startwin) */

/* flush buffer */
static int td_iflush(td *tdp) noex {
	td_win		*wp ;
	td_win		*mwp = nullptr ;
	uint		mtimecount = 0 ;
	int		rs = SR_OK ;
	for (int i = 0 ; vecitem_get(tdp->wlp,i,&wp) >= 0 ; i += 1) {
	    if (wp) {
	        if ((wp->move.row >= 0) || (wp->move.col >= 0)) {
	            if (wp->move.timecount > mtimecount) {
	                mtimecount = wp->move.timecount ;
	                mwp = wp ;
	            }
		}
	    } /* end if (non-null) */
	} /* end for */
	if (mwp != nullptr) {
	    rs = td_flushmove(tdp,mwp,-1,-1) ;
	}
	if ((rs >= 0) && (tdp->curlen > 0)) {
	    rs = u_write(tdp->tfd,tdp->buf,tdp->curlen) ;
	    tdp->curlen = 0 ;
	}
	return rs ;
}
/* end subroutine (td_iflush) */

#if	CF_SAVERESTORE

/* set the save-cursor stuff in the buffer */
static int td_isave(td *tdp) noex {
	int		rs ;
	int		len = 0 ;
	int		rv = 0 ;
#ifdef	COMMENT
	rs = termstr_csr(tdp->tsp,true) ;
	if (rs >= 0) {
	    rs = td_istore(tdp) ;
	}
	rv = (rs >= 0) ? len : rs ;
#else
	cp = strwcpy(tdp->buf,TERMSTR_RESTORE,-1) ;
	tdp->curlen = (cp - tdp->buf) ;
	rv = tdp->curlen ;
#endif /* COMMENT */
	return rv ;
}
/* end subroutine (td_isave) */

/* set the restore-cursor stuff in the buffer */
static int td_irestore(td *tdp) noex {
	cchar		*cp = strwcpy(tdp->buf,TERMSTR_RESTORE,-1) ;
	tdp->curlen = (cp - tdp->buf) ;
	return tdp->curlen ;
}
/* end subroutine (td_irestore) */

#endif /* CF_SAVERESTORE */

/* flush out any accumulated moves */
static int td_flushmove(td *tdp,td_win *wp,int r,int c) noex {
	int		rs = SR_OK ;
	int		nrow = (r >= 0) ? r : wp->move.row ;
	int		ncol = (c >= 0) ? c : wp->move.col ;
	if ((nrow < wp->rows) && (ncol < wp->cols)) {
	    int		wrow = (nrow >= 0) ? nrow : wp->cur.row ;
	    int		wcol = (ncol >= 0) ? ncol : wp->cur.col ;
	    if ((wrow < wp->rows) && (wcol < wp->cols)) {
	        int	arow = wrow + wp->srow ;
	        int	acol = wcol + wp->scol ;
	        bool	f_chosen = false ;
/* relocate to absolute coordinates */
/* search for optimizations */
	        if ((tdp->cur.row >= 0) && (tdp->cur.col >= 0)) {
	            if ((arow == tdp->cur.row) && (acol == tdp->cur.col)) {
	                f_chosen = true ;
	            }
	            if ((! f_chosen) && 
	                (arow == tdp->cur.row) && (acol == 0)) {
	                f_chosen = true ;
	                rs = termstr_char(tdp->tsp,'\r') ;
	            }
	            if ((! f_chosen) && 
	                (arow == tdp->cur.row) && 
	                (tdp->cur.col > 0) && (acol == (tdp->cur.col-1))) {
	                f_chosen = true ;
	                rs = termstr_char(tdp->tsp,'\b') ;
	            }
	            if ((! f_chosen) && 
	                (acol == tdp->cur.col) && (arow == (tdp->cur.row+1))) {
	                f_chosen = true ;
#ifdef	COMMENT
	                rs = termstr_char(tdp->tsp,CH_IND) ;
#else
	                rs = termstr_write(tdp->tsp,"\033D",2) ;
#endif

	            }
	        } /* end if (optimization) */
	        if (! f_chosen) {
	            f_chosen = true ;
	            rs = termstr_curh(tdp->tsp,arow,acol) ;
	        } /* end if */
	        if (rs >= 0) {
	            wp->cur.row = wrow ;
	            wp->cur.col = wcol ;
	            tdp->cur.row = arow ;
	            tdp->cur.col = acol ;
	            wp->move.row = -1 ;
	            wp->move.col = -1 ;
	        }
	    } /* end if (needed) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (td_flushmove) */

/* process the special C-language string-escape characters */
static int td_procstr(td *tdp,td_win *wp,int gr,cchar *sbuf,int slen) noex {
	cint		grmask = TD_GRMASK ;
	cint		clen = CBUFLEN ;
	int		rs = SR_OK ;
	int		sl ;
	int		cl ;
	int		ch ;
	int		irows, icols ;
	int		len = 0 ;
	cchar		*sp ;
	char		cbuf[CBUFLEN+1] ;
	bool		f_cr = false ;
	bool		f_inrow = true ;
	bool		f_incol = true ;
	bool		f_gr = false ;
	sp = sbuf ;
	sl = slen ;
	if (sl < 0) sl = lenstr(sp) ;
	if ((sl > 0) && (gr & grmask)) {
	    cint	a1 = (gr & TD_GRBOLD) ? ANSIGR_BOLD : -1 ;
	    cint	a2 = (gr & TD_GRUNDER) ? ANSIGR_UNDER : -1 ;
	    cint	a3 = (gr & TD_GRBLINK) ? ANSIGR_BLINK : -1 ;
	    cint	a4 = (gr & TD_GRREV) ? ANSIGR_REV : -1 ;
	    f_gr = true ;
	    if ((rs = termconseq(cbuf,clen,'m',a1,a2,a3,a4)) >= 0) {
	        cl = rs ;
	        rs = termstr_write(tdp->tsp,cbuf,cl) ;
	        len += rs ;
	    }
	} /* end if */
	while ((rs >= 0) && (sl > 0) && *sp) {
	    f_cr = false ;
	    irows = 0 ;
	    icols = 0 ;
	    f_inrow = (wp->cur.row < wp->rows) ;
	    f_incol = (wp->cur.col < wp->cols) ;
	    ch = (*sp & 0xff) ;
	    switch (ch) {
	    case '\v':
	        if (f_inrow && f_incol) {
	            rs = termstr_el(tdp->tsp,-1) ;
	            len += rs ;
	        }
	        break ;
	    case '\f':
	        if (f_inrow && f_incol) {
	            rs = termstr_ed(tdp->tsp,-1) ;
	            len += rs ;
	        }
	        break ;
	    case '\t':
	        if (f_inrow && f_incol) {
	            icols = tabcols(NTABCOLS,tdp->cur.col) ;
	            rs = termstr_char(tdp->tsp,'\t') ;
	            len += rs ;
	        }
	        break ;
	    case CH_CSI:
	    case CH_ESC:
	    case CH_BEL:
	    case CH_SO:
	    case CH_SI:
	    case CH_SS2:
	    case CH_SS3:
	        if (f_inrow && f_incol) {
	            rs = termstr_char(tdp->tsp,ch) ;
	            len += rs ;
	        }
	        break ;
	    case CH_DEL:
	        break ;
	    case '\r':
	        if (f_inrow) {
	            f_cr = true ;
	            rs = termstr_char(tdp->tsp,ch) ;
	            len += rs ;
	        }
	        break ;
	    case '\n':
	        if (f_inrow) {
	            irows = 1 ;
	            f_cr = tdp->f.nlcr ;
	            if (! f_cr) {
	                rs = termstr_char(tdp->tsp,'\r') ;
	                len += rs ;
	            }
	            if ((rs >= 0) && (wp->cur.row < (wp->rows-1))) {
	                rs = termstr_char(tdp->tsp,ch) ;
	                len += rs ;
	            }
	        }
	        break ;
	    case CH_IND:
	        if (f_inrow) {
	            irows = 1 ;
	            if (wp->cur.row < (wp->rows-1)) {
	                rs = termstr_char(tdp->tsp,ch) ;
	                len += rs ;
	            }
	        }
	        break ;
	    case '\b':
	        if (wp->cur.col > 0) {
	            f_incol = (tdp->cur.col < tdp->cols) ;
	            if (f_incol) {
	                tdp->cur.col -= 1 ;
	                rs = termstr_char(tdp->tsp,ch) ;
	                len += rs ;
	            }
	            wp->cur.col -= 1 ;
	        }
	        break ;
	    default:
	        if (f_inrow && ((ch & 0x7f) >= 0x20)) {
	            icols = 1 ;
	            if (f_incol) {
	                rs = termstr_char(tdp->tsp,ch) ;
	                len += rs ;
	            }
	        }
	        break ;
	    } /* end switch */
	    if (rs >= 0) {
	        sp += 1 ;
	        sl -= 1 ;
	        if (irows > 0) {
	            int	n, brows = irows ;
	            if ((wp->cur.row + irows) > (wp->rows - 1)) {
	                n = (wp->cur.row + irows) - (wp->rows - 1) ;
	                brows -= n ;
	            }
	            wp->cur.row += irows ;
	            tdp->cur.row = min((tdp->cur.row + brows),(tdp->rows-1)) ;
	        }
	        if (f_cr) {
	            f_cr = false ;
	            wp->cur.col = 0 ;
	            tdp->cur.col = 0 ;
	        } else if (icols > 0) {
	            int	n, bcols = icols ;
	            if ((wp->cur.col + icols) > (wp->cols - 1)) {
	                n = (wp->cur.col + icols) - (wp->cols - 1) ;
	                bcols -= n ;
	            }
	            wp->cur.col += icols ;
	            tdp->cur.col = min((tdp->cur.col + bcols),(tdp->cols-1)) ;
	        }
	    } /* end if */
	} /* end while */
	if ((rs >= 0) && f_gr) {
	    int	a1 = ANSIGR_OFFALL ;
	    rs = termconseq(cbuf,clen,'m',a1,-1,-1,-1) ;
	    cl = rs ;
	    if (rs >= 0) {
	        rs = termstr_write(tdp->tsp,cbuf,cl) ;
	        len += rs ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_procstr) */

static int td_termstrbegin(td *tdp) noex {
	int		rs ;
	rs = termstr_clean(tdp->tsp) ; /* clear any buffered actions */
	return rs ;
}
/* end subroutine (td_termstrbegin) */

static int td_termstrend(td *tdp) noex {
	int		rs ;
	int		len = 0 ;
	cchar		*bp ;
	if ((rs = termstr_get(tdp->tsp,&bp)) >= 0) {
	    cint	bl = rs ;
	    rs = td_store(tdp,bp,bl) ;
	    len = rs ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_termstrend) */

static int td_store(td *tdp,cchar *bp,int bl) noex {
	int		rs = SR_OK ;
	int		rlen = (tdp->buflen - tdp->curlen) ;
	int		len = 0 ;
	if (bl < 0) bl = lenstr(bp) ;
	if (bl > rlen) {
	    rs = u_write(tdp->tfd,tdp->buf,tdp->curlen) ;
	    tdp->curlen = 0 ;
	}
	if ((rs >= 0) && (bl > 0)) {
	    if (bl > tdp->buflen) {
	        rs = u_write(tdp->tfd,bp,bl) ;
	        len += rs ;
	    } else {
	        rlen = (tdp->buflen - tdp->curlen) ;
	        rs = snwcpy((tdp->buf + tdp->curlen),rlen,bp,bl) ;
	        if (rs >= 0) tdp->curlen += rs ;
	        len += rs ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_store) */

static int td_erase(td *tdp,int w,int item,int type) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = td_magic(tdp)) >= 0) {
	    td_win	*wp ;
	    if ((rs = vecitem_get(tdp->wlp,w,&wp)) >= 0) {
	        if ((rs = td_termstrbegin(tdp)) >= 0) {
	            if ((rs = td_flushmove(tdp,wp,-1,-1)) >= 0) {
			switch (item) {
			case erase_disp:
	                    rs = termstr_ed(tdp->tsp,type) ;
			    break ;
			case erase_line:
	                    rs = termstr_el(tdp->tsp,type) ;
			    break ;
			case erase_char:
	                    rs = termstr_ec(tdp->tsp,type) ;
			    break ;
			} /* end switch */
	                if (rs >= 0) {
	                    rs = td_termstrend(tdp) ;
	                    len = rs ;
	                }
		    } /* end if */
	        } /* end if (transaction) */
	    } /* end if (get-window-pointer) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (td_erase) */

#ifdef	COMMENT

/* match a terminal name with our list */
static int termmatch(const TERMTYPE *tlist,cchar *term) noex {
	int		i ;
	for (i = 0 ; tlist[i].term != nullptr ; i += 1) {
	    if (strcmp(term,tlist[i].term) == 0) break ;
	} /* end for */
	return (tlist[i].term != nullptr) ? i : -1 ;
}
/* end subroutine (termmatch) */

#endif /* COMMENT */


