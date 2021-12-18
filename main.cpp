#include "mesh.h"
#include "file.h"
#include <iostream>
#include <vector>
#include <cstdlib>


void print(const std::set<Edge> &set, const std::string &padding)
{
   if (set.empty())
      std::cout << padding
                << "Searched edges are empty." << std::endl;
   else
      for (auto iter = set.cbegin(); iter != set.cend(); iter++)
         std::cout << padding
                   << iter->v1 << "," << iter->v2 << std::endl;
}

void print(const std::map<Face, void *> &map, const std::string &padding)
{
   if (map.empty())
      std::cout << "Searched faces are empty." << std::endl;
   else
      for (auto iter = map.cbegin(); iter != map.cend(); iter++)
         std::cout << padding
                   << iter->first.v1 << ","
                   << iter->first.v2 << ","
                   << iter->first.v3 << std::endl;
}


int main()
{
   Verts verts;
   Faces faces;
   File::read_verts("verts.txt", verts);
   File::read_faces("faces.txt", faces);

   std::cout << "Total verts: " << verts.size() << std::endl;
   std::cout << "Total faces: " << faces.size() << std::endl;


   /* Test 1-1. Erasing a vertex with 4 neighbors.
       The vertex to be deleted will be one of the 6 starting vertices
       of the seeding octahedron.
       The total count of faces after deletion should decrease by 4.
    */
   std::cout << "\n\nTest 1-1. Size should decrease by 4." << std::endl;

   std::cout << "  Total faces before: " << faces.size() << std::endl;
   faces.erase(0); // (-2, 0, 0) connecting to 4 neighbors.
   std::cout << "  Total faces after:  " << faces.size() << std::endl;

   /* Test 1-2. Erasing a vertex with 6 neighbors.
       All the vertices that are not part of the original 6 vertices
       of the seeding octahedron have 6 neighboring points.
       Unless these selected vertices are adjacent to each other,
       deleting a vertex will decrase the face counts by 6 on each deletion.
    */
   std::cout << "\nTest 1-2. Size should decrease by 6 each time."
             << std::endl;

   std::cout << "  Total faces before:  " << faces.size() << std::endl;
   faces.erase(14973);
   std::cout << "  Total faces after 1: " << faces.size() << std::endl;
   faces.erase(40525);
   std::cout << "  Total faces after 2: " << faces.size() << std::endl;
   faces.erase(28356);
   std::cout << "  Total faces after 3: " << faces.size() << std::endl;
   faces.erase(45177);
   std::cout << "  Total faces after 4: " << faces.size() << std::endl;
   faces.erase(62428);
   std::cout << "  Total faces after 5: " << faces.size() << std::endl;


   /* Test 2-1. Erasing random edges.
       When deleting non-adjacent edges that are far away from each other
       the number of faces should decrease by 2, because 1 edge is
       shared by 2 triangle faces.
    */
   std::cout << "\n\nTest 2-1. Size should decrease by 2 each time."
             << std::endl;

   std::cout << "  Total faces before:  " << faces.size() << std::endl;
   faces.erase(Edge(39508, 39960));
   std::cout << "  Total faces after 1: " << faces.size() << std::endl;
   faces.erase(Edge(37217, 36745));
   std::cout << "  Total faces after 2: " << faces.size() << std::endl;
   faces.erase(Edge(46935, 46931));
   std::cout << "  Total faces after 3: " << faces.size() << std::endl;
   faces.erase(Edge(1359, 1343));
   std::cout << "  Total faces after 4: " << faces.size() << std::endl;
   faces.erase(Edge(27702, 27236));
   std::cout << "  Total faces after 5: " << faces.size() << std::endl;

   /* Test 2-2. Erasing 2 edges of the same triangle.
       In this case the second edge getting deleted only affects 1 face.
       The number of faces should decrease by 2 then 1.
    */
   std::cout << "\nTest 2-2. Size should decrease by 2 then 1."
             << std::endl;

   std::cout << "  Total faces before:  " << faces.size() << std::endl;
   faces.erase(Edge(9701, 9999));
   std::cout << "  Total faces after 1: " << faces.size() << std::endl;
   faces.erase(Edge(9999, 9931));
   std::cout << "  Total faces after 2: " << faces.size() << std::endl;


   /* Test 3. Erasing faces.
       The main purpose of tests below is to ensure the equivalent faces
       are properly recognized by deletion algorithm.
       For example Face(v1, v2, v3) is the same as
       Face(v2, v3, v1) and Face(v3, v1, v2) however
       Face(v1, v2, v3) is NOT the same as Face(v3, v2, v1).

       This distinction is in place for determining the normal vector
       of the face represeting the surface angle to reflect light.

       In this scenario the number of faces should decrease by
       1, 1, 1 then 0, 1.
       The first 3 deletions will be testing whether each rotation of
       the vertices is correctly recognized and the last 2 cases will
       test deletion of triangles in reversed vertex order.
    */
   std::cout << "\n\nTest 3. Size should decrease by 1, 1, 1, 0, 1."
             << std::endl;

   std::cout << "  Total faces before:  " << faces.size() << std::endl;
   faces.erase(Face(30950, 30454, 30462)); // (v1, v2, v3)
   std::cout << "  Total faces after 1: " << faces.size() << std::endl;
   faces.erase(Face(38961, 38497, 38505)); // (v2, v3, v1)
   std::cout << "  Total faces after 2: " << faces.size() << std::endl;
   faces.erase(Face(50807, 50839, 50523)); // (v3, v1, v2)
   std::cout << "  Total faces after 3: " << faces.size() << std::endl;
   faces.erase(Face(45178, 45194, 45574)); // reversed order
   std::cout << "  Total faces after 4: " << faces.size() << std::endl;
   faces.erase(Face(45574, 45194, 45178)); // correct order
   std::cout << "  Total faces after 5: " << faces.size() << std::endl;


   /* Test 4. Testing if Edges class can properly handle the changing mesh.
       The first edges object will update everytime a change happens whereas
       the second edges object will synchronize edges once at the end.

       However the results should be exactly the same.
    */
   std::cout << "\n\nTest 4. Two edges should be identical." << std::endl;

   verts.clear();
   faces.clear();

   Faces faces1, faces2;
   File::read_verts("verts.txt", verts);
   File::read_faces("faces.txt", faces1);
   faces2 = faces1;

   Edges edges1, edges2;
   faces1.sync(edges1);

   // edges1 is updated on every operation.
   faces1.erase(31024, edges1);
   faces1.erase(48614, edges1);
   faces1.erase(Edge(15902, 16230), edges1);
   faces1.erase(Edge(20105, 20121), edges1);
   faces1.erase(Edge(16248, 15928), edges1);
   faces1.erase(Edge(46975, 46575), edges1);
   faces1.erase(Face(12592, 12528, 12796), edges1);
   faces1.erase(Face(9303, 9327, 9579), edges1);
   faces1.erase(Face(49544, 49532, 49204), edges1);

   // edges2 is updated once at the end.
   faces2.erase(31024);
   faces2.erase(48614);
   faces2.erase(Edge(15902, 16230));
   faces2.erase(Edge(20121, 20105)); // order changed
   faces2.erase(Edge(16248, 15928));
   faces2.erase(Edge(46575, 46975)); // order changed
   faces2.erase(Face(12592, 12528, 12796));
   faces2.erase(Face(9327, 9579, 9303));    // order changed
   faces2.erase(Face(49204, 49544, 49532)); // order changed
   faces2.sync(edges2);

   // Converting edges to std::vector for easy comparison.
   std::vector<Edge> vector1, vector2;
   vector1.resize(edges1.size());
   vector2.resize(edges2.size());
   edges1.copy_all(&vector1[0]);
   edges2.copy_all(&vector2[0]);
   std::cout << "  Identical: "
             << (vector1 == vector2 ? "Yes" : "No") << std::endl;


   /* Test 5-1. Searching based on Vert.
       The vertex picked as the search input makes up 6 faces.
    */
   std::cout << "\n\nTest 5-1. Result should have 6 faces." << std::endl;

   auto result1 = faces1.search(56691);
   print(result1, "  ");

   /* Test 5-2. Searching based on Edge.
       Every edge making up the octasphere is touching 2 faces.
    */
   std::cout << "\nTest 5-2. Result should have 2 faces." << std::endl;

   auto result2 = faces1.search(Edge(11290, 11086));
   print(result2, "  ");


   /* Test 6. Searching for indices from Verts using (x, y, z) coord.
       Because in the original vertices dataset there are no duplicates.
       So I simply inserted 3 identical points to the end of verts.
    */
   std::cout << "\n\nTest 6. Result length should be 3." << std::endl;

   // Insert 3 identical points to verts.
   verts.insert(Vert(-1.6, 2, 4.8));
   verts.insert(Vert(-1.6, 2, 4.8));
   verts.insert(Vert(-1.6, 2, 4.8));

   auto result = verts.search(Vert(-1.6, 2, 4.8));
   std::cout << "  Length: " << result.size() << std::endl;


   /* Test 7. File writing test.
       Write all the verts.
       Write all the edges.
       Write all the faces.
    */
   std::cout << "\n\nTest 7. File output test." << std::endl;

   File::write_verts(verts, "output_verts.txt");
   File::write_edges(edges1, "output_edges.txt");
   File::write_faces(faces1, "output_faces.txt");


   system("pause");

   return 0;
}