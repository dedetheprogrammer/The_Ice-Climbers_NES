#include "raylib.h"
#include "colisiones.h"
#include "iostream"
using namespace std;

int main()
{
	cout << "Test:" << endl;
	Collider c1 = Collider(Rectangulo({0.0, 0.0}, {10.0, 10.0}), {-1.0, 0.0});
	Collider c2 = Collider(Rectangulo({3.0, 0.0}, {10.0, 10.0}), {-2.0, 0.0});
	Collision col = CollisionHelper::CollidesV(c1, c2);
	cout << "Collides?: " << col.valid
		<< "Normal: " << col.colNormal.x << ", " << col.colNormal.y;
}