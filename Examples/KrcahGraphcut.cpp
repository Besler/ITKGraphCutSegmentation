/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

 #include "ImageGraphCut3DFilter.h"


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

/** This example segments an image and writes the segmentation mask to a file.
* It can operate on a 3D image with one component per dimension (i.e.
* grayscale).
*/
int main(int argc, char *argv[]) {
    // Verify arguments
    if (argc != 7) {
        std::cerr << "Required: image.mhd foregroundMask.mhd backgroundMask.mhd output.mhd sigma lambda" << std::endl;
        std::cerr << "image.mhd:           3D image sheetness measure -1.0 to 1.0" << std::endl;
        std::cerr << "foregroundMask.mhd:  3D image non-zero pixels indicating foreground and 0 elsewhere" << std::endl;
        std::cerr << "backgroundMask.mhd:  3D image non-zero pixels indicating background and 0 elsewhere" << std::endl;
        std::cerr << "output.mhd:          3D image resulting segmentation" << std::endl;
        std::cerr << "                     Foreground as 127 and Background as 255" << std::endl;
        std::cerr << "sigma                estimated noise in boundary term, try 0.2" << std::endl;
        std::cerr << "lambda               boundary term weight, try 5.0" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse arguments
    std::string imageFilename = argv[1];
    std::string foregroundFilename = argv[2];   // This image should have non-zero pixels indicating foreground pixels and 0 elsewhere.
    std::string backgroundFilename = argv[3];   // This image should have non-zero pixels indicating background pixels and 0 elsewhere.
    std::string outputFilename = argv[4];
    double sigma = atof(argv[5]);               // Noise parameter
    double lambda = atof(argv[6]);               // Boundary term weight

    // Print arguments
    std::cout << "imageFilename: " << imageFilename << std::endl
            << "foregroundFilename: " << foregroundFilename << std::endl
            << "backgroundFilename: " << backgroundFilename << std::endl
            << "outputFilename: " << outputFilename << std::endl
            << "sigma: " << sigma << std::endl
            << "lambda: " << lambda << std::endl;

    // Define all image types
    typedef itk::Image<float, 3> ImageType;
    typedef itk::Image<unsigned char, 3> ForegroundMaskType;
    typedef itk::Image<unsigned char, 3> BackgroundMaskType;
    typedef itk::Image<unsigned char, 3> OutputImageType;

    // Read the image
    std::cout << "*** Reading image ***" << std::endl;
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(imageFilename);

    // Read the foreground and background masks
    std::cout << "*** Reading foreground mask ***" << std::endl;
    typedef itk::ImageFileReader<ForegroundMaskType> ForegroundMaskReaderType;
    ForegroundMaskReaderType::Pointer foregroundMaskReader = ForegroundMaskReaderType::New();
    foregroundMaskReader->SetFileName(foregroundFilename);

    std::cout << "*** Reading background mask ***" << std::endl;
    typedef itk::ImageFileReader<BackgroundMaskType> BackgroundMaskReaderType;
    BackgroundMaskReaderType::Pointer backgroundMaskReader = BackgroundMaskReaderType::New();
    backgroundMaskReader->SetFileName(backgroundFilename);

    // Set up the graph cut
    typedef itk::ImageGraphCut3DFilter<ImageType, ForegroundMaskType, BackgroundMaskType, OutputImageType> GraphCutFilterType;
    GraphCutFilterType::Pointer graphCutFilter = GraphCutFilterType::New();
    graphCutFilter->SetInputImage(reader->GetOutput());
    graphCutFilter->SetForegroundImage(foregroundMaskReader->GetOutput());
    graphCutFilter->SetBackgroundImage(backgroundMaskReader->GetOutput());

    // Set graph cut parameters
    graphCutFilter->SetVerboseOutput(true);
    graphCutFilter->SetSigma(sigma);
    graphCutFilter->SetBoundaryDirectionTypeToBrightDark();
    graphCutFilter->SetLambda(lambda);

    // Define the color values of the output
    graphCutFilter->SetForegroundPixelValue(1);
    graphCutFilter->SetBackgroundPixelValue(0);

    // Start the computation
    std::cout << "*** Performing Graph Cut ***" << std::endl;
    graphCutFilter->Update();

    // Get and write the result
    std::cout << "*** Writing Result ***" << std::endl;
    typedef itk::ImageFileWriter<GraphCutFilterType::OutputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(graphCutFilter->GetOutput());
    try {
        std::cout << "Writing output image " << outputFilename << std::endl;
        writer->Update();
    }
    catch (itk::ExceptionObject &err) {
        std::cerr << "ERROR: Exception caught while writing output image" << std::endl;
        std::cerr << err << std::endl;
    }
}
