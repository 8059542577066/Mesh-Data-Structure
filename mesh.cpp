#include "mesh.h"
#include <limits>


Vert::Vert() {}

Vert::Vert(double x, double y, double z)
    : x(x), y(y), z(z) {}

bool Vert::operator==(const Vert &other) const
{
    if (this->x != other.x)
        return false;
    else if (this->y != other.y)
        return false;
    else if (this->z != other.z)
        return false;
    else
        return true;
}

bool Vert::operator<(const Vert &other) const
{
    if (this->x != other.x)
        return this->x < other.x;
    else if (this->y != other.y)
        return this->y < other.y;
    else if (this->z != other.z)
        return this->z < other.z;
    else
        return false;
}


Edge::Edge() {}

Edge::Edge(std::size_t v1, std::size_t v2)
    : v1(v1), v2(v2) {}

bool Edge::operator==(const Edge &other) const
{
    if (this->v1 != other.v1)
        return false;
    else if (this->v2 != other.v2)
        return false;
    else
        return true;
}

bool Edge::operator<(const Edge &other) const
{
    if (this->v1 != other.v1)
        return this->v1 < other.v1;
    else if (this->v2 != other.v2)
        return this->v2 < other.v2;
    else
        return false;
}

bool Edge::OrderByV2::operator()(const Edge &edge1,
                                 const Edge &edge2) const
{
    return edge1.v2 < edge2.v2;
}


Face::Face() {}

Face::Face(std::size_t v1, std::size_t v2, std::size_t v3)
    : v1(v1), v2(v2), v3(v3) {}

bool Face::operator==(const Face &other) const
{
    if (this->v1 != other.v1)
        return false;
    else if (this->v2 != other.v2)
        return false;
    else if (this->v3 != other.v3)
        return false;
    else
        return true;
}

bool Face::operator<(const Face &other) const
{
    if (this->v1 != other.v1)
        return this->v1 < other.v1;
    else if (this->v2 != other.v2)
        return this->v2 < other.v2;
    else if (this->v3 != other.v3)
        return this->v3 < other.v3;
    else
        return false;
}

bool Face::OrderByV2::operator()(const Face &face1,
                                 const Face &face2) const
{
    return face1.v2 < face2.v2;
}

bool Face::OrderByV3::operator()(const Face &face1,
                                 const Face &face2) const
{
    return face1.v3 < face2.v3;
}


Vert Verts::operator[](std::size_t idx) const
{
    auto found = this->verts.find(idx);

    if (found == this->verts.cend())
    {
        auto INF = std::numeric_limits<double>::infinity();
        return Vert(INF, INF, INF);
    }
    else
        return found->second;
}

void Verts::insert(const Vert &vert)
{
    if (this->verts.empty())
    {
        this->verts[0] = vert;
        this->vertsInv.insert(std::pair<Vert, std::size_t>(vert, 0));
    }
    else
    {
        auto idx = this->verts.crbegin()->first + 1;
        this->verts[idx] = vert;
        this->vertsInv.insert(std::pair<Vert, std::size_t>(vert, idx));
    }
}

void Verts::modify(std::size_t idx, const Vert &vert)
{
    auto found = this->verts.find(idx);

    if (found == this->verts.cend())
        return;

    auto range = this->vertsInv.equal_range(found->second);
    auto lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first == found->second && iter->second == idx)
        {
            this->vertsInv.erase(iter);
            break;
        }

    this->vertsInv.insert(std::pair<Vert, std::size_t>(vert, idx));
    found->second = vert;
}

void Verts::erase(std::size_t idx)
{
    auto found = this->verts.find(idx);

    if (found == this->verts.cend())
        return;

    auto range = this->vertsInv.equal_range(found->second);
    auto lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first == found->second && iter->second == idx)
        {
            this->vertsInv.erase(iter);
            break;
        }

    this->verts.erase(found);
}

void Verts::erase(const Vert &vert)
{
    auto range = this->vertsInv.equal_range(vert);
    auto lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        this->verts.erase(iter->second);

    this->vertsInv.erase(vert);
}

void Verts::clear()
{
    this->verts.clear();
    this->vertsInv.clear();
}

std::size_t Verts::size() const
{
    return this->verts.size();
}

std::set<std::size_t> Verts::search(const Vert &vert) const
{
    std::set<std::size_t> set;
    auto range = this->vertsInv.equal_range(vert);
    auto lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        set.insert(iter->second);

    return set;
}

void Verts::copy_all(Vert *ptr) const
{
    auto lower = this->verts.cbegin();
    auto upper = this->verts.cend();

    for (auto iter = lower; iter != upper; iter++)
        *ptr++ = iter->second;
}


