#include "file.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>


void File::read_verts(const std::string &filename, Verts &verts)
{
    std::ifstream fin(filename.c_str());

    if (fin.fail())
        return;

    std::string x, y, z;
    std::stringstream ss;
    Vert vert;

    while (!std::getline(fin, x, ',').eof())
    {
        std::getline(fin, y, ',');
        std::getline(fin, z);
        ss << x, ss >> vert.x, ss.str(""), ss.clear();
        ss << y, ss >> vert.y, ss.str(""), ss.clear();
        ss << z, ss >> vert.z, ss.str(""), ss.clear();
        verts.insert(vert);
    }

    fin.close();
}

void File::read_faces(const std::string &filename, Faces &faces)
{
    std::ifstream fin(filename.c_str());

    if (fin.fail())
        return;

    std::string v1, v2, v3;
    std::stringstream ss;
    Face face;

    while (!std::getline(fin, v1, ',').eof())
    {
        std::getline(fin, v2, ',');
        std::getline(fin, v3);
        ss << v1, ss >> face.v1, ss.str(""), ss.clear();
        ss << v2, ss >> face.v2, ss.str(""), ss.clear();
        ss << v3, ss >> face.v3, ss.str(""), ss.clear();
        faces.insert(face, nullptr);
    }

    fin.close();
}


void File::write_verts(const Verts &verts, const std::string &filename)
{
    std::ofstream fout(filename.c_str());

    if (fout.fail())
        return;

    std::vector<Vert> vector;
    vector.resize(verts.size());
    verts.copy_all(&vector[0]);

    for (std::size_t i = 0; i != vector.size(); i++)
        fout << std::setprecision(15) << vector[i].x << ","
             << vector[i].y << "," << vector[i].z << "\n";

    fout.close();
}

void File::write_edges(const Edges &edges, const std::string &filename)
{
    std::ofstream fout(filename.c_str());

    if (fout.fail())
        return;

    std::vector<Edge> vector;
    vector.resize(edges.size());
    edges.copy_all(&vector[0]);

    for (std::size_t i = 0; i != vector.size(); i++)
        fout << vector[i].v1 << "," << vector[i].v2 << "\n";

    fout.close();
}

void File::write_faces(const Faces &faces, const std::string &filename)
{
    std::ofstream fout(filename.c_str());

    if (fout.fail())
        return;

    std::vector<Face> vectorFaces;
    std::vector<void *> vectorPtr;
    vectorFaces.resize(faces.size());
    vectorPtr.resize(faces.size());
    faces.copy_all(&vectorFaces[0], &vectorPtr[0]);

    for (std::size_t i = 0; i != vectorFaces.size(); i++)
        fout << vectorFaces[i].v1 << "," << vectorFaces[i].v2 << ","
             << vectorFaces[i].v3 << "\n";

    fout.close();
}