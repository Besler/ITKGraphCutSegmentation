#include <gtest/gtest.h>
#include "ModifiedSheetnessGraphCutImageFilter.h"
#include "itkImageRegionIterator.h"

namespace {
 // We can test setters and getters quickly in their own fixture
 template <typename T>
 class ModifiedSheetnessGraphCutImageFilterTestSettersAndGetters : public ::testing::Test {
 protected:
   typedef T                                                                                               LabelPixelType;
   typedef short                                                                                           SheetnessPixelType;
   static const unsigned int                                                                               DIMENSIONS = 3;
   typedef typename itk::Image<SheetnessPixelType, DIMENSIONS>                                                  SheetnessImageType;
   typedef typename SheetnessImageType::Pointer                                                                 SheetnessImagePointerType;
   typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
   typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
   typedef itk::ModifiedSheetnessGraphCutImageFilter<LabelImageType, SheetnessImageType, LabelImageType, LabelPixelType> FilterType;
   typedef typename FilterType::Pointer                                                                    FilterPointerType;

   ModifiedSheetnessGraphCutImageFilterTestSettersAndGetters() {
     m_Filter = FilterType::New();
   }
 
   virtual ~ModifiedSheetnessGraphCutImageFilterTestSettersAndGetters() {
     // Nothing to do
   }

   FilterPointerType     m_Filter;
 };

  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(ModifiedSheetnessGraphCutImageFilterTestSettersAndGetters, TestingLabelTypes);

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestSettersAndGetters, SigmaDefault) {
    EXPECT_DOUBLE_EQ(0.1, this->m_Filter->GetSigma());
  }

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestSettersAndGetters, SigmaSetter) {
    this->m_Filter->SetSigma(100);
    EXPECT_DOUBLE_EQ(100, this->m_Filter->GetSigma());
  }
} // namespace


namespace {
  // The fixture for testing class StandardGraphCutImageFilterTest.
  template <typename T>
  class ModifiedSheetnessGraphCutImageFilterTest : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           SheetnessPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<SheetnessPixelType, DIMENSIONS>                                                  SheetnessImageType;
    typedef typename SheetnessImageType::Pointer                                                                 SheetnessImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::ModifiedSheetnessGraphCutImageFilter<LabelImageType, SheetnessImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    ModifiedSheetnessGraphCutImageFilterTest() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 1;
      m_BackgroundValue = 2;

      // Create image
      CreateLabelImage();
      CreateSheetnessImage();

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetSheetnessInput(m_SheetnessImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~ModifiedSheetnessGraphCutImageFilterTest() {
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

    void CreateSheetnessImage() {
      typename SheetnessImageType::IndexType start;
      start.Fill(0);
      typename SheetnessImageType::SizeType size;
      size.Fill(m_Length);
      typename SheetnessImageType::RegionType region(start,size);

      m_SheetnessImage = SheetnessImageType::New();
      m_SheetnessImage->SetRegions(region);
      m_SheetnessImage->Allocate();
      m_SheetnessImage->FillBuffer(0);
    }

    FilterPointerType       m_Filter;
    LabelImagePointerType   m_LabelImage;
    SheetnessImagePointerType    m_SheetnessImage;
    LabelImagePointerType   m_OutputImage;
    unsigned int            m_Length;
    SizeType                m_Size;
    ImageIteratorType       m_Iterator;
    LabelPixelType          m_ForegroundValue;
    LabelPixelType          m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(ModifiedSheetnessGraphCutImageFilterTest, TestingLabelTypes);

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTest, SigmaIsDefault) {
    ASSERT_EQ(0.1, this->m_Filter->GetSigma());
  }

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTest, OutputPointerIsNotNull) {
    EXPECT_TRUE(this->m_OutputImage);
  }

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTest, OutputHasCorrectSize) {
    EXPECT_DOUBLE_EQ(this->m_Length, this->m_Size[0]);
    EXPECT_DOUBLE_EQ(this->m_Length, this->m_Size[1]);
    EXPECT_DOUBLE_EQ(this->m_Length, this->m_Size[2]);
  }
} // namespace

