/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "objloader.h"
#include <ctime> 

float speed_x=0;
float speed_y=0;
float aspectRatio=1;

ShaderProgram *sp;


//wierzcholki dla obiektu wulkan
float* vertices;
float* normals;
float* texCoords;
int vertexCount;

//wierzcholki dla obiektu sfera(chmura i lawa)
float* vertices3;
float* normals3;
float* texCoords3;
int vertexCount3;

//wierzcholki dla obiektu kamien
float* vertices5;
float* normals5;
float* texCoords5;
int vertexCount5;

//wierzcholki do wczytania dla obiektu wulkan
std::vector< glm::vec4 > vertices2;
std::vector< glm::vec2 > uvs2;
std::vector< glm::vec4 > normals2;

//wierzcholki do wczytania dla obiektu sfera(chmura i lawa)
std::vector< glm::vec4 > vertices4;
std::vector< glm::vec2 > uvs4;
std::vector< glm::vec4 > normals4;

//wierzcholki do wczytania dla obiektu kamien
std::vector< glm::vec4 > vertices6;
std::vector< glm::vec2 > uvs6;
std::vector< glm::vec4 > normals6;




//
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, -3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.05f; 

//do wczytaniu tekstur
GLuint tex0;
GLuint tex1;
GLuint tex2;

//konwersja wczytanych wierzcholkow na float*
float* convertVerticesToFloatArray(const std::vector<glm::vec4>& vertices) {
	size_t vertexCount = vertices.size();
	float* vertexArray = new float[vertexCount * 4]; // Każdy glm::vec4 ma 4 składowe

	for (size_t i = 0; i < vertexCount; ++i) {
		vertexArray[i * 4 + 0] = vertices[i].x;
		vertexArray[i * 4 + 1] = vertices[i].y;
		vertexArray[i * 4 + 2] = vertices[i].z;
		vertexArray[i * 4 + 3] = vertices[i].w;
	}

	return vertexArray;
}

//konwersja wczytanych uvs na float*
float* convertUVsToFloatArray(const std::vector<glm::vec2>& uvs) {
	size_t uvCount = uvs.size();
	float* uvArray = new float[uvCount * 2]; // Każdy glm::vec2 ma 2 składowe

	for (size_t i = 0; i < uvCount; ++i) {
		uvArray[i * 2 + 0] = uvs[i].x;
		uvArray[i * 2 + 1] = uvs[i].y;
	}

	return uvArray;
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


//obsluga klawiszy
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	float cameraSpeed = 0.05f; // Ustawienie prędkości kamery
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_W)
			cameraPos += cameraSpeed * cameraFront;
		if (key == GLFW_KEY_S)
			cameraPos -= cameraSpeed * cameraFront;
		if (key == GLFW_KEY_A)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (key == GLFW_KEY_D)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (key == GLFW_KEY_UP)
			cameraPos += cameraSpeed * cameraUp;
		if (key == GLFW_KEY_DOWN)
			cameraPos -= cameraSpeed * cameraUp;

		if (key == GLFW_KEY_LEFT) {
			speed_y = -PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_y = PI;
		}
		cameraFront = glm::normalize(cameraFront); 
	}
	else {
		speed_y = 0.0f;
	}
}


void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

//wczytanie tekstur
GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    //Wczytanie do pamięci komputera
    std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
    unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
    //Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);

    //Import do pamięci karty graficznej
    glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,key_callback);

	//wczytywane obiekty
	bool res = loadOBJ("gura2.obj", vertices2, uvs2, normals2);
	bool res2 = loadOBJ("idk.obj", vertices4, uvs4, normals4);
	bool res3 = loadOBJ("kamien.obj", vertices6, uvs6, normals6);
	

	//wlaczenie programu cieniującego
	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");

	//konwersja wierzcholkow wulkanu
	vertices = convertVerticesToFloatArray(vertices2);
	vertexCount= vertices2.size();
	normals = convertVerticesToFloatArray(normals2);
	texCoords = convertUVsToFloatArray(uvs2);

	//konwersja wierzcholkow sfery
	vertices3 = convertVerticesToFloatArray(vertices4);
	normals3 = convertVerticesToFloatArray(normals4);
	texCoords3 = convertUVsToFloatArray(uvs4);
	vertexCount3 = vertices4.size();

	//konwersja wierzcholkow kamienia
	vertices5 = convertVerticesToFloatArray(vertices6);
	normals5 = convertVerticesToFloatArray(normals6);
	texCoords5 = convertUVsToFloatArray(uvs6);
	vertexCount5 = vertices6.size();


	//wczytanie tekstur
	tex1 = readTexture("sky.png");
	tex0 = readTexture("test3.png");
	tex2 = readTexture("test4.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
    delete sp;
	glDeleteTextures(1, &tex0);
}

