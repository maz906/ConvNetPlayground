/** @file fisher.c
 ** @brief Fisher - Declaration
 ** @author David Novotny
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

/**
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page fisher Fisher Vector encoding (FV)
@author David Novotny
@author Andrea Vedaldi
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

@ref fisher.h implements the Fisher Vectors (FV) image representation
@cite{perronnin06fisher} @cite{perronnin10improving}. A FV is a
statistics capturing the distribution of a set of vectors, usually a
set of local image descriptors.

@ref fisher-starting demonstrates how to use VLFeat to compute the FV
representation of an image. For further details refer to:

- @subpage fisher-fundamentals - FV definition.
- @subpage fisher-kernel - Deriving the FV as a the Fisher Kernel.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section fisher-starting Getting started
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The Fisher Vector encoding of a set of features is obtained by using
the function ::vl_fisher_encode. Note that the function requires a
@ref gmm "Gaussian Mixture Model" (GMM) of the encoded feature
distribution.

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page fisher-fundamentals Fisher vector fundamentals
@author David Novotny
@author Andrea Vedaldi
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

This page describes the *Fisher Vector* (FV) of
@cite{perronnin06fisher} @cite{perronnin10improving}. See @ref fisher
for an overview of the C API and @ref fisher-kernel for its relation
to the more general notion of Fisher kernel.

The FV is an image representation obtained by pooling local image
features. It is frequently used as a global image descriptor in visual
classification.

While the FV can be @ref fisher-kernel "derived" as a special,
approximate, and improved case of the general Fisher Kernel framework,
it is easy to describe directly. Let $I = (\bx_1,\dots,\bx_N)$ be a
set of $D$ dimensional feature vectors (e.g. SIFT descriptors)
extracted from an image. Let
$\Theta=(\mu_k,\Sigma_k,\pi_k:k=1,\dots,K)$ be the parameters of a
@ref gmm "Gaussian Mixture Model" fitting the distribution of
descriptors. The GMM associates each vector $\bx_i$ to a mode $k$ in
the mixture with a strength given by the posterior probability:

\[
  q_{ik} =
  \frac
  {(\bx_i - \mu_k)^T \Sigma_k^{-1} (\bx_i - \mu_k)}
  {\sum_{t=1}^K (\bx_i - \mu_t)^T \Sigma_t^{-1} (\bx_i - \mu_t)}.
\]

For each mode $k$, consider the mean and covariance deviation vectors

@f{align*}
u_{jk} &=
{1 \over {N \sqrt{\pi_k}}}
\sum_{i=1}^{N}
q_{ik} \frac{x_{ji} - \mu_{ik}}{\sigma_i},
\\
v_{jk} &=
{1 \over {N \sqrt{2 \pi_k}}}
\sum_{i=1}^{N}
q_{ik} \left[ \left(\frac{x_{ji} - \mu_{ik}}{\sigma_j}\right)^2 - 1 \right].
@f}

where $j=1,2,\dots,D$ spans the vector dimensions. The FV of image $I$
is the stacking of the vectors $\bu_k$ and then of the vectors
$\bv_k$ for each of the $K$ modes in the Gaussian mixtures:

\[
 \Phi(I) = \begin{bmatrix} \vdots \\ \bu_k \\ \vdots \\ \bv_k \\ \vdots \end{bmatrix}.
\]

The *improved* Fisher Vector @cite{perronnin10improving} (IFV) improves the
classification performance of the representation by using to ideas:

1. *Non-linear additive kernel.* The Hellinger's kernel (or
   Bhattacharya coefficient) can be used instead of the linear one at
   no cost by signed squared rooting. This is obtained by applying the
   function $|z| \sign z$ to each dimension of the vector $\Phi(I)$.
   Other @ref homkermap "additive kernels" can also be used at an
   increased space or time cost.
2. *Normalization.* Before using the representation in a linear model
   (e.g. a @ref svm "support vector machine"), the vector $\Phi(I)$ is
   further normalized by the L2 norm (note that the standard Fisher
   vector is normalized by the number of encoded feature vectors).

After square-rooting and normalization, the IFV can is often used in a
linear classifier such as an @ref svm "SVM".

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@page fisher-kernel Fisher kernel
@tableofcontents
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

This page discusses the Fisher Kernels (FK) of
@cite{jaakkola98exploiting} and shows how the FV of
@cite{perronnin06fisher} can be derived from it as a special case. The
FK induces a similarity measures between data points $\bx$ and $\bx'$
from a parametric generative model $p(\bx|\Theta)$ of the data. The
parameter $\Theta$ of the model is selected to fit the a-priori
distribution of the data, and is usually the Maximum Likelihood (MLE)
estimate obtained from a set of training examples. Once the generative
model is learned, each particular datum $\bx$ is represented by
looking at how it affects the MLE parameter estimate. This effect is
measured by computing the gradient of the log-likelihood term
corresponding to $\bx$:

\[
  \hat\Phi(\bx) = \nabla_\Theta \log p(\bx|\Theta)
\]

The vectors $\hat\Phi(\bx)$ should be appropriately scaled before they
can be meaningfully compared. This is obtained by *whitening* the data
by multiplying the vectors by the inverse of the square root of their
*covariance matrix*. The covariance matrix can be obtained from the
generative model $p(\bx|\Theta)$ itself. Since $\Theta$ is the ML
parameter and $\hat\Phi(\bx)$ is the gradient of the log-likelihood
function, its expected value $E[\hat\Phi(\bx)]$ is zero. Thus, since
the vectors are already centered, their covariance matrix is simply:

\[
H = E_{\bx \sim p(\bx|\Theta)} [\hat\Phi(\bx) \hat\Phi(\bx)^\top]
\]

Note that $H$ is also the *Fisher information matrix* of the
model. The final FV encoding $\Phi(\bx)$ is given by the whitened
gradient of the log-likelihood function, i.e.:

\[
 \Phi(\bx) = H^{-\frac{1}{2}}  \nabla_\Theta \log p(\bx|\Theta).
\]

Taking the inner product of two such vectors yields the *Fisher
kernel*:

\[
 K(\bx,\bx')
= \langle \Phi(\bx),\Phi(\bx') \rangle
=  \nabla_\Theta \log p(\bx|\Theta)^\top H^{-1} \nabla_\Theta \log p(\bx'|\Theta).
\]

<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->
@section fisher-vector-derivation Fisher vector derivation
<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  -->

The FV of @cite{perronnin06fisher} is a special case of the @ref
fisher-kernel "Fisher kernel" construction to encode local image
features in an easy-to-compare vector representation. In this
construction, an image is modeled as a collection of $D$-dimensional
feature vectors $I=(\bx_1,\dots,\bx_n)$ generated by a GMM with $K$
components $\Theta=(\mu_k,\Sigma_k,\pi_k:k=1,\dots,K)$. The covariance
matrices are assumed to be diagonal, i.e. $\Sigma_k = \diag
\bsigma_k^2$, $\sigma_k \in \real^D_+$.

The generative model of *one* feature vector $\bx$ is given by the GMM
density function:

\[
 p(\bx|\Theta) =
\sum_{k=1}^K \pi_k p(\bx|\Theta_k),
\quad
p(\bx|\Theta_k)
=
\frac{1}{(2\pi)^\frac{D}{2} (\det \Sigma_k)^{\frac{1}{2}}}
\exp
\left[
-\frac{1}{2}
(\bx - \mu_k)^\top \Sigma_k^{-1} (\bx - \mu_k)
\right]
\]

where $\Theta_k = (\mu_k,\Sigma_k)$. The Fisher Vector requires
computing the derivative of the log-likelihood function with respect
to the various model parameters. Consider in particular the parameters
$\Theta_k$ of a mode. Due to the exponent in the Gaussian density
function, the derivative can be written as

\[
\nabla_{\Theta_k} p(\bx|\Theta_k) =
p(\bx|\Theta_k)
g(\bx|\Theta_k)
\]

for a simple vector function $g$. The derivative of the log-likelihood
function is then

\[
\nabla_{\Theta_k} \log p(\bx|\Theta)
=
\frac{\pi_k p(\bx|\Theta_k)}{\sum_{t=1}^K \pi_k p(\bx|\Theta_k)}
g(\bx|\Theta_k)
=
q_k(\bx) g(\bx|\Theta_k)
\]

where $q_k(\bx)$ is the soft-assignment of the point $\bx$ to the mode
$k$. We make the approximation that $q_k(\bx)\approx 1$ if $\bx$ is
sampled from mode $k$ and $\approx 0$ otherwise. Hence one gets:

\[
E_{\bx \sim p(\bx|\Theta)}
[
\nabla_{\Theta_k} \log p(\bx|\Theta)
\nabla_{\Theta_t} \log p(\bx|\Theta)^\top
]
\approx
\begin{cases}
\pi_k E_{\bx \sim p(\bx|\Theta_k)} [ g(\bx|\Theta_k) g(\bx|\Theta_k)^\top], & t = k, \\
0, & t\not=k.
\end{cases}
\]
Thus under this approximation there is no correlation between the
parameters of the various Gaussian modes.

The function $g$ can be further broken down as the stacking of the
derivative w.r.t. the mean and the diagonal covariance.

\[
g(\bx|\Theta_k)
=
\begin{bmatrix}
g(\bx|\mu_k) \\
g(\bx|\bsigma_k)
\end{bmatrix},
\quad
[g(\bx|\mu_k)]_j
=
\frac{x_j - \mu_{jk}}{\sigma_{jk}^2},
\quad
[g(\bx|\bsigma_k^2)]_j
=
\frac{1}{2\sigma_{jk}^2}
\left(
\left(\frac{x_j - \mu_{jk}}{\sigma_{jk}}\right)^2
-
1
\right)
\]

Thus the covariance of the model (Fisher information) is diagonal and
the diagonal entries are given by

\[
 H_{\mu_{jk}} = \pi_k E[g(\bx|\mu_{jk})g(\bx|\mu_{jk})]
 = \frac{\pi_k}{\sigma_{jk}^2},
 \quad
 H_{\sigma_{jk}^2} = \frac{\pi_k}{2 \sigma_{jk}^4}.
\]

where in the calculation it was used the fact that the fourth moment
of the standard Gaussian distribution is 3. Multiplying the inverse
square root of the matrix $H$ by the derivative of the log-likelihood
function results in the Fisher vector encoding of one image feature
$\bx$:

\[
 \Phi_{\mu_{jk}}(\bx) = H_{\mu_{jk}}^{-\frac{1}{2}} q_k(\bx) g(\bx|\mu_{jk})
= q_k(\bx) \frac{x_j - \mu_{jk}}{\sqrt{\pi_k}\sigma_{jk}},
\qquad
 \Phi_{\sigma^2_{jk}}(\bx) =
\frac{q_k(\bx)}{\sqrt{2 \pi_k}}
\left(
\left(\frac{x_j - \mu_{jk}}{\sigma_{jk}}\right)^2
-
1
\right)
\]

Assuming that features are sampled i.i.d. from the GMM results in the
formulas given in @ref fisher-fundamentals (note the normalization
factor). Note that:

* The Fisher components relative to the prior probabilities $\pi_k$
  have been ignored. This is because they have little effect on the
  representation @cite{perronnin10improving}.

* Technically, the derivation of the Fisher Vector for multiple image
  features requires the number of features to be the same in both
  images. Ultimately, however, the representation can be computed by
  using any number of features.

**/

