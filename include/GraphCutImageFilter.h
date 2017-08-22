/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __GraphCutImageFilter_h_
#define __GraphCutImageFilter_h_

// ITK
#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include <itkImageRegionIteratorWithIndex.h>
#include "itkShapedNeighborhoodIterator.h"
#include "itkImage.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"
#include "itkListSample.h"
#include "itkProgressReporter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkNumericTraits.h"
#include "Graph.h"

// STL
#include <memory> // std::unique_ptr

namespace itk {
    template<typename TInputImage, typename TOutputImage, typename TWeight = unsigned char>
    class ITK_EXPORT GraphCutImageFilter : public ImageToImageFilter< TInputImage, TOutputImage > {
    public:
        /** Standard class typedefs. **/
        typedef GraphCutImageFilter                                   Self;
        typedef ImageToImageFilter< TInputImage, TOutputImage >       Superclass;
        typedef SmartPointer< Self >                                  Pointer;
        typedef SmartPointer< const Self >                            ConstPointer;

        /** Some convenient typedefs. **/
        typedef TInputImage                            InputImageType;
        typedef TOutputImage                           OutputImageType;
        typedef unsigned int                           VertexDescriptorType;
        typedef typename InputImageType::Pointer       InputImagePointer;
        typedef typename InputImageType::ConstPointer  InputImageConstPointer;
        typedef typename InputImageType::RegionType    InputImageRegionType;
        typedef typename InputImageType::PixelType     InputImagePixelType;
        typedef typename OutputImageType::Pointer      OutputImagePointer;
        typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
        typedef typename OutputImageType::RegionType   OutputImageRegionType;
        typedef typename OutputImageType::PixelType    OutputImagePixelType;
        typedef Graph<TWeight>                         GraphType;

        /** Iterator typedefs **/
        typedef ImageRegionConstIteratorWithIndex<InputImageType> InputImageIteratorType;
        typedef ConstShapedNeighborhoodIterator<InputImageType>   InputImageNeighborhoodIteratorType;
        typedef ImageRegionIteratorWithIndex<OutputImageType>     OutputImageIteratorType;

        typedef typename InputImageNeighborhoodIteratorType::RadiusType    InputImageNeighborhoodIteratorRadiusType;
        typedef typename InputImageNeighborhoodIteratorType::IndexType     InputIndexType;

        /** This is an abstract class. Cannot instantiate. */
        // itkNewMacro(Self); // Commented out because it is an abstract class

        /** Runtime information support. */
        itkTypeMacro(GraphCutImageFilter, ImageToImageFilter);

        /** ImageDimension constants */
        itkStaticConstMacro(InputImageDimension,  unsigned int, TInputImage::ImageDimension);
        itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

        /** Setters and Getters **/
        itkSetMacro(VerboseOutput, bool);
        itkGetConstReferenceMacro(VerboseOutput, bool);
        itkBooleanMacro(VerboseOutput);

        itkSetMacro(DestroyGraph, bool);
        itkGetConstReferenceMacro(DestroyGraph, bool);
        itkBooleanMacro(DestroyGraph);

        itkSetMacro(BackgroundLabel, OutputImagePixelType);
        itkGetMacro(BackgroundLabel, OutputImagePixelType);
        itkSetMacro(ForegroundLabel, OutputImagePixelType);
        itkGetMacro(ForegroundLabel, OutputImagePixelType);
        itkGetMacro(MaxFlow, TWeight);

#ifdef ITK_USE_CONCEPT_CHECKING
        itkConceptMacro( SameDimension,
            ( Concept::SameDimension< itkGetStaticConstMacro(InputImageDimension),
                                      itkGetStaticConstMacro(OutputImageDimension) > ) );
        itkConceptMacro( InputPixelConvertableToTWeight,
            ( Concept::Convertible< InputImagePixelType, TWeight > ) );
        itkConceptMacro( TWeightConvertableToOutputPixel,
            ( Concept::Convertible< TWeight, OutputImagePixelType > ) );
#endif

    protected:
        GraphCutImageFilter();
        virtual ~GraphCutImageFilter();
        void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;
        void GenerateData() ITK_OVERRIDE;

        /* TODO: See if we can enable streaming... We should at least only need the input region to be largest */
        virtual void EnlargeOutputRequestedRegion(DataObject *output) ITK_OVERRIDE;
        /* TODO: Move the neighbourhood iterator to threaded */
        // virtual void BeforeThreadedGenerateData() ITK_OVERRIDE;
        // virtual void AfterThreadedGenerateData() ITK_OVERRIDE;

        /** Determining edge weights further on **/
        virtual TWeight ComputeRegionalTerm(InputIndexType index, OutputImagePixelType label) = 0;
        virtual TWeight ComputeBoundaryTerm(InputIndexType firstIndex, InputIndexType secondIndex) = 0;
    private:
        ITK_DISALLOW_COPY_AND_ASSIGN(GraphCutImageFilter);

        bool                    m_VerboseOutput;
        bool                    m_DestroyGraph;
        OutputImagePixelType    m_BackgroundLabel;
        OutputImagePixelType    m_ForegroundLabel;
        TWeight                 m_MaxFlow;
    }; /* end class GraphCutImageFilter */
} /* end namespace itk */

#ifndef ITK_MANUAL_INSTANTIATION
#include "GraphCutImageFilter.hxx"
#endif /* ITK_MANUAL_INSTANTIATION */

#endif /* __GraphCutImageFilter_h_ */