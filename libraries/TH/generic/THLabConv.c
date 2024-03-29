#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THLabConv.c"
#else

/*
  2D Input, 2D kernel  : convolve given image with the given kernel.
*/
TH_API void THLab_(validXCorr2Dptr)(real *r_,
                                    real alpha,
                                    real *t_, long ir, long ic,
                                    real *k_, long kr, long kc,
                                    long sr, long sc)
{
  long or = (ir - kr) / sr + 1;
  long oc = (ic - kc) / sc + 1;

  long xx, yy, kx, ky;

  if ((sc != 1) || (oc < 4))  {
    // regular convolution
    for(yy = 0; yy < or; yy++) {
      for(xx = 0; xx < oc; xx++) {
        /* Dot product in two dimensions... (between input image and the mask) */
        real *pi_ = t_ + yy*sr*ic + xx*sc;
        real *pw_ = k_;
        real sum = 0;
        for(ky = 0; ky < kr; ky++) {
          for(kx = 0; kx < kc; kx++) {
            sum += pi_[kx]*pw_[kx];
          }
          pi_ += ic; /* next input line */
          pw_ += kc; /* next mask line */
        }
        /* Update output */
        *r_ += alpha*sum;
        *r_++;
      }
    }

  } else {
    // SSE-based convolution
    for(yy = 0; yy < or; yy++) {
      real *pi_ = t_ + yy*sr*ic;
      real *pw_ = k_;
      for (ky = 0; ky < kr; ky++) {
        real *pis_ = pi_;
        for (kx = 0; kx < kc; kx++) {
          THVector_(add)(r_, pis_, alpha*pw_[kx], oc);
          pis_++;
        }
        pi_ += ic; /* next input line */
        pw_ += kc; /* next mask line */
      }
      r_ += oc;
    }
  }
}

/*
  2D Input, 2D kernel  : convolve given image with the given kernel.
*/
TH_API void THLab_(validConv2Dptr)(real *r_,
                                   real alpha,
                                   real *t_, long ir, long ic,
                                   real *k_, long kr, long kc,
                                   long sr, long sc)
{
  long or = (ir - kr) / sr + 1;
  long oc = (ic - kc) / sc + 1;

  long xx, yy, kx, ky;

  if ((sc != 1) || (oc < 4))  {
    // regular convolution
    for(yy = 0; yy < or; yy++) {
      for(xx = 0; xx < oc; xx++) {
        /* Dot product in two dimensions... (between input image and the mask) */
        real *pi_ = t_ + yy*sr*ic + xx*sc;
        real *pw_ = k_ + kr*kc - 1;
        real sum = 0;
        for(ky = 0; ky < kr; ky++) {
          for(kx = 0; kx < kc; kx++) {
            sum += pi_[kx]*pw_[-kx];
          }
          pi_ += ic; /* next input line */
          pw_ -= kc; /* next mask line */
        }
        /* Update output */
        *r_ += alpha*sum;
        *r_++;
      }
    }

  } else {
    // SSE-based convolution
    for(yy = 0; yy < or; yy++) {
      real *pw_ = k_ + kr*kc - 1;
      real *pi_ = t_ + yy*sr*ic;
      for (ky = 0; ky < kr; ky++) {
        real *pis_ = pi_;
        for (kx = 0; kx < kc; kx++) {
          THVector_(add)(r_, pis_, alpha*pw_[-kx], oc);
          pis_++;
        }
        pi_ += ic; /* next input line */
        pw_ -= kc; /* next mask line */
      }
      r_ += oc;
    }
  }
}

/*
  2D Input, 2D kernel  : convolve given image with the given kernel, full convolution.
*/
TH_API void THLab_(fullConv2Dptr)(real *r_,
                                  real alpha,
                                  real *t_, long ir, long ic,
                                  real *k_, long kr, long kc,
                                  long sr, long sc)
{
  long or = (ir - 1) * sr + kr;
  long oc = (ic - 1) * sc + kc;

  long xx, yy, kx, ky;

  if ((sc != 1) || (ic < 4))  {
    // regular convolution
    for(yy = 0; yy < ir; yy++) {
      for(xx = 0; xx < ic; xx++) {
        /* Outer product in two dimensions... (between input image and the mask) */
        real *po_ = r_ + yy*sr*oc + xx*sc;
        real *pw_ = k_;
        for(ky = 0; ky < kr; ky++)
        {
          real z = *t_ * alpha;
          for(kx = 0; kx < kc; kx++) {
            po_[kx] += z * pw_[kx];
          }
          po_ += oc; /* next input line */
          pw_ += kc; /* next mask line */
        }
        t_++;
      }
    }

  } else {
    // SSE-based convolution
    for(yy = 0; yy < ir; yy++) {
      real *po_ = r_ + yy*sr*oc;
      real *pw_ = k_;
      for (ky = 0; ky < kr; ky++) {
        real *pos_ = po_;
        for (kx = 0; kx < kc; kx++) {
          THVector_(add)(pos_, t_, alpha*pw_[kx], ic);
          pos_++;
        }
        po_ += oc; /* next input line */
        pw_ += kc; /* next mask line */
      }
      t_ += ic;
    }
  }
}

