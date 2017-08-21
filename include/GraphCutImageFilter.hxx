/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __GraphCutImageFilter_hxx_
#define __GraphCutImageFilter_hxx_

namespace itk {
    template<typename TInputImage, typename TOutputImage, typename TWeight>
    GraphCutImageFilter<TInputImage, TOutputImage, TWeight>
    ::GraphCutImageFilter()
            : m_VerboseOutput(false)
            , m_DestroyGraph(false)
            , m_IgnoreMaskValue(NumericTraits< InputImagePixelType >::ZeroValue())
            , m_BackgroundLabel(NumericTraits< OutputImagePixelType >::ZeroValue())
            , m_ForegroundLabel(NumericTraits< OutputImagePixelType >::OneValue())
    {
        this->SetNumberOfRequiredInputs(1);
    }

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    GraphCutImageFilter<TInputImage, TOutputImage, TWeight>
    ::~GraphCutImageFilter() {
    }

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    void GraphCutImageFilter<TInputImage, TOutputImage, TWeight>
    ::GenerateData() {
        // Initialize the data
        InputImageConstPointer inputImage        = this->GetInput();
        InputImageRegionType   inputImageRegion  = inputImage->GetLargestPossibleRegion();
        OutputImagePointer     outputImage       = this->GetOutput();  
        OutputImageRegionType  outputImageRegion = outputImage->GetRequestedRegion();

        // Allocate output
        outputImage->SetBufferedRegion(outputImageRegion);
        outputImage->Allocate();

        // Ask derived class to initialize the representation of the graph
        // TODO: InitializeGraph needs access to number of verticies, number of edges, etc.
        // InitializeGraph();
        typename InputImageType::SizeType inputImageSize = inputImageRegion.GetSize();
        std::unique_ptr<GraphType> graph = std::make_unique<GraphType>(
             inputImageSize[0], inputImageSize[1], inputImageSize[2]
            ,this->GetNumberOfThreads() , 100 // TODO: How to determine block_size?
        );

        /** Build the graph **/
        // https://itk.org/ITKExamples/src/Core/Common/IterateOverARegionWithAShapedNeighborhoodIterator/Documentation.html
        InputImageNeighborhoodIteratorRadiusType radius;
        radius.Fill(1); // Library only supports 6-26 connected
        InputImageNeighborhoodIteratorType iterator(radius, inputImage, inputImageRegion);
      
        typedef typename InputImageNeighborhoodIteratorType::OffsetType OffsetType;
        OffsetType x = {{+1, 0, 0}};
        iterator.ActivateOffset(x);
        OffsetType y = {{0, +1, 0}};
        iterator.ActivateOffset(y);
        OffsetType z = {{0, 0, +1}};
        iterator.ActivateOffset(z);

        bool pixelInRegion;
        for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator) {
            // Grab the center pixel and see if it is in our VOI
            typename InputImageNeighborhoodIteratorType::IndexType currentPixelIndex = iterator.GetIndex();
            InputImagePixelType currentPixelValue = iterator.GetPixel({0, 0, 0});
            if (currentPixelValue == GetIgnoreMaskValue()) {
                // Hard link to background
                graph->AddTLink(currentPixelIndex, NumericTraits<TWeight>::ZeroValue(), NumericTraits<TWeight>::max());
                continue;
            }

            // Compute the regional term, add T link
            TWeight foregroundTLinkCapacity = ComputeRegionalTerm(currentPixelIndex, GetForegroundLabel());
            TWeight backgroundTLinkCapacity = ComputeRegionalTerm(currentPixelIndex, GetBackgroundLabel());
            graph->AddTLink(currentPixelIndex, foregroundTLinkCapacity, backgroundTLinkCapacity);

            // Grab our neighborhood iterator, compute boundary term, add N links
            typename InputImageNeighborhoodIteratorType::ConstIterator neighborhoodIterator = iterator.Begin();
            while( !neighborhoodIterator.IsAtEnd() ) {
                // Continue outside of edge cases
                InputImagePixelType neighborPixel = iterator.GetPixel(neighborhoodIterator.GetNeighborhoodOffset(), pixelInRegion);
                if (!pixelInRegion) {
                    continue;
                }
                
                // Grab index, compute boundary, add N link
                typename InputImageNeighborhoodIteratorType::IndexType neighborIndex = iterator.GetIndex() + neighborhoodIterator.GetNeighborhoodOffset();

                TWeight capacity = ComputeBoundaryTerm(currentPixelIndex, neighborIndex);
                TWeight reverseCapacity = ComputeBoundaryTerm(neighborIndex, currentPixelIndex);

                graph->AddNLink(currentPixelIndex, neighborIndex, capacity, reverseCapacity);

                ++neighborhoodIterator;
            } // while( !neighborhoodIterator.IsAtEnd() )
        } // for iterator

        /* Compute the min-cut/max-flow */
        graph->SolveGraph();
        
        /* Fill output image */
        InputImageIteratorType      inputImageIterator(inputImage, outputImageRegion);
        OutputImageIteratorType     outputImageIterator(outputImage, outputImageRegion);

        for(inputImageIterator.GoToBegin(), outputImageIterator.GoToBegin(); !outputImageIterator.IsAtEnd(); ++inputImageIterator, ++outputImageIterator) {
            if (inputImageIterator.Get() == GetIgnoreMaskValue()) { // TODO: Do we need this or is uneccesary branching?
                outputImageIterator.Set(GetBackgroundLabel());
            } else {
                if (graph->IsForegroundLabel(graph->GetSegmentation(inputImageIterator.GetIndex()))) {
                    outputImageIterator.Set(GetForegroundLabel());
                } else {
                    outputImageIterator.Set(GetBackgroundLabel());
                }
            }
        }

        /* Destroy Graoh */
        // if (GetDestroyGraph()){
        //     DestroyGraph();
        // }
        // TODO: Graph falls out of scope and is cleaned up. Maybe we should keep it for fast editing...
    }

    template<typename TInputImage, typename TOutputImage, typename TWeight>
    void GraphCutImageFilter<TInputImage, TOutputImage, TWeight>
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
    void GraphCutImageFilter<TInputImage, TOutputImage, TWeight>
    ::PrintSelf(std::ostream & os, Indent indent) const {
        Superclass::PrintSelf(os, indent);

        os << indent << "VerboseOutput:   " << m_VerboseOutput   << std::endl
           << indent << "DestroyGraph:    " << m_DestroyGraph    << std::endl
           << indent << "IgnoreMaskValue: " << m_IgnoreMaskValue << std::endl
           << indent << "ForegroundLabel: " << m_ForegroundLabel << std::endl
           << indent << "BackgroundLabel: " << m_BackgroundLabel << std::endl;
    }
}

#endif // __GraphCutImageFilter_hxx_