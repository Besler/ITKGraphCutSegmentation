/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include "LabelImage.h"

LabelImage::LabelImage(ImageType::Pointer input) {
    m_connectedComponentFilter = ConnectedComponentImageFilterType::New();
    m_connectedComponentFilter->SetInput(input);

    // sort by size
    m_relabelComponentImageFilter = RelabelComponentImageFilterType::New();
    m_relabelComponentImageFilter->SetInput(m_connectedComponentFilter->GetOutput());
    m_relabelComponentImageFilter->Update();

}

LabelImage::ImageType::Pointer LabelImage::getLargestObject() {

    auto thresholdFilter = TresholdFilterType::New();
    thresholdFilter->SetInput(m_relabelComponentImageFilter->GetOutput());
    thresholdFilter->SetLowerThreshold(1);
    thresholdFilter->SetUpperThreshold(1);
    thresholdFilter->SetInsideValue(1);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->Update();

    return thresholdFilter->GetOutput();
}

LabelImage::ImageType::Pointer LabelImage::getAllButLargestObject() {

    auto thresholdFilter = TresholdFilterType::New();
    thresholdFilter->SetInput(m_relabelComponentImageFilter->GetOutput());
    thresholdFilter->SetLowerThreshold(2);
    thresholdFilter->SetUpperThreshold(255);
    thresholdFilter->SetInsideValue(1);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->Update();

    return thresholdFilter->GetOutput();
}