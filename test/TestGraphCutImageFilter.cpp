#include <gtest/gtest.h>
#include "GraphCutImageFilter.h"
#include "vnl/vnl_math.h"
#include "itkNumericTraits.h"

namespace {
  // Implement a mock of the abstract base class to test methods
  template<typename TInputImage, typename TOutputImage, typename TWeight>  
  class GraphCutImageFilterMock : public itk::GraphCutImageFilter<TInputImage, TOutputImage, TWeight> {
  public:
    typedef GraphCutImageFilterMock                                      Self;
    typedef itk::GraphCutImageFilter<TInputImage, TOutputImage, TWeight> Superclass;
    typedef itk::SmartPointer< Self >                                    Pointer;
    typedef itk::SmartPointer< const Self >                              ConstPointer;

    itkNewMacro(Self);
  protected:
    virtual TWeight ComputeRegionalTerm(typename Superclass::InputIndexType index, typename Superclass::OutputImagePixelType label) ITK_OVERRIDE {return TWeight(0);} 

    virtual TWeight ComputeBoundaryTerm(typename Superclass::InputIndexType firstIndex, typename Superclass::InputIndexType secondIndex) ITK_OVERRIDE {return TWeight(0);}
  }; // class itk::GraphCutImageFilterMock

  // The fixture for testing class GraphCutImageFilterTest
  template <typename T>
  class GraphCutImageFilterTest : public ::testing::Test {
  protected:
    typedef T                                                                         LabelPixelType;
    typedef unsigned int                                                              WeightType;
    static const unsigned int                                                         DIMENSIONS = 3;
    typedef typename itk::Image<LabelPixelType, DIMENSIONS>                           LabelImageType;
    typedef typename LabelImageType::Pointer                                          LabelImagePointerType;
    typedef GraphCutImageFilterMock<LabelImageType, LabelImageType, WeightType>  FilterType;
    typedef typename FilterType::Pointer                                              FilterPointerType;

    GraphCutImageFilterTest() {
      m_Filter = FilterType::New();
    }
  
    virtual ~GraphCutImageFilterTest() {
      // Nothing to do
    }

    FilterPointerType     m_Filter;
  };

  // Define the templates we would like to test
  typedef ::testing::Types<char, int, unsigned int> TestingTypes;
  TYPED_TEST_CASE(GraphCutImageFilterTest, TestingTypes);

  TYPED_TEST(GraphCutImageFilterTest, VerboseOutputDefault) {
    EXPECT_FALSE(this->m_Filter->GetVerboseOutput());
  }

  TYPED_TEST(GraphCutImageFilterTest, VerboseOutputSetterTrue) {
    this->m_Filter->SetVerboseOutput(true);
    EXPECT_TRUE(this->m_Filter->GetVerboseOutput());
  }

  TYPED_TEST(GraphCutImageFilterTest, VerboseOutputSetterFalse) {
    this->m_Filter->SetVerboseOutput(false);
    EXPECT_FALSE(this->m_Filter->GetVerboseOutput());
  }

  TYPED_TEST(GraphCutImageFilterTest, VerboseOutputSetterOn) {
    this->m_Filter->VerboseOutputOn();
    EXPECT_TRUE(this->m_Filter->GetVerboseOutput());
  }

  TYPED_TEST(GraphCutImageFilterTest, VerboseOutputSetterOff) {
    this->m_Filter->VerboseOutputOff();
    EXPECT_FALSE(this->m_Filter->GetVerboseOutput());
  }

  TYPED_TEST(GraphCutImageFilterTest, DestroyGraphDefault) {
    EXPECT_FALSE(this->m_Filter->GetDestroyGraph());
  }

  TYPED_TEST(GraphCutImageFilterTest, DestroyGraphSetterTrue) {
    this->m_Filter->SetDestroyGraph(true);
    EXPECT_TRUE(this->m_Filter->GetDestroyGraph());
  }

  TYPED_TEST(GraphCutImageFilterTest, DestroyGraphSetterFalse) {
    this->m_Filter->SetDestroyGraph(false);
    EXPECT_FALSE(this->m_Filter->GetDestroyGraph());
  }

  TYPED_TEST(GraphCutImageFilterTest, DestroyGraphSetterOn) {
    this->m_Filter->DestroyGraphOn();
    EXPECT_TRUE(this->m_Filter->GetDestroyGraph());
  }

  TYPED_TEST(GraphCutImageFilterTest, DestroyGraphSetterOff) {
    this->m_Filter->DestroyGraphOff();
    EXPECT_FALSE(this->m_Filter->GetDestroyGraph());
  }

  TYPED_TEST(GraphCutImageFilterTest, IgnoreMaskValueDefault) {
    EXPECT_DOUBLE_EQ(0, this->m_Filter->GetIgnoreMaskValue());
  }

  TYPED_TEST(GraphCutImageFilterTest, IgnoreMaskValueSetter) {
    this->m_Filter->SetIgnoreMaskValue(100);
    EXPECT_DOUBLE_EQ(100, this->m_Filter->GetIgnoreMaskValue());
  }

  TYPED_TEST(GraphCutImageFilterTest, BackgroundLabelDefault) {
    EXPECT_DOUBLE_EQ(0, this->m_Filter->GetBackgroundLabel());
  }

  TYPED_TEST(GraphCutImageFilterTest, BackgroundLabelSetter) {
    this->m_Filter->SetBackgroundLabel(100);
    EXPECT_DOUBLE_EQ(100, this->m_Filter->GetBackgroundLabel());
  }

  TYPED_TEST(GraphCutImageFilterTest, ForegroundLabelDefault) {
    EXPECT_DOUBLE_EQ(1, this->m_Filter->GetForegroundLabel());
  }

  TYPED_TEST(GraphCutImageFilterTest, ForegroundLabelSetter) {
    this->m_Filter->SetForegroundLabel(100);
    EXPECT_DOUBLE_EQ(100, this->m_Filter->GetForegroundLabel());
  }

  TYPED_TEST(GraphCutImageFilterTest, MaxFlowDefault) {
    EXPECT_DOUBLE_EQ(0, this->m_Filter->GetMaxFlow());
  }
}  // namespace