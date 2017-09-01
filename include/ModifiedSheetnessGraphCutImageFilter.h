#ifndef ModifiedSheetnessGraphCutImageFilter_h
#define ModifiedSheetnessGraphCutImageFilter_h

#include "GraphCutImageFilter.h"

namespace itk {
  template<typename TInputImage, typename TSheetnessImage, typename TOutputImage, typename TWeight = unsigned char>
  class ModifiedSheetnessGraphCutImageFilter : public GraphCutImageFilter<TInputImage, TOutputImage, TWeight> {
  public:
    /** Standard class typedefs. **/
    typedef ModifiedSheetnessGraphCutImageFilter                    Self;
    typedef GraphCutImageFilter<TInputImage, TOutputImage, TWeight> Superclass;
    typedef SmartPointer< Self >                                    Pointer;
    typedef SmartPointer< const Self >                              ConstPointer;

    /* Useful typedefs */
    typedef TOutputImage                              InputImageType;
    typedef typename InputImageType::Pointer          InputImagePointerType;
    typedef typename InputImageType::ConstPointer     InputImageConstPointerType;
    typedef TSheetnessImage                           SheetnessImageType;
    typedef typename SheetnessImageType::PixelType    SheetnessImagePixelType;
    typedef typename SheetnessImageType::Pointer      SheetnessImagePointerType;
    typedef typename SheetnessImageType::ConstPointer SheetnessImageConstPointerType;
    typedef typename Superclass::InputIndexType       InputIndexType;
    typedef typename Superclass::OutputImagePixelType LabelType;

    itkNewMacro(Self);
    itkTypeMacro(ModifiedSheetnessGraphCutImageFilter, GraphCutImageFilter);

    /* Setter/Getter for sheetness image */
    void SetSheetnessInput(const SheetnessImageType * input) {
      // Process object is not const-correct so the const casting is required.
      this->SetNthInput(1, const_cast< SheetnessImageType * >( input ) );
    }

    const SheetnessImageType * GetSheetnessInput() const {
      return itkDynamicCastInDebugMode< SheetnessImageType * >( const_cast< DataObject * >( this->ProcessObject::GetInput(1) ) );
    }

    itkSetMacro(Sigma, double);
    itkGetMacro(Sigma, double);

    void CutBrightToDarkOn() {
      m_BoundaryDirection = 1;
    }

    void CutDarkToBrightOn() {
      m_BoundaryDirection = -1;
    }

    void CutNoDirection() {
      m_BoundaryDirection = 0;
    }

    void CutBrightToDark(bool brightToDark) {
      if (brightToDark) {
        CutBrightToDarkOn();
      } else {
        CutDarkToBrightOn();
      }
    }
  protected:
    ModifiedSheetnessGraphCutImageFilter();
    virtual ~ModifiedSheetnessGraphCutImageFilter();
    void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

    /* Regional Term */
    virtual TWeight ComputeRegionalTerm(InputIndexType index, LabelType label) ITK_OVERRIDE;

    /* Boundary Term */
    virtual TWeight ComputeBoundaryTerm(InputIndexType firstIndex, InputIndexType secondIndex) ITK_OVERRIDE;

  private:
    ITK_DISALLOW_COPY_AND_ASSIGN(ModifiedSheetnessGraphCutImageFilter);

    double  m_Sigma;
    double  m_NumericalScaling;
    int     m_BoundaryDirection;
    //TODO: add a boundary direction
  }; // class ModifiedSheetnessGraphCutImageFilter
} /* end namespace itk */

#ifndef ITK_MANUAL_INSTANTIATION
#include "ModifiedSheetnessGraphCutImageFilter.hxx"
#endif /* ITK_MANUAL_INSTANTIATION */

#endif /* ModifiedSheetnessGraphCutImageFilter_h */