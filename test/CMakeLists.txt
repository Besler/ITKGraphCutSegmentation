
# Testing 
add_executable(TestGraphLibrary TestGraphLibrary.cpp)
target_link_libraries(TestGraphLibrary gtest gtest_main ${ITK_LIBRARIES} ${Boost_LIBRARIES} KolmogorovMaxFlow)
add_test(TestGraphLibrary TestGraphLibrary)

# Testing Graph.h
if (GRIDCUT_LIBRARY_AVAILABLE)
  add_executable(TestGraph TestGraph.cpp)
  target_link_libraries(TestGraph gtest gtest_main ${ITK_LIBRARIES} ${Boost_LIBRARIES})
  add_test(TestGraph TestGraph)

  add_executable(TestGraphCutImageFilter TestGraphCutImageFilter.cpp)
  target_link_libraries(TestGraphCutImageFilter gtest gtest_main ${ITK_LIBRARIES} ${Boost_LIBRARIES})
  add_test(TestGraphCutImageFilter TestGraphCutImageFilter)

  add_executable(TestStandardGraphCutImageFilter TestStandardGraphCutImageFilter.cpp)
  target_link_libraries(TestStandardGraphCutImageFilter gtest gtest_main ${ITK_LIBRARIES} ${Boost_LIBRARIES})
  add_test(TestStandardGraphCutImageFilter TestStandardGraphCutImageFilter)

  add_executable(TestModifiedSheetnessGraphCutImageFilter TestModifiedSheetnessGraphCutImageFilter.cpp)
  target_link_libraries(TestModifiedSheetnessGraphCutImageFilter gtest gtest_main ${ITK_LIBRARIES} ${Boost_LIBRARIES})
  add_test(TestModifiedSheetnessGraphCutImageFilter TestModifiedSheetnessGraphCutImageFilter)
endif(GRIDCUT_LIBRARY_AVAILABLE)

# Testing with known segmentations
#add_executable(TestSegmentation TestSegmentation.cpp)
#target_link_libraries(TestSegmentation gtest gtest_main ${ITK_LIBRARIES} KolmogorovMaxFlow)