//dane o polozeniu obiektu w przestrzeni xyz
std::vector<std::vector<std::vector<float>>> current_y_positions(20, std::vector<std::vector<float>>(20, std::vector<float>(20, 2.5f)));
std::vector<std::vector<std::vector<float>>> current_x_positions(20, std::vector<std::vector<float>>(20, std::vector<float>(20, -0.0f)));
std::vector<std::vector<std::vector<float>>> current_z_positions(20, std::vector<std::vector<float>>(30, std::vector<float>(20, 0.0f)));
//dane o predkosci obiektu
std::vector<std::vector<std::vector<float>>> movespeed(100, std::vector<std::vector<float>>(30, std::vector<float>(20, 0.008f)));


//rysowanie dymu
void render(GLFWwindow* window, float angle_x, float angle_y, glm::mat4 M) {
	
	//
	float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;

	//zmiana polozenia obiektu
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 13; ++j) {
			for (int k = 0; k < 10; k++) {
				//dla wiekszej roznicy w obiektach zmiana zmiennych
				int alfa, beta;
				if (i % 2 == 0)
					alfa = i;
				else
					alfa = -i;
				if (j % 2 == 0)
					beta = j;
				else
					beta = -j;

				//zmiana polozenia, zrobione metodą prob i bledow
				current_x_positions[i][j][k] += movespeed[i][j][k] * sqrt(alfa) * j * k / 100 * sqrt(k);
				current_y_positions[i][j][k] += movespeed[i][j][k] * j * alfa * k * k / 200;
				current_z_positions[i][j][k] += movespeed[i][j][k] * beta * sin(2 * PI * alfa) * 10 * k * k * k * k * alfa;
				//zmiana predkosci
				movespeed[i][j][k] *= 1.014;
				//w przypadku osiagniecia wysokosci krytycznej obiekt wraca do pozycji startowej, jego predkosc tez
				if (current_y_positions[i][j][k] > 6.0f ) {
					current_x_positions[i][j][k] = 0.0f;
					current_y_positions[i][j][k] = 2.5f;
					current_z_positions[i][j][k] = 0.0f;
					movespeed[i][j][k] = 0.008f;
				}
			}
		}
	}

	//wyliczenie macierzy rzutowania i widoku
	glm::mat4 P1 = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	glm::mat4 V1 = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	
	
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P1)); // Załadowanie macierzy rzutowania do programu cieniującego
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V1)); // Załadowanie macierzy widoku do programu cieniującego

	glm::mat4 Ms = glm::mat4(1.0f); // macierz jednostkowa


	// Rysowanie słońca dla każdej pozycji(kazdej ustawionej w petli)
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 12; ++j) {
			for (int k = 0; k < 10; k++) {
				// Przesunięcie obiektu zgodnie z aktualną pozycją
				glm::vec3 currentPosition = glm::vec3(current_x_positions[i][j][k],
					current_y_positions[i][j][k],
					current_z_positions[i][j][k]);

				//rotacja na podstawie angle_x/y
				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));

				glm::vec3 rotatedPosition = glm::vec3(rotationMatrix * glm::vec4(currentPosition, 1.0f));

				//translacja na podstawie rotacji
				Ms = glm::translate(glm::mat4(1.0f), rotatedPosition);
				//skalowanie obiektu
				Ms = glm::scale(Ms, glm::vec3(0.1f, 0.1f, 0.1f));

				// Pomnóż macierz modelu razy macierze obrotu
				//Ms = glm::rotate(Ms, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
				//Ms = glm::rotate(Ms, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));

				// Załadowanie macierzy modelu do programu cieniującego
				glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms));

				sp->use(); // Aktywacja programu cieniującego
				// Przeslij parametry programu cieniującego do karty graficznej
				glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P1));
				glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V1));
				glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms));

				glEnableVertexAttribArray(sp->a("vertex")); // Włącz przesyłanie danych do atrybutu vertex
				glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices3); // Wskaż tablicę z danymi dla atrybutu vertex

				glEnableVertexAttribArray(sp->a("normal")); // Włącz przesyłanie danych do atrybutu normal
				glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals3); // Wskaż tablicę z danymi dla atrybutu normal

				glEnableVertexAttribArray(sp->a("texCoord0")); // Włącz przesyłanie danych do atrybutu texCoord0
				glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords3);

				//zmiana tekstury na pewnej wysokosci
				if (current_y_positions[i][j][k]<2.7f) {
					glUniform1i(sp->u("textureMap0"), 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex2);
				}
				else {
					glUniform1i(sp->u("textureMap0"), 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex0);
				}

				glDrawArrays(GL_TRIANGLES, 0, vertexCount3); // Narysuj obiekt

				glDisableVertexAttribArray(sp->a("vertex")); // Wyłącz przesyłanie danych do atrybutu vertex
				glDisableVertexAttribArray(sp->a("normal")); // Wyłącz przesyłanie danych do atrybutu normal
				glDisableVertexAttribArray(sp->a("texCoord0")); // Wyłącz przesyłanie danych do atrybutu texCoord0
				// Resetowanie macierzy modelu
				Ms = glm::mat4(1.0f);
			}
		}
	}
}