/*
  2D Input, 2D kernel  : convolve given image with the given kernel, full convolution.
*/
TH_API void THLab_(fullXCorr2Dptr)(real *r_,
                                   real alpha,
                                   real *t_, long ir, long ic,
                                   real *k_, long kr, long kc,
                                   long sr, long sc)
{
  long or = (ir - 1) * sr + kr;
  long oc = (ic - 1) * sc + kc;

  long xx, yy, kx, ky;

  if ((sc != 1) || (ic < 4))  {
    // regular convolution
    for(yy = 0; yy < ir; yy++) {
      for(xx = 0; xx < ic; xx++) {
        /* Outer product in two dimensions... (between input image and the mask) */
        real *po_ = r_ + yy*sr*oc + xx*sc;
        real *pw_ = k_ + kr*kc -1;
        long kx, ky;
        for(ky = 0; ky < kr; ky++)
        {
          real z = *t_ * alpha;
          for(kx = 0; kx < kc; kx++) {
            po_[kx] += z * pw_[-kx];
          }
          po_ += oc; /* next input line */
          pw_ -= kc; /* next mask line */
        }
        t_++;
      }
    }

  } else {
    // SSE-based convolution
    for(yy = 0; yy < ir; yy++) {
      real *po_ = r_ + yy*sr*oc;
      real *pw_ = k_ + kr*kc -1;
      for (ky = 0; ky < kr; ky++) {
        real *pos_ = po_;
        for (kx = 0; kx < kc; kx++) {
          THVector_(add)(pos_, t_, pw_[-kx]*alpha, ic);
          pos_++;
        }
        po_ += oc; /* next input line */
        pw_ -= kc; /* next mask line */
      }
      t_ += ic;
    }
  }
}

/*
  2D Input, 2D kernel  : convolve given image with the given kernel, valid convolution.
  for sr,sc=1 this is equivalent to validXCorr2Dptr, but otherwise it is useful for
  calculating derivatives wrt a kernel that is applied with stride sr,sc != 1
*/
TH_API void THLab_(validXCorr2DRevptr)(real *r_,
                                       real alpha,
                                       real *t_, long ir, long ic,
                                       real *k_, long kr, long kc,
                                       long sr, long sc)
{
  long or = ir - (kr - 1) * sr;
  long oc = ic - (kc - 1) * sc;

  long xx, yy, kx, ky;

  if ((sc != 1) || (kc < 4))  {
    // regular convolution
    for(yy = 0; yy < kr; yy++) {
      for(xx = 0; xx < kc; xx++) {
        real *po_ = r_;
        real *pi_ = t_ + yy*sr*ic + xx*sc;
        real z = *k_++ * alpha;

        for(ky = 0; ky < or; ky++) {
          for(kx = 0; kx < oc; kx++)
            po_[kx] += z * pi_[kx];
          pi_ += ic;
          po_ += oc;
        }
      }
    }

  } else {
    // SSE-based convolution
    for(yy = 0; yy < kr; yy++) {
      for(xx = 0; xx < kc; xx++) {
        real *po_ = r_;
        real *pi_ = t_ + yy*sr*ic + xx*sc;
        real z = *k_++ * alpha;

        for(ky = 0; ky < or; ky++) {
          THVector_(add)(po_, pi_, z, oc);
          pi_ += ic;
          po_ += oc;
        }
      }
    }
  }
}
/*
  3D Input, 3D kernel  : convolve given volume with the given kernel.
*/
TH_API void THLab_(validXCorr3Dptr)(real *r_,
                                    real alpha,
                                    real *t_, long it, long ir, long ic,
                                    real *k_, long kt, long kr, long kc,
                                    long st, long sr, long sc)
{
  long ot = (it - kt) / st + 1;
  long or = (ir - kr) / sr + 1;
  long oc = (ic - kc) / sc + 1;

  long zz, xx, yy;

  for (zz = 0; zz < ot; zz++)
  {
    for(yy = 0; yy < or; yy++)
    {
      for(xx = 0; xx < oc; xx++)
      {
        /* Dot product in two dimensions... (between input image and the mask) */
        real *pi_ = t_ + zz*st*ir*ic + yy*sr*ic + xx*sc;
        real *pw_ = k_;
        real sum = 0;
        long kz, kx, ky;
        for(kz = 0; kz < kt; kz++)
        {
          for(ky = 0; ky < kr; ky++)
          {
            for(kx = 0; kx < kc; kx++) {
              sum += pi_[kx]*pw_[kx];
            }
            pi_ += ic; /* next input line */
            pw_ += kc; /* next mask line */
          }
        }
        /* Update output */
        *r_ += sum*alpha;
        *r_++;
      }
    }
  }
}

/*
  3D Input, 3D kernel  : convolve given volume with the given kernel.
*/
TH_API void THLab_(validConv3Dptr)(real *r_,
                                   real alpha,
                                   real *t_, long it, long ir, long ic,
                                   real *k_, long kt, long kr, long kc,
                                   long st, long sr, long sc)
{
  long ot = (it - kt) / st + 1;
  long or = (ir - kr) / sr + 1;
  long oc = (ic - kc) / sc + 1;

  long zz, xx, yy;

  for(zz = 0; zz < ot; zz++)
  {
    for(yy = 0; yy < or; yy++)
    {
      for(xx = 0; xx < oc; xx++)
      {
        /* Dot product in two dimensions... (between input image and the mask) */
        real *pi_ = t_ + zz*st*ir*ic + yy*sr*ic + xx*sc;
        real *pw_ = k_ + kt*kr*kc - 1;
        real sum = 0;
        long kz, kx, ky;
        for(kz = 0; kz < kt; kz++)
        {
          for(ky = 0; ky < kr; ky++)
          {
            for(kx = 0; kx < kc; kx++) {
              sum += pi_[kx]*pw_[-kx];
            }
            pi_ += ic; /* next input line */
            pw_ -= kc; /* next mask line */
          }
        }
        /* Update output */
        *r_ += alpha*sum;
        *r_++;
      }
    }
  }
}


