file(GLOB _sources
          tdliron.cpp
          tdliron/*.cc
    )

add_mlir_library(Tdliron 
    ${_sources}
    
    DEPENDS
    MLIRHbmPassIncGen
    MLIRTdlironIncGen
    MLIRShapeInterfaceIncGen
)