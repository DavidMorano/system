/* ismatstar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* see if a string matches a "ismatstar" ('*') expresssion */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISMATSTAR_INCLUDE
#define	ISMATSTAR_INCLUDE


EXTERNC_begin

extern bool	ismatstar(cchar *,cchar *) noex ;

extern bool	starmat(cchar *sw,cchar *sc) noex {
    return ismatstar(sw,sc) ;
}

EXTERNC_end


#endif /* ISMATSTAR_INCLUDE */


