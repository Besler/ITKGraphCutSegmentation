#include "itkIndex.h"
#include "lib/gridcut/include/GridCut/GridGraph_3D_6C_MT.h"

#include <memory>
#include <vector>
#include <limits>

// #include "gtest/gtest_prod.h"

/*
  Source == object == foreground == 0 (GridCut)
  Sink == background == 1 (GridCut)
*/

template<typename TWeight>
class Graph {
public:
  /* Constructors, Destructors */
  Graph(int width, int height, int depth, int num_threads, int block_size);
  ~Graph();

  /* Helpful templates */
  typedef GridGraph_3D_6C_MT<TWeight,TWeight,TWeight> GridGraphType;
  typedef int                                         NodeIndexType;    // Defined in GridGraph_3D_6C_MT
  typedef int                                         GraphIndexType;   // Defined in GridGraph_3D_6C_MT
  typedef int                                         SegmentationType; // Defined in GridGraph_3D_6C_MT
  typedef std::vector< TWeight >                      CapacityType;
  typedef std::vector< CapacityType >                 CapacityMatrix;
  typedef itk::Index<3>                               IndexType;

  /* Function for converting index */
  NodeIndexType GetNodeID(IndexType index);
  unsigned int GetArrayIndex(IndexType index);

  /* Work with graph */
  void SolveGraph();
  SegmentationType GetSegmentation(NodeIndexType nodeID);
  SegmentationType GetSegmentation(IndexType index);
  TWeight GetMaxFlow();

  void AddNLink(IndexType nodeID1, IndexType nodeID2, TWeight capacity, TWeight reverseCapacity);
  void AddTLink(IndexType nodeID, TWeight foregroundTLinkCapacity, TWeight backgroundTLinkCapacity);

  static bool IsForegroundLabel(SegmentationType label);

protected:
  // FRIEND_TEST(Graph, VolumeIndexToCapacityIndex);
  // FRIEND_TEST(Graph, VolumeIndexToCapacityIndexStandardPoints);
  unsigned int VolumeIndexToCapacityIndex(IndexType center, IndexType neighbour);

private:
  //std::unique_ptr<GridGraphType> m_Graph;
  GridGraphType * m_Graph;
  unsigned int m_Width, m_Height, m_Depth, m_NumberOfVoxels;
  bool m_IsSolved;
  CapacityMatrix m_Cap;

  Graph(const Graph &); // intentionally not implemented
	void operator=(const Graph &); // intentionally not implemented
}; // class Graph

template<typename TWeight>
Graph<TWeight>::Graph(int width, int height, int depth, int num_threads, int block_size)
  : m_IsSolved(false)
  , m_Width(width)
  , m_Height(height)
  , m_Depth(depth)
  , m_NumberOfVoxels(width*height*depth)
{
  //m_Graph = std::make_unique<GridGraphType>(width, height, depth, num_threads, block_size);
  m_Graph = new GridGraphType(width, height, depth, num_threads, block_size);

  m_Cap.resize(6 + 2); // 6 neighbours + 2 classes
  m_Cap.at(0) = CapacityType(m_NumberOfVoxels, 0); // Source / foreground
  m_Cap.at(1) = CapacityType(m_NumberOfVoxels, std::numeric_limits<TWeight>::max()); // Sink / background
  m_Cap.at(2) = CapacityType(m_NumberOfVoxels, 0); // -1 0 0 m_CapLEE
  m_Cap.at(3) = CapacityType(m_NumberOfVoxels, 0); // +1 0 0 m_CapGEE
  m_Cap.at(4) = CapacityType(m_NumberOfVoxels, 0); // 0 -1 0 m_CapELE
  m_Cap.at(5) = CapacityType(m_NumberOfVoxels, 0); // 0 +1 0 m_CapEGE
  m_Cap.at(6) = CapacityType(m_NumberOfVoxels, 0); // 0 0 -1 m_CapEEL
  m_Cap.at(7) = CapacityType(m_NumberOfVoxels, 0); // 0 0 +1 m_CapEEG
}

template<typename TWeight>
Graph<TWeight>::~Graph() {/* m_GridGraph automatically cleaned up because it is a std::unique_ptr */
	delete m_Graph;
}

template<typename TWeight>
typename Graph<TWeight>::NodeIndexType Graph<TWeight>::GetNodeID(IndexType index) {
  // No check for valid m_Graph
  return m_Graph->node_id(index[0], index[1], index[2]);
}

template<typename TWeight>
unsigned int Graph<TWeight>::GetArrayIndex(IndexType index) {
  // No check for valid m_Graph
  return index[0] + m_Width*index[1] + m_Height*m_Width*index[2];
}

template<typename TWeight>
void Graph<TWeight>::SolveGraph() {
  // No check for valid m_Graph
  m_Graph->set_caps(
     m_Cap.at(0).data(), m_Cap.at(1).data()
    ,m_Cap.at(2).data(), m_Cap.at(3).data()
    ,m_Cap.at(4).data(), m_Cap.at(5).data()
    ,m_Cap.at(6).data(), m_Cap.at(7).data()
  );
  m_Graph->compute_maxflow();
  m_IsSolved = true;
}

template<typename TWeight>
typename Graph<TWeight>::SegmentationType Graph<TWeight>::GetSegmentation(NodeIndexType nodeID) {
  if (m_IsSolved) {
    return m_Graph->get_segment(nodeID);
  } else {
    return (SegmentationType)-1;
  }
}

template<typename TWeight>
typename Graph<TWeight>::SegmentationType Graph<TWeight>::GetSegmentation(IndexType index) {
  return GetSegmentation(GetNodeID(index));
}

template<typename TWeight>
bool Graph<TWeight>::IsForegroundLabel(SegmentationType label) {
  return label == 0;
}

template<typename TWeight>
TWeight Graph<TWeight>::GetMaxFlow() {
  if (m_IsSolved) {
    return m_Graph->get_flow();
  } else {
    return (TWeight)-1;
  }
}

template<typename TWeight>
void Graph<TWeight>::AddNLink(IndexType nodeID1, IndexType nodeID2, TWeight capacity, TWeight reverseCapacity) {
  assert(capacity >= 0);
  assert(reverseCapacity >= 0);
  m_Cap.at(VolumeIndexToCapacityIndex(nodeID1, nodeID2)).at(GetArrayIndex(nodeID1)) = capacity;
  m_Cap.at(VolumeIndexToCapacityIndex(nodeID2, nodeID1)).at(GetArrayIndex(nodeID2)) = reverseCapacity;
}

template<typename TWeight>
void Graph<TWeight>::AddTLink(IndexType nodeID, TWeight foregroundTLinkCapacity, TWeight backgroundTLinkCapacity) {
  assert(foregroundTLinkCapacity >= 0);
  assert(backgroundTLinkCapacity >= 0);
  m_Cap.at(0).at(GetArrayIndex(nodeID)) = foregroundTLinkCapacity;
  m_Cap.at(1).at(GetArrayIndex(nodeID)) = backgroundTLinkCapacity;
}

template<typename TWeight>
unsigned int Graph<TWeight>::VolumeIndexToCapacityIndex(IndexType center, IndexType neighbour) {
  GraphIndexType n = (neighbour[0]-center[0]) + 2*(neighbour[1] - center[1]) + 3*(neighbour[2] - center[2]);
  // TODO: I rely on compiler optimization here
  return (n >= 0) ? 2*n+1 : -2*n ;
}
