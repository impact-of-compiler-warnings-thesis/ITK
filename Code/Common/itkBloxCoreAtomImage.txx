/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBloxCoreAtomImage.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkBloxCoreAtomImage_txx
#define __itkBloxCoreAtomImage_txx

#include <iostream>
#include "itkImageRegionIterator.h"
#include "itkConicShellInteriorExteriorSpatialFunction.h"
#include "itkFloodFilledSpatialFunctionConditionalIterator.h"

namespace itk
{

template<class TBoundaryPointImage, class TImageTraits>
BloxCoreAtomImage<TBoundaryPointImage, TImageTraits>
::BloxCoreAtomImage()
{
  m_BoundaryPointImage = NULL;

  m_NumCoreAtoms = 0;
  m_DistanceMin = 8;
  m_DistanceMax = 12;
  m_Epsilon = 0;
  m_Polarity = 0;
}

template<class TBoundaryPointImage, class TImageTraits>
BloxCoreAtomImage<TBoundaryPointImage, TImageTraits>
::~BloxCoreAtomImage()
{

}

template<class TBoundaryPointImage, class TImageTraits>
void
BloxCoreAtomImage<TBoundaryPointImage, TImageTraits>
::FindCoreAtoms()
{
  std::cout << "BloxCoreAtomImage::FindCoreAtoms() called\n";

  // Make sure we're getting everything
  m_BoundaryPointImage->SetRequestedRegionToLargestPossibleRegion();

  // Create an iterator to walk the source image
  typedef ImageRegionIterator<TBoundaryPointImage> TImageIteratorType;

  TImageIteratorType imageIt = TImageIteratorType(m_BoundaryPointImage,
                                                  m_BoundaryPointImage->GetRequestedRegion() );

  // Iterate through the entire image (all pixels) and look for core atoms
  for ( imageIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt)
    {
    // The iterator for accessing linked list info
    itk::BloxBoundaryPointPixel<NDimensions>::iterator bpiterator;

    // Walk through all of the elements at the pixel
    for (bpiterator = imageIt.Value().begin(); bpiterator != imageIt.Value().end(); ++bpiterator)
      {
      this->FindCoreAtomsAtBoundaryPoint( *bpiterator );
      }

    }
 
  std::cout << "Finished looking for core atoms\n";
  std::cout << "I found " << m_NumCoreAtoms << " core atoms\n";
}

template<class TBoundaryPointImage, class TImageTraits>
void
BloxCoreAtomImage<TBoundaryPointImage, TImageTraits>
::FindCoreAtomsAtBoundaryPoint(BloxBoundaryPointItem<NDimensions>* pBPOne)
{
  // When looking for core atoms at a boundary point, we want to examine
  // all of the boundary points within blox that are part of a conical
  // region extending out in the direction of the gradient of the boundary
  // point.

  //---------Create and initialize a conic shell spatial function-----------
  typedef itk::ConicShellInteriorExteriorSpatialFunction<NDimensions> TFunctionType;
  typedef TFunctionType::TGradientType TFunctionGradientType;

  TFunctionType::Pointer spatialFunc = TFunctionType::New();

  // Set the properties of the conic shell
  spatialFunc->SetDistanceMin(m_DistanceMin);
  spatialFunc->SetDistanceMax(m_DistanceMax);
  spatialFunc->SetEpsilon(m_Epsilon);
  spatialFunc->SetPolarity(m_Polarity);

  // Set the origin of the conic shell to the current boundary point location
  TPositionType spatialFunctionOrigin = pBPOne->GetPhysicalPosition();
  spatialFunc->SetOrigin(spatialFunctionOrigin);

  // Covert the origin position to a vector
  //TVectorType spatialFunctionOriginVector = spatialFunctionOrigin.GetVectorFromOrigin();

  TVectorType spatialFunctionOriginVector;
  spatialFunctionOriginVector.Set_vnl_vector( spatialFunctionOrigin.Get_vnl_vector() );

  // Set the gradient of the conic shell to the current boundary point gradient
  TFunctionGradientType spatialFunctionGradient = pBPOne->GetGradient();
  spatialFunc->SetOriginGradient(spatialFunctionGradient);

  // Create a seed position for the spatial function iterator we'll use shortly
  typename TBoundaryPointImage::IndexType seedIndex;

  // Normalize the origin gradient
  TVectorType seedVector;
  seedVector.Set_vnl_vector(spatialFunctionGradient.Get_vnl_vector());
  seedVector = seedVector / seedVector.GetNorm();
  
  // If the polarity is 1, the seed position is in the direction
  // opposite the gradient
  if(m_Polarity == 1)
    seedVector = seedVector * -1;
  
  // A "safe" seed position is the closest point in the conical region
  // along the axis of the cone
  TPositionType seedPos = spatialFunctionOrigin + (seedVector * m_DistanceMin);

  // If the seed position is inside the image, go ahead and process it
  if( this->ConvertPhysicalToDataCoords(seedPos, seedIndex) )
    {
    //std::cout << "Successfully created a conical iterator\n";

    // Create and initialize a spatial function iterator
    typedef itk::FloodFilledSpatialFunctionConditionalIterator<TBoundaryPointImage, TFunctionType> TSphereItType;
    TSphereItType sfi = TSphereItType(m_BoundaryPointImage, spatialFunc, seedIndex);

    // Walk the spatial function
    for( ; !( sfi.IsAtEnd() ); ++sfi)
      {
      // The iterator for accessing linked list info
      itk::BloxBoundaryPointPixel<NDimensions>::iterator bpiterator;

      // Walk through all of the elements at the pixel
      for (bpiterator = sfi.Get().begin(); bpiterator != sfi.Get().end(); ++bpiterator)
        {
        // Get the pointer of the blox
        TBPItemType* pBPTwo = *bpiterator;

        // Get the physical positions of the two boundary points
        TPositionType P1 = pBPOne->GetPhysicalPosition();
        TPositionType P2 = pBPTwo->GetPhysicalPosition();

        // Form the two vectors between them
        TVectorType C12 = P2 - P1;

        // If we don't meet distance criteria, move on
        if(!( (C12.GetNorm() > m_DistanceMin) && (C12.GetNorm() < m_DistanceMax) ) )
          continue;

        TVectorType C21 = P1 - P2;

        C12 = C12 / C12.GetNorm();
        C21 = C21 / C21.GetNorm();

        // Get the gradients of the two boundary points
        TGradientType G1 = pBPOne->GetGradient();
        TGradientType G2 = pBPTwo->GetGradient();

        G1 = G1 / G1.GetNorm();
        G2 = G2 / G2.GetNorm();

        // Calculate face-to-faceness
        double faceToFaceness = dot_product(G1.Get_vnl_vector(), C12.Get_vnl_vector() ) *
          dot_product(G2.Get_vnl_vector(), C21.Get_vnl_vector() );
        
        //std::cout << "Face to faceness = " << faceToFaceness << "\n";

        // If face-to-faceness meets threshold criteria
        if( faceToFaceness > (1.0 - m_Epsilon) )
          {
          //std::cout << "Passed, face to faceness = " << faceToFaceness << "\n";

          // Figure out the center of the core atom
          TPositionType coreAtomCenter = P1 + (P2 - P1) / 2;

          // Create a new core atom
          BloxCoreAtomItem<NDimensions>* pCoreAtom = new BloxCoreAtomItem<NDimensions>;
          
          // Set its boundary points and center
          pCoreAtom->SetBoundaryPointA(pBPOne);
          pCoreAtom->SetBoundaryPointB(pBPTwo);
          pCoreAtom->SetCenterPosition(coreAtomCenter);

          // Figure out the data space coordinates of the center
          IndexType coreAtomPos;
          
          this->ConvertPhysicalToDataCoords(coreAtomCenter, coreAtomPos);
         
          // Store the new core atom in the correct spot
          this->GetPixel(coreAtomPos).push_back(pCoreAtom);

          m_NumCoreAtoms++;

          } // end if face-to-faceness meets criteria
        } // end iterate through boundary points in pixel
      } // end iterate through the conic shell
  } // end if the seed position for the conic shell is in the image
  else
    {
    //std::cout << "Index not in image\n";
    }
}

template<class TBoundaryPointImage, class TImageTraits>
bool
BloxCoreAtomImage<TBoundaryPointImage, TImageTraits>::
ConvertPhysicalToDataCoords(TPositionType physicalCoords, IndexType& dataCoords)
{
  // How big is this blox image in pixels?
  SizeType bloxSizeObject = this->GetLargestPossibleRegion().GetSize();
  const unsigned long* mySize = bloxSizeObject.GetSize();

  // Get the origin and spacing of this image
  const double* myOrigin = this->GetOrigin();
  const double* mySpacing = this->GetSpacing();

  // Position in data space along the dimension of interest
  long int dimPosition;

  // Convert to data coordinates, abort if it's outside allowed data space bounds
  for (int ii = 0; ii < NDimensions; ++ii)
    {
    dimPosition = (long int) ( (physicalCoords[ii]/mySpacing[ii]) - myOrigin[ii]);
 
    if( (dimPosition < 0) || (dimPosition>=mySize[ii]) )
      return false;
    else
      dataCoords.m_Index[ii] = dimPosition;
    }
  
  return true;
}

template<class TBoundaryPointImage, class TImageTraits>
void
BloxCoreAtomImage<TBoundaryPointImage, TImageTraits>::
DoEigenanalysis()
{
  itk::ImageRegionIterator<Self> bloxit = 
    itk::ImageRegionIterator<Self>(this, this->GetLargestPossibleRegion() );

  for(bloxit.GoToBegin(); !bloxit.IsAtEnd(); ++bloxit)
    {
      ( &bloxit.Value() )->DoCoreAtomEigenanalysis();
    }
}

template<class TBoundaryPointImage, class TImageTraits>
void
BloxCoreAtomImage<TBoundaryPointImage, TImageTraits>::
DoCoreAtomVoting()
{
  itk::ImageRegionIterator<Self> bloxit = 
    itk::ImageRegionIterator<Self>(this, this->GetLargestPossibleRegion() );

  for(bloxit.GoToBegin(); !bloxit.IsAtEnd(); ++bloxit)
    {
	// Core atom voting code goes here
    }
}

} // end namespace itk

#endif
