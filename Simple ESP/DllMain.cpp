#include <Windows.h>
#include <list>

#include <gl/GL.h>
#pragma comment(lib, "OpenGL32.lib")

// Library for hooking functions
#include "../Detours/include/detours.h"
#pragma comment(lib, "../Detours/lib.X64/detours.lib")

// Library for mathematical calculations
#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"

struct TransformMatrix
{
	glm::mat4 projection;
	glm::mat4 modelview;

	TransformMatrix()
	{
		// Automatically fill it with the data of the current matrices
		glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projection));
		glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelview));
	}
};

enum ObjectType
{
	Entity,
	Chest,
	LargeChest
};

struct ObjectData
{
	TransformMatrix transformMatrix;
	ObjectType type;
};

std::list<ObjectData> objects;

// Pointers to original functions
decltype(&glOrtho) fpglOrtho = glOrtho;
decltype(&glScalef) fpglScalef = glScalef;
decltype(&glTranslatef) fpglTranslatef = glTranslatef;

void drawBox(glm::vec4 color)
{
	glColor4f(color.r, color.g, color.b, color.a);

	glBegin(GL_LINES);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();
}

// Hooked glOrtho function
void WINAPI myglOrtho(double left, double right, double bottom, double top, double zNear, double zFar)
{
	// Maybe game preparing to draw inventory (drawing the esp)
	if (zNear == 1000.0 and zFar == 3000.0)
	{
		// Saving settings (context and current matrix)
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();

		// Setting up the context
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_LINE_SMOOTH);
		glLineWidth(2.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Transforming matrices
		for (auto& object : objects)
		{
			glm::mat4& modelview = object.transformMatrix.modelview;
			glm::mat4& projection = object.transformMatrix.projection;

			glm::vec4 color = glm::vec4(1.0f);
			glm::vec3 translate = glm::vec3(1.0f);
			glm::vec3 scale = glm::vec3(1.0f);

			switch (object.type)
			{
				case ObjectType::Entity:
					color = glm::vec4(0.18f, 1.0f, 0.52f, 1.0f);
					translate = glm::vec3(0.0f, -1.0f, 0.0f);
					scale = glm::vec3(0.5f, 1.0f, 0.5f);
				break;

				case ObjectType::Chest:
					color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					translate = glm::vec3(0.5f, 0.5f, 0.5f);
					scale = glm::vec3(0.5f, 0.5f, 0.5f);
				break;

				case ObjectType::LargeChest:
					color = glm::vec4(0.91f, 0.87f, 0.42f, 1.0f);
					translate = glm::vec3(1.0f, 0.5f, 0.5f);
					scale = glm::vec3(1.0f, 0.5f, 0.5f);
				break;
			}

			modelview = glm::translate(modelview, translate);
			modelview = glm::scale(modelview, scale);

			// Setup matrices
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(glm::value_ptr(projection));

			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(glm::value_ptr(modelview));

			// Drawing box
			drawBox(color);
		}

		objects.clear();

		// Restoring settings (context and matrix)
		glPopAttrib();
		glPopMatrix();
	}

	// Calling the original function
	fpglOrtho(left, right, bottom, top, zNear, zFar);
}

// Hooked glScalef function
void WINAPI myglScalef(float x, float y, float z)
{
	// Maybe game preparing to draw entity (player, villager or witch)
	if (x == 0.9375f and y == 0.9375f and z == 0.9375f)
		objects.push_back({ TransformMatrix(), ObjectType::Entity });

	// Calling the original function
	fpglScalef(x, y, z);
}

// Hooked glTranslatef function
void WINAPI myglTranslatef(float x, float y, float z)
{
	// Maybe game preparing to draw chest
	if (x == 0.5f and y == 0.4375f and z == 0.9375f)
		objects.push_back({ TransformMatrix(), ObjectType::Chest });

	// Maybe game preparing to draw large chest
	if (x == 1.0f and y == 0.4375f and z == 0.9375f)
		objects.push_back({ TransformMatrix(), ObjectType::LargeChest });

	// Calling the original function
	fpglTranslatef(x, y, z);
}

// Dynamic Link Library entry point
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DetourTransactionBegin();
			DetourAttach(reinterpret_cast<void**>(&fpglOrtho), myglOrtho);
			DetourAttach(reinterpret_cast<void**>(&fpglScalef), myglScalef);
			DetourAttach(reinterpret_cast<void**>(&fpglTranslatef), myglTranslatef);
			return DetourTransactionCommit() == NO_ERROR;
		}

		break;

		case DLL_PROCESS_DETACH:
		{
			DetourTransactionBegin();
			DetourDetach(reinterpret_cast<void**>(&fpglScalef), myglScalef);
			DetourDetach(reinterpret_cast<void**>(&fpglTranslatef), myglTranslatef);
			DetourDetach(reinterpret_cast<void**>(&fpglOrtho), myglOrtho);
			DetourTransactionCommit();
		}

		break;
	}

	return TRUE;
}