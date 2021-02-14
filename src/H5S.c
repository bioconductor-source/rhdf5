#include "H5S.h"

/* hid_t H5Screate( H5S_class_t type ) */
SEXP _H5Screate( SEXP _type ) {
    H5S_class_t type = INTEGER(_type)[0];
    hid_t hid = H5Screate( type );
    addHandle(hid);
    
    SEXP Rval;
    PROTECT(Rval = HID_2_STRSXP(hid));
    UNPROTECT(1);
    return Rval;
}

/* hid_t H5Scopy( hid_t space_id ) */
SEXP _H5Scopy( SEXP _space_id ) {
    hid_t space_id = STRSXP_2_HID( _space_id );
    hid_t hid = H5Scopy( space_id );
    addHandle(hid);
    
    SEXP Rval;
    PROTECT(Rval = HID_2_STRSXP(hid));
    UNPROTECT(1);
    return Rval;
}

/* herr_t H5Sclose( hid_t space_id ) */
SEXP _H5Sclose( SEXP _space_id ) {
    hid_t space_id = STRSXP_2_HID( _space_id );
    herr_t herr = H5Sclose( space_id );
    if (herr == 0) {
        removeHandle(space_id);
    }
    
    SEXP Rval;
    PROTECT(Rval = allocVector(INTSXP, 1));
    INTEGER(Rval)[0] = herr;
    UNPROTECT(1);
    return Rval;
}

/* hid_t H5Screate_simple( int rank, const hsize_t * dims, const hsize_t * maxdims ) */
SEXP _H5Screate_simple( SEXP _dims, SEXP _maxdims ) {
    hid_t hid;
    int rank = length(_dims);
    hsize_t dims[rank];
    for (int i=0; i<rank; i++) {
        dims[i] = REAL(_dims)[i];
    }
    if (length(_maxdims) == 0) {
        hid = H5Screate_simple( rank, dims, NULL);
        addHandle(hid);
    } else {
        if (length(_maxdims) != length(_dims)) {
            warning("dims and maxdims must have the same length.\n");
            hid = -1;
        } else {
            hsize_t maxdims[rank];
            for (int i=0; i<rank; i++) {
                maxdims[i] = REAL(_maxdims)[i];
            }
            hid = H5Screate_simple( rank, dims, maxdims);
            addHandle(hid);
        }
    }
    
    SEXP Rval;
    PROTECT(Rval = HID_2_STRSXP(hid));
    UNPROTECT(1);
    return Rval;
}

/* htri_t H5Sis_simple( hid_t space_id ) */
SEXP _H5Sis_simple( SEXP _space_id ) {
    hid_t space_id = STRSXP_2_HID( _space_id );
    htri_t htri = H5Sis_simple( space_id );
    
    SEXP Rval;
    PROTECT(Rval = allocVector(INTSXP, 1));
    INTEGER(Rval)[0] = htri;
    UNPROTECT(1);
    return Rval;
}

