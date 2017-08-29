#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "ModifiedSheetnessGraphCutImageFilter.h"

// Define all image types
typedef short                                         SheetnessPixelType;
typedef unsigned char                                 LabelPixelType;
const unsigned int DIMENSIONS =                       3;
typedef itk::Image<SheetnessPixelType, DIMENSIONS>    SheetnessImageType;
typedef itk::Image<SheetnessPixelType, DIMENSIONS>    LabelImageType;
typedef char                                          WeightType;

typedef itk::ImageFileReader<SheetnessImageType>  ReaderType;
typedef itk::ImageFileReader<LabelImageType>      LabelReaderType;
typedef itk::ModifiedSheetnessGraphCutImageFilter<LabelImageType, SheetnessImageType, LabelImageType, WeightType> ModifiedSheetnessGraphCutFilterType;
typedef itk::ImageFileWriter<LabelImageType>      WriterType;

int main(int argc, char *argv[]) {
    // Verify arguments
    if (argc != 7) {
        std::cerr << "Required: image.mhd labels.mhd output.mhd sigma foregroundLabel backgroundLabel" << std::endl;
        std::cerr << "image.mhd:           3D sheetness image" << std::endl;
        std::cerr << "labels.mhd:          3D image with all labels" << std::endl;
        std::cerr << "output.mhd:          3D image resulting segmentation" << std::endl;
        std::cerr << "sigma                estimated noise in boundary term" << std::endl;
        std::cerr << "foregroundLabel      foreground label value" << std::endl;
        std::cerr << "backgroundLabel      background label value" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse arguments
    std::string imageFilename       = argv[1];
    std::string labelsFilename      = argv[2];
    std::string outputFilename      = argv[3];
    double sigma                    = atof(argv[4]);
    LabelPixelType foregroundLabel  = static_cast<LabelPixelType>(atof(argv[5]));
    LabelPixelType backgroundLabel  = static_cast<LabelPixelType>(atof(argv[6]));

    // Print arguments
    std::cout << "imageFilename:      " << imageFilename << std::endl
              << "labelsFilename:     " << labelsFilename << std::endl
              << "outputFilename:     " << outputFilename << std::endl
              << "sigma:              " << sigma << std::endl
              << "foregroundLabel:    " << (int)foregroundLabel << std::endl
              << "backgroundLabel:    " << (int)backgroundLabel << std::endl;

    // Read the image
    std::cout << "*** Reading image ***" << std::endl;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(imageFilename);
    reader->Update();

    // Read the foreground and background masks
    std::cout << "*** Reading labels ***" << std::endl;
    typename LabelReaderType::Pointer labelReader = LabelReaderType::New();
    labelReader->SetFileName(labelsFilename);
    labelReader->Update();

    // Set up the graph cut
    typename ModifiedSheetnessGraphCutFilterType::Pointer graphCutFilter = ModifiedSheetnessGraphCutFilterType::New();
    graphCutFilter->SetInput(labelReader->GetOutput());
    graphCutFilter->SetSheetnessInput(reader->GetOutput());

    // Define the color values of the output
    graphCutFilter->SetForegroundLabel(foregroundLabel);
    graphCutFilter->SetBackgroundLabel(backgroundLabel);
    graphCutFilter->SetSigma(sigma);

    // Start the computation
    std::cout << "*** Performing Graph Cut ***" << std::endl;
    graphCutFilter->Update();

    // Get and write the result
    std::cout << "*** Writing Result ***" << std::endl;
    typename WriterType::Pointer writer = WriterType::New();
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
