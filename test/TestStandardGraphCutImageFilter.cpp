#include <gtest/gtest.h>
#include "StandardGraphCutImageFilter.h"
#include "itkImageRegionIterator.h"

namespace {
 // We can test setters and getters quickly in their own fixture
 template <typename T>
 class StandardGraphCutImageFilterTestSettersAndGetters : public ::testing::Test {
 protected:
   typedef T                                                                                               LabelPixelType;
   typedef short                                                                                           GreyPixelType;
   static const unsigned int                                                                               DIMENSIONS = 3;
   typedef typename itk::Image<GreyPixelType, DIMENSIONS>                                                  GreyImageType;
   typedef typename GreyImageType::Pointer                                                                 GreyImagePointerType;
   typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
   typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
   typedef itk::StandardGraphCutImageFilter<LabelImageType, GreyImageType, LabelImageType, LabelPixelType> FilterType;
   typedef typename FilterType::Pointer                                                                    FilterPointerType;

   StandardGraphCutImageFilterTestSettersAndGetters() {
     m_Filter = FilterType::New();
   }
 
   virtual ~StandardGraphCutImageFilterTestSettersAndGetters() {
     // Nothing to do
   }

   FilterPointerType     m_Filter;
 };

  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(StandardGraphCutImageFilterTestSettersAndGetters, TestingLabelTypes);

  TYPED_TEST(StandardGraphCutImageFilterTestSettersAndGetters, SigmaDefault) {
    EXPECT_DOUBLE_EQ(0.1, this->m_Filter->GetSigma());
  }

  TYPED_TEST(StandardGraphCutImageFilterTestSettersAndGetters, SigmaSetter) {
    this->m_Filter->SetSigma(100);
    EXPECT_DOUBLE_EQ(100, this->m_Filter->GetSigma());
  }
} // namespace


namespace {
  // The fixture for testing class StandardGraphCutImageFilterTest.
  template <typename T>
  class StandardGraphCutImageFilterTest : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           GreyPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<GreyPixelType, DIMENSIONS>                                                  GreyImageType;
    typedef typename GreyImageType::Pointer                                                                 GreyImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::StandardGraphCutImageFilter<LabelImageType, GreyImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    StandardGraphCutImageFilterTest() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 1;
      m_BackgroundValue = 2;

      // Create image
      CreateLabelImage();
      CreateGreyImage();

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetGreyInput(m_GreyImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~StandardGraphCutImageFilterTest() {
      // Nothing to do
    }

    void CreateLabelImage() {
      typename LabelImageType::IndexType start;
      start.Fill(0);
      typename LabelImageType::SizeType size;
      size.Fill(m_Length);
      typename LabelImageType::RegionType region(start,size);

      m_LabelImage = LabelImageType::New();
      m_LabelImage->SetRegions(region);
      m_LabelImage->Allocate();
      m_LabelImage->FillBuffer(0);
    }

    void CreateGreyImage() {
      typename GreyImageType::IndexType start;
      start.Fill(0);
      typename GreyImageType::SizeType size;
      size.Fill(m_Length);
      typename GreyImageType::RegionType region(start,size);

      m_GreyImage = GreyImageType::New();
      m_GreyImage->SetRegions(region);
      m_GreyImage->Allocate();
      m_GreyImage->FillBuffer(0);
    }

    FilterPointerType       m_Filter;
    LabelImagePointerType   m_LabelImage;
    GreyImagePointerType    m_GreyImage;
    LabelImagePointerType   m_OutputImage;
    unsigned int            m_Length;
    SizeType                m_Size;
    ImageIteratorType       m_Iterator;
    LabelPixelType          m_ForegroundValue;
    LabelPixelType          m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(StandardGraphCutImageFilterTest, TestingLabelTypes);

  TYPED_TEST(StandardGraphCutImageFilterTest, SigmaIsDefault) {
    ASSERT_EQ(0.1, this->m_Filter->GetSigma());
  }

  TYPED_TEST(StandardGraphCutImageFilterTest, OutputPointerIsNotNull) {
    EXPECT_TRUE(this->m_OutputImage);
  }

  TYPED_TEST(StandardGraphCutImageFilterTest, OutputHasCorrectSize) {
    EXPECT_DOUBLE_EQ(this->m_Length, this->m_Size[0]);
    EXPECT_DOUBLE_EQ(this->m_Length, this->m_Size[1]);
    EXPECT_DOUBLE_EQ(this->m_Length, this->m_Size[2]);
  }
} // namespace

namespace {
  // The fixture for testing class StandardGraphCutImageFilterTestTLinks.
  template <typename T>
  class StandardGraphCutImageFilterTestTLinks : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           GreyPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<GreyPixelType, DIMENSIONS>                                                  GreyImageType;
    typedef typename GreyImageType::Pointer                                                                 GreyImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::StandardGraphCutImageFilter<LabelImageType, GreyImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    StandardGraphCutImageFilterTestTLinks() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 1;
      m_BackgroundValue = 2;