void Edges::insert(Edge edge)
{
    if (edge.v1 == edge.v2)
        return;

    if (edge.v1 > edge.v2)
        std::swap(edge.v1, edge.v2);

    auto before = this->edgesByV1.size();
    this->edgesByV1.insert(edge);

    if (this->edgesByV1.size() > before)
        this->edgesByV2.insert(edge);
}

void Edges::erase(std::size_t idx)
{
    auto lower = this->edgesByV1.lower_bound(Edge(idx, 0));
    auto upper = this->edgesByV1.upper_bound(Edge(idx, -1));

    for (auto iter = lower; iter != upper;)
    {
        auto range2 = this->edgesByV2.equal_range(*iter);
        auto lower2 = range2.first, upper2 = range2.second;

        for (auto iter2 = lower2; iter2 != upper2; iter2++)
            if (*iter2 == *iter)
            {
                this->edgesByV2.erase(iter2);
                break;
            }

        this->edgesByV1.erase(iter++);
    }

    auto range = this->edgesByV2.equal_range(Edge(0, idx));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        this->edgesByV1.erase(*iter);

    this->edgesByV2.erase(Edge(0, idx));
}

void Edges::erase(Edge edge)
{
    if (edge.v1 > edge.v2)
        std::swap(edge.v1, edge.v2);

    auto range = this->edgesByV2.equal_range(edge);
    auto lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        if (*iter == edge)
        {
            this->edgesByV2.erase(iter);
            break;
        }

    this->edgesByV1.erase(edge);
}

void Edges::clear()
{
    this->edgesByV1.clear();
    this->edgesByV2.clear();
}

bool Edges::find(Edge edge) const
{
    if (edge.v1 > edge.v2)
        std::swap(edge.v1, edge.v2);

    return this->edgesByV1.find(edge) != this->edgesByV1.cend();
}

std::size_t Edges::size() const
{
    return this->edgesByV1.size();
}

std::set<Edge> Edges::search(std::size_t idx) const
{
    std::set<Edge> set;
    auto lower = this->edgesByV1.lower_bound(Edge(idx, 0));
    auto upper = this->edgesByV1.upper_bound(Edge(idx, -1));

    for (auto iter = lower; iter != upper; iter++)
        set.insert(*iter);

    auto range = this->edgesByV2.equal_range(Edge(0, idx));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        set.insert(*iter);

    return set;
}

void Edges::copy_all(Edge *ptr) const
{
    auto lower = this->edgesByV1.cbegin();
    auto upper = this->edgesByV1.cend();

    for (auto iter = lower; iter != upper; iter++)
        *ptr++ = *iter;
}


void *Faces::operator[](Face face) const
{
    if (face.v2 < face.v3 && face.v2 < face.v1)
        face = Face(face.v2, face.v3, face.v1);
    else if (face.v3 < face.v1 && face.v3 < face.v2)
        face = Face(face.v3, face.v1, face.v2);

    auto found = this->facesByV1.find(face);

    if (found == this->facesByV1.cend())
        return nullptr;
    else
        return found->second;
}

void Faces::insert(Face face, void *ptr)
{
    if (face.v1 == face.v2 || face.v2 == face.v3 || face.v3 == face.v1)
        return;

    if (face.v2 < face.v3 && face.v2 < face.v1)
        face = Face(face.v2, face.v3, face.v1);
    else if (face.v3 < face.v1 && face.v3 < face.v2)
        face = Face(face.v3, face.v1, face.v2);

    auto before = this->facesByV1.size();
    this->facesByV1[face] = ptr;

    if (this->facesByV1.size() > before)
    {
        this->facesByV2.insert(std::pair<Face, void *>(face, ptr));
        this->facesByV3.insert(std::pair<Face, void *>(face, ptr));
    }
    else
    {
        auto range = facesByV2.equal_range(face);
        auto lower = range.first, upper = range.second;

        for (auto iter = lower; iter != upper; iter++)
            if (iter->first == face)
            {
                iter->second = ptr;
                break;
            }

        range = facesByV3.equal_range(face);
        lower = range.first, upper = range.second;

        for (auto iter = lower; iter != upper; iter++)
            if (iter->first == face)
            {
                iter->second = ptr;
                break;
            }
    }
}

void Faces::insert(const Face &face, void *ptr, Edges &edges)
{
    this->insert(face, ptr);
    edges.insert(Edge(face.v1, face.v2));
    edges.insert(Edge(face.v2, face.v3));
    edges.insert(Edge(face.v3, face.v1));
}