/*
  3D Input, 3D kernel  : convolve given volume with the given kernel, full convolution.
*/
TH_API void THLab_(fullConv3Dptr)(real *r_,
                                  real alpha,
                                  real *t_, long it, long ir, long ic,
                                  real *k_, long kt, long kr, long kc,
                                  long st, long sr, long sc)
{
  long ot = (it - 1) * st + kt;
  long or = (ir - 1) * sr + kr;
  long oc = (ic - 1) * sc + kc;

  long zz, xx, yy;

  for(zz = 0; zz < it; zz++)
  {
    for(yy = 0; yy < ir; yy++)
    {
      for(xx = 0; xx < ic; xx++)
      {
        /* Outer product in two dimensions... (between input image and the mask) */
        real *po_ = r_ + zz*st*or*oc + yy*sr*oc + xx*sc;
        real *pw_ = k_;
        long kz, kx, ky;
        //printf("Output Plane : %ld,%ld,%ld, input val=%g\n",zz,yy,xx,*t_);
        for(kz = 0; kz < kt; kz++)
        {
          for(ky = 0; ky < kr; ky++)
          {
            real z = *t_ * alpha;
            for(kx = 0; kx < kc; kx++) {
              //printf("o=%g,k=%g," , po_[kx],pw_[kx]);
              po_[kx] += z * pw_[kx];
              //printf("o=%g " , po_[kx]);
            }
            //printf("\n");
            po_ += oc; /* next input line */
            pw_ += kc; /* next mask line */
          }
          //printf("\n");
        }
        t_++;
      }
    }
  }
}

/*
  3D Input, 3D kernel  : convolve given volume with the given kernel, full convolution.
*/
TH_API void THLab_(fullXCorr3Dptr)(real *r_,
                                   real alpha,
                                   real *t_, long it, long ir, long ic,
                                   real *k_, long kt, long kr, long kc,
                                   long st, long sr, long sc)
{
  long ot = (it - 1) * st + kt;
  long or = (ir - 1) * sr + kr;
  long oc = (ic - 1) * sc + kc;

  long zz, xx, yy;

  for(zz = 0; zz < it; zz++)
  {
    for(yy = 0; yy < ir; yy++)
    {
      for(xx = 0; xx < ic; xx++)
      {
        /* Outer product in two dimensions... (between input image and the mask) */
        real *po_ = r_ + zz*st*or*oc + yy*sr*oc + xx*sc;
        real *pw_ = k_ + kt*kr*kc -1;
        long kz, kx, ky;
        for(kz = 0; kz < kt; kz++)
        {
          for(ky = 0; ky < kr; ky++)
          {
            real z = *t_ * alpha;
            for(kx = 0; kx < kc; kx++) {
              po_[kx] += z * pw_[-kx];
            }
            po_ += oc; /* next input line */
            pw_ -= kc; /* next mask line */
          }
        }
        t_++;
      }
    }
  }
}

/*
  3D Input, 3D kernel  : convolve given image with the given kernel, valid convolution.
  for sr,sc=1 this is equivalent to validXCorr3Dptr, but otherwise it is useful for
  calculating derivatives wrt a kernel that is applied with stride sr,sc != 1
*/
TH_API void THLab_(validXCorr3DRevptr)(real *r_,
                                       real alpha,
                                       real *t_, long it, long ir, long ic,
                                       real *k_, long kt, long kr, long kc,
                                       long st, long sr, long sc)
{
  long ot = it - (kt - 1) * st;
  long or = ir - (kr - 1) * sr;
  long oc = ic - (kc - 1) * sc;

  long zz, xx, yy;
  for(zz = 0; zz < kt; zz++)
  {
    for(yy = 0; yy < kr; yy++)
    {
      for(xx = 0; xx < kc; xx++)
      {
        real *po_ = r_;
        real *pi_ = t_ + zz*st*ir*ic + yy*sr*ic + xx*sc;
        real z = *k_++ * alpha;
        long kz, kx, ky;
        for(kz = 0; kz < ot; kz++)
        {
          for(ky = 0; ky < or; ky++)
          {
            for(kx = 0; kx < oc; kx++)
              po_[kx] += z * pi_[kx];
            pi_ += ic;
            po_ += oc;
          }
        }
      }
    }
  }
}

void THLab_(conv2d)(real* output_data,
                    real alpha,
                    real* ptr_input, long nInputRows, long nInputCols,
                    real* ptr_weight, long nKernelRows, long nKernelCols,
                    long srow, long scol,
                    const char* type)
{
  THArgCheck(type[0] == 'v' || type[0] == 'f', 7, "type of convolution can be 'v' or 'f'");
  THArgCheck(type[1] == 'c' || type[1] == 'x', 7, "type of convolution can be 'x' or 'c'");
  if (type[0] == 'f')
    if (type[1] == 'x')
      THLab_(fullXCorr2Dptr)(output_data,
                             alpha,
                             ptr_input,  nInputRows,  nInputCols,
                             ptr_weight, nKernelRows, nKernelCols,
                             srow, scol);
    else
      THLab_(fullConv2Dptr)(output_data,
                            alpha,
                            ptr_input,  nInputRows,  nInputCols,
                            ptr_weight, nKernelRows, nKernelCols,
                            srow, scol);
  else
    if (type[1] == 'x')
      THLab_(validXCorr2Dptr)(output_data,
                              alpha,
                              ptr_input,  nInputRows,  nInputCols,
                              ptr_weight, nKernelRows, nKernelCols,
                              srow, scol);
    else
      THLab_(validConv2Dptr)(output_data,
                             alpha,
                             ptr_input,  nInputRows,  nInputCols,
                             ptr_weight, nKernelRows, nKernelCols,
                             srow, scol);
}

