#ifndef MYCUBE_H_INCLUDED
#define MYCUBE_H_INCLUDED


//myCubeVertices - homogeniczne współrzędne wierzchołków w przestrzeni modelu
//myCubeNormals - homogeniczne wektory normalne ścian (per wierzchołek) w przestrzeni modelu
//myCubeVertexNormals - homogeniczne wektory normalne wierzchołków w przestrzeni modelu
//myCubeTexCoords - współrzędne teksturowania
//myCubeColors - kolory wierzchołków
//myCubeC1 - kolumna macierzy TBN^-1
//myCubeC2 - kolumna macierzy TBN^-1
//myCubeC3 - kolumna macierzy TBN^-1

int myCubeVertexCount=18;

float myCubeVertices[]={
		   -1.0f, -1.0f, -1.0f, 1.0f,  // Front left
	1.0f, -1.0f, -1.0f, 1.0f,   // Front right
	1.0f, -1.0f, 1.0f, 1.0f,   // Back right

	-1.0f, -1.0f, -1.0f, 1.0f,  // Front left
	1.0f, -1.0f, 1.0f, 1.0f,   // Back right
	-1.0f, -1.0f, 1.0f, 1.0f,  // Back left

	// Sides
	0.0f, 1.0f, 0.0f, 1.0f,   // Apex
	-1.0f, -1.0f, -1.0f, 1.0f,  // Front left
	1.0f, -1.0f, -1.0f, 1.0f,   // Front right

	0.0f, 1.0f, 0.0f, 1.0f,   // Apex
	1.0f, -1.0f, -1.0f, 1.0f,   // Front right
	1.0f, -1.0f, 1.0f, 1.0f,   // Back right

	0.0f, 1.0f, 0.0f, 1.0f,   // Apex
	1.0f, -1.0f, 1.0f, 1.0f,   // Back right
	-1.0f, -1.0f, 1.0f, 1.0f,  // Back left

	0.0f, 1.0f, 0.0f, 1.0f,   // Apex
	-1.0f, -1.0f, 1.0f, 1.0f,  // Back left
	-1.0f, -1.0f, -1.0f, 1.0f  // Front left



			};


