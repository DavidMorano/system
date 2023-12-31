/* hasx */
/* lang=C++20 */

/* has a c-string some characteristic? */
/* version %I% last-modified %G% */

#define	CF_HASNOTDOTSWITCH	1	/* switch or not */

/* revision history:

	= 1998-10-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.


	Name:
	hsempty

	Description:
        This subroutine determines if a given string is empty or not.

	Synopsis:
	int hasempty(cchar *sp,int sl) noex

	Arguments:
	sp		string pointer
	sl		string length

	Returns:
	1		true (empty)
	0		false (not empty)


	Name:
	haseoh

	Description:
	Determine if the given string consists of an End-Of-Header
	(EOH) sequence.  An EOH is a leadering blank line of two
	sorts:
	<NL>
	<CR><NL>

	Synopsis:
	int haseoh(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	false		assertion fails
	true		assertion succeeds


	Name:
	hasnonwhite

	Description:
	Determine if the given c-string has a non-white character.

	Synopsis:
	int hasnonwhite(cchar *sp,int sl) noex

	Arguments:
	sp		string pointer
	sl		string length

	Returns:
	false		string is empty
	true		string has some non-white content


	Name:
	hasleadcolon

	Description:
	Does the given string have a colon character (':') occur
	before a slash character ('/')?

	Synopsis:
	int hasleadcolon(cchar *sp,int sl) noex

	Arguments:
	sp		pointer to string
	sl		length of string

	Returns:
	false		answer NO
	true		answer YES


	Name:
	hasprintlatin

	Description:
	Are all of the characters printable Latin characters?

	Synopsis:
	int hasprintlatin(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	false		string does not have bad stuff in it
	true		string has some bad stuff in it


	Name:
	hasprintbad

	Description:
	Does the given string have characters that are bad to print?

	Synopsis:
	int hasprintbad(ccha *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	false		string does not have bad stuff in it
	true		string has some bad stuff in it


	Name:
	hasallalpha

	Description:
	Are all of the characters Alpha?

	Synopsis:
	int hasallalpha(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of string to test

	Returns:
	false		string does not have all digits
	true		string has all digits in it


	Name:
	hasalnum

	Description:
	Are all of the characters Alpha-Num?

	Synopsis:
	int hasallalnum(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of string to test

	Returns:
	false		string does not have all digits
	true		string has all digits in it


	Name:
	hasalldig

	Description:
	Are all of the characters in the given string digits?

	Synopsis:
	int hasalldig(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	false		string does not have all digits
	true		string has all digits in it


	Name:
	hasallbase

	Description:
	We test if a counted strin contains all of the proper digits
	consistent with the numeric base supplied.

	Synopsis:
	int hasallbase(cchar *sp,int sl,int base) noex

	Arguments:
	sp		string to test
	sl		length of strin to test
	base		base to check against

	Returns:
	false		string does not have all digits
	true		string has all digits in it


	Name:
	hasvarprefix

	Description:
	This subroutine tests whether the file-name has the var-path
	prefix on it.  A so-called "var-path" prefix looks like
	either one of the following:

	%<string>/<comething>/<...>
	/%<string>/<something>/...


	Name:
	hasValidMagic

	Description:
	This subroutine verifies that the given unknown string
	contains the given valid magic string.

	Synopsis:
	int hasValidMagic(cchar *sp,int sl,cchar *ms) noex

	Arguments:
	cchar	*sp		string buffer to test
	int	sl		length of string buffer
	int	ms		givem valid magic string

	Returns:
	false		does not match
	true		YES it matches


	Name:
	hasnotdots

	Description:
	This subroutine determines if the given string has neither of the
	following:
	+ one dot character
	+ two dot characters

	Synopsis:
	int hasNotDots(cchar *sp,int sl) noex

	Arguments:
	sp		pointer to given string
	sl		length of given string

	Returns:
	false		string has the standard dot-dirs
	true		string does not have the standard dot-dirs


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<ucvariables.hh>
#include	<ascii.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"hasx.h"


/* local defines */

#ifndef	CF_HASNOTDOTSWITCH
#define	CF_HASNOTDOTSWITCH	0
#endif


/* local variables */

constexpr bool		f_hasnotdotswitch = CF_HASNOTDOTSWITCH ;

static const int	maxbase = strlen(varname.digtab) ;


/* forward references */

static int twochars(char *,cchar *,int) noex ;


/* exported subroutines */