void THLab_(conv3d)(real* output_data,
                    real alpha,
                    real* ptr_input, long nInputDepth, long nInputRows, long nInputCols,
                    real* ptr_weight, long nKernelDepth, long nKernelRows, long nKernelCols,
                    long sdepth, long srow, long scol,
                    const char* type)
{
  THArgCheck(type[0] == 'v' || type[0] == 'f', 7, "type of convolution can be 'v' or 'f'");
  THArgCheck(type[1] == 'c' || type[1] == 'x', 7, "type of convolution can be 'x' or 'c'");
  if (type[0] == 'f')
    if (type[1] == 'x')
      THLab_(fullXCorr3Dptr)(output_data,
                             alpha,
                             ptr_input, nInputDepth, nInputRows,  nInputCols,
                             ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                             sdepth, srow, scol);
    else
      THLab_(fullConv3Dptr)(output_data,
                            alpha,
                            ptr_input, nInputDepth, nInputRows,  nInputCols,
                            ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                            sdepth, srow, scol);
  else
    if (type[1] == 'x')
      THLab_(validXCorr3Dptr)(output_data,
                              alpha,
                              ptr_input, nInputDepth, nInputRows,  nInputCols,
                              ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                              sdepth, srow, scol);
    else
      THLab_(validConv3Dptr)(output_data,
                             alpha,
                             ptr_input, nInputDepth, nInputRows,  nInputCols,
                             ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                             sdepth, srow, scol);
}

long THLab_(convsize)(long x, long k, long s, const char* type)
{
  THArgCheck(type[0] == 'v' || type[0] == 'f', 1, "type of convolution can be 'v' or 'f'");
  if (*type == 'v')
    return (x-k)/s + 1;
  else
    return (x-1)*s + k;
}


