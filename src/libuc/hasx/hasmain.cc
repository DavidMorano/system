/* hasmain SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* has a counted c-string some characteristic? */
/* version %I% last-modified %G% */

#define	CF_HASNOTDOTSWITCH	1	/* switch or not */

/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	has{x}

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.


	Name:
	hsempty

	Description:
        This subroutine determines if a given c-string is empty or not.

	Synopsis:
	bool hasempty(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	1		true (empty)
	0		false (not empty)


	Name:
	haseoh

	Description:
	Determine if the given c-string consists of an End-Of-Header
	(EOH) sequence.  An EOH is a leadering blank line of two
	sorts:
	<NL>
	<CR><NL>

	Synopsis:
	bool haseoh(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		assertion fails
	true		assertion succeeds


	Name:
	hasnonwhite

	Description:
	Determine if the given c-string has a non-white character.

	Synopsis:
	bool hasnonwhite(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string is empty
	true		c-string has some non-white content


	Name:
	hasleadcolon

	Description:
	Does the given c-string have a colon character (':') occur
	before a slash character ('/')?

	Synopsis:
	int hasleadcolon(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		answer NO
	true		answer YES


	Name:
	hasprintlatin

	Description:
	Are all of the characters printable Latin characters?

	Synopsis:
	bool hasprintlatin(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have bad stuff in it
	true		c-string has some bad stuff in it


	Name:
	hasprintbad

	Description:
	Does the given c-string have characters that are bad to print?

	Synopsis:
	bool hasprintbad(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have bad stuff in it
	true		c-string has some bad stuff in it


	Name:
	hasallalpha

	Description:
	Are all of the characters Alpha?

	Synopsis:
	bool hasallalpha(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it


	Name:
	hasalnum

	Description:
	Are all of the characters Alpha-Num?

	Synopsis:
	bool hasallalnum(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it


	Name:
	hasalldig

	Description:
	Are all of the characters in the given c-string digits?

	Synopsis:
	bool hasalldig(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it


	Name:
	hasallbase

	Description:
	We test if a counted strin contains all of the proper digits
	consistent with the numeric base supplied.

	Synopsis:
	bool hasallbase(cchar *sp,int sl,int base) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length
	base		base to check against

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it


	Name:
	hasValidMagic

	Description:
	This subroutine verifies that the given unknown c-string
	contains the given valid magic c-string.

	Synopsis:
	bool hasValidMagic(cchar *sp,int sl,cchar *ms) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length
	ms		givem valid magic c-string

	Returns:
	false		does not match
	true		YES it matches


	Name:
	hasnotdots

	Description:
	This subroutine determines if the given c-string has neither of the
	following:
	+ one dot character
	+ two dot characters

	Synopsis:
	bool hasNotDots(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string has the standard dot-dirs
	true		c-string does not have the standard dot-dirs


	Name:
	hasmealone

	Description:
	We test whether the given c-string consists only of those
	characters that symbolically represent the "current" user.

	Synopsis:
	int hasmealone(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		assertion fails
	true		assertion succeeds


	Name:
	hasINET4AddrStr

	Description:
	We test whether the given c-string consists of an INET4 address
	c-string.

	Synopsis:
	int hasINET4AddrStr(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		assertion fails
	true		assertion succeeds

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| + |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<mkchar.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasx.h"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */
import ulibvals ;			/* |ulibval(3u)| */

/* local defines */

#ifndef	CF_HASNOTDOTSWITCH
#define	CF_HASNOTDOTSWITCH	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	twochars(char *,cchar *,int) noex ;
static bool	hasINET4Num(cchar *,int) noex ;


/* local variables */

static cint		maxbase = ulibval.maxbase ;

constexpr bool		f_hasnotdotswitch = CF_HASNOTDOTSWITCH ;


/* exported variables */


/* exported subroutines */

bool haswhite(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISWHITE(*sp) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (haswhite) */

bool haslc(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISLC(*sp) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (haslc) */

bool hasuc(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISUC(*sp) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasuc) */

bool hasdig(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISDIG(*sp) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasdig) */

bool hasempty(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISWHITE(*sp) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    if ((! f) && *sp) {
	        f = (*sp == '\n') ;
	    } /* end if */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasempty) */

bool hasdots(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    if ((sl != 0) && (sp[0] == '.')) {
	        if (sl < 0) sl = lenstr(sp) ;
	        f = f || (sl == 1) ;
	        f = f || ((sl == 2) && (sp[1] == '.')) ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasdots) */

bool haseoh(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    if (sl > 0) {
	        f = (sp[0] == '\n') ;
	        f = f || ((sl > 1) && (sp[0] == '\r') && (sp[1] == '\n')) ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (haseoh) */

bool hasalpha(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isalphalatin(ch) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasalpha) */

bool hasalnum(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isalnumlatin(ch) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasalnum) */

/* has any number ('0' - '9') character */
bool hasnum(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isdigitlatin(ch) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasnum) */

bool hasdoublewhite(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    bool	f_prev = false ;
	    for (int i = 0 ; sl && sp[i] ; i += 1) {
	        cbool	f_white = CHAR_ISWHITE(sp[i]) ;
	        if (f_white) {
		    f = f_prev ;
	            f = f || (sp[i] != CH_SP) ;	/* to force compaction */
	            f = f || (i == 0) ; 	/* to force compaction */
	            if (f) break ;
	        } /* end if */
	        f_prev = f_white ;
	        sl -= 1 ;
	    } /* end for */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasdoublewhite) */

bool hasnonwhite(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = (! CHAR_ISWHITE(*sp)) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasnonwhite) */

bool hasleadcolon(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = (*sp == ':') ;
	        if (f) break ;
	        if (*sp == '/') break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasleadcolon) */

bool hasprintlatin(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isprintlatin(ch) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasprintlatin) */

bool hasprintbad(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isprintbad(ch) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasprintbad) */

bool hasallalpha(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isalphalatin(ch) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallalpha) */

bool hasallalnum(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isalnumlatin(ch) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallalnum) */

bool hasallwhite(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISWHITE(*sp) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallwhite) */

bool hasalllc(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISLC(*sp) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasalllc) */

bool hasalluc(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        f = CHAR_ISUC(*sp) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasalluc) */

bool hasalldig(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isdigitlatin(ch) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasalldig) */

bool hasallbase(cchar *sp,int sl,int b) noex {
    	cauto		iswht = char_iswhite ;
	bool		f = false ;
	if (sp && (maxbase >= 0)) ylikely {
	    if ((b >= 2) && (b <= maxbase)) {
	        while (sl && *sp) {
	            cint	ch = mkchar(*sp) ;
	            cint	v = CHAR_TOVAL(*sp) ;
	            f = (v < b) ;
	            f = f || ((ch == '-') || iswht(ch) || (ch == CH_NBSP)) ;
	            if (! f) break ;
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	    } /* end if (valid base) */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallbase) */

bool hasallchr(cchar *sp,int sl,int ch_s) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = (ch == ch_s) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallchr) */

bool hasallhdrkey(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = ishdrkey(ch) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallhdrkey) */

bool hasonlyminus(cchar *sp,int sl) noex {
        bool            f = false ;
	if (sp) ylikely {
            if (*sp != '\0') {
		f = true ;
                f = f && (sp[0] == '-') ;
                f = f && ((sl == 1) || (sp[1] == '\0')) ;
            }
	} /* end if (non-null) */
        return f ;
}
/* end subroutine (hasonlyminus) */

bool hasonlyplusminus(cchar *sp,int sl) noex {
        bool            f = false ;
	if (sp) ylikely {
            if (*sp != '\0') {
		f = true ;
                f = f && ((sp[0] == '+') || (sp[0] == '-')) ;
                f = f && ((sl == 1) || (sp[1] == '\0')) ;
            }
	} /* end if (non-null) */
        return f ;
}
/* end subroutine (hasonlyplusminus) */

bool hascdpath(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    if (sl && sp[0]) {
	        cint	chx_ec = mkchar('¬') ;
	        int	ch = mkchar(sp[0]) ;
	        f = (ch == chx_ec) ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hascdpath) */

bool hasmacro(cchar *lp,int ll) noex {
	bool		f = false ;
	if (lp) ylikely {
	    if ((ll > 1) && (lp[0] == '.')) {
	        if (char tbuf[3] ; twochars(tbuf,(lp+1),(ll-1)) > 1) {
	            f = (strcmp(tbuf,"\\\"") != 0) ;
	        }
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasmacro) */

bool hasvalidmagic(cchar *tbuf,int tlen,cchar *ms) noex {
	bool		f = false ;
	if (tbuf && ms) ylikely {
	    cint	ml = lenstr(ms) ;
	    if (tlen >= (ml + 1)) {
	        f = true ;
	        f = f && (strncmp(tbuf,ms,ml) == 0) ;
	        f = f && (tbuf[ml] == '\n') ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasvalidmagic) */

bool hasnotdots(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    if (sp[0] == '.') {
	        if (sl < 0) sl = lenstr(sp) ;
	        if_constexpr (f_hasnotdotswitch) {
	            switch (sl) {
	            case 1:
	                f = false ;
	                break ;
	            case 2:
	                f = (sp[1] != '.') ;
	                break ;
	            }  /* end switch */
	        } else {
	            if (sl <= 2) {
	                f = (sl != 1) ;
	                if ((! f) && (sl == 2)) f = (sp[1] != '.') ;
	            }
	        } /* end if_constexpr (f_hasnotdotswitch) */
	    } /* end if (had a leading dot) */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasnotdots) */

bool hasnotempty(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = ((!CHAR_ISWHITE(ch)) && (ch != CH_NL)) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasnotempty) */

bool hasmealone(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl == 1) {
		switch (cint ch = mkchar(*sp) ; ch) {
	        case '+':
	        case '-':
	        case '!':
		    f = true ;
		    break ;
	        } /* end switch */
	    } /* end if */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasMeAlone) */

bool hasinet4addrstr(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    int		c = 0 ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (cc *tp ; (tp = strnchr(sp,sl,'.')) != nullptr ; ) {
	        cint tl = intconv(tp - sp) ;
	        f = hasINET4Num(sp,tl) ;
	        if (! f) break ;
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        c += 1 ;
	    } /* end while */
	    if (f && (sl > 0)) {
	        c += 1 ;
	        f = hasINET4Num(sp,sl) ;
	    } /* end if */
	    if (f && (c != 4)) {
	        f = false ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasINET4AddrStr) */


/* local subroutines */

static int twochars(char *tbuf,cchar *sp,int sl) noex {
	int		c = 0 ;
	while (sl && (c < 2)) {
	    if (! CHAR_ISWHITE(*sp)) {
		tbuf[c++] = *sp ;
	    }
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	tbuf[c] = '\0' ;
	return c ;
}
/* end subroutine (twochars) */

static bool hasINET4Num(cchar *sp,int sl) noex {
	return ((sl <= 3) && hasalldig(sp,sl)) ;
}
/* end subroutine (hasINET4Num) */