bool haswhite(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    f = CHAR_ISWHITE(*sp) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (haswhite) */

bool haslc(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    f = CHAR_ISLC(*sp) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (haslc) */

bool hasuc(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    f = CHAR_ISUC(*sp) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasuc) */

bool hasdig(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    f = CHAR_ISDIG(*sp) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasdig) */

bool hasempty(cchar *sp,int sl) noex {
	bool		f = true ;
	while (sl && *sp) {
	    f = CHAR_ISWHITE(*sp) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	if ((!f) && *sp) {
	    f = (*sp == '\n') ;
	} /* end if */
	return f ;
}
/* end subroutine (hasempty) */

bool hasdots(cchar *np,int nl) noex {
	bool		f = false ;
	if (np[0] == '.') {
	    if (nl < 0) nl = strlen(np) ;
	    f = f || (nl == 1) ;
	    f = f || ((nl == 2) && (np[1] == '.')) ;
	}
	return f ;
}
/* end subroutine (hasdots) */

bool haseoh(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sl >  0) {
	    f = (sp[0] == '\n') ;
	    f = f || ((sl > 1) && (sp[0] == '\r') && (sp[1] == '\n')) ;
	}
	return f ;
}
/* end subroutine (haseoh) */

bool hasalpha(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isalphalatin(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasalpha) */

bool hasnum(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isdigitlatin(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasnum) */

bool hasdoublewhite(cchar *sp,int sl) noex {
	bool		f_prev = false ;
	bool		f = false ;
	for (int i = 0 ; sl && sp[i] ; i += 1) {
	    cbool	f_white = CHAR_ISWHITE(sp[i]) ;
	    if (f_white) {
		f = f_prev ;
	        f = f || (sp[i] != CH_SP) ;	/* to force compaction */
	        f = f || (i == 0) ; 		/* to force compaction */
	        if (f) break ;
	    } /* end if */
	    f_prev = f_white ;
	    sl -= 1 ;
	} /* end for */
	return f ;
}
/* end subroutine (hasdoublewhite) */

bool hasnonwhite(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) {
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

bool hascontent(cchar *sp,int sl) noex {
	return hasnotempty(sp,sl) ;
}
/* end subroutine (hascontent) */

bool hasleadcolon(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    f = (*sp == ':') ;
	    if (f) break ;
	    if (*sp == '/') break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasleadcolon) */

bool hasprintlatin(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isprintlatin(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasprintlatin) */

bool hasprintbad(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isprintbad(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasprintbad) */

bool hasallalpha(cchar *sp,int sl) noex {
	bool		f = true ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isalphalatin(ch) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasallalpha) */

bool hasallalnum(cchar *sp,int sl) noex {
	bool		f = true ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isalnumlatin(ch) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasallalnum) */

bool hasallwhite(cchar *sp,int sl) noex {
	bool		f = true ;
	while (sl && *sp) {
	    f = CHAR_ISWHITE(*sp) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasallwhite) */

bool hasalllc(cchar *sp,int sl) noex {
	bool		f = true ;
	while (sl && *sp) {
	    f = CHAR_ISLC(*sp) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasalllc) */

bool hasalluc(cchar *sp,int sl) noex {
	bool		f = true ;
	while (sl && *sp) {
	    f = CHAR_ISUC(*sp) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasalluc) */

bool hasalldig(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isdigitlatin(ch) ;
	    if (! f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasalldig) */

bool hasallminus(cchar *sp,int sl) noex {
	bool		f = false ;
	if (*sp != '\0') {
	    f = (sp[0] == '-') ;
	    f = f && ((sl == 1) || (sp[1] == '\0')) ;
	}
	return f ;
}
/* end subroutine (hasallminus) */

bool hasallplusminus(cchar *sp,int sl) noex {
	bool		f = false ;
	if (*sp != '\0') {
	    f = (sp[0] == '+') || (sp[0] == '-') ;
	    f = f && ((sl == 1) || (sp[1] == '\0')) ;
	}
	return f ;
}
/* end subroutine (hasallplusminus) */

bool hasallbase(cchar *sp,int sl,int b) noex {
	bool		f = false ;
	if ((b >= 2) && (b <= maxbase)) {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        cint	v = CHAR_TOVAL(*sp) ;
	        f = (v < b) ;
	        f = f || ((ch == '-') || CHAR_ISWHITE(ch) || (ch == CH_NBSP)) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (valid base) */
	return f ;
}
/* end subroutine (hasallbase) */

bool hasallchr(cchar *sp,int sl,int sch) noex {
	bool		f = true ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = (ch == sch) ;
	    if (!f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasallchr) */

bool hasvarpathprefix(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) {
	    cint	ec = '%' ;
	    f = f || (sl && (sp[0] == ec)) ;
	    if (!f) {
		f = true ;
	        f = f && ((sl < 0) || (sl > 1)) ;
		f = f && (sp[0] == '/') && (sp[1] == ec) ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasvarpathprefix) */

bool hascdpath(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sl && sp) {
	    cint	ec = UC('�') ;
	    int		ch = mkchar(sp[0]) ;
	    f = (ch == ec) ;
	}
	return f ;
}
/* end subroutine (hascdpath) */

bool hasmacro(cchar *lp,int ll) noex {
	bool		f = false ;
	if ((ll > 1) && (lp[0] == '.')) {
	    int		c ;
	    char	tbuf[3] ;
	    if ((c = twochars(tbuf,(lp+1),(ll-1))) > 1) {
	        f = (strcmp(tbuf,"\\\"") != 0) ;
	    }
	}
	return f ;
}
/* end subroutine (hasmacro) */

bool hasvalidmagic(char *tbuf,int tlen,char *ms) noex {
	cint		ml = strlen(ms) ;
	bool		f = false ;
	if (tlen >= (ml+1)) {
	    f = true ;
	    f = f && (strncmp(tbuf,ms,ml) == 0) ;
	    f = f && (tbuf[ml] == '\n') ;
	}
	return f ;
}
/* end subroutine (hasvalidmagic) */

bool hasnotdots(cchar *np,int nl) noex {
	bool		f = true ;
	if (np[0] == '.') {
	    if (nl < 0) nl = strlen(np) ;
	    if constexpr (f_hasnotdotswitch) {
	        switch (nl) {
	        case 1:
	            f = false ;
	            break ;
	        case 2:
	            f = (np[1] != '.') ;
	            break ;
	        }  /* end switch */
	    } else {
	        if (nl <= 2) {
	            f = (nl != 1) ;
	            if ((!f) && (nl == 2)) f = (np[1] != '.') ;
	        }
	    } /* end if constexpr) */
	} /* end if (had a leading dot) */
	return f ;
}
/* end subroutine (hasnotdots) */

bool hasnotempty(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = ((!CHAR_ISWHITE(ch)) && (ch != CH_NL)) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasnotempty) */


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


