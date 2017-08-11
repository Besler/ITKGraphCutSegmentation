ADD_EXECUTABLE(KrcahGraphcut KrcahGraphcut.cpp)
TARGET_LINK_LIBRARIES(KrcahGraphcut
        ${ITK_LIBRARIES}
        ${ImageGraphCut3DSegmentation_libraries})

ADD_EXECUTABLE(KrcahSplit KrcahSplit.cpp LabelImage.cpp)
TARGET_LINK_LIBRARIES(KrcahSplit
        ${ITK_LIBRARIES}
        ${ImageGraphCut3DSegmentation_libraries})