/* int H5Sget_simple_extent_dims(hid_t space_id, hsize_t *dims, hsize_t *maxdims ) */
SEXP _H5Sget_simple_extent_dims( SEXP _space_id ) {
    hid_t space_id = STRSXP_2_HID( _space_id );
    hsize_t   size[H5S_MAX_RANK];
    hsize_t   maxsize[H5S_MAX_RANK];
    int rank = H5Sget_simple_extent_dims(space_id, size, maxsize);
    
    SEXP Rval = PROTECT(allocVector(VECSXP, 3));
    SET_VECTOR_ELT(Rval,0,ScalarInteger(rank));
    
    SEXP Rsize;
    SEXP Rmaxsize;
    int size_is_numeric = 0, maxsize_is_numeric = 0;
    if (rank < 0) {
        Rsize = PROTECT(allocVector(INTSXP, 0));
        Rmaxsize = PROTECT(allocVector(INTSXP, 0));
        SET_VECTOR_ELT(Rval,1,Rsize);
        SET_VECTOR_ELT(Rval,2,Rmaxsize);
        UNPROTECT(2);
    } else {
        for (int i=0; i < rank; i++) {
            size_is_numeric += size[i] > R_LEN_T_MAX;
            maxsize_is_numeric += maxsize[i] > R_LEN_T_MAX;
        }
        Rsize = PROTECT(allocVector(REALSXP, rank));
        Rmaxsize = PROTECT(allocVector(REALSXP, rank));
        for (int i=0; i < rank; i++) {
            REAL(Rsize)[i] = size[i];
            REAL(Rmaxsize)[i] = maxsize[i];
        }
        SET_VECTOR_ELT(Rval,1,Rsize);
        SET_VECTOR_ELT(Rval,2,Rmaxsize);
        UNPROTECT(2);
    }
    
    if (!size_is_numeric)
        SET_VECTOR_ELT(Rval, 1, AS_INTEGER(VECTOR_ELT(Rval, 1)));
    if (!maxsize_is_numeric)
        SET_VECTOR_ELT(Rval, 2, AS_INTEGER(VECTOR_ELT(Rval, 2)));
    
    SEXP names = PROTECT(allocVector(STRSXP, 3));
    SET_STRING_ELT(names, 0, mkChar("rank"));
    SET_STRING_ELT(names, 1, mkChar("size"));
    SET_STRING_ELT(names, 2, mkChar("maxsize"));
    SET_NAMES(Rval, names);
    UNPROTECT(1);
    UNPROTECT(1);
    return(Rval);
}

/* herr_t H5Sset_extent_simple( hid_t space_id, int rank, const hsize_t *current_size, const hsize_t *maximum_size ) */
SEXP _H5Sset_extent_simple( SEXP _space_id, SEXP _current_size, SEXP _maximum_size ) {
    hid_t space_id = STRSXP_2_HID( _space_id );
    hid_t herr;
    int rank = length(_current_size);
    hsize_t current_size[rank];
    for (int i=0; i<rank; i++) {
        current_size[i] = REAL(_current_size)[i];
    }
    if (length(_maximum_size) == 0) {
        herr = H5Sset_extent_simple( space_id, rank, current_size, NULL);
    } else {
        if (length(_maximum_size) != length(_current_size)) {
            warning("dims and maxdims must have the same length.\n");
            herr = -1;
        } else {
            hsize_t maximum_size[rank];
            for (int i=0; i<rank; i++) {
                maximum_size[i] = REAL(_maximum_size)[i];
            }
            herr = H5Sset_extent_simple( space_id, rank, current_size, maximum_size);
        }
    }
    
    SEXP Rval = ScalarInteger(herr);
    return Rval;
}


/* hssize_t H5S_GET_SELECT_NPOINTS(hid_t spaceid) */
SEXP _H5Sget_select_npoints( SEXP _space_id ) {
  hid_t space_id = STRSXP_2_HID( _space_id );
  
  hssize_t npoints = H5Sget_select_npoints(space_id);
  SEXP Rval = ScalarReal( (double) npoints);
  return Rval;
}


/* herr_t H5Sselect_none(hid_t spaceid) */
SEXP _H5Sselect_none( SEXP _space_id ) {
  
  hid_t space_id = STRSXP_2_HID( _space_id );
  herr_t herr = H5Sselect_none(space_id);
  
  SEXP Rval = ScalarInteger(herr);
  return Rval;
}

