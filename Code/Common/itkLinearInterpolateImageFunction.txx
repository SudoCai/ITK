/*==========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLinearInterpolateImageFunction.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

==========================================================================*/
#ifndef _itkLinearInterpolateImageFunction_txx
#define _itkLinearInterpolateImageFunction_txx

#include <iostream>

#include "itkPixelTraits.h"
#include "itkIndex.h"
#include "vnl/vnl_math.h"

namespace itk
{

/**
 *
 */
template<class TInputImage>
void
LinearInterpolateImageFunction<TInputImage>
::SetInputImage( InputImageType *ptr )
{
  this->Superclass::SetInputImage( ptr );

  m_ImageSize =
    this->GetInputImage()->GetLargestPossibleRegion().GetSize();

  m_Neighbors = 1UL << ImageDimension;

}


/**
 *
 */
template<class TInputImage>
void
LinearInterpolateImageFunction<TInputImage>
::PrintSelf(std::ostream& os, Indent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "linearly interpolate image." << std::endl;
}


/**
 *
 */
template<class TInputImage>
double
LinearInterpolateImageFunction<TInputImage>
::Evaluate(
const IndexType& index )
{
  double value = 0.0;    // Value of interpolated pixel

  /* Check if indices are within image; immediately return 0 if not */
  for( int j = 0; j < ImageDimension; j++ )
    {
    if( (unsigned long)index[j] >= m_ImageSize[j] )
      {
        return 0;
      }
    }

  /* Otherwise return the appropriate pixel value */
  value = (double)this->GetInputImage()->GetPixel( index );
  return ( value );
}


/**
 *
 */
template<class TInputImage>
double
LinearInterpolateImageFunction<TInputImage>
::Evaluate(
double * dblIndex ) const
{

  /* Prepare coordinates; check whether inside image or not.  */

  for (int j = 0; j < ImageDimension; j++)
  {
    m_Base[j]  = (long)floor(dblIndex[j]);
    m_Alpha[j] = dblIndex[j] - m_Base[j];
  }

  return EvaluateFromBaseAndAlpha();

}


/**
 *
 */
template<class TInputImage>
double
LinearInterpolateImageFunction<TInputImage>
::Evaluate(
const PointType & point ) const
{

  /* Prepare coordinates; check whether inside image or not. */


  for (int j = 0; j < ImageDimension; j++)
  {
    m_Base[j]  = (long)floor(point[j]);
    m_Alpha[j] = point[j] - m_Base[j];
  }

  return EvaluateFromBaseAndAlpha();

}



/**
 *
 */
template<class TInputImage>
double
LinearInterpolateImageFunction<TInputImage>
::EvaluateFromBaseAndAlpha() const
{
  int j;        // Index over coordinates

  /* Prepare coordinates; check whether inside image or not.
     If completely outside image, return 0 immediately. */

  bool partial = false;                 // Partially inside image?

  for (j = 0; j < ImageDimension; j++) {
    if ( m_Base[j] < -1 || (unsigned long)m_Base[j] >= m_ImageSize[j] ) {
      // Completely outside
      return 0; }
    else if ( m_Base[j] < 0 || (unsigned long)m_Base[j] >= m_ImageSize[j]-1) {
      // Overlaps the boundary
      partial = true; }
  }

  IndexType neighIndex;
  double value = 0.0;            // Interpolated pixel value
  typename InputImageType::ConstPointer image = this->GetInputImage();

  /* Case 1: Interpolation neighborhood overlaps the image boundary */
  if (partial) {
    for (unsigned int counter = 0; counter < m_Neighbors; counter++) {
      double alf = 1.0;          // Inter parameter for each neighbor
      int upper = counter;       // Each bit indicates upper/lower neighbor
      for (j = 0; j < ImageDimension; j++) {

        neighIndex[j] = m_Base[j];

        if (upper & 1) {
          if ( m_Base[j] >= 0 &&
               (unsigned long)m_Base[j] > m_ImageSize[j] - 2 ) {
            alf = 0.0;
            break; }
          else {
            neighIndex[j] += 1;
            alf *= m_Alpha[j]; }
          }
        else {
          if ( m_Base[j] < 0 ) {
            alf = 0.0;
            break;  }
          else
            alf *= 1.0 - m_Alpha[j];
        }
        upper >>= 1;
      }

      value += alf * (double)image->GetPixel( neighIndex );
    }
  }

  /* Case 2: Interpolation neighborhood is completely inside the image */
  else {
    for (unsigned int counter = 0; counter < m_Neighbors; counter++) {
      double alf = 1.0;              // Interp parameter for each neighbor
      unsigned int upper = counter;  // Each bit indicates upper/lower neighbor
      for (j = 0; j < ImageDimension; j++) {

        if (upper & 1) {
          neighIndex[j] = m_Base[j] + 1;
          alf *= m_Alpha[j];
        }
        else {
          neighIndex[j] = m_Base[j];
          alf *= 1.0 - m_Alpha[j];
        }
        upper >>= 1;
      }

      value += alf * (double) image->GetPixel( neighIndex );
    }
  }

  return ( value );

}

} // namespace itk

#endif
