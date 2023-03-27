/*
*	Proyecto de pruebas de Gabriel. El mal c�digo introducido ser� castigado con la muerte.
*	Quedas avisado.
*/

#include "raylib.h"

#include "colisiones.h"

#include <sstream>

float SCREEN_WIDTH = 1280;
float SCREEN_HEIGHT = 720;

struct Bounds
{
	Collider top, bot, left, right;
	Bounds()
	{
		top = Collider(Rectangulo{ 100, 100, SCREEN_WIDTH - 200, 2 });
		bot = Collider(Rectangulo{ 100, SCREEN_HEIGHT - 100, SCREEN_WIDTH - 200, 2 });
		left = Collider(Rectangulo{ 100, 100, 2, SCREEN_HEIGHT - 200 });
		right = Collider(Rectangulo{ SCREEN_WIDTH - 100, 100, 2, SCREEN_HEIGHT - 200 });
	}
	void Draw()
	{
		top.shape.Draw(YELLOW);
		bot.shape.Draw(ORANGE);
		left.shape.Draw(GREEN);
		right.shape.Draw(SKYBLUE);
	}
} bounds;

struct Colisionador
{
	Collider collider;
	Collision collision = Collision(false);
	float radians;
	Color color;

	void Update(Bounds bounds, Collider otherColl)
	{
		collision = Collision(false);
		/*
		if(CollisionHelper::Collides(collider.shape, bounds.top.shape)){
			collision.valid = true;
			normal = {0.0, 1.0};
		}
		else if(CollisionHelper::Collides(collider.shape, bounds.bot.shape)){
			collision.valid = true;
			normal = {0.0, -1.0};
		}
		else if(CollisionHelper::Collides(collider.shape, bounds.left.shape)){
			collision.valid = true;
			normal = {1.0, 0.0};
		}
		else if(CollisionHelper::Collides(collider.shape, bounds.right.shape)){
			collision.valid = true;
			normal = {-1.0, 0.0};
		}
		else{
			collision.valid = false;
		}

		if(Mod(normal) > 0){
			normal = Norm(normal);

			collider.speed = Reflect(collider.speed, normal);
		}*/
		Collision colAux = CollisionHelper::CollidesV(collider, bounds.top);
		if (colAux.valid)
		{
			collision.valid = true;
			collision.colNormal = collision.colNormal + colAux.colNormal;
			collision.debugMsg = collision.debugMsg + colAux.debugMsg;
		}

		colAux = CollisionHelper::CollidesV(collider, bounds.bot);
		if (colAux.valid) 
		{
			collision.valid = true;
			collision.colNormal = collision.colNormal + colAux.colNormal;
			collision.debugMsg = collision.debugMsg + colAux.debugMsg;
		}
		
		colAux = CollisionHelper::CollidesV(collider, bounds.left);
		if (colAux.valid)
		{
			collision.valid = true;
			collision.colNormal = collision.colNormal + colAux.colNormal;
			collision.debugMsg = collision.debugMsg + colAux.debugMsg;
		}

		colAux = CollisionHelper::CollidesV(collider, bounds.right);
		if (colAux.valid)
		{
			collision.valid = true;
			collision.colNormal = collision.colNormal + colAux.colNormal;
			collision.debugMsg = collision.debugMsg + colAux.debugMsg;
		}
		
		if(Mod(collision.colNormal) > 0)
		{
			collision.colNormal = Norm(collision.colNormal);
			collider.speed = Reflect(collider.speed, collision.colNormal);
			collision.debugMsg = collision.debugMsg + colAux.debugMsg;
		}
		

		// si me salgo vuelta al centro
		if (collider.shape.x + collider.shape.width < 0 || collider.shape.x > SCREEN_WIDTH)
		{
			radians = (float)GetRandomValue(0, PI * 200) / 100.0f;
			collider.speed = { std::cos(radians) * 40, std::sin(radians) * 40 };
			collider.shape.x = SCREEN_WIDTH / 2;
		}
		if (collider.shape.y + collider.shape.height < 0 || collider.shape.y > SCREEN_HEIGHT)
		{
			radians = (float)GetRandomValue(0, PI * 200) / 100.0f;
			collider.speed = { std::cos(radians) * 40, std::sin(radians) * 40 };
			collider.shape.y = SCREEN_HEIGHT / 2;
		}
		// QUADRANTS
		// 1: [0, PI/2]
		// 2: [PI/2, PI]
		// 3: [PI, 3PI/2]
		// 4: [3PI/2, 2PI]

		/*
		if (CollisionHelper::Collides(shape, bounds.top))
		{
			// 2, 1
			if (dir.x < 0) radians = GetRandomValue(PI * 100, PI * 150) / 100.0f;
			else radians = GetRandomValue(PI * 150, PI * 200) / 100.0f;
			dir = { std::cos(radians) * 5, -std::sin(radians) * 5 };
		}
		if (CollisionHelper::Collides(shape, bounds.bot))
		{
			// 3, 4
			if (dir.x < 0) radians = GetRandomValue(PI * 50, PI * 100) / 100.0f;
			else radians = GetRandomValue(0, PI * 50) / 100.0f;
			dir = { std::cos(radians) * 5, -std::sin(radians) * 5 };
		}
		if (CollisionHelper::Collides(shape, bounds.left))
		{
			// 4, 1
			if (dir.y > 0) radians = GetRandomValue(PI * 150, PI * 200) / 100.0f;
			else radians = GetRandomValue(0, PI * 50) / 100.0f;
			dir = { std::cos(radians) * 5, -std::sin(radians) * 5 };
		}
		if (CollisionHelper::Collides(shape, bounds.right))
		{
			// 3, 2
			if (dir.y > 0) radians = GetRandomValue(PI * 100, PI * 150) / 100.0f;
			else radians = GetRandomValue(PI * 50, PI * 100) / 100.0f;
			dir = { std::cos(radians) * 5, -std::sin(radians) * 5 };
		}
		if (CollisionHelper::Collides(shape, other1.shape)) {
			dir.x = -dir.x;
			dir.y = -dir.y;
		}
		*/
		collider.shape.x += collider.speed.x;
		collider.shape.y += collider.speed.y;
	}
	void Draw()
	{
		collider.shape.Draw(color);
	}
};

