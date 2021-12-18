#ifndef FILE_H
#define FILE_H

#ifdef __WIN32__
#ifdef BUILD_LIB
#define LIB_CLASS __declspec(dllexport)
#else
#define LIB_CLASS __declspec(dllimport)
#endif
#else
#define LIB_CLASS
#endif

#include "mesh.h"
#include <string>


class LIB_CLASS File
{
public:
    static void read_verts(const std::string &, Verts &);
    static void read_faces(const std::string &, Faces &);

    static void write_verts(const Verts &, const std::string &);
    static void write_edges(const Edges &, const std::string &);
    static void write_faces(const Faces &, const std::string &);
};


#endif