      // Create image
      CreateLabelImage(); // First voxel background, last voxel background, the rest foreground
      CreateGreyImage();  // All voxels 0

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetGreyInput(m_GreyImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~StandardGraphCutImageFilterTestTLinks() {
      // Nothing to do
    }

    void CreateLabelImage() {
      typename LabelImageType::IndexType start;
      start.Fill(0);
      typename LabelImageType::SizeType size;
      size.Fill(m_Length);
      typename LabelImageType::RegionType region(start,size);

      m_LabelImage = LabelImageType::New();
      m_LabelImage->SetRegions(region);
      m_LabelImage->Allocate();
      m_LabelImage->FillBuffer(m_ForegroundValue);

      typename LabelImageType::IndexType ind;
      ind[0] = 0; ind[1] = 0; ind[2] = 0;
      m_LabelImage->SetPixel(ind, m_BackgroundValue);
      ind[0] = m_Length-1; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_LabelImage->SetPixel(ind, m_BackgroundValue);
    }

    void CreateGreyImage() {
      typename GreyImageType::IndexType start;
      start.Fill(0);
      typename GreyImageType::SizeType size;
      size.Fill(m_Length);
      typename GreyImageType::RegionType region(start,size);

      m_GreyImage = GreyImageType::New();
      m_GreyImage->SetRegions(region);
      m_GreyImage->Allocate();
      m_GreyImage->FillBuffer(0);
    }

    FilterPointerType       m_Filter;
    LabelImagePointerType   m_LabelImage;
    GreyImagePointerType    m_GreyImage;
    LabelImagePointerType   m_OutputImage;
    unsigned int            m_Length;
    SizeType                m_Size;
    ImageIteratorType       m_Iterator;
    LabelPixelType          m_ForegroundValue;
    LabelPixelType          m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(StandardGraphCutImageFilterTestTLinks, TestingLabelTypes);

  TYPED_TEST(StandardGraphCutImageFilterTestTLinks, MaxFlowIsZero) {
    // We have to cut the N links for the two background pixels
    // The background pixels are at the corner of the image, so they each have three N links
    // Each N link is 1
    EXPECT_DOUBLE_EQ(6, this->m_Filter->GetMaxFlow());
  }

  TYPED_TEST(StandardGraphCutImageFilterTestTLinks, SegmentationIsCorrect) {
    auto it = this->m_Iterator;
    while(!it.IsAtEnd()) {
      auto index = it.GetIndex();
      unsigned int flatIndex = index[0] + index[1] * this->m_Length + index[2] * this->m_Length * this->m_Length;
      if ( (flatIndex == this->m_Length*this->m_Length*this->m_Length - 1 )
        || (flatIndex == 0) ) {
        EXPECT_DOUBLE_EQ(this->m_BackgroundValue, it.Get()) << " at index " << index;
      } else {
        EXPECT_DOUBLE_EQ(this->m_ForegroundValue, it.Get()) << " at index " << index;
      }
      ++it;
    }
  }
} // namespace

namespace {
  // The fixture for testing class StandardGraphCutImageFilterTestNLinks.
  template <typename T>
  class StandardGraphCutImageFilterTestNLinks : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           GreyPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<GreyPixelType, DIMENSIONS>                                                  GreyImageType;
    typedef typename GreyImageType::Pointer                                                                 GreyImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::StandardGraphCutImageFilter<LabelImageType, GreyImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    StandardGraphCutImageFilterTestNLinks() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 1;
      m_BackgroundValue = 2;

