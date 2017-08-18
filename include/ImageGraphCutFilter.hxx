/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageGraphCutFilter_hxx_
#define __ImageGraphCutFilter_hxx_

#include "itkTimeProbesCollectorBase.h"

namespace itk {
    template<typename TInputImage, typename TOutputImage, typename TWeight>
    ImageGraphCut3DFilter<TInputImage, TOutputImage, TWeight>
    ::ImageGraphCut3DFilter()
            : m_VerboseOutput(false)
            , m_DestroyGraph(false)
            , m_IgnoreMaskValue(NumericTraits< InputImagePixelType >::ZeroValue())
            , m_BackgroundLabel(NumericTraits< OutputImagePixelType >::ZeroValue())
            , m_ForegroundLabel(NumericTraits< OutputImagePixelType >::OneValue())
    {
        this->SetNumberOfRequiredInputs(1);
    }

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    ImageGraphCut3DFilter<TInputImage, TOutputImage, TWeight>
    ::~ImageGraphCut3DFilter() {
    }

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    void ImageGraphCut3DFilter<TInputImage, TOutputImage, TWeight>
    ::GenerateData() {
        // Initialize the data
        typename InputImageConstPointer inputImage        = this->GetInputImage();
        typename InputImageRegionType   inputImageRegion  = inputImage->GetLargestPossibleRegion();
        typename OutputImagePointer     outputImage       = this->GetOutput();  
        typename OutputImageRegionType  outputImageRegion = outputImage->GetRequestedRegion();

        // Allocate output
        outputImage->SetBufferedRegion(outputImageRegion);
        outputImage->Allocate();

        // Ask derived class to initialize the representation of the graph
        // TODO: InitializeGraph needs access to number of verticies, number of edges, etc.
        InitializeGraph();

        /** Build the graph **/
        // https://itk.org/ITKExamples/src/Core/Common/IterateOverARegionWithAShapedNeighborhoodIterator/Documentation.html
        typename InputImageNeighborhoodIteratorType iterator = GetIterator();
        // TODO: Setup operators
        for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator) {
            // Grab the center pixel and see if it is in our VOI
            InputImageNeighborhoodIteratorType::PixelType currentPixelValue = iterator.Get();
            if (currentPixelValue == GetIgnoreMaskValue()) {
                continue;
            }

            // Compute the regional term, add T link
            InputImageNeighborhoodIteratorType::IndexType currentPixelIndex = iterator.GetIndex();
            TWeight foregroundTLinkCapacity = ComputeRegionalTerm(currentPixelIndex, GetForegroundLabel());
            TWeight backgroundTLinkCapacity = ComputeRegionalTerm(currentPixelIndex, GetBackgroundLabel());
            AddTLink(ConvertIndexToVertexDescriptor(currentPixelIndex), foregroundTLinkCapacity, backgroundTLinkCapacity);

            // Grab our neighborhood iterator, compute boundary term, add N links
            InputImageNeighborhoodIteratorType::ConstIterator neighborhoodIterator = iterator.Begin();
            while( !neighborhoodIterator.IsAtEnd() ) {
                // Continue outside of edge cases
                if (!neighborhoodIterator.InBounds()) {
                    continue;
                }
                
                // Grab index, compute boundary, add N link
                InputImageNeighborhoodIteratorType::IndexType neighborIndex = iterator.GetIndex() + neighborhoodIterator.GetNeighborhoodOffset();

                TWeight capacity = ComputeBoundaryTerm(currentPixelIndex, neighborIndex);
                TWeight reverseCapacity = ComputeBoundaryTerm(neighborIndex, currentPixelIndex);

                AddNLink(  ConvertIndexToVertexDescriptor(currentPixelIndex)
                          ,ConvertIndexToVertexDescriptor(neighborIndex)
                          ,capacity
                          ,reverseCapacity
                );

                ++neighborhoodIterator;
            } // while( !neighborhoodIterator.IsAtEnd() )
        } // for iterator

        /* Compute the min-cut/max-flow */
        SolveGraph();
        
        /* Fill output image */
        InputImageIteratorType      inputImageIterator(inputImage, outputImageRegion);
        OutputImageIteratorType     outputImageIterator(outputImage, outputImageRegion);

        for(inputImageIterator.GoToBegin(), outputImageIterator.GoToBegin(); !outputImageIterator.IsAtEnd(); ++inputImageIterator, ++outputImageIterator) {
            if (inputImageIterator.Get() == GetIgnoreMaskValue()) {
                outputImageIterator.Set(GetBackgroundLabel());
            } else {
                outputImageIterator.Set(GetGraphCutSegmentation(ConvertIndexToVertexDescriptor(inputImageIterator.GetIndex(), inputImageRegion)));
            }
        }

        /* Destroy Graoh */
        if (GetDestroyGraph()){
            DestroyGraph();
        }
    }

    // InputImageNeighborhoodIteratorType
    // InputImageNeighborhoodIteratorRadiusType GetRadius() {
    //     InputImageNeighborhoodIteratorRadiusType radius;
    //     radius.Fill(1); // Library only supports 6-26 connected
    //     return radius;
    // }
    // (GetRadius(), inputImage, inputImageRegion);

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    unsigned int ImageGraphCut3DFilter<TInputImage, TOutputImage, TWeight>
    ::EnlargeOutputRequestedRegion(DataObject *output)
    {
      // call the superclass' implementation of this method
      Superclass::EnlargeOutputRequestedRegion(output);
    
      // get pointer to the output
      OutputImagePointer outputPtr = this->GetOutput();
    
      // set the requested region to the largest possible region (in this case
      // the amount of data that we have)
      outputPtr->SetRequestedRegion( outputPtr->GetLargestPossibleRegion() );
    }

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    VertexDescriptorType ImageGraphCut3DFilter<TInputImage, TOutputImage, TWeight>
    ::ConvertIndexToVertexDescriptor(const InputIndexType index, InputImageRegionType region) {
        // We 'unroll' the index by the region. In 2D that looks like:
        //      vertexDescriptor = index[0] + index[1] * size[0]
        typename InputImageType::SizeType size = region.GetSize();
        VertexDescriptorType dimension = 1;
        VertexDescriptorType vertexDescriptor = 0;

        for (unsigned int i = 0; i < InputImageDimension; ++i){
            vertexDescriptor += index[i] * dimension;
            dimension *= size[i];
        }

        return VertexDescriptorType;
    }

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    void ImageGraphCut3DFilter<TInputImage, TOutputImage, TWeight>
    ::PrintSelf(std::ostream & os, Indent indent) const {
        Superclass::PrintSelf(os, indent);

        os << indent << "VerboseOutput: " << m_VerboseOutput << std::endl;
    }
}

#endif // __ImageGraphCutFilter_hxx_