#include "fisher.h"
#include "mathop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef VL_FISHER_INSTANTIATING

#endif

#ifdef VL_FISHER_INSTANTIATING

static void
VL_XCAT(_vl_fisher_encode_, SFX)
(TYPE * enc,
 TYPE const * means, vl_size dimension, vl_size numClusters,
 TYPE const * covariances,
 TYPE const * priors,
 TYPE const * data, vl_size numData,
 int flags)
{
  vl_size dim;
  vl_index i_cl, i_d;
  TYPE * invSigma;
  TYPE * sqrtInvSigma;
  TYPE * posteriors;
  TYPE * logSigmas;
  TYPE * logWeights;
//  TYPE halfDimLog2Pi = (dimension/2.0)*log(2.0*VL_PI);

#if (FLT == VL_TYPE_FLOAT)
  VlFloatVector3ComparisonFunction distFn = vl_get_vector_3_comparison_function_f(VlDistanceMahal) ;
#else
  VlDoubleVector3ComparisonFunction distFn = vl_get_vector_3_comparison_function_d(VlDistanceMahal) ;
#endif

  logSigmas = vl_malloc(sizeof(TYPE) * numClusters);
  logWeights = vl_malloc(sizeof(TYPE) * numClusters);
  invSigma = vl_malloc(dimension * sizeof(TYPE) * numClusters);
  sqrtInvSigma = vl_malloc(dimension * sizeof(TYPE) * numClusters);
  posteriors = vl_malloc(numData * numClusters * sizeof(TYPE));

  memset(enc, 0, sizeof(TYPE) * 2 * dimension * numClusters) ;

#if defined(_OPENMP)
#pragma omp parallel for default(shared) private(i_cl,dim) num_threads(vl_get_max_threads())
#endif
  for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
    TYPE logSigma = 0;
    logWeights[i_cl] = log(priors[i_cl]) ;

    for(dim = 0; dim < dimension; dim++) {
      logSigma += log(covariances[i_cl*dimension + dim]);
      invSigma[i_cl*dimension + dim] = 1/(covariances[i_cl*dimension + dim]);
      sqrtInvSigma[i_cl*dimension + dim] = sqrt(invSigma[i_cl*dimension + dim]);
    }

    logSigmas[i_cl] = logSigma;
  } /* end of parallel region */

