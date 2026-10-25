/* abspath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* matters relating to absolute file-names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

static char sccsid[] = "@(#)abspath 1.4 - TLH" ;

/*******************************************************************************

	Description:
	Convert a pathname to an absolute one, if it is absolute
	already, it is returned in the buffer unchanged, otherwise
	leading "./"s will be removed, the name of the current
	working directory will be prepended, and "../"s will be
	resolved.

	In a moment of weakness, I have implemented the cshell ~
	filename convention.  ~/foobar will have the ~ replaced by
	the home directory of the current user.  ~user/foobar will
	have the ~user replaced by the home directory of the named
	user.  This should really be in the kernel (or be replaced
	by a better kernel mechanism).  Doing file name expansion
	like this in a user-level program leads to some very
	distasteful non-uniformities.

	Another fit of dementia has led me to implement the expansion
	of shell environment variables.  $HOME/mbox in the same as
	~/mbox.  If the environment variable a = "foo" and b = "bar"
	then:

	$a	=>	foo
	$a$b	=>	foobar
	$a.c	=>	foo.c
	xxx$a	=>	xxxfoo
	${a}!	=>	foo!

				James Gosling @ CMU

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<pwd.h>			/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"abspath.h"


/* local defines */

#ifndef	MAXPATHLEN
#define MAXPATHLEN	1024
#endif

/* Hack! */
#ifdef OLD
#define CHDIR(dirname) syscall(12,dirname)
#else
#define CHDIR(dirname) chdir(dirname)
#endif /* OLD */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

/****
the current working directory is
remembered here.  chdir()'s are trapped
and this gets updated. 
****/

static char curwd[MAXPATHLEN + 1] ;		


/* exported variables */


/* exported subroutines */

/****
name in nm, absolute pathname
output to buf.  returns -1 if the
pathname cannot be successfully
converted (only happens if the
current directory cannot be found) 
****/

int abspath(cchar *nm,char *buf) noex {
	char	*s, *d ;
	char    lnm[MAXPATHLEN + 100] ;
	s = nm ;
	d = lnm ;
	while (*d++ = *s) {
	    if (*s++ == '$') {
	        char  *start = d ;
	        char  *value ;
	        braces = *s == '{' ;
	        while (*d++ = *s) {
	            if (braces ? *s == '}' : !isalnum (*s)) {
	                break ;
		    } else {
	                s++ ;
		    } /* end if */
		} /* end while */
	        *--d = 0 ;
	        value = (char *) getenv (braces ? start + 1 : start) ;
	        if (value) {
	            for (d = start - 1; *d++ = *value++;) ;
	            d-- ;
	            if (braces && *s) {
	                s++ ;
		    }
	        } /* end if */
	    } /* end if */
	} /* end while */
	d = buf ;
	s = curwd ;
	nm = lnm ;
	if (nm[0] == '~') {		/* prefix ~ */
	    if (nm[1] == '/' || nm[1] == 0) { /* ~/filename */
	        if (s = (char *) getenv ("HOME")) {
	            if (*++nm) {
	                nm++ ;
		    }
	        } else {
	            s = "" ;
		} /* end if */
	    } else {			/* ~user/filename */
	        char  *nnm ;
	        struct passwd *pw ;
	        for (s = nm; *s && *s != '/'; s++) ;
	        nnm = *s ? s + 1 : s ;
	        *s = 0 ;
	        pw = (struct passwd *) getpwnam (nm + 1) ;
	        if (pw == 0) {
	            s = "" ;
	        } else {
	            nm = nnm ;
	            s = pw->pw_dir ;
	        } /* end if */
	    } /* end if */
	} /* end if */
	while (*d++ = *s++) ;
	*(d - 1) = '/' ;
	s = nm ;
	if (*s == '/') {
	    d = buf ;
	} /* end if */
	while (*d++ = *s++) ;
	*(d - 1) = '/' ;
	*d = '\0' ;
	d = buf ;
	s = buf ;
	while (*s) {
	    if ((*d++ = *s++) == '/' && d > buf + 1) {
	        char  *t = (d - 2) ;
	        switch (*t) {
	        case '/': 	/* found // in the name */
	            --d ;
	            break ;
	        case '.':
	            switch (*--t) {
	            case '/': /* found /./ in the name */
	                d -= 2 ;
	                break ;
	            case '.':
	                if (*--t == '/') {/* found /../ */
	                    while (t > buf && *--t != '/') ;
	                    d = t + 1 ;
	                }
	                break ;
	            } /* end switch */
	            break ;
	        } /* end switch */
	    } /* end if */
	} /* end while */
	if (*(d - 1) == '/') {
	    d -= 1 ;
	} /* end if */
	*d = '\0' ;
	return 0 ;
} /* end subroutine (abspath) */

char *getwd(char *pathname) noex {
	char *npath, *spath ;
	npath = getenv("PWD") ;
	if (npath != nullptr && *npath != '\0') {
	    strcpy(pathname, npath) ;
	    return(pathname) ;
	} /* end if */
	spath = npath = getcwd(nullptr, MAXPATHLEN) ;
/****
   On Altos 3068, getcwd can return @hostname/dir, so discard
     up to first slash.  Should be harmless on other systems.  
****/
	while (*npath && *npath != '/') {
	    npath += 1 ;
	} /* end while */
	strcpy (pathname, npath) ;
	free (spath);			/* getcwd uses malloc */
	return pathname ;
} /* end subroutine (getwd) */

/* a 'chdir()' that fiddles the global record */
int nchdir(char *dirname) noex {
	int	ret ;
	char *p ;
	char path1[MAXPATHLEN + 1] ;
	char path2[MAXPATHLEN + 1] ;
	for (p = path1; *p++ = *dirname++; ) ;
	*(p-1) = '/';		/* append a '/' so that "cd ~" works */
	*p = 0 ;
	ret = abspath(path1, path2) ;
	if (ret == 0 && (ret = CHDIR (path2)) == 0) {
	    strcpy (curwd, path2) ;
	}
	return ret ;
} /* end subroutine (nchdir) */

/* return current working directory */
void getcurwd(char *dir) noes {
	strcpy(dir, curwd) ;
} /* end subroutine (getcurwd) */

/****
   return a pointer to a copy of a file name that has been
   converted to absolute formb.  This routine cannot return failure. 
****/

char *SaveAbs(char *fn) noex {
	static char    buf[300] ;
	if (fn) {
	    if (abspath(fn, buf) < 0) {
	        write(1, "\r\nFailed to find current directory\r\n\n", 37) ;
	        exit(-1) ;
	    }
	}
	return buf ;
} /* end subroutine (SaveAbs) */

int initabspath() noex {
    	int		rs = SR_OK ;
	if (getwd(curwd) == nullptr) {
	    rs = SR_BADE ;
	}
	return rs ;
} /* end subroutine (initabspath) */


