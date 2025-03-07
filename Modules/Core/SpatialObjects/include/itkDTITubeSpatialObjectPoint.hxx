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
#ifndef itkDTITubeSpatialObjectPoint_hxx
#define itkDTITubeSpatialObjectPoint_hxx

#include "itksys/SystemTools.hxx"

namespace itk
{

template <unsigned int TPointDimension>
DTITubeSpatialObjectPoint<TPointDimension>::DTITubeSpatialObjectPoint()
{
  // Initialize the tensor matrix to identity
  for (auto & i : m_TensorMatrix)
  {
    i = 0;
  }
  m_TensorMatrix[0] = 1;
  m_TensorMatrix[3] = 1;
  m_TensorMatrix[5] = 1;
}

template <unsigned int TPointDimension>
DTITubeSpatialObjectPoint<TPointDimension>::DTITubeSpatialObjectPoint(const DTITubeSpatialObjectPoint & other)
  : Superclass(other)
{
  m_Fields.clear();
  const FieldListType & fields = other.GetFields();
  auto                  it = fields.begin();
  while (it != fields.end())
  {
    this->AddField(it->first.c_str(), it->second);
    ++it;
  }
  for (unsigned int i = 0; i < 6; ++i)
  {
    m_TensorMatrix[i] = other.m_TensorMatrix[i];
  }
}

template <unsigned int TPointDimension>
void
DTITubeSpatialObjectPoint<TPointDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

template <unsigned int TPointDimension>
std::string
DTITubeSpatialObjectPoint<TPointDimension>::TranslateEnumToChar(DTITubeSpatialObjectPointFieldEnum name) const
{
  // Do the translation
  switch (static_cast<int>(name))
  {
    case 0:
      return std::string("FA");
    case 1:
      return std::string("ADC");
    case 2:
      return std::string("GA");
    default:
      // Just fall through.
      break;
  }
  return std::string("");
}

template <unsigned int TPointDimension>
void
DTITubeSpatialObjectPoint<TPointDimension>::AddField(const char * name, float value)
{
  FieldType field(itksys::SystemTools::LowerCase(name), value);

  m_Fields.push_back(field);
}

template <unsigned int TPointDimension>
void
DTITubeSpatialObjectPoint<TPointDimension>::SetField(const char * name, float value)
{
  auto it = m_Fields.begin();

  while (it != m_Fields.end())
  {
    if (!strcmp(it->first.c_str(), itksys::SystemTools::LowerCase(name).c_str()))
    {
      it->second = value;
    }
    ++it;
  }
}

template <unsigned int TPointDimension>
void
DTITubeSpatialObjectPoint<TPointDimension>::SetField(DTITubeSpatialObjectPointFieldEnum name, float value)
{
  std::string charname = this->TranslateEnumToChar(name);

  if (!charname.empty())
  {
    this->SetField(charname.c_str(), value);
  }
  else
  {
    std::cout << "DTITubeSpatialObjectPoint::SetField(): enum not defined" << std::endl;
  }
}

template <unsigned int TPointDimension>
void
DTITubeSpatialObjectPoint<TPointDimension>::AddField(DTITubeSpatialObjectPointFieldEnum name, float value)
{
  std::string charname = this->TranslateEnumToChar(name);

  if (!charname.empty())
  {
    FieldType field(itksys::SystemTools::LowerCase(charname).c_str(), value);
    m_Fields.push_back(field);
  }
  else
  {
    std::cout << "DTITubeSpatialObjectPoint::AddField() : enum not defined" << std::endl;
  }
}

template <unsigned int TPointDimension>
float
DTITubeSpatialObjectPoint<TPointDimension>::GetField(const char * name) const
{
  auto it = m_Fields.begin();

  while (it != m_Fields.end())
  {
    if (!strcmp(it->first.c_str(), itksys::SystemTools::LowerCase(name).c_str()))
    {
      return it->second;
    }
    ++it;
  }
  return -1;
}

template <unsigned int TPointDimension>
float
DTITubeSpatialObjectPoint<TPointDimension>::GetField(DTITubeSpatialObjectPointFieldEnum name) const
{
  std::string charname = this->TranslateEnumToChar(name);
  if (!charname.empty())
  {
    return this->GetField(itksys::SystemTools::LowerCase(charname).c_str());
  }
  std::cout << "DTITubeSpatialObjectPoint::GetField() : enum not defined" << std::endl;
  return -1;
}

template <unsigned int TPointDimension>
auto
DTITubeSpatialObjectPoint<TPointDimension>::operator=(const DTITubeSpatialObjectPoint & rhs) -> Self &
{
  if (this != &rhs)
  {
    Superclass::operator=(rhs);
    m_Fields.clear();
    const FieldListType & fields = rhs.GetFields();
    auto                  it = fields.begin();
    while (it != fields.end())
    {
      this->AddField(it->first.c_str(), it->second);
      ++it;
    }
    for (unsigned int i = 0; i < 6; ++i)
    {
      m_TensorMatrix[i] = rhs.m_TensorMatrix[i];
    }
  }
  return *this;
}
} // end namespace itk

#endif