#if defined(_OPENMP)
#pragma omp parallel for default(shared) private(i_d,i_cl) num_threads(vl_get_max_threads())
#endif
  for (i_d = 0 ; i_d < (signed)numData ; ++i_d) {
    TYPE clusterPosteriorsSum = 0;
    TYPE maxPosterior = (TYPE)(-VL_INFINITY_D); /* log p can be negative */

    for (i_cl = 0 ; i_cl < (signed)numClusters ; i_cl++) {
      TYPE p =
      logWeights[i_cl]
      - 0.5 * logSigmas[i_cl]
      - 0.5 * distFn (dimension,
                      data + i_d * dimension,
                      means + i_cl * dimension,
                      invSigma + i_cl * dimension);
      maxPosterior = VL_MAX(p, maxPosterior) ;
      posteriors[i_cl + i_d * numClusters] = p ;
    }

    for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
      TYPE p = posteriors[i_cl + i_d * numClusters] ;
      p = exp(p - maxPosterior) ;
      if (flags & VL_FISHER_FLAG_FAST && p < 1e-3) { p = 0 ; }
      clusterPosteriorsSum += p ;
      posteriors[i_cl + i_d * numClusters] = p ;
    }

    for (i_cl = 0 ; i_cl < (signed)numClusters ; ++i_cl) {
      posteriors[i_cl + i_d * numClusters] /= clusterPosteriorsSum ;
    }
  }

