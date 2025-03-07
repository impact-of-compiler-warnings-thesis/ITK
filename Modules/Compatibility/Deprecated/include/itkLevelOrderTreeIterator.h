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
#ifndef itkLevelOrderTreeIterator_h
#define itkLevelOrderTreeIterator_h

#include <queue>
#include <climits>
#include "itkTreeIteratorBase.h"

namespace itk
{
/**
 * \class LevelOrderTreeIterator
 * \brief Iterate over a tree in level order.
 *
 * \ingroup ITKDeprecated
 */
template <typename TTreeType>
class ITK_TEMPLATE_EXPORT LevelOrderTreeIterator : public TreeIteratorBase<TTreeType>
{
public:
  /** Typedefs */
  using Self = LevelOrderTreeIterator;
  using Superclass = TreeIteratorBase<TTreeType>;
  using TreeType = TTreeType;
  using ValueType = typename TTreeType::ValueType;
  using typename Superclass::TreeNodeType;
  using typename Superclass::NodeType;

  /** Constructor with end level specification */
  LevelOrderTreeIterator(TreeType * tree, int endLevel = INT_MAX, const TreeNodeType * start = nullptr);

  /** Constructor with end level specification. */
  LevelOrderTreeIterator(TreeType * tree, int startLevel, int endLevel, const TreeNodeType * start = nullptr);

  ~LevelOrderTreeIterator() override = default;

  /** Get the type of the iterator. */
  NodeType
  GetType() const override;

  /** Get the start level. */
  int
  GetStartLevel() const;

  /** Get the end level. */
  int
  GetEndLevel() const;

  /** Get the current level. */
  int
  GetLevel() const;

  /** Clone function. */
  TreeIteratorBase<TTreeType> *
  Clone() override;

  /** operator = */
  const Self &
  operator=(const Self & iterator)
  {
    if (this != &iterator)
    {
      this->Superclass::operator=(iterator);
      m_StartLevel = iterator.m_StartLevel;
      m_EndLevel = iterator.m_EndLevel;
      m_Queue = iterator.m_Queue;
    }
    return *this;
  }

protected:
  /** Get the next node. */
  const ValueType &
  Next() override;

  /** Return true if the next node exists. */
  bool
  HasNext() const override;

private:
  /** Find the next available node. */
  const TreeNodeType *
  FindNextNode() const;

  /** Helper function to find the next node. */
  const TreeNodeType *
  FindNextNodeHelp() const;

  /** Get the level given a node. */
  int
  GetLevel(const TreeNodeType * node) const;

  int                                      m_StartLevel;
  int                                      m_EndLevel;
  mutable std::queue<const TreeNodeType *> m_Queue;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkLevelOrderTreeIterator.hxx"
#endif

#endif
