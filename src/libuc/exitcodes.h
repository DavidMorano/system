/* exitcodes HEADER */
/* lang=C20 */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	EXITCODES_INCLUDE
#define	EXITCODES_INCLUDE


#include	<sysexits.h>


/*
 *  SYSEXITS.H -- Exit status codes employed by the mail subsystem.
 *
 *	This include file attempts to categorize possible error
 *	exit statuses for mail subsystem.
 *
 *	Error numbers begin at EX__BASE to reduce the possibility of
 *	clashing with other exit statuses that random programs may
 *	already return.  The meaning of the codes is [sic] approximately
 *	as follows:
 *
 *	EX_USAGE (64) -- The command was used incorrectly, e.g., with
 *		the wrong number of arguments, a bad flag, a bad
 *		syntax in a parameter, or whatever.
 *	EX_DATAERR (65) -- The input data was incorrect in some way.
 *		This should only be used for user's data & not
 *		system files.
 *	EX_NOINPUT (66) -- An input file (not a system file) did not
 *		exist or was not readable.  This could also include
 *		errors like "No message" to a mailer (if it cared
 *		to catch it).
 *	EX_NOUSER (67) -- The user specified did not exist.  This might
 *		be used for mail addresses or remote logins.
 *	EX_NOHOST (68) -- The host specified did not exist.  This is used
 *		in mail addresses or network requests.
 *	EX_UNAVAILABLE (69) -- A service is unavailable.  This can occur
 *		if a support program or file does not exist.  This
 *		can also be used as a catchall message when something
 *		you wanted to do doesn't work, but you don't know
 *		why.
 *	EX_SOFTWARE (70) -- An internal software error has been detected.
 *		This should be limited to non-operating system related
 *		errors as possible.
 *	EX_OSERR (71) -- An operating system error has been detected.
 *		This is intended to be used for such things as "cannot
 *		fork", "cannot create pipe", or the like.  It includes
 *		things like getuid returning a user that does not
 *		exist in the passwd file.
 *	EX_OSFILE (72) -- Some system file (e.g., /etc/passwd, /etc/utmp,
 *		etc.) does not exist, cannot be opened, or has some
 *		sort of error (e.g., syntax error).
 *	EX_CANTCREAT (73) -- A (user specified) output file cannot be
 *		created.
 *	EX_IOERR (74) -- An error occurred while doing I/O on some file.
 *	EX_TEMPFAIL (75) -- temporary failure, indicating something that
 *		is not really an error.  In sendmail, this means
 *		that a mailer (e.g.) could not create a connection,
 *		and the request should be reattempted later.
 *	EX_PROTOCOL (76) -- the remote system returned something that
 *		was "not possible" during a protocol exchange.
 *	EX_NOPERM (77) -- You did not have sufficient permission to
 *		perform the operation.  This is not intended for
 *		file system problems, which should use NOINPUT or
 *		CANTCREAT, but rather for higher level permissions.
 *		For example, kre uses this to restrict who students
 *		can send mail to.

	EX_CONFIG (78)		configuration error

	EX_NOTFOUND (79)	entry not found

 */

/* exit OK */
#define	EX_OK		0		/* this is general (good) success */

/* general favorites */
#define	EX_UNKNOWN	122		/* unknown reason */
#define	EX_INTR		123		/* program interrupted */
#define	EX_MUTEX	124		/* mutual exclusion conflict */
#define	EX_INFO		125		/* information only */

/* SHELL favorites */
#define	EX_NOEXEC	126		/* could not 'exec(2)' */
#define	EX_NOPROG	127		/* program not found */
#define	EX_TERM		128		/* program terminated */


#endif /* EXITCODES_INCLUDE */


