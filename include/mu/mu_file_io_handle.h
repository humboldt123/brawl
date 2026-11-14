#pragma once

#include <gf/gf_file_io_handle.h>

// A file handle of the menus (each menu module has its own copy of the constructor and the destructor).
class muFileIOHandle : public gfFileIOHandle {
public:
    muFileIOHandle() { }
    ~muFileIOHandle() { }
};
