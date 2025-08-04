#include "spine_data.hpp"




/// Set the default extension used for memory allocations and file I/O
spine::SpineExtension *spine::getDefaultExtension() {
    return new spine::DefaultSpineExtension();
}


