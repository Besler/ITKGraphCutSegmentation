itk_module_test()

set(GraphCutSegmentationTests
  itkMinimalStandardRandomVariateGeneratorTest.cxx
  itkNormalDistributionImageSourceTest.cxx
  )

CreateTestDriver(GraphCutSegmentation "${GraphCutSegmentation-Test_LIBRARIES}" "${GraphCutSegmentationTests}")

itk_add_test(NAME itkMinimalStandardRandomVariateGeneratorTest
  COMMAND GraphCutSegmentationTestDriver itkMinimalStandardRandomVariateGeneratorTest
  )

itk_add_test(NAME itkNormalDistributionImageSourceTest
  COMMAND GraphCutSegmentationTestDriver --without-threads
  --compare
    ${ITK_TEST_OUTPUT_DIR}/itkNormalDistributionImageSourceTestOutput.mha
    DATA{Baseline/itkNormalDistributionImageSourceTestOutput.mha}
  itkNormalDistributionImageSourceTest
    ${ITK_TEST_OUTPUT_DIR}/itkNormalDistributionImageSourceTestOutput.mha
  )
