#ifndef __ModifiedSheetnessGraphCutImageFilter_hxx_
#define __ModifiedSheetnessGraphCutImageFilter_hxx_

namespace itk {
  template<typename TInputImage, typename TSheetnessImage, typename TOutputImage, typename TWeight>
  ModifiedSheetnessGraphCutImageFilter<TInputImage, TSheetnessImage, TOutputImage, TWeight>
  ::ModifiedSheetnessGraphCutImageFilter()
    : Superclass()
    , m_Sigma(0.1)
    , m_BoundaryDirection(1) // Cut bright-to-dark
  {
    this->SetNumberOfRequiredInputs(2);
  }

  template<typename TInputImage, typename TSheetnessImage, typename TOutputImage, typename TWeight>
  ModifiedSheetnessGraphCutImageFilter<TInputImage, TSheetnessImage, TOutputImage, TWeight>
  ::~ModifiedSheetnessGraphCutImageFilter()
  {}

    template<typename TInputImage, typename TSheetnessImage, typename TOutputImage, typename TWeight>
  TWeight ModifiedSheetnessGraphCutImageFilter<TInputImage, TSheetnessImage, TOutputImage, TWeight>
  ::ComputeRegionalTerm(InputIndexType index, LabelType label)  {
    // Grab input image
    InputImageConstPointerType labelImage = Superclass::GetInput();
    TWeight weight;

    // Hardlink labels
    if (labelImage->GetPixel(index) == Superclass::GetForegroundLabel()) {
      weight = (label == Superclass::GetForegroundLabel()) ? itk::NumericTraits<TWeight>::max() : (TWeight)0;
    } else if (labelImage->GetPixel(index) == Superclass::GetBackgroundLabel()) {
      weight = (label == Superclass::GetBackgroundLabel()) ? itk::NumericTraits<TWeight>::max() : (TWeight)0;
    } else {
      // We could add a lambda R(A) here.
      weight = (TWeight)0;
    }

    return static_cast<TWeight>(weight);
  }

  template<typename TInputImage, typename TSheetnessImage, typename TOutputImage, typename TWeight>
  TWeight ModifiedSheetnessGraphCutImageFilter<TInputImage, TSheetnessImage, TOutputImage, TWeight>
  ::ComputeBoundaryTerm(InputIndexType firstIndex, InputIndexType secondIndex) {
    SheetnessImageConstPointerType image      = GetSheetnessInput();
    SheetnessImagePixelType firstPixelValue   = image->GetPixel(firstIndex);
    SheetnessImagePixelType secondPixelValue  = image->GetPixel(secondIndex);

    double weight, difference;
    if (m_BoundaryDirection * (firstPixelValue - secondPixelValue) >= 0) {
      difference = firstPixelValue - secondPixelValue;
      weight = vcl_exp( - vcl_pow(difference, 2) / (2.0 * vcl_pow(m_Sigma, 2)) );
    } else {
      weight = 1;
    }

    return static_cast<TWeight>(weight);
  }

  template<typename TInputImage, typename TSheetnessImage, typename TOutputImage, typename TWeight>
  void ModifiedSheetnessGraphCutImageFilter<TInputImage, TSheetnessImage, TOutputImage, TWeight>
  ::PrintSelf(std::ostream & os, Indent indent) const {
      Superclass::PrintSelf(os, indent);
      os << indent << "Sigma:           " << m_Sigma << std::endl;
  }

} /* namespace itk */

#endif /* __ModifiedSheetnessGraphCutImageFilter_hxx_ */