/* serialbuf */
/* lang=C++20 */

/* serializing sbuffer object handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-04, David A­D­ Morano
	This object was first written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides a means by which data units can be
	serialized into a sbuffer and unserialized back again. The
	serialization into the sbuffer is done in a portable way
	using Network Byte Order.

	This whole function is similar to XDR in general but this
	object allows the details of the serial sbuffer to be known
	rather than opaque as in XDR!

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<stdorder.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"serialbuf.h"


/* extern subroutines */


/* local structures */


/* forward references */


/* exported subroutines */

int serialbuf_start(serialbuf *sbp,char *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    rs = SR_OK ;
	    sbp->bp = sbuf ;
	    sbp->len = slen ;
	    sbp->i = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (serialbuf_start) */

int serialbuf_finish(serialbuf *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    sbp->bp = NULL ;
	    rs = sbp->i ;
	}
	return rs ;
}
/* end subroutine (serialbuf_finish) */

int serialbuf_getlen(serialbuf *sbp) noex {
	return sbp->i ;
}
/* end subroutine (serialbuf_getlen) */

int serialbuf_robj(serialbuf *sbp,void *sbuf,int slen) noex {
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= slen) {
	        memcpy(sbuf,sbp->bp,slen) ;
	        sbp->bp += slen ;
	        sbp->i += slen ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_robj) */

int serialbuf_wobj(serialbuf *sbp,const void *sbuf,int slen) noex {
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= slen) {
	        memcpy(sbp->bp,sbuf,slen) ;
	        sbp->bp += slen ;
	        sbp->i += slen ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wobj) */

/* "advance" the sbuffer as if we wrote something in there */
int serialbuf_adv(serialbuf *sbp,int size) noex {
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_adv) */

int serialbuf_rc(serialbuf *sbp,char *rp) noex {
	const int	size = sizeof(char) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        *rp = *sbp->bp++ ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rc) */