int main() {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Prueba");
	SetTargetFPS(60);

	Colisionador c1;
	c1.collider = Collider(Rectangle{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 50 });
	c1.radians = (float)GetRandomValue(0, PI * 200) / 100.0f;
	c1.collider.speed = { std::cos(c1.radians) * 40, std::sin(c1.radians) * 40 };
	c1.color = RED;

	Colisionador c2;
	c2.collider = Collider(Rectangle{ SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 200, 100, 50 });
	c2.radians = (float)GetRandomValue(0, PI * 200) / 100.0f;
	c2.collider.speed = { std::cos(c2.radians) * 40, std::sin(c2.radians) * 40 };
	c2.color = BLUE;

	int frame = 0;
	// Game Loop
	while (!WindowShouldClose())
	{
		// Update
		frame = (frame + 1) % 60;

		if(frame == 0)
		{
			c1.Update(bounds, c2.collider/*, c3*/);
			c2.Update(bounds, c1.collider/*, c3*/);
		}
		
		// Draw
		BeginDrawing();
		{
			// Clear
			ClearBackground(BLACK);

			
			std::stringstream text1;
			text1 << "Normal: " << c1.collision.colNormal.x << " " << c1.collision.colNormal.y << "}\n"
				<< "collides: " << c1.collision.valid << "}\n"
				<< "cos: " << std::cos(c1.radians) << "}\n"
				<< "sin: " << std::sin(c1.radians) << "}\n"
				<< "dir: {" << c1.collider.speed.x << "," << c1.collider.speed.y << "}\n"
				<< "Debug: " << c1.collision.debugMsg << "\n";

			std::stringstream text2;
			text2 << "Normal: " << c2.collision.colNormal.x << " " << c2.collision.colNormal.y << "}\n"
				<< "collides: " << c2.collision.valid << "}\n"
				<< "cos: " << std::cos(c2.radians) << "}\n"
				<< "sin: " << std::sin(c2.radians) << "}\n"
				<< "dir: {" << c2.collider.speed.x << "," << c2.collider.speed.y << "}\n"
				<< "Debug: " << c2.collision.debugMsg << "\n";

			DrawText(text1.str().c_str(), 100, 100, 20, RED);
			DrawText(text2.str().c_str(), 800, 100, 20, BLUE);
			

			// Bounds
			bounds.Draw();

			// Objects
			c1.Draw();
			c2.Draw();
		}
		EndDrawing();
	}

	CloseWindow();
}