/*
  3D input, 3D kernel, 4D output
  like rank1 update
  A <- xx' + beta*A
  for sr,sc=1 this is equivalent to xcorr2Dger, but otherwise it is useful for
  calculating derivatives wrt a kernel that is applied with stride sr,sc != 1
*/
void THLab_(conv2DRevger)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_, long srow, long scol)
{
  long nInputPlane, nInputRows, nInputCols;
  long nKernelPlane, nKernelRows, nKernelCols;
  long nOutputPlane, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 3 , 3, "input: 3D Tensor expected");
  THArgCheck(k_->nDimension == 3 , 4, "kernel: 3D Tensor expected");
  THArgCheck(srow >= 1, 5, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 6, "Stride should be a positive integer");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor *kernel = THTensor_(newContiguous)(k_);

  nInputPlane = input->size[0];
  istride0    = input->stride[0];
  nInputRows  = input->size[1];
  nInputCols  = input->size[2];

  kstride0 = kernel->stride[0];
  nKernelPlane = kernel->size[0];
  nKernelRows = kernel->size[1];
  nKernelCols = kernel->size[2];
  nOutputPlane = nInputPlane * kernel->size[0];

  THArgCheck(nInputRows >= nKernelRows && nInputCols >= nKernelCols , 2, "conv2DRevger : Input image is smaller than kernel");

  nOutputRows = nInputRows - (nKernelRows - 1) * srow;
  nOutputCols = nInputCols - (nKernelCols - 1) * scol;

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize4d)(r_,nKernelPlane, nInputPlane, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k,i;
  for(k = 0; k < nKernelPlane; k++)
  {
    /* get kernel */
    real *ptr_weight = weight_data+k*kstride0;

    for(i = 0; i < nInputPlane; i++)
    {
      /* get input */
      real *ptr_input = input_data+i*istride0;

      /* do image, kernel convolution */
      THLab_(validXCorr2DRevptr)(output_data,
                                 alpha,
                                 ptr_input,  nInputRows,  nInputCols,
                                 ptr_weight, nKernelRows, nKernelCols,
                                 srow, scol);
      /* Next output plane */
      output_data += nOutputCols*nOutputRows;
    }
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}


/*
  3D input, 3D kernel, 4D output
  like rank1 update
  A <- xx' + beta*A
*/
void THLab_(conv2Dger)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_, long srow, long scol, const char *type)
{
  long nInputPlane, nInputRows, nInputCols;
  long nKernelPlane, nKernelRows, nKernelCols;
  long nOutputPlane, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 3 , 3, "input: 3D Tensor expected");
  THArgCheck(k_->nDimension == 3 , 4, "kernel: 3D Tensor expected");
  THArgCheck(srow >= 1, 5, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 6, "Stride should be a positive integer");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor *kernel = THTensor_(newContiguous)(k_);

  nInputPlane = input->size[0];
  istride0    = input->stride[0];
  nInputRows  = input->size[1];
  nInputCols  = input->size[2];

  kstride0     = kernel->stride[0];
  nKernelPlane = kernel->size[0];
  nKernelRows  = kernel->size[1];
  nKernelCols  = kernel->size[2];
  nOutputPlane = nInputPlane * kernel->size[0];

  THArgCheck((nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *type == 'f', 2, "conv2Dger : Input image is smaller than kernel");

  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize4d)(r_,nKernelPlane, nInputPlane, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k,i;
  for(k = 0; k < nKernelPlane; k++)
  {
    /* get kernel */
    real *ptr_weight = weight_data+k*kstride0;

    for(i = 0; i < nInputPlane; i++)
    {
      /* get input */
      real *ptr_input = input_data+i*istride0;

      /* do image, kernel convolution */
      THLab_(conv2d)(output_data,
                     alpha,
                     ptr_input, nInputRows, nInputCols,
                     ptr_weight, nKernelRows, nKernelCols,
                     srow, scol, type);

      /* Next output plane */
      output_data += nOutputCols*nOutputRows;
    }
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  3D input, 4D kernel, 3D output
  matrix vector product like
  y <- Ax + beta*y
*/
void THLab_(conv2Dmv)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_, long srow, long scol, const char *type)
{
  long nInputPlane, nInputRows, nInputCols;
  long nKernelRows, nKernelCols;
  long nOutputPlane, nOutputRows, nOutputCols;
  long istride0, kstride0, kstride1;

  THArgCheck(t_->nDimension == 3 , 3, "input: 3D Tensor expected");
  THArgCheck(k_->nDimension == 4 , 4, "kernel: 4D Tensor expected");
  THArgCheck(srow >= 1, 5, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 6, "Stride should be a positive integer");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel;
  if (!(k_->stride[3] == 1) || !(k_->stride[2] == k_->size[3])) {
    kernel = THTensor_(newContiguous)(k_);
  } else {
    THTensor_(retain)(k_);
    kernel = k_;
  }

  nInputPlane = input->size[0];
  istride0    = input->stride[0];
  nInputRows  = input->size[1];
  nInputCols  = input->size[2];

  kstride0    = kernel->stride[0];
  kstride1    = kernel->stride[1];
  nKernelRows = kernel->size[2];
  nKernelCols = kernel->size[3];
  nOutputPlane = kernel->size[0];
  THArgCheck(kernel->size[1] == nInputPlane, 2, "invalid number of input planes");

  THArgCheck( (nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *type == 'f', 2, "conv2Dmv : Input image is smaller than kernel");

  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize3d)(r_, nOutputPlane, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k,i;
  for(k = 0; k < nOutputPlane; k++)
  {
    for(i = 0; i < nInputPlane; i++)
    {
      /* get kernel */
      real *ptr_weight = weight_data + k*kstride0 + i*kstride1;
      /* get input */
      real *ptr_input = input_data + i*istride0;

      /* do image, kernel convolution */
      THLab_(conv2d)(output_data,
                     alpha,
                     ptr_input, nInputRows, nInputCols,
                     ptr_weight, nKernelRows, nKernelCols,
                     srow, scol, type);
    }
    /* Next output plane */
    output_data += nOutputCols*nOutputRows;
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  2D input, 2D kernel, 2D output
  scalar multiplication like
  y <- x*y + beta*y
*/
void THLab_(conv2Dmul)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_, long srow, long scol, const char *type)
{

  THArgCheck(t_->nDimension == 2 , 3, "input: 2D Tensor expected");
  THArgCheck(k_->nDimension == 2 , 4, "kernel: 2D Tensor expected");
  THArgCheck(srow >= 1, 5, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 6, "Stride should be a positive integer");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel = THTensor_(newContiguous)(k_);

  long nInputRows  = input->size[0];
  long nInputCols  = input->size[1];
  long nKernelRows = kernel->size[0];
  long nKernelCols = kernel->size[1];
  long nOutputRows, nOutputCols;

  THArgCheck((nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *type == 'f', 2, "conv2Dmul : Input image is smaller than kernel");

  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize2d)(r_, nOutputRows, nOutputCols);
  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
    THTensor_(zero)(r_);
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *ptr_input = THTensor_(data)(input);
  real *ptr_weight = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);


  /* do image, kernel convolution */
  THLab_(conv2d)(output_data,
                 alpha,
                 ptr_input, nInputRows, nInputCols,
                 ptr_weight, nKernelRows, nKernelCols,
                 srow, scol, type);
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  3D input, 3D kernel, 3D output
  component wise multiplication like
  y <- y.*x + beta*y
*/
void THLab_(conv2Dcmul)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_, long srow, long scol, const char *type)
{
  long nInputPlane, nInputRows, nInputCols;
  long nKernelRows, nKernelCols;
  long nOutputPlane, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 3 , 3, "input: 3D Tensor expected");
  THArgCheck(k_->nDimension == 3 , 4, "kernel: 3D Tensor expected");
  THArgCheck(srow >= 1, 5, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 6, "Stride should be a positive integer");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel = THTensor_(newContiguous)(k_);

  istride0    = input->stride[0];
  nInputPlane = input->size[0];
  nInputRows  = input->size[1];
  nInputCols  = input->size[2];

  kstride0    = kernel->stride[0];
  nOutputPlane = kernel->size[0];
  nKernelRows = kernel->size[1];
  nKernelCols = kernel->size[2];

  THArgCheck(nOutputPlane == nInputPlane, 2, "invalid number of input/kernel planes");
  THArgCheck( (nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *type == 'f', 2, "conv2Dcmul : Input image is smaller than kernel");

  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize3d)(r_, nOutputPlane, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k;
  for(k = 0; k < nOutputPlane; k++)
  {
    /* get kernel */
    real *ptr_weight = weight_data + k*kstride0;
    /* get input */
    real *ptr_input = input_data + k*istride0;

    /* do image, kernel convolution */
    THLab_(conv2d)(output_data,
                   alpha,
                   ptr_input, nInputRows, nInputCols,
                   ptr_weight, nKernelRows, nKernelCols,
                   srow, scol, type);
    /* Next output plane */
    output_data += nOutputCols*nOutputRows;
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  3D input, 3D kernel, 3D output
  component wise multiplication like with a permutation map
  y <- y.*x + beta*y
*/
void THLab_(conv2Dmap)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_, THTensor *map, long srow, long scol, const char *type)
{
  long nInputPlane, nInputRows, nInputCols;
  long nKernelRows, nKernelCols;
  long nOutputPlane, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 3 , 3, "input: 3D Tensor expected");
  THArgCheck(k_->nDimension == 3 , 4, "kernel: 3D Tensor expected");
  THArgCheck(map->nDimension == 2 , 4, "map: 2D Tensor expected");
  THArgCheck(srow >= 1, 6, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 7, "Stride should be a positive integer");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel = THTensor_(newContiguous)(k_);

  istride0    = input->stride[0];
  nInputPlane = input->size[0];
  nInputRows  = input->size[1];
  nInputCols  = input->size[2];

  kstride0    = kernel->stride[0];
  nOutputPlane = kernel->size[0];
  nKernelRows = kernel->size[1];
  nKernelCols = kernel->size[2];

  THArgCheck(nOutputPlane == nInputPlane, 2, "invalid number of input/kernel planes");
  THArgCheck( (nInputRows >= nKernelRows && nInputCols >= nKernelCols)
              || *type == 'f', 2, "conv2Dmap : Input image is smaller than kernel");

  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize3d)(r_, nOutputPlane, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long nmaps = map->size[0];

  long k;
  for(k = 0; k < nmaps; k++)
  {
    /* get indices */
    long from = (long)THTensor_(get2d)(map,k,0)-1;
    long to   = (long)THTensor_(get2d)(map,k,1)-1;

    /* get kernel */
    real *ptr_weight = weight_data + k*kstride0;
    /* get input */
    real *ptr_input = input_data + from*istride0;
    /* get output */
    real *ptr_output = output_data + to*nOutputRows*nOutputCols;

    /* do image, kernel convolution */
    THLab_(conv2d)(ptr_output,
                   alpha,
                   ptr_input, nInputRows, nInputCols,
                   ptr_weight, nKernelRows, nKernelCols,
                   srow, scol, type);
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  4D input, 4D kernel, 5D output
  like rank1 update
  A <- xx' + beta*A
  for sr,sc=1 this is equivalent to xcorr2Dger, but otherwise it is useful for
  calculating derivatives wrt a kernel that is applied with stride sr,sc != 1
*/
void THLab_(conv3DRevger)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_,
                          long sdepth, long srow, long scol)
{
  long nInputPlane, nInputDepth, nInputRows, nInputCols;
  long nKernelPlane, nKernelDepth, nKernelRows, nKernelCols;
  long nOutputPlane, nOutputDepth, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 4 , 3, "input: 4D Tensor expected");
  THArgCheck(k_->nDimension == 4 , 4, "kernel: 4D Tensor expected");
  THArgCheck(sdepth >= 1, 5, "Stride should be a positive integer");
  THArgCheck(srow >= 1, 6, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 7, "Stride should be a positive integer");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor *kernel = THTensor_(newContiguous)(k_);

  nInputPlane = input->size[0];
  istride0    = input->stride[0];
  nInputDepth = input->size[1];
  nInputRows  = input->size[2];
  nInputCols  = input->size[3];

  kstride0 = kernel->stride[0];
  nKernelPlane = kernel->size[0];
  nKernelDepth= kernel->size[1];
  nKernelRows = kernel->size[2];
  nKernelCols = kernel->size[3];
  nOutputPlane = nInputPlane * kernel->size[0];

  THArgCheck(nInputDepth >= nKernelDepth && nInputRows >= nKernelRows && nInputCols >= nKernelCols , 2, "conv3DRevger : Input image is smaller than kernel");

  nOutputDepth = nInputDepth - (nKernelDepth - 1) * sdepth;
  nOutputRows = nInputRows - (nKernelRows - 1) * srow;
  nOutputCols = nInputCols - (nKernelCols - 1) * scol;

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize5d)(r_,nKernelPlane, nInputPlane, nOutputDepth, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k,i;
  for(k = 0; k < nKernelPlane; k++)
  {
    /* get kernel */
    real *ptr_weight = weight_data+k*kstride0;

    for(i = 0; i < nInputPlane; i++)
    {
      /* get input */
      real *ptr_input = input_data+i*istride0;

      /* do image, kernel convolution */
      THLab_(validXCorr3DRevptr)(output_data,
                                 alpha,
                                 ptr_input,  nInputDepth, nInputRows,  nInputCols,
                                 ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                                 sdepth, srow, scol);
      /* Next output plane */
      output_data += nOutputDepth*nOutputCols*nOutputRows;
    }
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}


/*
  4D input, 4D kernel, 5D output
  like rank1 update
  A <- xx' + beta*A
*/
void THLab_(conv3Dger)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_,
                       long sdepth, long srow, long scol, const char *type)
{
  long nInputPlane, nInputDepth, nInputRows, nInputCols;
  long nKernelPlane, nKernelDepth, nKernelRows, nKernelCols;
  long nOutputPlane, nOutputDepth, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 4 , 3, "input: 4D Tensor expected");
  THArgCheck(k_->nDimension == 4 , 4, "kernel: 4D Tensor expected");
  THArgCheck(sdepth >= 1, 5, "Stride should be a positive integer");
  THArgCheck(srow >= 1, 6, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 7, "Stride should be a positive integer");
  THArgCheck(type[0] == 'v' || type[0] == 'f', 8, "type of convolution can 'v' or 'f'");
  THArgCheck(type[1] == 'c' || type[1] == 'x', 8, "type of convolution can 'x' or 'c'");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor *kernel = THTensor_(newContiguous)(k_);

  nInputPlane = input->size[0];
  istride0    = input->stride[0];
  nInputDepth = input->size[1];
  nInputRows  = input->size[2];
  nInputCols  = input->size[3];

  kstride0     = kernel->stride[0];
  nKernelPlane = kernel->size[0];
  nKernelDepth = kernel->size[1];
  nKernelRows  = kernel->size[2];
  nKernelCols  = kernel->size[3];
  nOutputPlane = nInputPlane * kernel->size[0];

  THArgCheck((nInputDepth >= nKernelDepth
              && nInputRows >= nKernelRows
              && nInputCols >= nKernelCols)
             || *type == 'f', 2, "conv3Dger : Input image is smaller than kernel");

  nOutputDepth = THLab_(convsize)(nInputDepth, nKernelDepth, sdepth, type);
  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize5d)(r_,nKernelPlane, nInputPlane, nOutputDepth, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k,i;
  for(k = 0; k < nKernelPlane; k++)
  {
    /* get kernel */
    real *ptr_weight = weight_data+k*kstride0;

    for(i = 0; i < nInputPlane; i++)
    {
      /* get input */
      real *ptr_input = input_data+i*istride0;

      /* do image, kernel convolution */
      THLab_(conv3d)(output_data,
                     alpha,
                     ptr_input,  nInputDepth, nInputRows,  nInputCols,
                     ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                     sdepth, srow, scol, type);

      /* Next output plane */
      output_data += nOutputDepth*nOutputCols*nOutputRows;
    }
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  4D input, 5D kernel, 4D output
  matrix vector product like
  y <- Ax + beta*y
*/
void THLab_(conv3Dmv)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_,
                      long sdepth, long srow, long scol, const char *type)
{
  long nInputPlane, nInputDepth, nInputRows, nInputCols;
  long nKernelDepth, nKernelRows, nKernelCols;
  long nOutputPlane, nOutputDepth, nOutputRows, nOutputCols;
  long istride0, kstride0, kstride1;

  THArgCheck(t_->nDimension == 4 , 3, "input: 4D Tensor expected");
  THArgCheck(k_->nDimension == 5 , 4, "kernel: 5D Tensor expected");
  THArgCheck(sdepth >= 1, 5, "Stride should be a positive integer");
  THArgCheck(srow >= 1, 6, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 7, "Stride should be a positive integer");
  THArgCheck(type[0] == 'v' || type[0] == 'f', 8, "type of convolution can 'v' or 'f'");
  THArgCheck(type[1] == 'c' || type[1] == 'x', 8, "type of convolution can 'x' or 'c'");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel;
  if (!(k_->stride[4] == 1) || !(k_->stride[3] == k_->size[4])) {
    kernel = THTensor_(newContiguous)(k_);
  } else {
    THTensor_(retain)(k_);
    kernel = k_;
  }

  nInputPlane = input->size[0];
  istride0    = input->stride[0];
  nInputDepth = input->size[1];
  nInputRows  = input->size[2];
  nInputCols  = input->size[3];

  kstride0    = kernel->stride[0];
  kstride1    = kernel->stride[1];
  nKernelDepth = kernel->size[2];
  nKernelRows = kernel->size[3];
  nKernelCols = kernel->size[4];
  nOutputPlane = kernel->size[0];
  THArgCheck(kernel->size[1] == nInputPlane, 2, "invalid number of input planes");

  THArgCheck( (nInputDepth >= nKernelDepth && nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *type == 'f', 2, "conv3Dmv : Input image is smaller than kernel");

  nOutputDepth = THLab_(convsize)(nInputDepth, nKernelDepth, sdepth, type);
  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize4d)(r_, nOutputPlane, nOutputDepth, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k,i;
  for(k = 0; k < nOutputPlane; k++)
  {
    for(i = 0; i < nInputPlane; i++)
    {
      /* get kernel */
      real *ptr_weight = weight_data + k*kstride0 + i*kstride1;
      /* get input */
      real *ptr_input = input_data + i*istride0;

      /* do image, kernel convolution */
      THLab_(conv3d)(output_data,
                     alpha,
                     ptr_input,  nInputDepth, nInputRows,  nInputCols,
                     ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                     sdepth, srow, scol, type);
    }
    /* Next output plane */
    output_data += nOutputDepth*nOutputCols*nOutputRows;
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  3D input, 3D kernel, 3D output
  scalar multiplication like
  y <- x*y + beta*y
*/
void THLab_(conv3Dmul)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_,
                       long sdepth, long srow, long scol, const char *type)
{

  THArgCheck(t_->nDimension == 2 , 3, "input: 2D Tensor expected");
  THArgCheck(k_->nDimension == 2 , 4, "kernel: 2D Tensor expected");
  THArgCheck(sdepth >= 1, 5, "Stride should be a positive integer");
  THArgCheck(srow >= 1, 6, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 7, "Stride should be a positive integer");
  THArgCheck(type[0] == 'v' || type[0] == 'f', 8, "type of convolution can 'v' or 'f'");
  THArgCheck(type[1] == 'c' || type[1] == 'x', 8, "type of convolution can 'x' or 'c'");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel = THTensor_(newContiguous)(k_);

  long nInputDepth = input->size[0];
  long nInputRows  = input->size[1];
  long nInputCols  = input->size[2];
  long nKernelDepth = kernel->size[0];
  long nKernelRows = kernel->size[1];
  long nKernelCols = kernel->size[2];
  long nOutputDepth, nOutputRows, nOutputCols;

  THArgCheck((nInputDepth >= nKernelDepth && nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *type == 'f', 2, "conv3Dmul : Input image is smaller than kernel");

  nOutputDepth = THLab_(convsize)(nInputDepth, nKernelDepth, sdepth, type);
  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize3d)(r_, nOutputDepth, nOutputRows, nOutputCols);
  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
    THTensor_(zero)(r_);
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *ptr_input = THTensor_(data)(input);
  real *ptr_weight = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);


  /* do image, kernel convolution */
  THLab_(conv3d)(output_data,
                 alpha,
                 ptr_input,  nInputDepth, nInputRows,  nInputCols,
                 ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                 sdepth, srow, scol, type);
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  4D input, 4D kernel, 4D output
  component wise multiplication like
  y <- y.*x + beta*y
*/
void THLab_(conv3Dcmul)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_,
                        long sdepth, long srow, long scol, const char *type)
{
  long nInputPlane, nInputDepth, nInputRows, nInputCols;
  long nKernelDepth, nKernelRows, nKernelCols;
  long nOutputPlane, nOutputDepth, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 4 , 3, "input: 3D Tensor expected");
  THArgCheck(k_->nDimension == 4 , 4, "kernel: 3D Tensor expected");
  THArgCheck(srow >= 1, 5, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 6, "Stride should be a positive integer");
  THArgCheck(type[0] == 'v' || type[0] == 'f', 7, "type of convolution can 'v' or 'f'");
  THArgCheck(type[1] == 'c' || type[1] == 'x', 7, "type of convolution can 'x' or 'c'");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel = THTensor_(newContiguous)(k_);

  istride0    = input->stride[0];
  nInputPlane = input->size[0];
  nInputDepth = input->size[1];
  nInputRows  = input->size[2];
  nInputCols  = input->size[3];

  kstride0    = kernel->stride[0];
  nOutputPlane = kernel->size[0];
  nKernelDepth = kernel->size[1];
  nKernelRows = kernel->size[2];
  nKernelCols = kernel->size[3];

  THArgCheck(nOutputPlane == nInputPlane, 2, "invalid number of input/kernel planes");
  THArgCheck( (nInputDepth >= nKernelDepth && nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *type == 'f', 2, "conv3Dcmul : Input image is smaller than kernel");

  nOutputDepth = THLab_(convsize)(nInputDepth, nKernelDepth, sdepth, type);
  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize4d)(r_, nOutputPlane, nOutputDepth, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long k;
  for(k = 0; k < nOutputPlane; k++)
  {
    /* get kernel */
    real *ptr_weight = weight_data + k*kstride0;
    /* get input */
    real *ptr_input = input_data + k*istride0;

    /* do image, kernel convolution */
    THLab_(conv3d)(output_data,
                   alpha,
                   ptr_input,  nInputDepth, nInputRows,  nInputCols,
                   ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                   sdepth, srow, scol, type);

    /* Next output plane */
    output_data += nOutputDepth*nOutputCols*nOutputRows;
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

/*
  4D input, 4D kernel, 4D output
  component wise multiplication like with a permutation map
  y <- y.*x + beta*y
*/
void THLab_(conv3Dmap)(THTensor *r_, real beta, real alpha, THTensor *t_, THTensor *k_, THTensor *map,
                       long sdepth, long srow, long scol, const char *type)
{
  long nInputPlane, nInputDepth, nInputRows, nInputCols;
  long nKernelDepth, nKernelRows, nKernelCols;
  long nOutputPlane, nOutputDepth, nOutputRows, nOutputCols;
  long istride0, kstride0;

  THArgCheck(t_->nDimension == 4 , 3, "input: 4D Tensor expected");
  THArgCheck(k_->nDimension == 4 , 4, "kernel: 4D Tensor expected");
  THArgCheck(map->nDimension == 2 , 4, "map: 2D Tensor expected");
  THArgCheck(srow >= 1, 6, "Stride should be a positive integer");
  THArgCheck(scol >= 1, 7, "Stride should be a positive integer");
  THArgCheck(type[0] == 'v' || type[0] == 'f', 8, "type of convolution can 'v' or 'f'");
  THArgCheck(type[1] == 'c' || type[1] == 'x', 8, "type of convolution can 'x' or 'c'");

  THTensor *input = THTensor_(newContiguous)(t_);
  THTensor* kernel = THTensor_(newContiguous)(k_);

  istride0    = input->stride[0];
  nInputPlane = input->size[0];
  nInputDepth = input->size[1];
  nInputRows  = input->size[2];
  nInputCols  = input->size[3];

  kstride0    = kernel->stride[0];
  nOutputPlane = kernel->size[0];
  nKernelDepth = kernel->size[1];
  nKernelRows = kernel->size[2];
  nKernelCols = kernel->size[3];

  THArgCheck(nOutputPlane == nInputPlane, 2, "invalid number of input/kernel planes");
  THArgCheck((nInputDepth >= nKernelDepth
              && nInputRows >= nKernelRows
              && nInputCols >= nKernelCols) || *type == 'f',
             2, "conv3Dmap : Input image is smaller than kernel");

  nOutputDepth = THLab_(convsize)(nInputDepth, nKernelDepth, sdepth, type);
  nOutputRows = THLab_(convsize)(nInputRows, nKernelRows, srow, type);
  nOutputCols = THLab_(convsize)(nInputCols, nKernelCols, scol, type);

  long nelem = THTensor_(nElement)(r_);
  THTensor_(resize4d)(r_, nOutputPlane, nOutputDepth, nOutputRows, nOutputCols);

  if (nelem == 0 || beta == 0 || nelem != THTensor_(nElement)(r_))
  {
    THTensor_(zero)(r_);
  }
  else if (beta != 1)
    THTensor_(mul)(r_, beta);

  real *input_data = THTensor_(data)(input);
  real *weight_data = THTensor_(data)(kernel);
  real *output_data = THTensor_(data)(r_);

  long nmaps = map->size[0];

  long k;
  for(k = 0; k < nmaps; k++)
  {
    /* get indices */
    long from = (long)THTensor_(get2d)(map,k,0)-1;
    long to   = (long)THTensor_(get2d)(map,k,1)-1;

    /* get kernel */
    real *ptr_weight = weight_data + k*kstride0;
    /* get input */
    real *ptr_input = input_data + from*istride0;
    /* get output */
    real *ptr_output = output_data + to*nOutputDepth*nOutputRows*nOutputCols;

    /* do image, kernel convolution */
    THLab_(conv3d)(ptr_output,
                   alpha,
                   ptr_input,  nInputDepth, nInputRows,  nInputCols,
                   ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                   sdepth, srow, scol, type);
  }
  THTensor_(free)(input);
  THTensor_(free)(kernel);
}

#endif
