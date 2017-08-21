#include <gtest/gtest.h>

#include "Graph.h"

#include <math.h>

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"

TEST(Graph, VolumeIndexToCapacityIndex) {
  typedef char TWeight;
  Graph<TWeight> graph(1,1,1,1,100);

  itk::Index<3> center, neighbor;

  center = {1,0,0}; neighbor = {0, 0, 0};
  ASSERT_EQ(2, graph.VolumeIndexToCapacityIndex(center, neighbor));

  center = {0,0,0}; neighbor = {1, 0, 0};
  ASSERT_EQ(3, graph.VolumeIndexToCapacityIndex(center, neighbor));

  center = {0,1,0}; neighbor = {0, 0, 0};
  ASSERT_EQ(4, graph.VolumeIndexToCapacityIndex(center, neighbor));

  center = {0,0,0}; neighbor = {0, 1, 0};
  ASSERT_EQ(5, graph.VolumeIndexToCapacityIndex(center, neighbor));

  center = {0,0,1}; neighbor = {0, 0, 0};
  ASSERT_EQ(6, graph.VolumeIndexToCapacityIndex(center, neighbor));

  center = {0,0,0}; neighbor = {0, 0, 1};
  ASSERT_EQ(7, graph.VolumeIndexToCapacityIndex(center, neighbor));
}

TEST(Graph, VolumeIndexToCapacityIndexStandardPoints) {
  typedef char TWeight;
  Graph<TWeight> graph(1,1,1,1,100);

  itk::Index<3> center, neighbor;

  center = {101,79,56}; neighbor = {100,79,56};
  ASSERT_EQ(2, graph.VolumeIndexToCapacityIndex(center, neighbor));
}

TEST(Graph, GetArrayIndexWorks) {
  int d = 3;
  
    typedef unsigned int PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image< PixelType, Dimension > ImageType;
    ImageType::Pointer image = ImageType::New();
    ImageType::IndexType start;
    start.Fill(0);
    ImageType::SizeType size;
    size.Fill(d);
    ImageType::RegionType region(start,size);
    image->SetRegions(region);
    image->Allocate();
    image->FillBuffer(0);
  
    typedef char TWeight;
    int width = d, height = d, depth = d;
    Graph<TWeight> graph(width, height, depth, 1,100);

    itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image,region);
    while(!imageIterator.IsAtEnd()) {
      auto index = imageIterator.GetIndex();
      EXPECT_EQ(
        index[0] + d*index[1] + d*d*index[2],
        graph.GetArrayIndex(index)
      );
      ++imageIterator;
    }
}

TEST(Graph, EmptyCutMaxFlow) {
  typedef char TWeight;
  int width = 2, height = 2, depth = 2;
  Graph<TWeight> graph(width, height, depth, 1,100);

  graph.SolveGraph();

  EXPECT_EQ(0, graph.GetMaxFlow());
}

TEST(Graph, EmptyCutReturnsBackgroundChar) {
  typedef unsigned int PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start;
  start.Fill(0);
  ImageType::SizeType size;
  size.Fill(2);
  ImageType::RegionType region(start,size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  typedef char TWeight;
  int width = 2, height = 2, depth = 2;
  Graph<TWeight> graph(width, height, depth, 1, 100);

  graph.SolveGraph();

  EXPECT_EQ(0, graph.GetMaxFlow());

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image,region);
  while(!imageIterator.IsAtEnd()) {
    EXPECT_EQ(1, graph.GetSegmentation(imageIterator.GetIndex()));
    ++imageIterator;
  }
}

TEST(Graph, EmptyCutReturnsBackgroundFloat) {
  typedef unsigned int PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start;
  start.Fill(0);
  ImageType::SizeType size;
  size.Fill(2);
  ImageType::RegionType region(start,size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);


  typedef float TWeight;
  int width = 2, height = 2, depth = 2;
  Graph<TWeight> graph(width, height, depth, 1,100);

  graph.SolveGraph();

  EXPECT_EQ(0, graph.GetMaxFlow());

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image,region);
  while(!imageIterator.IsAtEnd()) {
    EXPECT_EQ(1, graph.GetSegmentation(imageIterator.GetIndex()));
    ++imageIterator;
  }
}

TEST(Graph, CutAllForegroundChar) {
  int d = 3;

  typedef unsigned int PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start;
  start.Fill(0);
  ImageType::SizeType size;
  size.Fill(d);
  ImageType::RegionType region(start,size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  typedef char TWeight;
  int width = d, height = d, depth = d;
  Graph<TWeight> graph(width, height, depth, 1,100);

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image,region);

  for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator) {
    graph.AddTLink(
      imageIterator.GetIndex(),
      std::numeric_limits<TWeight>::max(),
      0
    );
  }

  graph.SolveGraph();

  EXPECT_EQ(0, graph.GetMaxFlow());

  for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator) {
    EXPECT_EQ(0, graph.GetSegmentation(imageIterator.GetIndex()));
  }
}

TEST(Graph, IsForegroundLabel) {
  int d = 3;
  
    typedef unsigned int PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image< PixelType, Dimension > ImageType;
    ImageType::Pointer image = ImageType::New();
    ImageType::IndexType start;
    start.Fill(0);
    ImageType::SizeType size;
    size.Fill(d);
    ImageType::RegionType region(start,size);
    image->SetRegions(region);
    image->Allocate();
    image->FillBuffer(0);
  
    typedef char TWeight;
    int width = d, height = d, depth = d;
    Graph<TWeight> graph(width, height, depth, 1,100);
  
    itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image,region);
  
    for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator) {
      graph.AddTLink(
        imageIterator.GetIndex(),
        std::numeric_limits<TWeight>::max(),
        0
      );
    }
  
    graph.SolveGraph();
  
    EXPECT_EQ(0, graph.GetMaxFlow());

    for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator) {
    EXPECT_TRUE(graph.IsForegroundLabel(graph.GetSegmentation(imageIterator.GetIndex())));
  }
}

