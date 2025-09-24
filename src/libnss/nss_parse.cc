/* nss_parse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Name-Server-Switch (NSS) database parsing support */
/* version %I% last-modified %G% */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */


#include	<envstandards.h>	/* MUST be ordered first to configure */

#ifdef	COMMENT /* modification by David A-D- Morano */
#include "mt.h"
#include <cstdio>
#endif /* COMMENT */

#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<strx.h>
#include	<localmisc.h>

/* modification by David A-D- Morano */
#include	"nss_parse.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

constexpr local int lenstr(cchar *s) noex {
    cchar	*ss = s ;
    while (*s) {
	s += 1 ;
    }
    return intconv(s - ss) ;
} /* end subroutine (lenstr) */


/* local variables */


/* exported variables */


/* exported subroutines */

/*
 * _strtok_escape()
 *   Like strtok_r, except we do not break on a token if it is escaped
 *   with the escape character (\).
 */
char * _strtok_escape(char *locstr,char *sepset,char **lasts) noex {
	char	*r;

	/* first or subsequent call */
	if (locstr == nullptr)
		locstr = *lasts;

	if (locstr == nullptr)		/* return if no tokens remaining */
		return (nullptr);

	if (*locstr == '\0')		/* return if no tokens remaining */
		return (nullptr);

	/* move past token */
	if ((r = _strpbrk_escape(locstr,sepset)) == nullptr)
		*lasts = 0;	/* indicate this is last token */
	else {
		*r = '\0';
		*lasts = r+1;
	}
	return locstr ;
} /* end subroutine (_strtok_escape) */

/*
 * Return ptr to first occurrence of any non-escaped character from `brkset'
 * in the character string `string'; nullptr if none exists.
 */ 
char * _strpbrk_escape(char *locstr,char *brkset) noex {
	const char *p;

	do {
		for (p = brkset; *p != '\0' && *p != *locstr; ++p)
			;
		if (p == locstr)
			return ((char *)locstr);
		if (*p != '\0') {
			if (*(locstr-1) != '\\')
				return ((char *)locstr);
		}
	} while (*locstr++);

	return nullptr ;
} /* end subroutine (_strpbrk_escape) */

char * _escape(char *s,char *esc) noex {
	int	nescs = 0;	/* number of escapes to place in s */
	int	i, j;
	int	len_s;
	char	*tmp;

	if (s == nullptr || esc == nullptr)
		return (nullptr);

	len_s = lenstr(s) ;
	for (i = 0; i < len_s; i++) {
		if (strchr(esc, s[i])) {
			nescs++;
		}
	}
	if ((tmp = charp(malloc(nescs + len_s + 1))) == nullptr)
		return (nullptr);
	for (i = 0, j = 0; i < len_s; i++) {
		if (strchr(esc, s[i])) {
			tmp[j++] = '\\';
		}
		tmp[j++] = s[i];
	}
	tmp[len_s + nescs] = '\0';
	return (tmp);
} /* end subroutine (_escape) */

char * _unescape(char *s,char *esc) noex {
	int	len_s;
	int	i, j;
	char	*tmp;

	if (s == nullptr || esc == nullptr)
		return (nullptr);

	len_s = lenstr(s);
	if ((tmp = charp(malloc(len_s + 1))) == nullptr)
		return (nullptr);
	for (i = 0, j = 0; i < len_s; i++) {
		if (s[i] == '\\' && strchr(esc, s[i + 1])) {
			tmp[j++] = s[++i];
		} else {
			tmp[j++] = s[i];
		}
	} /* end for */
	tmp[j] = '\0';
	return tmp ;
} /* end subroutine (_unescape) */

char * _strdup_null(char *s) noex {
	return (strdup(s ? s : ""));
}


#ifdef	COMMENT /* modification by David A-D- Morano */

/*
 * read a line into buffer from a mmap'ed file.
 * return length of line read.
 */
int _readbufline(char *mapbuf,	/* input mmap buffer */
    int mapsize,		/* input size */
    char *buffer,		/* output storage */
    int buflen,			/* output size */
    int *lastlen) noex		/* input read till here last time */
{
	int	linelen;

	for (;;) {
		linelen = 0;
		while (linelen < buflen - 1) {	/* "- 1" saves room for \n\0 */
			if (*lastlen >= mapsize) {
				if (linelen == 0 ||
					buffer[linelen - 1] == '\\') {
						return (-1);
					} else {
						buffer[linelen] = '\n';
						buffer[linelen + 1] = '\0';
						return (linelen);
					}
			}
			switch (mapbuf[*lastlen]) {
			case '\n':
				(*lastlen)++;
				if (linelen > 0 &&
				    buffer[linelen - 1] == '\\') {
					--linelen;	/* remove the '\\' */
				} else {
					buffer[linelen] = '\n';
					buffer[linelen + 1] = '\0';
					return (linelen);
				}
				break;
			default:
				buffer[linelen] = mapbuf[*lastlen];
				(*lastlen)++;
				linelen++;
			}
		}
		/* Buffer overflow -- eat rest of line and loop again */
		while (mapbuf[*lastlen] != '\n') {
			if (mapbuf[*lastlen] == EOF) {
				return (-1);
			}
			(*lastlen)++;
		};
	} /* end for */
	/* NOTREACHED */
} /* end subroutine (_readbufline) */

#endif /* COMMENT */