//dane o polozeniu obiektu w przestrzeni xyz
std::vector<std::vector<std::vector<float>>> current_y_positions2(10, std::vector<std::vector<float>>(30, std::vector<float>(20, 2.4f)));
std::vector<std::vector<std::vector<float>>> current_x_positions2(10, std::vector<std::vector<float>>(30, std::vector<float>(20, -0.0f)));
std::vector<std::vector<std::vector<float>>> current_z_positions2(10, std::vector<std::vector<float>>(30, std::vector<float>(20, 0.0f))); 
//dane o kierunku lotu obiektu w przestrzeni xyz(1 góra, -1 dół)
std::vector<std::vector<std::vector<float>>> direction(100, std::vector<std::vector<float>>(30, std::vector<float>(20, 1))); 

float move_speed2 = 0.01f; // Prędkość ruchu w osi y
//resetowanie polozenai wszystkich obiektow danego typu, nieuzywane aktualnie
/*void restartxyz() {
	current_y_positions2 = std::vector<std::vector<std::vector<float>>>(10, std::vector<std::vector<float>>(30, std::vector<float>(20, 2.4f)));
	current_x_positions2 = std::vector<std::vector<std::vector<float>>>(10, std::vector<std::vector<float>>(30, std::vector<float>(20, -0.0f)));
	current_z_positions2 = std::vector<std::vector<std::vector<float>>>(10, std::vector<std::vector<float>>(30, std::vector<float>(20, 0.0f)));
	direction = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(30, std::vector<float>(20, 1)));
}*/
//rysowanie kamieni
void render2(GLFWwindow* window, float angle_x, float angle_y, glm::mat4 M) {
	//
	float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	
	//zmiana polozenia obiektu
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 30; ++j) {
			for (int k = 0; k < 20; k++) {
				//dla wiekszej roznicy w obiektach zmiana zmiennych
				int alfa2;
				if (j % 2 == 0)
					alfa2 = j;
				else
					alfa2 = -j;

				int beta2;
				if (j % 3 == 0)
					beta2 = j;
				else
					beta2 = -j;

				//zmiana polozenia w przestrzeni xyz
				current_x_positions2[i][j][k] += move_speed2 * i *j *2 *alfa2;
				current_y_positions2[i][j][k] += move_speed2 * j * i * direction[i][j][k];
				current_z_positions2[i][j][k] += move_speed2 * j * cos(2 * PI * i)*2;

				//dla k nieparzystych kierunek odwrocony
				if (k % 2 == 1)
					current_z_positions2[i][j][k] *= (-1);

				//obiekty powyzej 3.0f maja zmieniony kierunek lotu
				if (current_y_positions2[i][j][k] > 3.0f) {
					direction[i][j][k] = -1*sqrt(i*i*i*i*j);
				}

				//obiekty ponizej 0.0f wracaja do pozycji startowej(kazdy z osobna)
				if (current_y_positions2[i][j][k] <0.0f) {
					current_x_positions2[i][j][k] = 0.0f;
					current_y_positions2[i][j][k] = 2.4f;
					current_z_positions2[i][j][k] = 0.0f;
					direction[i][j][k] = 1;
				}
			}
		}
	}


	// Wyliczenie macierzy rzutowania i widoku
	glm::mat4 P2 = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	glm::mat4 V2 = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	
	// Aktywacja programu cieniującego
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P2)); // Załadowanie macierzy rzutowania do programu cieniującego
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V2)); // Załadowanie macierzy widoku do programu cieniującego

	glm::mat4 Ms2 = glm::mat4(1.0f); // Macierz słońca to macierz jednostkowa

	//rysowanie kamieni dla kazdej pozycji
	for (int i = 1; i < 2; ++i) {
		for (int j = 1; j < 5; ++j) {
			for (int k = 1; k < 2; k++) {
				//przesuniecie obiektu zgodnie z aktualną pozycja
				glm::vec3 currentPosition2 = glm::vec3(current_x_positions2[i][j][k],
					current_y_positions2[i][j][k],
					current_z_positions2[i][j][k]);

				//rotacja pozycji przez angle x/y
				glm::mat4 rotationMatrix2 = glm::rotate(glm::mat4(1.0f), angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix2 = glm::rotate(rotationMatrix2, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
				glm::vec3 rotatedPosition2 = glm::vec3(rotationMatrix2 * glm::vec4(currentPosition2, 1.0f));

				//translate obiektu wzgledem wyliczonej rotatedpostion(ruch obiektu)
				Ms2 = glm::translate(glm::mat4(1.0f), rotatedPosition2);	
				//skalowanie obiektu
				Ms2 = glm::scale(Ms2, glm::vec3(0.5f, 0.5f, 0.5f));

				// Pomnóż macierz modelu razy macierze obrotu
				Ms2 = glm::rotate(Ms2, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
				Ms2 = glm::rotate(Ms2, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));

				// Załadowanie macierzy modelu do programu cieniującego
				glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms2));


				sp->use(); // Aktywacja programu cieniującego
				// Przeslij parametry programu cieniującego do karty graficznej
				glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P2));
				glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V2));
				glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms2));

				glEnableVertexAttribArray(sp->a("vertex")); // Włącz przesyłanie danych do atrybutu vertex
				glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices5); // Wskaż tablicę z danymi dla atrybutu vertex

				glEnableVertexAttribArray(sp->a("normal")); // Włącz przesyłanie danych do atrybutu normal
				glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals5); // Wskaż tablicę z danymi dla atrybutu normal

				glEnableVertexAttribArray(sp->a("texCoord0")); // Włącz przesyłanie danych do atrybutu texCoord0
				glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords5);


				//teksturowanie
				glUniform1i(sp->u("textureMap0"), 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex1);

				glDrawArrays(GL_TRIANGLES, 0, vertexCount5); // Narysuj obiekt

				glDisableVertexAttribArray(sp->a("vertex")); // Wyłącz przesyłanie danych do atrybutu vertex
				glDisableVertexAttribArray(sp->a("normal")); // Wyłącz przesyłanie danych do atrybutu normal
				glDisableVertexAttribArray(sp->a("texCoord0")); // Wyłącz przesyłanie danych do atrybutu texCoord0
				// Resetowanie macierzy modelu
				Ms2 = glm::mat4(1.0f);
			}
		}
	}
}