float myCubeColors[]={
                //Ściana 1
				  1.0f, 0.0f, 0.0f, 1.0f, // Front left
	1.0f, 0.0f, 0.0f, 1.0f, // Front right
	1.0f, 0.0f, 0.0f, 1.0f, // Back right

	1.0f, 0.0f, 0.0f, 1.0f, // Front left
	1.0f, 0.0f, 0.0f, 1.0f, // Back right
	1.0f, 0.0f, 0.0f, 1.0f, // Back left

	// Sides
	// Front
	0.0f, 1.0f, 0.0f, 1.0f, // Apex
	0.0f, 1.0f, 0.0f, 1.0f, // Front left
	0.0f, 1.0f, 0.0f, 1.0f, // Front right

	// Right
	0.0f, 1.0f, 0.0f, 1.0f, // Apex
	0.0f, 1.0f, 0.0f, 1.0f, // Front right
	0.0f, 1.0f, 0.0f, 1.0f, // Back right

	// Back
	0.0f, 1.0f, 0.0f, 1.0f, // Apex
	0.0f, 1.0f, 0.0f, 1.0f, // Back right
	0.0f, 1.0f, 0.0f, 1.0f, // Back left

	// Left
	0.0f, 1.0f, 0.0f, 1.0f, // Apex
	0.0f, 1.0f, 0.0f, 1.0f, // Back left
	0.0f, 1.0f, 0.0f, 1.0f  // Front left
			};

			float myCubeNormals[]={
				//Ściana 1
			   // Base
	0.0f, -1.0f, 0.0f, 0.0f, // Front left
	0.0f, -1.0f, 0.0f, 0.0f, // Front right
	0.0f, -1.0f, 0.0f, 0.0f, // Back right

	0.0f, -1.0f, 0.0f, 0.0f, // Front left
	0.0f, -1.0f, 0.0f, 0.0f, // Back right
	0.0f, -1.0f, 0.0f, 0.0f, // Back left

	// Sides
	// Front
	0.0f, 0.447214f, -0.894427f, 0.0f, // Apex
	-0.57735f, -0.447214f, -0.894427f, 0.0f, // Front left
	0.57735f, -0.447214f, -0.894427f, 0.0f, // Front right

	// Right
	0.0f, 0.447214f, 0.894427f, 0.0f, // Apex
	0.57735f, -0.447214f, -0.894427f, 0.0f, // Front right
	0.57735f, -0.447214f, 0.894427f, 0.0f, // Back right

	// Back
	0.0f, 0.447214f, 0.894427f, 0.0f, // Apex
	0.57735f, -0.447214f, 0.894427f, 0.0f, // Back right
	-0.57735f, -0.447214f, 0.894427f, 0.0f, // Back left

	// Left
	0.0f, 0.447214f, -0.894427f, 0.0f, // Apex
	-0.57735f, -0.447214f, 0.894427f, 0.0f, // Back left
	-0.57735f, -0.447214f, -0.894427f, 0.0f // Front left


			};

			float myCubeVertexNormals[]={
				//Ściana 1
				   0.0f, -1.0f, 0.0f, 0.0f, // Front left
	0.0f, -1.0f, 0.0f, 0.0f, // Front right
	0.0f, -1.0f, 0.0f, 0.0f, // Back right

	0.0f, -1.0f, 0.0f, 0.0f, // Front left
	0.0f, -1.0f, 0.0f, 0.0f, // Back right
	0.0f, -1.0f, 0.0f, 0.0f, // Back left

	// Sides
	// Front
	0.0f, 0.447214f, -0.894427f, 0.0f, // Apex
	-0.57735f, -0.447214f, -0.894427f, 0.0f, // Front left
	0.57735f, -0.447214f, -0.894427f, 0.0f, // Front right

	// Right
	0.0f, 0.447214f, 0.894427f, 0.0f, // Apex
	0.57735f, -0.447214f, -0.894427f, 0.0f, // Front right
	0.57735f, -0.447214f, 0.894427f, 0.0f, // Back right

	// Back
	0.0f, 0.447214f, 0.894427f, 0.0f, // Apex
	0.57735f, -0.447214f, 0.894427f, 0.0f, // Back right
	-0.57735f, -0.447214f, 0.894427f, 0.0f, // Back left

	// Left
	0.0f, 0.447214f, -0.894427f, 0.0f, // Apex
	-0.57735f, -0.447214f, 0.894427f, 0.0f, // Back left
	-0.57735f, -0.447214f, -0.894427f, 0.0f // Front left
			};

			float myCubeTexCoords[]={
				 0.5f, 0.5f, // Apex
	0.0f, 1.0f, // Front left
	1.0f, 1.0f, // Front right

	0.5f, 0.5f, // Apex
	1.0f, 1.0f, // Front right
	1.0f, 0.0f, // Back right

	0.5f, 0.5f, // Apex
	1.0f, 0.0f, // Back right
	0.0f, 0.0f, // Back left

	0.5f, 0.5f, // Apex
	0.0f, 0.0f, // Back left
	0.0f, 1.0f, // Front left

	// Sides
	// Front
	0.5f, 0.5f, // Apex
	0.0f, 0.0f, // Front left
	1.0f, 0.0f, // Front right

	// Right
	0.5f, 0.5f, // Apex
	1.0f, 0.0f, // Front right
	1.0f, 1.0f, // Back right

	// Back
	0.5f, 0.5f, // Apex
	1.0f, 1.0f, // Back right
	0.0f, 1.0f, // Back left

	// Left
	0.5f, 0.5f, // Apex
	0.0f, 1.0f, // Back left
	0.0f, 0.0f  // Front left
			};

			


#endif // MYCUBE_H_INCLUDED