#if defined(_OPENMP)
#pragma omp parallel for default(shared) private(i_cl,i_d,dim) num_threads(vl_get_max_threads())
#endif
  for(i_cl = 0; i_cl < (signed)numClusters; ++ i_cl) {
    TYPE uprefix;
    TYPE vprefix;

    TYPE * uk = enc + i_cl*dimension ;
    TYPE * vk = enc + i_cl*dimension + numClusters*dimension ;

    for(i_d = 0; i_d < (signed)numData; i_d++) {
      TYPE p = posteriors[i_cl + i_d * numClusters] ;
      if (p == 0) continue ;
      for(dim = 0; dim < dimension; dim++) {
        TYPE diff = data[i_d*dimension + dim] - means[i_cl*dimension + dim] ;
        diff *= sqrtInvSigma[i_cl*dimension + dim] ;
        *(uk + dim) += p * diff ;
        *(vk + dim) += p * (diff * diff - 1);
      }
    }

    uprefix = 1/(numData*sqrt(priors[i_cl]));
    vprefix = 1/(numData*sqrt(2*priors[i_cl]));

    for(dim = 0; dim < dimension; dim++) {
      *(uk + dim) = *(uk + dim) * uprefix;
      *(vk + dim) = *(vk + dim) * vprefix;
    }
  }

  vl_free(invSigma);
  vl_free(sqrtInvSigma);
  vl_free(logSigmas);
  vl_free(logWeights);
  vl_free(posteriors);

  if (flags & VL_FISHER_FLAG_SQUARE_ROOT) {
    for(dim = 0; dim < 2 * dimension * numClusters ; dim++) {
      TYPE z = enc [dim] ;
      if (z >= 0) {
        enc[dim] = VL_XCAT(vl_sqrt_, SFX)(z) ;
      } else {
        enc[dim] = - VL_XCAT(vl_sqrt_, SFX)(- z) ;
      }
    }
  }

  if (flags & VL_FISHER_FLAG_NORMALIZED) {
    TYPE n = 0 ;
    for(dim = 0 ; dim < 2 * dimension * numClusters ; dim++) {
      TYPE z = enc [dim] ;
      n += z * z ;
    }
    n = VL_XCAT(vl_sqrt_, SFX)(n) ;
    n = VL_MAX(n, 1e-12) ;
    for(dim = 0 ; dim < 2 * dimension * numClusters ; dim++) {
      enc[dim] /= n ;
    }
  }
}