void Faces::erase(std::size_t idx)
{
    auto lower = this->facesByV1.lower_bound(Face(idx, 0, 0));
    auto upper = this->facesByV1.upper_bound(Face(idx, -1, -1));

    for (auto iter = lower; iter != upper;)
    {
        auto range2 = this->facesByV2.equal_range(iter->first);
        auto lower2 = range2.first, upper2 = range2.second;

        for (auto iter2 = lower2; iter2 != upper2; iter2++)
            if (iter2->first == iter->first)
            {
                this->facesByV2.erase(iter2);
                break;
            }

        auto range3 = this->facesByV3.equal_range(iter->first);
        auto lower3 = range3.first, upper3 = range3.second;

        for (auto iter3 = lower3; iter3 != upper3; iter3++)
            if (iter3->first == iter->first)
            {
                this->facesByV3.erase(iter3);
                break;
            }

        this->facesByV1.erase(iter++);
    }

    auto range = this->facesByV2.equal_range(Face(0, idx, 0));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
    {
        auto range3 = this->facesByV3.equal_range(iter->first);
        auto lower3 = range3.first, upper3 = range3.second;

        for (auto iter3 = lower3; iter3 != upper3; iter3++)
            if (iter3->first == iter->first)
            {
                this->facesByV3.erase(iter3);
                break;
            }

        this->facesByV1.erase(iter->first);
    }

    range = this->facesByV3.equal_range(Face(0, 0, idx));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
    {
        auto range2 = this->facesByV2.equal_range(iter->first);
        auto lower2 = range2.first, upper2 = range2.second;

        for (auto iter2 = lower2; iter2 != upper2; iter2++)
            if (iter2->first == iter->first)
            {
                this->facesByV2.erase(iter2);
                break;
            }

        this->facesByV1.erase(iter->first);
    }

    this->facesByV2.erase(Face(0, idx, 0));
    this->facesByV3.erase(Face(0, 0, idx));
}

void Faces::erase(std::size_t idx, Edges &edges)
{
    this->erase(idx);
    edges.erase(idx);
}

void Faces::erase(const Edge &edge)
{
    auto lower = this->facesByV1.lower_bound(Face(edge.v1, 0, 0));
    auto upper = this->facesByV1.upper_bound(Face(edge.v1, -1, -1));

    for (auto iter = lower; iter != upper;)
        if (iter->first.v2 == edge.v2 || iter->first.v3 == edge.v2)
        {
            auto range2 = this->facesByV2.equal_range(iter->first);
            auto lower2 = range2.first, upper2 = range2.second;

            for (auto iter2 = lower2; iter2 != upper2; iter2++)
                if (iter2->first == iter->first)
                {
                    this->facesByV2.erase(iter2);
                    break;
                }

            auto range3 = this->facesByV3.equal_range(iter->first);
            auto lower3 = range3.first, upper3 = range3.second;

            for (auto iter3 = lower3; iter3 != upper3; iter3++)
                if (iter3->first == iter->first)
                {
                    this->facesByV3.erase(iter3);
                    break;
                }

            this->facesByV1.erase(iter++);
        }
        else
            iter++;

    lower = this->facesByV1.lower_bound(Face(edge.v2, 0, 0));
    upper = this->facesByV1.upper_bound(Face(edge.v2, -1, -1));

    for (auto iter = lower; iter != upper;)
        if (iter->first.v2 == edge.v1 || iter->first.v3 == edge.v1)
        {
            auto range2 = this->facesByV2.equal_range(iter->first);
            auto lower2 = range2.first, upper2 = range2.second;

            for (auto iter2 = lower2; iter2 != upper2; iter2++)
                if (iter2->first == iter->first)
                {
                    this->facesByV2.erase(iter2);
                    break;
                }

            auto range3 = this->facesByV3.equal_range(iter->first);
            auto lower3 = range3.first, upper3 = range3.second;

            for (auto iter3 = lower3; iter3 != upper3; iter3++)
                if (iter3->first == iter->first)
                {
                    this->facesByV3.erase(iter3);
                    break;
                }

            this->facesByV1.erase(iter++);
        }
        else
            iter++;

    auto range = this->facesByV2.equal_range(Face(0, edge.v1, 0));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper;)
        if (iter->first.v3 == edge.v2)
        {
            auto range3 = this->facesByV3.equal_range(iter->first);
            auto lower3 = range3.first, upper3 = range3.second;

            for (auto iter3 = lower3; iter3 != upper3; iter3++)
                if (iter3->first == iter->first)
                {
                    this->facesByV3.erase(iter3);
                    break;
                }

            this->facesByV1.erase(iter->first);
            this->facesByV2.erase(iter++);
        }
        else
            iter++;

    range = this->facesByV2.equal_range(Face(0, edge.v2, 0));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper;)
        if (iter->first.v3 == edge.v1)
        {
            auto range3 = this->facesByV3.equal_range(iter->first);
            auto lower3 = range3.first, upper3 = range3.second;

            for (auto iter3 = lower3; iter3 != upper3; iter3++)
                if (iter3->first == iter->first)
                {
                    this->facesByV3.erase(iter3);
                    break;
                }

            this->facesByV1.erase(iter->first);
            this->facesByV2.erase(iter++);
        }
        else
            iter++;
}