//wektory 3d o polozeniu obiektow w przestrzeni xyz
std::vector<std::vector<std::vector<float>>> current_y_positions3(20, std::vector<std::vector<float>>(20, std::vector<float>(20, 2.5f)));
std::vector<std::vector<std::vector<float>>> current_x_positions3(20, std::vector<std::vector<float>>(20, std::vector<float>(20, 0.2f)));
std::vector<std::vector<std::vector<float>>> current_z_positions3(20, std::vector<std::vector<float>>(30, std::vector<float>(20, -0.1f))); 
std::vector<std::vector<std::vector<float>>> direction3(100, std::vector<std::vector<float>>(30, std::vector<float>(20, 1))); 
float move_speed3 = 0.0006f; 

//resetowanie polozenia obiektow
void restartxyz3() {
	current_y_positions3 = std::vector<std::vector<std::vector<float>>>(10, std::vector<std::vector<float>>(30, std::vector<float>(20, 2.5f)));
	current_x_positions3 = std::vector<std::vector<std::vector<float>>>(10, std::vector<std::vector<float>>(30, std::vector<float>(20, 0.2f)));
	current_z_positions3 = std::vector<std::vector<std::vector<float>>>(10, std::vector<std::vector<float>>(30, std::vector<float>(20, -0.1f)));
	direction3 = std::vector<std::vector<std::vector<float>>>(100, std::vector<std::vector<float>>(30, std::vector<float>(20, 1)));
}
//rysowanie lawy
void render3(GLFWwindow* window, float angle_x, float angle_y, glm::mat4 M) {
	
	float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	
	//wwyliczenie poloenia lawy
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 13; ++j) {
			for (int k = 0; k < 20; k++) {
				//zmiana znaku wartosci dla wiekszej roznorodnosci
				int alfa, beta;
				if (i % 2 == 0)
					alfa = i;
				else
					alfa = -i;
				if (k % 2 == 0)
					beta = k;
				else
					beta = -k;

				//wyliczenie pozycji xyz
				current_x_positions3[i][j][k] += move_speed3 * sqrt(i) * j * k / 100 * sqrt(k)*beta*alfa;
				current_y_positions3[i][j][k] += move_speed3 * j*k*1.5  * k *k / 300*direction3[i][j][k];
				current_z_positions3[i][j][k] += move_speed3 * j * sin(2 * PI * i) * 10 * k * k  * i*k*k*beta * sqrt(k);

				//zmiana kierunku lotu powyzej 2.7f
				if (current_y_positions3[i][j][k] > 2.7f) {
					direction3[i][j][k] = -1 * sqrt(i * i * i * i * j);
				}

				//powrot do pozycji startowej obiektu dla wysokosci ponizej 2.0f
				if (current_y_positions3[i][j][k] < 2.0f) {
					current_x_positions3[i][j][k] = 0.0f;
					current_y_positions3[i][j][k] = 2.5f;
					current_z_positions3[i][j][k] = 0.0f;
					direction[i][j][k] = 1;
				}
			}
		}
	}

	// Wyliczenie macierzy rzutowania i widoku
	glm::mat4 P3= glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	glm::mat4 V3 = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


	// Aktywacja programu cieniującego
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P3)); // Załadowanie macierzy rzutowania do programu cieniującego
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V3)); // Załadowanie macierzy widoku do programu cieniującego

	glm::mat4 Ms3 = glm::mat4(1.0f); // Macierz słońca to macierz jednostkowa


	//rysowanie obiektow
	for (int i = 1; i < 3; ++i) {
		for (int j = 0; j < 10; ++j) {
			for (int k = 1; k < 20; k++) {
				//przesunięcie obiektu zgodnie z aktualną pozycjaa
				glm::vec3 currentPosition3 = glm::vec3(current_x_positions3[i][j][k],
					current_y_positions3[i][j][k],
					current_z_positions3[i][j][k]);

				//rotacja na podstawie angle x/y
				glm::mat4 rotationMatrix3 = glm::rotate(glm::mat4(1.0f), angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix3 = glm::rotate(rotationMatrix3, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
				glm::vec3 rotatedPosition3 = glm::vec3(rotationMatrix3 * glm::vec4(currentPosition3, 1.0f));

				//translacja obiektu
				Ms3 = glm::translate(glm::mat4(1.0f), rotatedPosition3);
				//skalowanie obiektu
				Ms3 = glm::scale(Ms3, glm::vec3(0.1f, 0.1f, 0.1f));

				//idk relikt
				// Pomnóż macierz modelu razy macierze obrotu
				//Ms = glm::rotate(Ms, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
				//Ms = glm::rotate(Ms, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));

				// Załadowanie macierzy modelu do programu cieniującego
				glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms3));

				sp->use(); // Aktywacja programu cieniującego
				// Przeslij parametry programu cieniującego do karty graficznej
				glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P3));
				glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V3));
				glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms3));

				glEnableVertexAttribArray(sp->a("vertex")); // Włącz przesyłanie danych do atrybutu vertex
				glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices3); // Wskaż tablicę z danymi dla atrybutu vertex

				glEnableVertexAttribArray(sp->a("normal")); // Włącz przesyłanie danych do atrybutu normal
				glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals3); // Wskaż tablicę z danymi dla atrybutu normal

				glEnableVertexAttribArray(sp->a("texCoord0")); // Włącz przesyłanie danych do atrybutu texCoord0
				glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords3);

				//teksturowanie
				glUniform1i(sp->u("textureMap0"), 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex2);
				
				
				glDrawArrays(GL_TRIANGLES, 0, vertexCount3); // Narysuj obiekt

				glDisableVertexAttribArray(sp->a("vertex")); // Wyłącz przesyłanie danych do atrybutu vertex
				glDisableVertexAttribArray(sp->a("normal")); // Wyłącz przesyłanie danych do atrybutu normal
				glDisableVertexAttribArray(sp->a("texCoord0")); // Wyłącz przesyłanie danych do atrybutu texCoord0
				// Resetowanie macierzy modelu
				Ms3 = glm::mat4(1.0f);
			}
		}
	}
}