/* VL_FISHER_INSTANTIATING */
#else

#ifndef __DOXYGEN__
#define FLT VL_TYPE_FLOAT
#define TYPE float
#define SFX f
#define VL_FISHER_INSTANTIATING
#include "fisher.c"

#define FLT VL_TYPE_DOUBLE
#define TYPE double
#define SFX d
#define VL_FISHER_INSTANTIATING
#include "fisher.c"
#endif

#endif

/* ================================================================ */
#ifndef VL_FISHER_INSTANTIATING

/** @brief Fisher vector encoding of a set of vectors.
 ** @param dataType the type of the input data (::VL_TYPE_DOUBLE or ::VL_TYPE_FLOAT).
 ** @param enc Fisher vector (output).
 ** @param means Gaussian mixture means.
 ** @param dimension dimension of the data.
 ** @param numClusters number of Gaussians mixture components.
 ** @param covariances Gaussian mixture diagonal covariances.
 ** @param priors Gaussian mixture prior probabilities.
 ** @param data vectors to encode.
 ** @param numData number of vectors to encode.
 ** @param flags options.
 **
 ** @a means and @a covariances have @a dimension rows and @a numCluster columns.
 ** @a priors is a vector of size @a numCluster. @a data has @a dimension
 ** rows and @a numData columns. @a enc is a vecotr of size equal
 ** to twice the product of @a dimension and @a numClusters.
 ** All these vectors and matrices have the same class, as specified
 ** by @a dataType.
 **
 ** @a flag can be used to control several options:
 ** ::VL_FISHER_FLAG_SQUARE_ROOT, ::VL_FISHER_FLAG_NORMALIZE,
 ** ::VL_FISHER_FLAG_IMPROVED.
 **
 ** @sa @ref fisher
 **/

VL_EXPORT void
vl_fisher_encode
(void * enc, vl_type dataType,
 void const * means, vl_size dimension, vl_size numClusters,
 void const * covariances,
 void const * priors,
 void const * data,  vl_size numData,
 int flags
)
{
  switch(dataType) {
    case VL_TYPE_FLOAT:
      _vl_fisher_encode_f
      ((float *) enc,
       (float const *) means, dimension, numClusters,
       (float const *) covariances,
       (float const *) priors,
       (float const *) data, numData,
       flags);
      break;
    case VL_TYPE_DOUBLE:
      _vl_fisher_encode_d
      ((double *) enc,
       (double const *) means, dimension, numClusters,
       (double const *) covariances,
       (double const *) priors,
       (double const *) data, numData,
       flags);
      break;
    default:
      abort();
  }
}

#endif

#undef SFX
#undef TYPE
#undef FLT
#undef VL_FISHER_INSTANTIATING
