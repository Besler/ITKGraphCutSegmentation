/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */


#ifndef __LabelImage_h_
#define __LabelImage_h_

// ITK
#include <itkImage.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkBinaryImageToLabelMapFilter.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelMapToLabelImageFilter.h>
#include <itkLabelSelectionLabelMapFilter.h>
#include <itkBinaryThresholdImageFilter.h>

class LabelImage {
public:
    typedef unsigned char PixelType;
    static const unsigned int Dimension = 3;

    typedef itk::Image<PixelType, Dimension> ImageType;

    typedef itk::ConnectedComponentImageFilter <ImageType, ImageType > ConnectedComponentImageFilterType;
    typedef itk::RelabelComponentImageFilter <ImageType, ImageType> RelabelComponentImageFilterType;
    typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> TresholdFilterType;

    LabelImage(ImageType::Pointer input);

    unsigned int getNumberOfObjects(){
        return m_connectedComponentFilter->GetObjectCount();
    }

    ImageType::Pointer getLargestObject();

    ImageType::Pointer getAllButLargestObject();

private:
    ConnectedComponentImageFilterType::Pointer m_connectedComponentFilter;
    RelabelComponentImageFilterType::Pointer m_relabelComponentImageFilter;
};

#endif