namespace {
  // The fixture for testing class StandardGraphCutImageFilterTestTLinks.
  template <typename T>
  class ModifiedSheetnessGraphCutImageFilterTestHardLinks : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           SheetnessPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<SheetnessPixelType, DIMENSIONS>                                                  SheetnessImageType;
    typedef typename SheetnessImageType::Pointer                                                                 SheetnessImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::ModifiedSheetnessGraphCutImageFilter<LabelImageType, SheetnessImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    ModifiedSheetnessGraphCutImageFilterTestHardLinks() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 1;
      m_BackgroundValue = 2;

      // Create image
      CreateLabelImage(); // First voxel background, last voxel background, the rest foreground
      CreateSheetnessImage();  // All voxels 0

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetSheetnessInput(m_SheetnessImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~ModifiedSheetnessGraphCutImageFilterTestHardLinks() {
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

    void CreateSheetnessImage() {
      typename SheetnessImageType::IndexType start;
      start.Fill(0);
      typename SheetnessImageType::SizeType size;
      size.Fill(m_Length);
      typename SheetnessImageType::RegionType region(start,size);

      m_SheetnessImage = SheetnessImageType::New();
      m_SheetnessImage->SetRegions(region);
      m_SheetnessImage->Allocate();
      m_SheetnessImage->FillBuffer(0);
    }

    FilterPointerType         m_Filter;
    LabelImagePointerType     m_LabelImage;
    SheetnessImagePointerType m_SheetnessImage;
    LabelImagePointerType     m_OutputImage;
    unsigned int              m_Length;
    SizeType                  m_Size;
    ImageIteratorType         m_Iterator;
    LabelPixelType            m_ForegroundValue;
    LabelPixelType            m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(ModifiedSheetnessGraphCutImageFilterTestHardLinks, TestingLabelTypes);

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestHardLinks, MaxFlowIsZero) {
    // We have to cut the N links for the two background pixels
    // The background pixels are at the corner of the image, so they each have three N links
    // Each N link is 1
    EXPECT_DOUBLE_EQ(6 * static_cast<TypeParam>((itk::NumericTraits<TypeParam>::max()-1)/6.0 - 1), this->m_Filter->GetMaxFlow());
  }

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestHardLinks, SegmentationIsCorrect) {
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
  // The fixture for testing class ModifiedSheetnessGraphCutImageFilterTestHardLinksDifferentForegroundBackground.
  template <typename T>
  class ModifiedSheetnessGraphCutImageFilterTestHardLinksDifferentForegroundBackground : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           SheetnessPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<SheetnessPixelType, DIMENSIONS>                                                  SheetnessImageType;
    typedef typename SheetnessImageType::Pointer                                                                 SheetnessImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::ModifiedSheetnessGraphCutImageFilter<LabelImageType, SheetnessImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    ModifiedSheetnessGraphCutImageFilterTestHardLinksDifferentForegroundBackground() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 2;
      m_BackgroundValue = 1;

      // Create image
      CreateLabelImage(); // First voxel background, last voxel background, the rest foreground
      CreateSheetnessImage();  // All voxels 0

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetSheetnessInput(m_SheetnessImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~ModifiedSheetnessGraphCutImageFilterTestHardLinksDifferentForegroundBackground() {
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

    void CreateSheetnessImage() {
      typename SheetnessImageType::IndexType start;
      start.Fill(0);
      typename SheetnessImageType::SizeType size;
      size.Fill(m_Length);
      typename SheetnessImageType::RegionType region(start,size);

      m_SheetnessImage = SheetnessImageType::New();
      m_SheetnessImage->SetRegions(region);
      m_SheetnessImage->Allocate();
      m_SheetnessImage->FillBuffer(0);
    }

    FilterPointerType         m_Filter;
    LabelImagePointerType     m_LabelImage;
    SheetnessImagePointerType m_SheetnessImage;
    LabelImagePointerType     m_OutputImage;
    unsigned int              m_Length;
    SizeType                  m_Size;
    ImageIteratorType         m_Iterator;
    LabelPixelType            m_ForegroundValue;
    LabelPixelType            m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(ModifiedSheetnessGraphCutImageFilterTestHardLinksDifferentForegroundBackground, TestingLabelTypes);

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestHardLinksDifferentForegroundBackground, MaxFlowIsZero) {
    // We have to cut the N links for the two background pixels
    // The background pixels are at the corner of the image, so they each have three N links
    // Each N link is 1
    EXPECT_DOUBLE_EQ(6 * static_cast<TypeParam>(((itk::NumericTraits<TypeParam>::max()-1)/6.0 - 1)), this->m_Filter->GetMaxFlow());
  }

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestHardLinksDifferentForegroundBackground, SegmentationIsCorrect) {
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
  // The fixture for testing class ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinks.
  template <typename T>
  class ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinks : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           SheetnessPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<SheetnessPixelType, DIMENSIONS>                                                  SheetnessImageType;
    typedef typename SheetnessImageType::Pointer                                                                 SheetnessImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::ModifiedSheetnessGraphCutImageFilter<LabelImageType, SheetnessImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinks() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 1;
      m_BackgroundValue = 2;

      // Create image
      CreateLabelImage(); // First voxel background, last voxel background, the rest foreground
      CreateSheetnessImage();  // All voxels 0

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetSheetnessInput(m_SheetnessImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinks() {
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

    void CreateSheetnessImage() {
      typename SheetnessImageType::IndexType start;
      start.Fill(0);
      typename SheetnessImageType::SizeType size;
      size.Fill(m_Length);
      typename SheetnessImageType::RegionType region(start,size);

      m_SheetnessImage = SheetnessImageType::New();
      m_SheetnessImage->SetRegions(region);
      m_SheetnessImage->Allocate();
      m_SheetnessImage->FillBuffer(0);

      typename SheetnessImageType::IndexType ind;
      ind[0] = m_Length-1; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_SheetnessImage->SetPixel(ind, 100);
      ind[0] = m_Length-2; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_SheetnessImage->SetPixel(ind, 100);
    }

    FilterPointerType         m_Filter;
    LabelImagePointerType     m_LabelImage;
    SheetnessImagePointerType m_SheetnessImage;
    LabelImagePointerType     m_OutputImage;
    unsigned int              m_Length;
    SizeType                  m_Size;
    ImageIteratorType         m_Iterator;
    LabelPixelType            m_ForegroundValue;
    LabelPixelType            m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinks, TestingLabelTypes);

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinks, MaxFlowIsZero) {
    // We have to cut the N links for the two background pixels
    // The background pixels are at the corner of the image, so they each have three N links
    // Each N link is 1
    EXPECT_DOUBLE_EQ(0, this->m_Filter->GetMaxFlow());
  }

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinks, SegmentationIsCorrect) {
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

namespace {
  // The fixture for testing class ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinksWithOtherMaskValues.
  template <typename T>
  class ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinksWithOtherMaskValues : public ::testing::Test {
  protected:
    typedef T                                                                                               LabelPixelType;
    typedef short                                                                                           SheetnessPixelType;
    static const unsigned int                                                                               DIMENSIONS = 3;
    typedef typename itk::Image<SheetnessPixelType, DIMENSIONS>                                                  SheetnessImageType;
    typedef typename SheetnessImageType::Pointer                                                                 SheetnessImagePointerType;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                                                 LabelImageType;
    typedef typename LabelImageType::Pointer                                                                LabelImagePointerType;
    typedef itk::ModifiedSheetnessGraphCutImageFilter<LabelImageType, SheetnessImageType, LabelImageType, LabelPixelType> FilterType;
    typedef typename FilterType::Pointer                                                                    FilterPointerType;
    typedef typename LabelImageType::RegionType                                                             RegionType;
    typedef typename RegionType::SizeType                                                                   SizeType;
    typedef itk::ImageRegionIterator<LabelImageType>                                                        ImageIteratorType;
    
    ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinksWithOtherMaskValues() {
      // Images are m_Length * m_Length * m_Length
      m_Length = 3;
      m_ForegroundValue = 2;
      m_BackgroundValue = 1;

      // Create image
      CreateLabelImage(); // First voxel background, last voxel background, the rest foreground
      CreateSheetnessImage();  // All voxels 0

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetBackgroundLabel(m_BackgroundValue);
      m_Filter->SetForegroundLabel(m_ForegroundValue);
      m_Filter->SetInput(m_LabelImage);
      m_Filter->SetSheetnessInput(m_SheetnessImage);

      // Update
      m_Filter->Update();
      m_OutputImage = m_Filter->GetOutput();

      // Get Size
      RegionType imageRegion = this->m_OutputImage->GetLargestPossibleRegion();
      m_Size = imageRegion.GetSize();

      // Get Iterator
      m_Iterator = ImageIteratorType(m_OutputImage, imageRegion);
    }
  
    virtual ~ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinksWithOtherMaskValues() {
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
      m_LabelImage->FillBuffer(m_BackgroundValue + m_ForegroundValue);

      typename LabelImageType::IndexType ind;
      ind[0] = 0; ind[1] = 0; ind[2] = 0;
      m_LabelImage->SetPixel(ind, m_BackgroundValue);
      ind[0] = m_Length-1; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_LabelImage->SetPixel(ind, m_ForegroundValue);
    }

    void CreateSheetnessImage() {
      typename SheetnessImageType::IndexType start;
      start.Fill(0);
      typename SheetnessImageType::SizeType size;
      size.Fill(m_Length);
      typename SheetnessImageType::RegionType region(start,size);

      m_SheetnessImage = SheetnessImageType::New();
      m_SheetnessImage->SetRegions(region);
      m_SheetnessImage->Allocate();
      m_SheetnessImage->FillBuffer(0);

      typename SheetnessImageType::IndexType ind;
      ind[0] = m_Length-1; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_SheetnessImage->SetPixel(ind, 50);
      ind[0] = m_Length-2; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_SheetnessImage->SetPixel(ind, 50);
      ind[0] = m_Length-3; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_SheetnessImage->SetPixel(ind, 100);
    }

    FilterPointerType         m_Filter;
    LabelImagePointerType     m_LabelImage;
    SheetnessImagePointerType m_SheetnessImage;
    LabelImagePointerType     m_OutputImage;
    unsigned int              m_Length;
    SizeType                  m_Size;
    ImageIteratorType         m_Iterator;
    LabelPixelType            m_ForegroundValue;
    LabelPixelType            m_BackgroundValue;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int, unsigned char> TestingLabelTypes;
  TYPED_TEST_CASE(ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinksWithOtherMaskValues, TestingLabelTypes);

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinksWithOtherMaskValues, MaxFlowIsZero) {
    // We have to cut the N links for the two background pixels
    // The background pixels are at the corner of the image, so they each have three N links
    // Each N link is 1
    EXPECT_DOUBLE_EQ(0, this->m_Filter->GetMaxFlow());
  }

  TYPED_TEST(ModifiedSheetnessGraphCutImageFilterTestBrightToDarkNLinksWithOtherMaskValues, SegmentationIsCorrect) {
    auto it = this->m_Iterator;
    while(!it.IsAtEnd()) {
      auto index = it.GetIndex();
      if ( (index[0]==this->m_Length - 1 && index[1]==this->m_Length - 1  && index[2]==this->m_Length - 1 )
      || (index[0]==this->m_Length - 2 && index[1]==this->m_Length - 1  && index[2]==this->m_Length - 1) 
      || (index[0]==this->m_Length - 3 && index[1]==this->m_Length - 1  && index[2]==this->m_Length - 1) ) {
        EXPECT_DOUBLE_EQ(this->m_ForegroundValue, it.Get()) << " at index " << index;
      } else {//m_BackgroundValue,m_ForegroundValue
        EXPECT_DOUBLE_EQ(this->m_BackgroundValue, it.Get()) << " at index " << index;
      }
      ++it;
    }
  }
} // namespace