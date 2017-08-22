#ifndef StandardGraphCutImageFilter_h
#define StandardGraphCutImageFilter_h

#include "GraphCutImageFilter.h"

namespace itk {
  template<typename TInputImage, typename TGreyImage, typename TOutputImage, typename TWeight = unsigned char>
  class StandardGraphCutImageFilter : public GraphCutImageFilter<TInputImage, TOutputImage, TWeight> {
  public:
    /** Standard class typedefs. **/
    typedef StandardGraphCutImageFilter                             Self;
    typedef GraphCutImageFilter<TInputImage, TOutputImage, TWeight> Superclass;
    typedef SmartPointer< Self >                                    Pointer;
    typedef SmartPointer< const Self >                              ConstPointer;

    /* Useful typedefs */
    typedef TOutputImage                              InputImageType;
    typedef typename InputImageType::Pointer          InputImagePointerType;
    typedef typename InputImageType::ConstPointer     InputImageConstPointerType;
    typedef TGreyImage                                GreyImageType;
    typedef typename GreyImageType::PixelType         GreyImagePixelType;
    typedef typename GreyImageType::Pointer           GreyImagePointerType;
    typedef typename GreyImageType::ConstPointer      GreyImageConstPointerType;
    typedef typename Superclass::InputIndexType       InputIndexType;
    typedef typename Superclass::OutputImagePixelType LabelType;

    itkNewMacro(Self);
    itkTypeMacro(StandardGraphCutImageFilter, GraphCutImageFilter);

    /* Setter/Getter for grey image */
    void SetGreyInput(const GreyImageType * input) {
      // Process object is not const-correct so the const casting is required.
      this->SetNthInput(1, const_cast< GreyImageType * >( input ) );
    }

    const GreyImageType * GetGreyInput() const {
      return itkDynamicCastInDebugMode< GreyImageType * >( const_cast< DataObject * >( this->ProcessObject::GetInput(1) ) );
    }

    itkSetMacro(Sigma, double);
    itkGetMacro(Sigma, double);
  protected:
    StandardGraphCutImageFilter();
    virtual ~StandardGraphCutImageFilter();
    void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

    /* Regional Term */
    virtual TWeight ComputeRegionalTerm(InputIndexType index, LabelType label) ITK_OVERRIDE;

    /* Boundary Term */
    virtual TWeight ComputeBoundaryTerm(InputIndexType firstIndex, InputIndexType secondIndex) ITK_OVERRIDE;

  private:
    ITK_DISALLOW_COPY_AND_ASSIGN(StandardGraphCutImageFilter);

    double  m_Sigma;
    double  m_WeightScale;
  }; // class StandardGraphCutImageFilter
} /* end namespace itk */

#ifndef ITK_MANUAL_INSTANTIATION
#include "StandardGraphCutImageFilter.hxx"
#endif /* ITK_MANUAL_INSTANTIATION */

#endif /* StandardGraphCutImageFilter_h */