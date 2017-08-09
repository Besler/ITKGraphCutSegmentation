set(GraphCutSegmentation_SRCS
  itkMinimalStandardRandomVariateGenerator.cxx
  )

add_library(GraphCutSegmentation ${GraphCutSegmentation_SRCS})
itk_module_link_dependencies()
itk_module_target(GraphCutSegmentation)