/* herr_t H5Sselect_hyperslab(hid_t space_id, H5S_seloper_t op, const hsize_t *start, const hsize_t *stride, const hsize_t *count, const hsize_t *block ) */
SEXP _H5Sselect_hyperslab( SEXP _space_id, SEXP _op, SEXP _start, SEXP _stride, SEXP _count, SEXP _block ) {
    hid_t space_id = STRSXP_2_HID( _space_id );
    H5S_seloper_t op =  INTEGER(_op)[0];
    hsize_t start[LENGTH(_start)];
    hsize_t stride[LENGTH(_stride)];
    hsize_t count[LENGTH(_count)];
    hsize_t block[LENGTH(_block)];
    int i;
    for (i=0; i < LENGTH(_start); i++) {
        start[i] = REAL(_start)[i];
    }
    for (i=0; i < LENGTH(_stride); i++) {
        stride[i] = REAL(_stride)[i];
    }
    for (i=0; i < LENGTH(_count); i++) {
        count[i] = REAL(_count)[i];
    }
    for (i=0; i < LENGTH(_block); i++) {
        block[i] = REAL(_block)[i];
    }
    
    herr_t herr = H5Sselect_hyperslab( space_id, op, start, stride, count, block );

    /*
    if(H5Sis_regular_hyperslab(space_id)) {
      Rprintf("Regular\n");
    } else {
      Rprintf("Not regular\n");
    }
    
    if(H5Sselect_valid(space_id) > 0) {
      Rprintf("Valid\n");
    } else {
      Rprintf("Not valid\n");
    }
    
    int npoints = H5Sget_select_npoints(space_id);
    int nblocks = H5Sget_select_hyper_nblocks(space_id);
    Rprintf("N points: %d - N blocks: %d\n", npoints, nblocks);
    
    int RANK2 = 2;
    int l, k;
    hsize_t *buf = (hsize_t *)malloc(sizeof(hsize_t)*2*RANK2*nblocks);
    
    H5Sget_select_hyper_blocklist (space_id, (hsize_t)0, nblocks, buf);
    
    for (l=0; l<nblocks; l++) {
      Rprintf("(");
      for (k=0; k<RANK2-1; k++) 
        Rprintf("%d,", (int)buf[k]);
      Rprintf("%d ) - (", (int)buf[k]);
      for (k=0; k<RANK2-1; k++) 
        Rprintf("%d,", (int)buf[RANK2+k]);
      Rprintf("%d)\n", (int)buf[RANK2+k]);
    }
    free(buf);
    
    hsize_t start_out[2],stride_out[2],count_out[2],block_out[2];
    if (H5Sis_regular_hyperslab(space_id)) {
      H5Sget_regular_hyperslab (space_id, start_out, stride_out, count_out, block_out);
      Rprintf("         start  = [%llu, %llu] \n", (unsigned long long)start_out[0], (unsigned long long)start_out[1]);
      Rprintf("         stride = [%llu, %llu] \n", (unsigned long long)stride_out[0], (unsigned long long)stride_out[1]);
      Rprintf("         count  = [%llu, %llu] \n", (unsigned long long)count_out[0], (unsigned long long)count_out[1]);
      Rprintf("         block  = [%llu, %llu] \n", (unsigned long long)block_out[0], (unsigned long long)block_out[1]);
    } */

    SEXP Rval = ScalarInteger(0);
    return Rval;
    
}

/* hid_t  H5Scombine_hyperslab ( hid_t space_id, H5S_seloper_t op, const hsize_t start[],
 const hsize_t stride[], const hsize_t count[], const hsize_t block[] ) */
