/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkVectorGradientNDAnisotropicDiffusionFunction_h
#define itkVectorGradientNDAnisotropicDiffusionFunction_h

#include "itkVectorAnisotropicDiffusionFunction.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkVectorNeighborhoodInnerProduct.h"
#include "itkDerivativeOperator.h"

namespace itk
{
/** \class VectorGradientNDAnisotropicDiffusionFunction
 *
 * This class is a simple extension of the
 * GradientNDAnisotropicDiffusionFunction to pixel types of multiple
 * components.  Vector components are diffused separately, but diffusion of
 * each component is limited by a conductance term which depends on all
 * components.
 *
 * For more information, please see GradientNDAnisotropicDiffusionFunction.
 *
 * \sa GradientNDAnisotropicDiffusionFunction
 * \sa VectorCurvatureNDAnisotropicDiffusionFunction
 * \sa AnisotropicDiffusionFunction
 * \ingroup ITKAnisotropicSmoothing
 */
template <typename TImage>
class ITK_TEMPLATE_EXPORT VectorGradientNDAnisotropicDiffusionFunction
  : public VectorAnisotropicDiffusionFunction<TImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(VectorGradientNDAnisotropicDiffusionFunction);

  /** Standard class type aliases. */
  using Self = VectorGradientNDAnisotropicDiffusionFunction;
  using Superclass = VectorAnisotropicDiffusionFunction<TImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(VectorGradientNDAnisotropicDiffusionFunction, ScalarAnisotropicDiffusionFunction);

  /** Inherit some parameters from the superclass type. */
  using typename Superclass::ImageType;
  using typename Superclass::PixelType;
  using typename Superclass::TimeStepType;
  using typename Superclass::RadiusType;
  using typename Superclass::NeighborhoodType;
  using typename Superclass::FloatOffsetType;

  /** Extract vector and image dimension from superclass. */
  static constexpr unsigned int ImageDimension = Superclass::ImageDimension;
  static constexpr unsigned int VectorDimension = Superclass::VectorDimension;

  /** Type of a value in a vector (double, float, etc.) */
  using ScalarValueType = typename PixelType::ValueType;

  /** Compute the equation value. */
  PixelType
  ComputeUpdate(const NeighborhoodType & it,
                void *                   globalData,
                const FloatOffsetType &  offset = FloatOffsetType(0.0)) override;

  /** This method is called prior to each iteration of the solver. */
  void
  InitializeIteration() override
  {
    m_K = this->GetAverageGradientMagnitudeSquared() * this->GetConductanceParameter() *
          this->GetConductanceParameter() * -2.0f;
  }

protected:
  VectorGradientNDAnisotropicDiffusionFunction();
  ~VectorGradientNDAnisotropicDiffusionFunction() override = default;

private:
  /** Inner product function. */
  VectorNeighborhoodInnerProduct<ImageType> m_InnerProduct{};

  /** Slices for the ND neighborhood. */
  std::slice x_slice[ImageDimension];
  std::slice xa_slice[ImageDimension][ImageDimension];
  std::slice xd_slice[ImageDimension][ImageDimension];

  /** Derivative operators. */
  DerivativeOperator<ScalarValueType, Self::ImageDimension> m_DerivativeOperator{};

  /** Modified global average gradient magnitude term. */
  ScalarValueType m_K;

  static double m_MIN_NORM;

  SizeValueType m_Stride[ImageDimension]{};
  SizeValueType m_Center{};
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkVectorGradientNDAnisotropicDiffusionFunction.hxx"
#endif

#endif