void Faces::erase(const Edge &edge, Edges &edges)
{
    this->erase(edge);
    edges.erase(edge);
}

void Faces::erase(Face face)
{
    if (face.v2 < face.v3 && face.v2 < face.v1)
        face = Face(face.v2, face.v3, face.v1);
    else if (face.v3 < face.v1 && face.v3 < face.v2)
        face = Face(face.v3, face.v1, face.v2);

    auto range = facesByV2.equal_range(face);
    auto lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first == face)
        {
            this->facesByV2.erase(iter);
            break;
        }

    range = facesByV3.equal_range(face);
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first == face)
        {
            this->facesByV3.erase(iter);
            break;
        }

    this->facesByV1.erase(face);
}

void Faces::erase(const Face &face, Edges &edges)
{
    this->erase(face);

    if (this->search(Edge(face.v1, face.v2)).empty())
        edges.erase(Edge(face.v1, face.v2));

    if (this->search(Edge(face.v2, face.v3)).empty())
        edges.erase(Edge(face.v2, face.v3));

    if (this->search(Edge(face.v3, face.v1)).empty())
        edges.erase(Edge(face.v3, face.v1));
}

void Faces::clear()
{
    this->facesByV1.clear();
    this->facesByV2.clear();
    this->facesByV3.clear();
}

std::size_t Faces::size() const
{
    return this->facesByV1.size();
}

std::map<Face, void *> Faces::search(std::size_t idx) const
{
    std::map<Face, void *> map;
    auto lower = this->facesByV1.lower_bound(Face(idx, 0, 0));
    auto upper = this->facesByV1.upper_bound(Face(idx, -1, -1));

    for (auto iter = lower; iter != upper; iter++)
        map[iter->first] = iter->second;

    auto range = this->facesByV2.equal_range(Face(0, idx, 0));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        map[iter->first] = iter->second;

    range = this->facesByV3.equal_range(Face(0, 0, idx));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        map[iter->first] = iter->second;

    return map;
}

std::map<Face, void *> Faces::search(const Edge &edge) const
{
    std::map<Face, void *> map;
    auto lower = this->facesByV1.lower_bound(Face(edge.v1, 0, 0));
    auto upper = this->facesByV1.upper_bound(Face(edge.v1, -1, -1));

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first.v2 == edge.v2 || iter->first.v3 == edge.v2)
            map[iter->first] = iter->second;

    lower = this->facesByV1.lower_bound(Face(edge.v2, 0, 0));
    upper = this->facesByV1.upper_bound(Face(edge.v2, -1, -1));

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first.v2 == edge.v1 || iter->first.v3 == edge.v1)
            map[iter->first] = iter->second;

    auto range = this->facesByV2.equal_range(Face(0, edge.v1, 0));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first.v3 == edge.v2)
            map[iter->first] = iter->second;

    range = this->facesByV2.equal_range(Face(0, edge.v2, 0));
    lower = range.first, upper = range.second;

    for (auto iter = lower; iter != upper; iter++)
        if (iter->first.v3 == edge.v1)
            map[iter->first] = iter->second;

    return map;
}

void Faces::sync(Edges &edges) const
{
    edges.clear();
    auto lower = this->facesByV1.cbegin();
    auto upper = this->facesByV1.cend();

    for (auto iter = lower; iter != upper; iter++)
    {
        edges.insert(Edge(iter->first.v1, iter->first.v2));
        edges.insert(Edge(iter->first.v2, iter->first.v3));
        edges.insert(Edge(iter->first.v3, iter->first.v1));
    }
}

void Faces::copy_all(Face *ptrFace, void **ptrPtr) const
{
    auto lower = this->facesByV1.cbegin();
    auto upper = this->facesByV1.cend();

    for (auto iter = lower; iter != upper; iter++)
    {
        *ptrFace++ = iter->first;
        *ptrPtr++ = iter->second;
    }
}