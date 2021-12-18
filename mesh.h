#ifndef MESH_H
#define MESH_H

#ifdef __WIN32__
#ifdef BUILD_LIB
#define LIB_CLASS __declspec(dllexport)
#else
#define LIB_CLASS __declspec(dllimport)
#endif
#else
#define LIB_CLASS
#endif

#include <map>
#include <set>


struct LIB_CLASS Vert
{
    double x;
    double y;
    double z;

    Vert();
    Vert(double, double, double);

    bool operator==(const Vert &) const;
    bool operator<(const Vert &) const;
};


struct LIB_CLASS Edge
{
    std::size_t v1;
    std::size_t v2;

    Edge();
    Edge(std::size_t, std::size_t);

    bool operator==(const Edge &) const;
    bool operator<(const Edge &) const;
    struct OrderByV2
    {
        bool operator()(const Edge &,
                        const Edge &) const;
    };
};


struct LIB_CLASS Face
{
    std::size_t v1;
    std::size_t v2;
    std::size_t v3;

    Face();
    Face(std::size_t, std::size_t, std::size_t);

    bool operator==(const Face &) const;
    bool operator<(const Face &) const;
    struct OrderByV2
    {
        bool operator()(const Face &,
                        const Face &) const;
    };
    struct OrderByV3
    {
        bool operator()(const Face &,
                        const Face &) const;
    };
};


class LIB_CLASS Verts
{
    std::map<std::size_t, Vert> verts;
    std::multimap<Vert, std::size_t> vertsInv;

public:
    Vert operator[](std::size_t) const;

    void insert(const Vert &);
    void modify(std::size_t, const Vert &);
    void erase(std::size_t);
    void erase(const Vert &);
    void clear();

    std::size_t size() const;
    std::set<std::size_t> search(const Vert &) const;
    void copy_all(Vert *) const;
};


class LIB_CLASS Edges
{
    std::set<Edge> edgesByV1;
    std::multiset<Edge, Edge::OrderByV2> edgesByV2;

public:
    void insert(Edge);
    void erase(std::size_t);
    void erase(Edge);
    void clear();

    bool find(Edge) const;
    std::size_t size() const;
    std::set<Edge> search(std::size_t) const;
    void copy_all(Edge *) const;
};


class LIB_CLASS Faces
{
    std::map<Face, void *> facesByV1;
    std::multimap<Face, void *, Face::OrderByV2> facesByV2;
    std::multimap<Face, void *, Face::OrderByV3> facesByV3;

public:
    void *operator[](Face) const;

    void insert(Face, void *);
    void insert(const Face &, void *, Edges &);
    void erase(std::size_t);
    void erase(std::size_t, Edges &);
    void erase(const Edge &);
    void erase(const Edge &, Edges &);
    void erase(Face);
    void erase(const Face &, Edges &);
    void clear();

    std::size_t size() const;
    std::map<Face, void *> search(std::size_t) const;
    std::map<Face, void *> search(const Edge &) const;
    void sync(Edges &) const;
    void copy_all(Face *, void **) const;
};


#endif