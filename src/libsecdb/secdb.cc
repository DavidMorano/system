/* secdb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Security Data-Base (SecDB) management support */
/* version %I% last-modified %G% */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
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
 * Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Group:
	secdb

	Description:
	This contains security related stuff.

	Notes:
	1. Ths subroutine |_unescape()| used below (starting at
	line number 336) has a second argument of type 'char *'.
	That suggests (indicates) that the correspoing passed
	c-string could be modified by the called subroutine.  But
	analysis shows that it does not appear to be so; that is,
	the called subroutine does not appear to actually modify
	that c-string argument.  But I honor the subroutine function
	signature anyway and arrange to pass down to it a modifiable
	c-string.  I could not just pass down the constant c-string
	of escape characters because the C++ language does not allow
	constant c-strings to used in a non-constant context (good
	for it -- than you C++ ISO committe).

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include <cstdio>
#include <cstdlib>
#include <strings.h>
#include <secdb.h>
#include <ctype.h>

#include	"secdb.h"


/* From libnsl */
extern "C" {
    extern char *_strdup_null(char *) noex ;
    extern char *_strtok_escape(char *, char *, char **) noex ;
    extern char *_strpbrk_escape(char *, char *) noex ;
    extern char *_unescape(char *, char *) noex ;
}

extern "C" {
    char *_do_unescape(char *) noex ;
}


/* forward references */

