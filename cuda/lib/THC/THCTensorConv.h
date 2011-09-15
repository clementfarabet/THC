#ifndef TH_CUDA_TENSOR_CONV_INC
#define TH_CUDA_TENSOR_CONV_INC

#include "THCTensor.h"

TH_API void THCudaTensor_conv2Dmv(THCudaTensor *output, float beta, THCudaTensor *input,
                                  THCudaTensor *kernel, long srow, long scol, const char *type);

TH_API void THCudaTensor_conv2Dger(THCudaTensor *output, float beta, THCudaTensor *input, 
                                   THCudaTensor *kernel, long srow, long scol, const char *type);

TH_API void THCudaTensor_conv2DRevger(THCudaTensor *output, float beta, float alpha, 
                                      THCudaTensor *input, THCudaTensor *kernel, 
                                      long srow, long scol);

#endif