SEXP _H5Scombine_hyperslab( SEXP _space_id, SEXP _start, SEXP _stride, SEXP _count, SEXP _block ) {
  hid_t space_id = STRSXP_2_HID( _space_id );
  hsize_t start[LENGTH(_start)];
  hsize_t stride[LENGTH(_stride)];
  hsize_t count[LENGTH(_count)];
  hsize_t block[LENGTH(_block)];
  int i;
  for (i=0; i < LENGTH(_start); i++) {
    start[i] = REAL(_start)[i];
  }
  for (i=0; i < LENGTH(_stride); i++) {
    stride[i] = REAL(_stride)[i];
  }
  for (i=0; i < LENGTH(_count); i++) {
    count[i] = REAL(_count)[i];
  }
  for (i=0; i < LENGTH(_block); i++) {
    block[i] = REAL(_block)[i];
  }
  
  space_id = H5Scombine_hyperslab( space_id, H5S_SELECT_XOR, start, stride, count, block );
   /*
  if(H5Sis_regular_hyperslab(space_id)) {
    Rprintf("Regular\n");
  } else {
    Rprintf("Not regular\n");
  }
  
  int npoints = H5Sget_select_npoints(space_id);
  int nblocks = H5Sget_select_hyper_nblocks(space_id);
  Rprintf("N points: %d - N blocks: %d\n", npoints, nblocks);
  
  int RANK2 = 2;
  int l, k;
  hsize_t *buf = (hsize_t *)malloc(sizeof(hsize_t)*2*RANK2*nblocks);
  
  H5Sget_select_hyper_blocklist (space_id, (hsize_t)0, nblocks, buf);
  
  for (l=0; l<nblocks; l++) {
    Rprintf("(");
    for (k=0; k<RANK2-1; k++) 
      Rprintf("%d,", (int)buf[k]);
    Rprintf("%d ) - (", (int)buf[k]);
    for (k=0; k<RANK2-1; k++) 
      Rprintf("%d,", (int)buf[RANK2+k]);
    Rprintf("%d)\n", (int)buf[RANK2+k]);
  }
  free(buf);
  
  hsize_t start_out[2],stride_out[2],count_out[2],block_out[2];
  if (H5Sis_regular_hyperslab(space_id)) {
    H5Sget_regular_hyperslab (space_id, start_out, stride_out, count_out, block_out);
    Rprintf("         start  = [%llu, %llu] \n", (unsigned long long)start_out[0], (unsigned long long)start_out[1]);
    Rprintf("         stride = [%llu, %llu] \n", (unsigned long long)stride_out[0], (unsigned long long)stride_out[1]);
    Rprintf("         count  = [%llu, %llu] \n", (unsigned long long)count_out[0], (unsigned long long)count_out[1]);
    Rprintf("         block  = [%llu, %llu] \n", (unsigned long long)block_out[0], (unsigned long long)block_out[1]);
  } */

  SEXP Rval;
  PROTECT(Rval = HID_2_STRSXP(space_id));
  UNPROTECT(1);
  return Rval;
  
}

/* H5Sselect_index is not part of the standart H5S interfaces. It is a iteratie call to H5Sselect_point. */
SEXP _H5Sselect_index( SEXP _space_id, SEXP _start, SEXP _count) {
    hid_t space_id = STRSXP_2_HID( _space_id );
  
    int l = LENGTH(_start);
    
    herr_t herr = H5Sselect_none(space_id);
    hsize_t start[l];
    hsize_t stride[l];
    hsize_t count[l];
    hsize_t block[l];
    int index[l];
    for (int i=0; i<l; i++) {
        stride[i] = 1;
        index[i] = 0;
        block[i] = 1;
    }
    int cont = 1;
    if (herr < 0) {
        cont = 0;
    }
    int k = l-1;
    while(cont > 0) {
        for (int i=0; i<l; i++) {
            start[i] = REAL(VECTOR_ELT(_start,i))[index[i]];
            count[i] = REAL(VECTOR_ELT(_count,i))[index[i]];
        }
        herr = H5Sselect_hyperslab(space_id, H5S_SELECT_OR, start, stride, count, block);
        if (herr < 0) {
            cont = 0;
        } else {
            k = l-1;
            index[k]++;
            int carry = 0;
            if (index[k] >= LENGTH(VECTOR_ELT(_count,k))) {
                carry = 1;
            }
            while ((k >= 0) & (carry > 0)) {
                index[k] = 0;
                k--;
                if (k >= 0) {
                    index[k]++;
                    if (index[k] >= LENGTH(VECTOR_ELT(_count,k))) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                }
            }
            if (k < 0) {
                cont = 0;
            }
        }
    }
    
    SEXP Rval = ScalarInteger(herr);
    return Rval;
}

