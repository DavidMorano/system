int fifostr::start(int an,int ao) noex {
	return fifostr_start(this,an,ao) ;
}

int fifostr::add(cchar *sp,int sl) noex {
	return fifostr_add(this,sp,sl) ;
}

int fifostr::adduniq(cchar *sp,int sl) noex {
	return fifostr_adduniq(this,sp,sl) ;
}

int fifostr::addsyms(cchar *on,mainv sv) noex {
	return fifostr_addsyms(this,on,sv) ;
}

int fifostr::insert(int ai,cchar *sp,int sl) noex {
	return fifostr_insert(this,ai,sp,sl) ;
}

int fifostr::get(int ai,cchar **rpp) noex {
	return fifostr_get(this,ai,rpp) ;
}

int fifostr::envfile(cchar *fn) noex {
	return fifostr_envfile(this,fn) ;
}

int fifostr::getvec(mainv *rppp) noex {
	return fifostr_getvec(this,rppp) ;
}

int fifostr::del(int ai) noex {
	if (ai < 0) ai = 0 ;
	return fifostr_del(this,ai) ;
}

void fifostr::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("fifostr",rs,"fini-finish") ;
	}
}

fifostr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fifostrmem_count:
	        rs = fifostr_count(op) ;
	        break ;
	    case fifostrmem_delall:
	        rs = fifostr_delall(op) ;
	        break ;
	    case fifostrmem_strsize:
	        rs = fifostr_strsize(op) ;
	        break ;
	    case fifostrmem_recsize:
	        rs = fifostr_recsize(op) ;
	        break ;
	    case fifostrmem_audit:
	        rs = fifostr_audit(op) ;
	        break ;
	    case fifostrmem_finish:
	        rs = fifostr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (fifostr_co::operator) */

bool fifostr_iter::operator == (const fifostr_iter &oit) noex {
	return (va == oit.va) && (i == oit.i) && (ii == oit.ii) ;
}

bool fifostr_iter::operator != (const fifostr_iter &oit) noex {
	bool		f = false ;
	f = f || (va != oit.va) ;
	f = f || (ii != oit.ii) ;
	if (!f) {
	    f = (i < oit.i) ;
	}
	return f ;
}
/* end method (fifostr_iter::operator) */

fifostr_iter fifostr_iter::operator + (int n) const noex {
	fifostr_iter	rit(va,i,i) ;
	rit.i = ((rit.i + n) >= 0) ? (rit.i + n) : 0 ;
	return rit ;
}

fifostr_iter fifostr_iter::operator += (int n) noex {
	fifostr_iter	rit(va,i,i) ;
	i = ((i + n) >= 0) ? (i + n) : 0 ;
	rit.i = i ;
	return rit ;
}

fifostr_iter fifostr_iter::operator ++ () noex { /* pre */
	increment() ;
	return (*this) ;
}

fifostr_iter fifostr_iter::operator ++ (int) noex { /* post */
	fifostr_iter	pre(*this) ;
	increment() ;
	return pre ;
}

void fifostr_iter::increment(int n) noex {
	if ((i + n) < 0) n = -i ;
	if (n != 0) {
	    i += n ;
	    while ((i < ii) && (va[i] == nullptr)) {
	        i += 1 ;
	    }
	}
}
/* end method (fifostr_iter::increment) */