int serialbuf_rs(serialbuf *sbp,short *rp) noex {
	const int	size = sizeof(ushort) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rs(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rshort) */

int serialbuf_ri(serialbuf *sbp,int *rp) noex {
	const int	size = sizeof(int) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_ri(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_ri) */

int serialbuf_ria(serialbuf *sbp,int *rp,int n) noex {
	const int	size = sizeof(int) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_ri(sbp->bp,(rp + i)) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_ria) */

int serialbuf_rl(serialbuf *sbp,long *rp) noex {
	const int	size = sizeof(long) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rl(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rl) */

int serialbuf_rla(serialbuf *sbp,long *rp,int n) noex {
	const int	size = sizeof(long) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rl(sbp->bp,(rp + i)) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_rla) */

int serialbuf_rll(serialbuf *sbp,longlong *rp) noex {
	const int	size = sizeof(longlong) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rll(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rll) */

/* read a fixed length string (possibly not NUL-terminated) */
int serialbuf_rstrn(serialbuf *sbp,char *sbuf,int slen) noex {
	int		rs = SR_INVALID ;
	if (slen >= 0) {
	    rs = SR_OK ;
	    if (sbp->i >= 0) {
	        if ((sbp->len - sbp->i) >= slen) {
	            strwcpy(sbuf,sbp->bp,slen) ;
	            sbp->bp += slen ;
	            sbp->i += slen ;
	        } else {
	            sbp->i = SR_TOOBIG ;
	        }
	    }
	} /* end if (valid) */
	return (rs >= 0) ? sbp->i : rs ;
}
/* end subroutine (serialbuf_rstrn) */

/* read a NUL-terminated variable length string */
int serialbuf_rstrw(serialbuf *sbp,char *sbuf,int slen) noex {
	if (sbp->i >= 0) {
	    int		sl ;
	    int		cl ;
	    char	*cp ;
	    if (slen >= 0) {
	        int	i = 0 ;
	        cp = sbp->bp + (sbp->len - sbp->i) ;
	        while ((i < slen) && (sbp->bp < cp) && 
	            (sbp->bp[0] != '\0')) {
	            sbuf[i] = *sbp->bp++ ;
	            i += 1 ;
	        } /* end while */
	        sbuf[i] = '\0' ;
	        if ((i <= slen) && (sbp->bp == cp)) {
	            sbp->i = SR_TOOBIG ;
	        } else if ((i == slen) && (sbp->bp[0] != '\0')) {
	            cint	rl = strlen((char *) sbp->bp) ;
	            sbp->bp += (rl + 1) ;
	            sbp->i = SR_TOOBIG ;
	        } else {
	            sbp->bp += 1 ;
	            sbp->i += (i + 1) ;
	        } /* end if */
	    } else {
	        cl = sbp->len - sbp->i ;
	        sl = strwcpy(sbuf,sbp->bp,cl) - sbuf ;
	        if (sl == cl) {
	            sbp->bp += sl ;
	            sbp->i = SR_TOOBIG ;
	        } else {
	            sbp->bp += (sl + 1) ;
	            sbp->i += (sl + 1) ;
	        }
	    } /* end if */
	} /* end if (ok) */
	return sbp->i ;
}
/* end subroutine (serialbuf_rstrw) */

int serialbuf_rbuf(serialbuf *sbp,char *sbuf,int slen) noex {
	int		rs = SR_INVALID ;
	if (slen >= 0) {
	    rs = SR_OK ;
	    if (sbp->i >= 0) {
	        if ((sbp->len - sbp->i) >= slen) {
	            memcpy(sbuf,sbp->bp,slen) ;
	            sbp->bp += slen ;
	            sbp->i += slen ;
	        } else {
	            sbp->i = SR_TOOBIG ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? sbp->i : rs ;
}
/* end subroutine (serialbuf_rbuf) */

int serialbuf_ruc(serialbuf *sbp,uchar *rp) noex {
	const int	size = sizeof(uchar) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        *rp = (uchar) *sbp->bp++ ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_ruc) */

int serialbuf_rus(serialbuf *sbp,ushort *rp) noex {
	const int	size = sizeof(ushort) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rus(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rus) */

int serialbuf_rui(serialbuf *sbp,uint *rp) noex {
	const int	size = sizeof(uint) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rui(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rui) */

int serialbuf_ruia(serialbuf *sbp,uint *rp,int n) noex {
	const int	size = sizeof(uint) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rui(sbp->bp,(rp + i)) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_ruia) */

int serialbuf_rul(serialbuf *sbp,ulong *rp) noex {
	const int	size = sizeof(ulong) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rul(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rul) */

int serialbuf_rula(serialbuf *sbp,ulong *rp,int n) noex {
	const int	size = sizeof(ulong) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rul(sbp->bp,(rp + i)) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_rula) */

int serialbuf_rull(serialbuf *sbp,ulonglong *rp) noex {
	const int	size = sizeof(ulonglong) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_rull(sbp->bp,rp) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_rull) */

int serialbuf_rustrn(serialbuf *sbp,uchar *usbuf,int slen) noex {
	char		*sbuf = (char *) usbuf ;
	int		rs = SR_INVALID ;
	if (slen >= 0) {
	    rs = SR_OK ;
	    if (sbp->i >= 0) {
	        if ((sbp->len - sbp->i) >= slen) {
	            strwcpy(sbuf,sbp->bp,slen) ;
	            sbp->bp += slen ;
	            sbp->i += slen ;
	        } else {
	            sbp->i = SR_TOOBIG ;
	        }
	    }
	} /* end if (valid) */
	return (rs >= 0) ? sbp->i : rs ;
}
/* end subroutine (serialbuf_rustrn) */

int serialbuf_rustrw(serialbuf *sbp,uchar *usbuf,int slen) noex {
	if (sbp->i >= 0) {
	    int		sl ;
	    int		cl ;
	    char	*cp ;
	    char	*sbuf = (char *) usbuf ;
	    if (slen >= 0) {
	        int	i = 0 ;
	        cp = sbp->bp + (sbp->len - sbp->i) ;
	        while ((i < slen) && (sbp->bp < cp) && (sbp->bp[0] != '\0')) {
	            sbuf[i] = *sbp->bp++ ;
	            i += 1 ;
	        }
	        sbuf[i] = '\0' ;
	        if ((i <= slen) && (sbp->bp == cp)) {
	            sbp->i = SR_TOOBIG ;
	        } else if ((i == slen) && (sbp->bp[0] != '\0')) {
	            cint	rl = strlen((char *) sbp->bp) ;
	            sbp->bp += (rl + 1) ;
	            sbp->i = SR_TOOBIG ;
	        } else {
	            sbp->bp += 1 ;
	            sbp->i += (i + 1) ;
	        }
	    } else {
	        cl = sbp->len - sbp->i ;
	        sl = strwcpy(sbuf,sbp->bp,cl) - sbuf ;
	        if (sl == cl) {
	            sbp->bp += sl ;
	            sbp->i = SR_TOOBIG ;
	        } else {
	            sbp->bp += (sl + 1) ;
	            sbp->i += (sl + 1) ;
	        }
	    } /* end if */
	} /* end if (ok) */
	return sbp->i ;
}
/* end subroutine (serialbuf_rustrw) */

int serialbuf_rubuf(serialbuf *sbp,uchar *sbuf,int slen) noex {
	int		rs = SR_INVALID ;
	if (slen >= 0) {
	    rs = SR_OK ;
	    if (sbp->i >= 0) {
	        if ((sbp->len - sbp->i) >= slen) {
	            memcpy(sbuf,sbp->bp,slen) ;
	            sbp->bp += slen ;
	            sbp->i += slen ;
	        } else {
	            sbp->i = SR_TOOBIG ;
	        }
	    }
	} /* end if (valid) */
	return (rs >= 0) ? sbp->i : rs ;
}
/* end subroutine (serialbuf_rubuf) */

int serialbuf_wc(serialbuf *sbp,int ch) noex {
	const int	size = sizeof(char) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        *sbp->bp++ = (char) ch ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wc) */

int serialbuf_ws(serialbuf *sbp,int sw) noex {
	const int	size = sizeof(short) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_ws(sbp->bp,sw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_ws) */

int serialbuf_wi(serialbuf *sbp,int iw) noex {
	const int	size = sizeof(int) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wi(sbp->bp,iw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wi) */

int serialbuf_wia(serialbuf *sbp,int *iwa,int n) noex {
	const int	size = sizeof(int) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wi(sbp->bp,iwa[i]) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_wia) */

int serialbuf_wl(serialbuf *sbp,long lw) noex {
	const int	size = sizeof(long) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wl(sbp->bp,lw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wl) */

int serialbuf_wla(serialbuf *sbp,long *lwa,int n) noex {
	const int	size = sizeof(long) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wl(sbp->bp,lwa[i]) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_wla) */

int serialbuf_wll(serialbuf *sbp,longlong lw) noex {
	const int	size = sizeof(longlong) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wll(sbp->bp,lw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wll) */

/* write a fixed length string (possibly not NUL-terminated) */
int serialbuf_wstrn(serialbuf *sbp,cchar *s,int slen) noex {
	if (slen < 0) slen = strlen(s) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= slen) {
	        strncpy(sbp->bp,s,slen) ;
	        sbp->bp += slen ;
	        sbp->i += slen ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wstrn) */

/* write a NUL-terminated variable length string */
int serialbuf_wstrw(serialbuf *sbp,cchar *sbuf,int slen) noex {
	slen = strnlen(sbuf,slen) ;
	if (sbp->i >= 0) {
	    int		cl ;
	    if ((sbp->len - sbp->i) >= (slen + 1)) {
	        cl = strwcpy(sbp->bp,sbuf,slen) - sbp->bp ;
	        sbp->bp += (cl + 1) ;
	        sbp->i += (cl + 1) ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wstrw) */

int serialbuf_wbuf(serialbuf *sbp,cchar *sbuf,int slen) noex {
	int		rs = SR_INVALID ;
	if (slen >= 0) {
	    rs = SR_OK ;
	    if (sbp->i >= 0) {
	        if ((sbp->len - sbp->i) >= slen) {
	            memcpy(sbp->bp,sbuf,slen) ;
	            sbp->bp += slen ;
	            sbp->i += slen ;
	        } else {
	            sbp->i = SR_TOOBIG ;
	        }
	    }
	} /* end if (valid) */
	return (rs >= 0) ? sbp->i : rs ;
}
/* end subroutine (serialbuf_wbuf) */

int serialbuf_wuc(serialbuf *sbp,uint ch) noex {
	const int	size = sizeof(uchar) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        *sbp->bp++ = (char) ch ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wuchar) */

int serialbuf_wus(serialbuf *sbp,uint sw) noex {
	const int	size = sizeof(ushort) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wus(sbp->bp,sw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wus) */

int serialbuf_wui(serialbuf *sbp,uint iw) noex {
	const int	size = sizeof(uint) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wui(sbp->bp,iw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wui) */

int serialbuf_wuia(serialbuf *sbp,uint *iwa,int n) noex {
	const int	size = sizeof(uint) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wui(sbp->bp,iwa[i]) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_wuia) */

int serialbuf_wul(serialbuf *sbp,ulong lw) noex {
	const int	size = sizeof(ulong) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wul(sbp->bp,lw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wul) */

int serialbuf_wula(serialbuf *sbp,ulong *lwa,int n) noex {
	const int	size = sizeof(ulong) ;
	for (int i = 0 ; (sbp->i >= 0) && (i < n) ; i += 1) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wul(sbp->bp,lwa[i]) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	} /* end for */
	return sbp->i ;
}
/* end subroutine (serialbuf_wula) */

int serialbuf_wull(serialbuf *sbp,ulonglong lw) noex {
	const int	size = sizeof(ulonglong) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= size) {
	        stdorder_wull(sbp->bp,lw) ;
	        sbp->bp += size ;
	        sbp->i += size ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wull) */

int serialbuf_wustrn(serialbuf *sbp,const uchar *usbuf,int slen) noex {
	char		*sbuf = (char *) usbuf ;
	if (slen < 0) slen = strlen(sbuf) ;
	if (sbp->i >= 0) {
	    if ((sbp->len - sbp->i) >= slen) {
	        strncpy(sbp->bp,sbuf,slen) ;
	        sbp->bp += slen ;
	        sbp->i += slen ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return sbp->i ;
}
/* end subroutine (serialbuf_wustrn) */

int serialbuf_wustrw(serialbuf *sbp,const uchar *usbuf,int slen) noex {
	char		*sbuf = (char *) usbuf ;
	int		rs = SR_INVALID ;
	if (slen < 0) slen = strlen(sbuf) ;
	if (sbp->i >= 0) {
	    int		cl ;
	    if ((sbp->len - sbp->i) >= (slen + 1)) {
	        cl = strwcpy(sbp->bp,sbuf,slen) - sbp->bp ;
	        sbp->bp += (cl + 1) ;
	        sbp->i += (cl + 1) ;
	    } else {
	        sbp->i = SR_TOOBIG ;
	    }
	}
	return (rs >= 0) ? sbp->i : rs ;
}
/* end subroutine (serialbuf_wustrw) */

int serialbuf_wubuf(serialbuf *sbp,const uchar *sbuf,int slen) noex {
	int		rs = SR_INVALID ;
	if (slen >= 0) {
	     rs = SR_OK ;
	    if (sbp->i >= 0) {
	        if ((sbp->len - sbp->i) >= slen) {
	            memcpy(sbp->bp,sbuf,slen) ;
	            sbp->bp += slen ;
	            sbp->i += slen ;
	        } else {
	            sbp->i = SR_TOOBIG ;
	        }
	    }
	} /* end if (valid) */
	return (rs >= 0) ? sbp->i : rs ;
}
/* end subroutine (serialbuf_wubuf) */