      // Create image
      CreateLabelImage(); // First voxel background, last voxel background, the rest foreground
      CreateGreyImage();  // All voxels 0

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetGreyInput(m_GreyImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~StandardGraphCutImageFilterTestNLinks() {
      // Nothing to do
    }

    void CreateLabelImage() {
      typename LabelImageType::IndexType start;
      start.Fill(0);
      typename LabelImageType::SizeType size;
      size.Fill(m_Length);
      typename LabelImageType::RegionType region(start,size);

      m_LabelImage = LabelImageType::New();
      m_LabelImage->SetRegions(region);
      m_LabelImage->Allocate();
      m_LabelImage->FillBuffer(0);

      typename LabelImageType::IndexType ind;
      ind[0] = 0; ind[1] = 0; ind[2] = 0;
      m_LabelImage->SetPixel(ind, m_BackgroundValue);
      ind[0] = m_Length-1; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_LabelImage->SetPixel(ind, m_ForegroundValue);
    }

    void CreateGreyImage() {
      typename GreyImageType::IndexType start;
      start.Fill(0);
      typename GreyImageType::SizeType size;
      size.Fill(m_Length);
      typename GreyImageType::RegionType region(start,size);

      m_GreyImage = GreyImageType::New();
      m_GreyImage->SetRegions(region);
      m_GreyImage->Allocate();
      m_GreyImage->FillBuffer(0);

      typename GreyImageType::IndexType ind;
      ind[0] = m_Length-1; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_GreyImage->SetPixel(ind, 100);
      ind[0] = m_Length-2; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_GreyImage->SetPixel(ind, 100);
    }

    FilterPointerType       m_Filter;
    LabelImagePointerType   m_LabelImage;
    GreyImagePointerType    m_GreyImage;
    LabelImagePointerType   m_OutputImage;
    unsigned int            m_Length;
    SizeType                m_Size;
    ImageIteratorType       m_Iterator;
    LabelPixelType          m_ForegroundValue;
    LabelPixelType          m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(StandardGraphCutImageFilterTestNLinks, TestingLabelTypes);

  TYPED_TEST(StandardGraphCutImageFilterTestNLinks, MaxFlowIsZero) {
    // There exists a perfect cut in this case
    EXPECT_DOUBLE_EQ(0, this->m_Filter->GetMaxFlow());
  }

  TYPED_TEST(StandardGraphCutImageFilterTestNLinks, SegmentationIsCorrect) {
    auto it = this->m_Iterator;
    while(!it.IsAtEnd()) {
      auto index = it.GetIndex();
      if ( (index[0]==this->m_Length - 1 && index[1]==this->m_Length - 1  && index[2]==this->m_Length - 1 )
        || (index[0]==this->m_Length - 2 && index[1]==this->m_Length - 1  && index[2]==this->m_Length - 1) ) {
        EXPECT_DOUBLE_EQ(this->m_ForegroundValue, it.Get()) << " at index " << index;
      } else {//m_BackgroundValue,m_ForegroundValue
        EXPECT_DOUBLE_EQ(this->m_BackgroundValue, it.Get()) << " at index " << index;
      }
      ++it;
    }
  }
} // namespace
