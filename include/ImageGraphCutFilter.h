/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageGraphCutFilter_h_
#define __ImageGraphCutFilter_h_

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

// STL
#include <vector>

namespace itk {
    template<typename TInputImage, typename TOutputImage, typename TWeight = unsigned char>
    class ITK_EXPORT ImageGraphCut3DFilter : public ImageToImageFilter< TInputImage, TOutputImage > {
    public:
        /** Standard class typedefs. **/
        typedef ImageGraphCut3DFilter                                 Self;
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

        /** Iterator typedefs **/
        typedef ImageRegionConstIteratorWithIndex<InputImageType> InputImageIteratorType;
        typedef ConstShapedNeighborhoodIterator<InputImageType>   InputImageNeighborhoodIteratorType;
        typedef ImageRegionIteratorWithIndex<OutputImageType>     OutputImageIteratorType;
        typedef InputImageNeighborhoodIteratorType::RadiusType    InputImageNeighborhoodIteratorRadiusType;
        typedef InputImageNeighborhoodIteratorType::IndexType     InputIndexType;

        /** This is a base abstract class. Cannot instantiate. */
        itkNewMacro(Self); // TODO: Verify if this is correct

        /** Runtime information support. */
        itkTypeMacro(ImageGraphCut3DFilter, ImageToImageFilter);

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

        InputImageNeighborhoodIteratorRadiusType GetRadius();

        itkSetMacro(IgnoreMaskValue, InputImagePixelType);
        itkGetMacro(IgnoreMaskValue, OutputImagePixelType);
        itkSetMacro(BackgroundLabel, OutputImagePixelType);
        itkGetMacro(BackgroundLabel, OutputImagePixelType);
        itkSetMacro(ForegroundLabel, OutputImagePixelType);
        itkGetMacro(ForegroundLabel, OutputImagePixelType);

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
        // /** Image container for straightforward access **/
        // struct ImageContainer {
        //     typename InputImageConstPointer input;
        //     typename InputImageRegionType   inputRegion;
        //     typename OutputImagePointer     output;
        //     typename OutputImageRegionType  outputRegion; // TODO: Was InputImageType::RegionType. Check
        // };

        ImageGraphCut3DFilter();
        virtual ~ImageGraphCut3DFilter();
        void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;
        void GenerateData() ITK_OVERRIDE;

        /* TODO: See if we can enable streaming... We should at least only need the input region to be largest*/
        virtual void EnlargeOutputRequestedRegion(DataObject *output) ITK_OVERRIDE;
        /* TODO: Move the neighbourhood iterator to threaded */
        // virtual void BeforeThreadedGenerateData() ITK_OVERRIDE;
        // virtual void AfterThreadedGenerateData() ITK_OVERRIDE;
        
        /** Implemented by subclasses per-library **/
        virtual void InitializeGraph() = 0;
        virtual void SolveGraph() = 0;
        virtual void GetGraphCutSegmentation(VertexDescriptorType nodeID) = 0;
        virtual void DestroyGraph() = 0;

        virtual typename InputImageNeighborhoodIteratorType GetIterator() = 0;
        virtual void AddNLink(VertexDescriptorType nodeID1, VertexDescriptorType nodeID2, TWeight capacity, TWeight reverseCapacity) = 0;
        virtual void AddTLink(VertexDescriptorType nodeID, TWeight foregroundTLinkCapacity, TWeight backgroundTLinkCapacity) = 0;

        /** Determining edge weights further on **/
        virtual TWeight ComputeRegionalTerm(InputIndexType index, OutputImagePixelType label) = 0;
        virtual TWeight ComputeBoundaryTerm(InputIndexType firstIndex, InputIndexType secondIndex) = 0;

        /** convert itk indices to a continuously numbered indices **/
        VertexDescriptorType ConvertIndexToVertexDescriptor(const InputIndexType, InputImageRegionType);
    private:
        ITK_DISALLOW_COPY_AND_ASSIGN(ImageGraphCut3DFilter);

        bool                    m_VerboseOutput;
        bool                    m_DestroyGraph;
        InputImagePixelType     m_IgnoreMaskValue;
        OutputImagePixelType    m_BackgroundLabel;
        OutputImagePixelType    m_ForegroundLabel;
    }; /* end class ImageGraphCut3DFilter */
} /* end namespace itk */

#ifndef ITK_MANUAL_INSTANTIATION
#include "ImageGraphCutFilter.hxx"
#endif /* ITK_MANUAL_INSTANTIATION */

#endif /* __ImageGraphCutFilter_h_ */