//licznik
long long counter = 0;


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości pikseli


	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wyliczenie macierzy rzutowania
	//maksymalne drganie
	float maxJitter = 0.05f;

	//wyliczenie losowego trzesienia sie dla kazdego xyz
	float jitterX = ((float)rand() / RAND_MAX) * 2.0f * maxJitter - maxJitter;
	float jitterY = ((float)rand() / RAND_MAX) * 2.0f * maxJitter - maxJitter;
	float jitterZ = ((float)rand() / RAND_MAX) * 2.0f * maxJitter - maxJitter;

	//implikacja drgania do kamery
	glm::vec3 jitter(jitterX, jitterY, jitterZ);
	cameraPos += jitter;
	glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    

	sp->use(); // Aktywacja programu cieniującego
    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle_x, glm::vec3(1.0f, 0.0f, 0.0f)); //rotacja wokol obiektu
	M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); //rotacja wokol obiektu
	M = glm::translate(M, glm::vec3(0.0f, 0.1f, 0.3f)); //przesuniecie obiektu
	
    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

    glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,vertices); //Wskaż tablicę z danymi dla atrybutu vertex


	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord0
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	//teksturowanie
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);

	
    glDrawArrays(GL_TRIANGLES,0, vertexCount); //Narysuj obiekt


    glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	//glDisableVertexAttribArray(sp->a("color"));  //Wyłącz przesyłanie danych do atrybutu color
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));   //Wyłącz przesyłanie danych do atrybutu normal
	//restart obiektu z render3 po 40 powtorzeniach
	if (counter % 40 == 0) {
		restartxyz3();
	}
	//zaczecie sie eruopcji po chwili
	if (counter > 100) {
		render(window, angle_x, angle_y, M);
		
	}
	if (counter > 120) {
		render3(window, angle_x, angle_y, M);
	}
	if (counter > 140) {
		render2(window, angle_x, angle_y, M);
	}

	counter++;
	//relikt, testart obiektu1 po 170 obrotach
	/*if (counter % 170 == 0) {
		restartxyz();
	}*/


    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni

}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x=0; //Aktualny kąt obrotu obiektu
	float angle_y=0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
        angle_x+=speed_x*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        angle_y+=speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        glfwSetTime(0); //Zeruj timer
		drawScene(window,angle_x,angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
