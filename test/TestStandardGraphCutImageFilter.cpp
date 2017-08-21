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
  typedef ::testing::Types<char, int, unsigned int> TestingLabelTypes;
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

      // Create image
      CreateLabelImage();
      CreateMaskImage();
      CreateGreyImage();

      // Create filter
      m_Filter = FilterType::New();
      m_Filter->SetInput(m_MaskImage);
      m_Filter->SetLabelInput(m_LabelImage);
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

      typename LabelImageType::IndexType ind;
      ind[0] = 0; ind[1] = 0; ind[2] = 0;
      m_LabelImage->SetPixel(ind, 0);
      ind[0] = m_Length-1; ind[1] = m_Length-1; ind[2] = m_Length-1;
      m_LabelImage->SetPixel(ind, 1);
    }

    void CreateMaskImage() {
      typename LabelImageType::IndexType start;
      start.Fill(0);
      typename LabelImageType::SizeType size;
      size.Fill(m_Length);
      typename LabelImageType::RegionType region(start,size);

      m_MaskImage = LabelImageType::New();
      m_MaskImage->SetRegions(region);
      m_MaskImage->Allocate();
      m_MaskImage->FillBuffer(1);
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
      ind[0] = 0; ind[1] = 0; ind[2] = 0;
      m_GreyImage->SetPixel(ind, 100);
    }

    FilterPointerType       m_Filter;
    LabelImagePointerType   m_LabelImage;
    LabelImagePointerType   m_MaskImage;
    GreyImagePointerType    m_GreyImage;
    LabelImagePointerType   m_OutputImage;
    unsigned int            m_Length;
    SizeType                m_Size;
    ImageIteratorType       m_Iterator;
  };
  
  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int> TestingLabelTypes;
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

  TYPED_TEST(StandardGraphCutImageFilterTest, SegmentationIsCorrect) {
    auto it = this->m_Iterator;
    while(!it.IsAtEnd()) {
      auto index = it.GetIndex();
      unsigned int flatIndex = index[0] + index[1] * this->m_Length + index[2] * this->m_Length * this->m_Length;
      if (flatIndex == this->m_Length*this->m_Length*this->m_Length - 1) {
        EXPECT_DOUBLE_EQ(1, it.Get());
      } else {
        EXPECT_DOUBLE_EQ(0, it.Get());
      }
      ++it;
    }
  }
} // namespace