static int lenstr(cchar *s) noex {
    	return intconv(strlen(s)) ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

/*
 * kva_match(): Given a key-value array and a key, return a pointer to the
 * value that matches the key.
 */
char * kva_match(kva_t *kva, char *key) noex {
	int	i;
	kv_t	*data;

	if (kva == NULL || key == NULL) {
		return (NULL);
	}
	data = kva->data;
	for (i = 0; i < kva->length; i++) {
		if (strcmp(data[i].key, key) == 0) {
			return (data[i].value);
		}
	}

	return (NULL);
} /* end subroutine (kva_match) */

/*
 * _kva_free(): Free up memory.
 */
void _kva_free(kva_t *kva) noex {
	int	i;
	kv_t	*data;

	if (kva == NULL) {
		return;
	}
	data = kva->data;
	for (i = 0; i < kva->length; i++) {
		if (data[i].key != NULL) {
			free(data[i].key);
			data[i].key = NULL;
		}
		if (data[i].value != NULL) {
			free(data[i].value);
			data[i].value = NULL;
		}
	}
	free(kva->data);
	free(kva);
} /* end if (_kva_free) */

/*
 * _kva_free_value(): Free up memory (value) for all the occurrences of
 * the given key.
 */
void _kva_free_value(kva_t *kva, char *key) noex {
	int	ctr;
	kv_t	*data;

	if (kva == NULL) {
		return;
	}

	ctr = kva->length;
	data = kva->data;

	while (ctr--) {
		if (strcmp(data->key, key) == 0 && data->value != NULL) {
			free(data->value);
			data->value = NULL;
		}
		data++;
	}
} /* end subroutine (_kva_free_value) */

/* * new_kva(): Allocate a key-value array.  */
kva_t  * _new_kva(int size) noex {
	kva_t	*new_kva;

	if ((new_kva = (kva_t *)calloc(1, sizeof (kva_t))) == NULL) {
		return (NULL);
	}
	if ((new_kva->data = (kv_t *)calloc(1, (size*sizeof (kv_t)))) == NULL) {
		free(new_kva);
		return (NULL);
	}

	return (new_kva);
} /* end subroutine (_new_kva) */

/*
 * _str2kva(): Given a string (s) of key-value pairs, separated by delimeter
 * (del), place the values into the key value array (nkva).
 */
kva_t  * _str2kva(char *s, char *ass, char *del) noex {
	int	n = 0;
	int	m;
	int	size = KV_ADD_KEYS;
	char	*buf;
	char	*p;
	char	*pair;
	char	*key;
	char	*last_pair;
	char	*last_key;
	kv_t	*data;
	kva_t	*nkva;

	if (s == NULL ||
	    ass == NULL ||
	    del == NULL ||
	    *s == '\0' ||
	    *s == '\n' ||
	    (lenstr(s) <= 1)) {
		return (NULL);
	}
	p = s;
	while ((p = _strpbrk_escape(p, ass)) != NULL) {
		n++;
		p++;
	}
	if (n > size) {
		m = n/size;
		if (n%size) {
			++m;
		}
		size = m * KV_ADD_KEYS;
	}
	if ((nkva = _new_kva(size)) == NULL) {
		return (NULL);
	}
	data = nkva->data;
	nkva->length = 0;
	if ((buf = strdup(s)) == NULL) {
		return (NULL);
	}
	pair = _strtok_escape(buf, del, &last_pair);
	do {
		key = _strtok_escape(pair, ass, &last_key);
		if (key != NULL) {
			data[nkva->length].key = _do_unescape(key);
			data[nkva->length].value = _do_unescape(last_key);
			nkva->length++;
		}
	} while ((pair = _strtok_escape(NULL, del, &last_pair)) != NULL);
	free(buf);
	return (nkva);
} /* end subroutine (_str2kva) */

/*
 * _kva2str(): Given an array of key-value pairs, place them into a string
 * (buf). Use delimeter (del) to separate pairs.  Use assignment character
 * (ass) to separate keys and values.
 *
 * Return Values: 0  Success 1  Buffer too small
 */
int _kva2str(kva_t *kva, char *buf, int buflen, char *ass, char *del) noex {
	int	i;
	int	len;
	int	off = 0;
	kv_t	*data;

	if (kva == NULL) {
		return (0);
	}

	buf[0] = '\0';
	data = kva->data;

	for (i = 0; i < kva->length; i++) {
		if (data[i].value != NULL) {
			len = snprintf(buf + off, buflen - off, "%s%s%s%s",
			    data[i].key, ass, data[i].value, del);
			if (len < 0 || len + off >= buflen) {
				return (1);
			}
			off += len;
		}
	}

	return (0);
} /* end subroutine (_kva2str) */

int _insert2kva(kva_t *kva, char *key, char *value) noex {
	int	i;
	kv_t	*data;

	if (kva == NULL) {
		return (0);
	}
	data = kva->data;
	for (i = 0; i < kva->length; i++) {
		if (strcmp(data[i].key, key) == 0) {
			if (data[i].value != NULL)
				free(data[i].value);
			data[i].value = _strdup_null(value);
			return (0);
		}
	}
	return (1);
} /* end subroutine (_insert2kva) */

kva_t  * _kva_dup(kva_t *old_kva) noex {
	int	i;
	int	size;
	kv_t	*old_data;
	kv_t	*new_data;
	kva_t	*nkva = NULL;

	if (old_kva == NULL) {
		return (NULL);
	}
	old_data = old_kva->data;
	size = old_kva->length;
	if ((nkva = _new_kva(size)) == NULL) {
		return (NULL);
	}
	new_data = nkva->data;
	nkva->length = old_kva->length;
	for (i = 0; i < nkva->length; i++) {
		new_data[i].key = _strdup_null(old_data[i].key);
		new_data[i].value = _strdup_null(old_data[i].value);
	}

	return (nkva);
} /* end subroutine (_kva_dup) */

static void strip_spaces(char **valuep) noex {
	char *p, *start;

	/* Find first non-white space character and return pointer to it */
	for (p = *valuep; *p != '\0' && isspace((unsigned char)*p); p++)
		;

	*valuep = start = p;

	if (*p == '\0')
		return;

	p = p + lenstr(p) - 1;

	/* Remove trailing spaces */
	while (p > start && isspace((unsigned char)*p))
		p--;

	p[1] = '\0';
} /* end subroutine (strip_spaces) */

char * _do_unescape(char *src) noex {
	char *tmp = NULL;
	char *dst = NULL;

	if (src == NULL) {
		dst = _strdup_null(src);
	} else {
	    cnothrow	nt{} ;
	    cnullptr	np{} ;
	    cchar	escs[] = "=;:,\\" ;
	    {
		cint	elen = lenstr(escs) ;
	        strip_spaces(&src) ;
	        if (char *tbuf ; (tbuf = new(nt) char[elen + 1]) != np) {
		    strcpy(tbuf,escs) ;
		    {
		        tmp = _unescape(src,tbuf) ;
		        dst = (tmp == NULL) ? _strdup_null(src) : tmp ;
		    }
		    delete [] tbuf ;
	        } /* end if (m-a-f) */
	    } /* end block */
	} /* end if */

	return (dst);
} /* end subroutine (_do_unescape) */

/* * Some utilities for handling comma-separated lists.  */
char * _argv_to_csl(char **strings) noex {
	int len = 0;
	int i = 0;
	char *newstr = NULL;

	if (strings == NULL)
		return (NULL);
	for (i = 0; strings[i] != NULL; i++) {
		len += lenstr(strings[i]) + 1;
	}
	if ((len > 0) && ((newstr = (char *)malloc(len + 1)) != NULL)) {
		(void) memset(newstr, 0, len);
		for (i = 0; strings[i] != NULL; i++) {
			(void) strcat(newstr, strings[i]);
			(void) strcat(newstr, ",");
		}
		newstr[len-1] = '\0';
		return (newstr);
	} else
		return (NULL);
} /* end subroutine (_argv_to_csl) */

char ** _csl_to_argv(char *csl) noex {
	int len = 0;
	int ncommas = 0;
	int i = 0;
	char **spc = NULL;
	char *copy = NULL;
	char *pc;
	char *lasts = NULL;

	len = lenstr(csl);
	for (i = 0; i < len; i++) {
		if (csl[i] == ',')
			ncommas++;
	}
	if ((spc = (char **)malloc((ncommas + 2) * sizeof (char *))) == NULL) {
		return (NULL);
	}
	copy = strdup(csl);
	for (pc = strtok_r(copy, ",", &lasts), i = 0; pc != NULL;
	    pc = strtok_r(NULL, ",", &lasts), i++) {
		spc[i] = strdup(pc);
	}
	spc[i] = NULL;
	free(copy);
	return (spc);
} /* end subroutine (_csl_to_argv) */

void _free_argv(char **p_argv) noex {
	char **p_a;

	for (p_a = p_argv; *p_a != NULL; p_a++)
		free(*p_a);
	free(p_argv);
} /* end subroutine (_free_argv) */


#ifdef DEBUG
void print_kva(kva_t *kva) noex {
	int	i;
	kv_t	*data;

	if (kva == NULL) {
		(void) printf("  (empty)\n");
		return;
	}
	data = kva->data;
	for (i = 0; i < kva->length; i++) {
		(void) printf("  %s = %s\n",
		    data[i].key != NULL ? data[i].key : "NULL",
		    data[i].value != NULL ? data[i].value : "NULL");
	}
} /* end subroutine (print_kva) */
#endif  /* DEBUG */