TEST(Graph, TLinkCutOnly) {
  int d = 3;

  typedef unsigned int PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start;
  start.Fill(0);
  ImageType::SizeType size;
  size.Fill(d);
  ImageType::RegionType region(start,size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  typedef char TWeight;
  int width = d, height = d, depth = d;
  Graph<TWeight> graph(width, height, depth, 1,100);

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image,region);
  TWeight K = std::numeric_limits<TWeight>::max();
  unsigned int nOdd = 0, nEven = 0;
  for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator) {
    auto index = imageIterator.GetIndex();
    unsigned int flatIndex = index[0] + index[1]*width + index[2]*width*height;

    if (flatIndex % 2 == 0) {// even
      graph.AddTLink(index, K, 0);
      ++nEven;
    } else { // odd
      graph.AddTLink(index, 0, K);
      ++nOdd;
    }
  }

  // Sanity check
  ASSERT_EQ(13, nOdd);
  ASSERT_EQ(14, nEven);

  graph.SolveGraph();

  EXPECT_EQ(0, graph.GetMaxFlow());

  nOdd = 0; nEven = 0;
  for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator) {
    auto index = imageIterator.GetIndex();
    unsigned int flatIndex = index[0] + index[1]*width + index[2]*width*height;

    if (flatIndex % 2 == 0) {// even
      EXPECT_EQ(0, graph.GetSegmentation(imageIterator.GetIndex()));
      ++nEven;
    } else {
      EXPECT_EQ(1, graph.GetSegmentation(imageIterator.GetIndex()));
      ++nOdd;
    }
  }

  // Sanity check
  ASSERT_EQ(13, nOdd);
  ASSERT_EQ(14, nEven);
}

TEST(Graph, NLinkCheck) {
  int d = 3;

  typedef unsigned int PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;
  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start;
  start.Fill(0);
  ImageType::SizeType size;
  size.Fill(d);
  ImageType::RegionType region(start,size);
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  typedef char TWeight;
  int width = d, height = d, depth = d;
  Graph<TWeight> graph(width, height, depth, 1,100);

  // Make all T links 0 and all pixels 100
  // Set the last pixel to 0 and add a T link to source
  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image,region);
  for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator) {
    imageIterator.Set(100);
  }
  imageIterator.GoToReverseBegin();
  imageIterator.Set(0);

  // Loop over, setup our boundary term alone
  typedef itk::ShapedNeighborhoodIterator<ImageType> IteratorType;
  IteratorType::RadiusType radius;
  radius.Fill(1);

  IteratorType iterator(radius, image, image->GetLargestPossibleRegion());
  IteratorType::OffsetType x = {{+1, 0, 0}};
  iterator.ActivateOffset(x);
  IteratorType::OffsetType y = {{0, +1, 0}};
  iterator.ActivateOffset(y);
  IteratorType::OffsetType z = {{0, 0, +1}};
  iterator.ActivateOffset(z);

  bool validPixel;
  unsigned int nNeighboursVisited = 0, nVoxelsVisited = 0, nHardLinks = 0;
  for( iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator ) {
    ++nVoxelsVisited;
    auto centerPixel = iterator.GetPixel({0, 0, 0});
    auto index = iterator.GetIndex();
    unsigned int flatIndex = index[0] + index[1]*width + index[2]*width*height;

    if (flatIndex == 0) {
      ASSERT_EQ(100, centerPixel) << "index is " << index << ", flatIndex is "<< flatIndex;
      graph.AddTLink(iterator.GetIndex(), std::numeric_limits<TWeight>::max(), 0);
      ++nHardLinks;
    } else if (flatIndex == d*d*d -1 ) {
      EXPECT_EQ(0, centerPixel) << "index is " << index << ", flatIndex is "<< flatIndex;
      graph.AddTLink(iterator.GetIndex(), 0, std::numeric_limits<TWeight>::max());
      ++nHardLinks;
    } else {
      graph.AddTLink(iterator.GetIndex(), 0, 0);
    }

    IteratorType::ConstIterator ci = iterator.Begin();
    while( !ci.IsAtEnd() ) {
      auto neighborPixel = iterator.GetPixel(ci.GetNeighborhoodOffset(), validPixel);
      if (validPixel) {
        TWeight d = std::numeric_limits<TWeight>::max() * exp( - pow(centerPixel - neighborPixel, 2) / 10*10);
        graph.AddNLink(
          iterator.GetIndex(),
          iterator.GetIndex() + ci.GetNeighborhoodOffset(),
          d,
          d
        );
        ++nNeighboursVisited;
      }
      ++ci;
    }
  }

  // Sanity checks
  EXPECT_EQ(d*d*d, nVoxelsVisited) << "Did not visit each voxel";
  EXPECT_EQ(54, nNeighboursVisited) << "Visited an incorrect number of neighbours"; // 54 only valid for d=3
  EXPECT_EQ(2, nHardLinks);

  graph.SolveGraph();

  EXPECT_EQ(0, graph.GetMaxFlow());

  for( iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator ) {
    auto index = iterator.GetIndex();
    unsigned int flatIndex = index[0] + index[1]*width + index[2]*width*height;
    if (flatIndex == d*d*d - 1){
      EXPECT_EQ(1, graph.GetSegmentation(iterator.GetIndex()));
    } else {
      EXPECT_EQ(0, graph.GetSegmentation(iterator.GetIndex()));
    }
  }
}
