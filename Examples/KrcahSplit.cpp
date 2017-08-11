/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include "ImageGraphCut3DFilter.h"
#include "LabelImage.h"


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

//global typedefs
typedef itk::Image<unsigned char, 3> ImageType;

//function
void writeImage(ImageType::Pointer image, std::string path);

//TODO: Make more practical by iterating through multiple objects in input and after graphcut
//TODO: Should output be a label image or multiple image masks?

/** This example picks the largest component of the input mask and tries to split it using erosion and graphcut.
*/
int main(int argc, char *argv[]) {
    // Verify arguments
    if (argc < 4) {
        std::cerr << "Required: image.mhd output.mhd radius debug_prefix(optional)" << std::endl;
        std::cerr << "image.mhd:           3D binary image mask, object  is 1, background is 0" << std::endl;
        std::cerr << "output.mhd:          3D binary image mask of largest split object" << std::endl;
        std::cerr << "radius               Radius of sphere for 3D binary erosion" << std::endl;
        std::cerr << "debug_prefix         (Optional) if present, intermediate images are saved with this prefix." << std::endl;
        return EXIT_FAILURE;
    }

    // Parse arguments
    std::string imageFilename = argv[1];
    std::string outputFilename = argv[2];
    int radius = atoi(argv[3]);               // Erosion sphere radius

    bool verbose = false;
    std::string prefix = "";

    // Print arguments
    std::cout << "imageFilename: " << imageFilename << std::endl
            << "outputFilename: " << outputFilename << std::endl
            << "radius: " << radius << std::endl;

    // Check if intermediate images should be saved
    if (argc > 4){
        verbose = true;
        prefix = argv[4];
        std::cout <<"debug_prefix: '"<<prefix<<"' - saving intermediate images (.nrrd) "<<std::endl;
    }

    // Read the image
    std::cout << "*** Reading image ***" << std::endl;
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(imageFilename);

    //Not working on multiple objects -> Extract largest object
    LabelImage* input_objects = new LabelImage(reader->GetOutput());
    if(input_objects->getNumberOfObjects() > 1) {
        std::cout << "*** Input has " << input_objects->getNumberOfObjects() << " objects. Picking largest ***" <<
        std::endl;
    }

    ImageType::Pointer input = input_objects->getLargestObject();

    //TODO: refactor into a function?
    // Perform erosion
    std::cout << "*** Performing Erosion ***" << std::endl;
    typedef itk::BinaryBallStructuringElement<
            ImageType::PixelType, 3>                  StructuringElementType;
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();

    typedef itk::BinaryErodeImageFilter <ImageType, ImageType, StructuringElementType>
            BinaryErodeImageFilterType;

    BinaryErodeImageFilterType::Pointer erodeFilter
            = BinaryErodeImageFilterType::New();
    erodeFilter->SetInput(input);
    erodeFilter->SetKernel(structuringElement);
    erodeFilter->SetErodeValue(1);
    erodeFilter->Update();

    if (verbose) writeImage(erodeFilter->GetOutput(),prefix + "_debug_erode.nrrd");

    // If after erosion we have more than one object -> separate by graphcut
    LabelImage* eroded_objects = new LabelImage(erodeFilter->GetOutput());
    if(eroded_objects->getNumberOfObjects() > 1) {
        std::cout << "*** After Erosion, "<<eroded_objects->getNumberOfObjects()<<" objects found. Picking largest ***" << std::endl;
        // --> pick largest as foreground, rest as background
        ImageType::Pointer foreground_image = eroded_objects->getLargestObject();

        if (verbose) writeImage(foreground_image,prefix + "_debug_fg.nrrd");

        ImageType::Pointer background_image = eroded_objects->getAllButLargestObject();

        if (verbose) writeImage(background_image,prefix + "_debug_bg.nrrd");

        // --> Set up the graph cut
        typedef itk::ImageGraphCut3DFilter<ImageType, ImageType, ImageType, ImageType> GraphCutFilterType;
        GraphCutFilterType::Pointer graphCutFilter = GraphCutFilterType::New();
        graphCutFilter->SetInputImage(input);
        graphCutFilter->SetForegroundImage(foreground_image);
        graphCutFilter->SetBackgroundImage(background_image);

        // --> Set graph cut parameters
        graphCutFilter->SetVerboseOutput(true);
        graphCutFilter->SetBoundaryDirectionTypeToBrightDark();
        graphCutFilter->SetLambda(1.0);
        graphCutFilter->SetSigma(0.001); //something small
        graphCutFilter->SetTerminalWeight(std::numeric_limits<float>::max());

        // --> Define the color values of the output
        graphCutFilter->SetForegroundPixelValue(1);
        graphCutFilter->SetBackgroundPixelValue(0);

        // --> Start the computation
        std::cout << "*** Performing Graph Cut ***" << std::endl;
        graphCutFilter->Update();

        // --> get largest component - just in case there are small disjoint parts creeping in...
        std::cout << "*** Writing Result ***" << std::endl;

        LabelImage* graphcut_objects = new LabelImage(graphCutFilter->GetOutput());
        ImageType::Pointer output = graphcut_objects->getLargestObject();

        writeImage(output,outputFilename);


    } else {
        // else: Nothing needs to be done.
        std::cout << "*** After Erosion, only one object found - no output produced.***" << std::endl;
    }



}


void writeImage(ImageType::Pointer image, std::string path){
    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(path);
    writer->SetInput(image);
    try {
        std::cout << "Writing output image " << path << std::endl;
        writer->Update();
    }
    catch (itk::ExceptionObject &err) {
        std::cerr << "ERROR: Exception caught while writing output image" << std::endl;
        std::cerr << err << std::endl;
    }

}