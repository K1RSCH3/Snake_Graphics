// slashes prob mean to be found or TBD or for precautions zzz

#include <iostream>
#include <raylib.h>
#include <deque>  //deque is like add and removing thing (sa snek)
#include <raymath.h>

using namespace std;


Color neonpink = { 255, 97, 248, 255 };
int cellsize = 30;
int cellcount = 25;
int offset = 75;

double lastUpdateTime = 0; //time last update

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(deque[i], element))
		{
			return true;
		}
	}
	return false;
}

bool eventTriggered(double interval) //its like, checking time after and update, bacially like an update rate too
{
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snek //also movement
{
public:
	deque<Vector2> body = { Vector2{6, 9}, Vector2 {5,9}, Vector2{4,9} };
	Vector2 direction = { 1,0 }; //MOVEMENT
	bool addSegment = false;

	void Draw()
	{
		for (unsigned int i = 0; i < body.size(); i++)
		{
			int x = body[i].x;
			int y = body[i].y;
			DrawEllipse( offset + x * cellsize, offset + y * cellsize, cellsize / 1.5, cellsize / 1.5, neonpink);
		}
	}
	//BODY
	void Update()
	{
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment == true) //add segment when eaten food
		{
			addSegment = false;
		}else
		{
			body.pop_back();
		}
		
	}
	void Reset()
	{
		body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
		direction = { 1,0 };
	}
};


class Food 
{

public:
	Vector2 position; //position x and position y, in cells (WALA NANG COORDS KASI GENERATE RANDOM NA)
	float x = 0, y = 0;
	Texture2D texture;

	Food (deque<Vector2> snakeBody)
	{
		position = GenerateRandomPos(snakeBody);
	}


	void Draw()
	{
		//position = GenerateRandomPos();
		DrawCircle(offset + position.x * cellsize, offset + position.y * cellsize, cellsize / 2, RED);
	}

	Vector2 GenerateRandomCell()
	{
		float x = GetRandomValue(1, cellcount - 1);
		float y = GetRandomValue(+2, cellcount -1); //i think okay na?
		return Vector2{ x,y };
	}
	                   
	Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
	{
		Vector2 position = GenerateRandomCell();
		while (ElementInDeque(position, snakeBody)) //random position = segment of snek
		{
			position = GenerateRandomCell();
		}
		return position;
	}
 
};


class Game //basically put everything here for convenience
{
public:
	Snek snake = Snek();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;
	Sound nomnom;
	Sound wallboogsh;
	
	Game()
	{
		InitAudioDevice();
		nomnom = LoadSound("C:/Users/ndram/Desktop/1T Finals Graphics/nomnom.mp3");
		wallboogsh = LoadSound("C:/Users/ndram/Desktop/1T Finals Graphics/wallcollision.mp3");
	}
	~Game()
	{
		UnloadSound(nomnom);
		UnloadSound(wallboogsh);
		CloseAudioDevice();
	}

	void Draw()
	{
		food.Draw();
		snake.Draw();
	}

	void Update()
	{
		if (running)
		{
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdges();
			CheckCollisionWithTail();
		}
	}

	void CheckCollisionWithFood()
	{
		if (Vector2Equals(snake.body[0], food.position)) 
		{
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score++;
			PlaySound(nomnom);
		}
	}

	void CheckCollisionWithEdges()
	{
		if (snake.body[0].x == cellcount-0 || snake.body[0].x == 0)
		{
			GameOver();
		}
		if (snake.body[0].y == cellcount+0 || snake.body[0].y == 0)
		{
			GameOver();
		}

	}
	void GameOver() //basically retries from original position if game ends
	{
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		running = false;
		score = 0;
		PlaySound(wallboogsh);
	}
	void CheckCollisionWithTail()
		{
			deque<Vector2> headlessBody = snake.body;
			headlessBody.pop_front();
			if (ElementInDeque(snake.body[0], headlessBody))
			{
				GameOver();
			}
		}
};
int main()
{
	cout << "starting..." << endl;
	InitWindow(2*offset + cellsize*cellcount, 2*offset +cellsize*cellcount, "snek geym");
	SetTargetFPS(60);
	
	Game game = Game();

	while (WindowShouldClose() == false)
	{
		// BEGIN
		BeginDrawing();

		//UPDATES
		if (eventTriggered(0.2))
		{
			game.Update();
		}

		if (IsKeyPressed(KEY_UP) && game.snake.direction.y !=1)    //&& function here basically stops the zoomies and only moves snek 1 by 1
		{
			game.snake.direction = { 0, -1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
		{
			game.snake.direction = { 0,1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x !=1)
		{
			game.snake.direction = { -1, 0 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
		{
			game.snake.direction = { 1, 0 };
			game.running = true;
		}
		// DRAW
		ClearBackground(BLACK);
		DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellsize * cellcount + 10, (float)cellsize * cellcount + 10 }, 5, neonpink);;
		DrawText("CYBERSNEK", offset - 5, 20, 40, neonpink); //text, x, y, fontsize, color
		DrawText(TextFormat ("%i", game.score), offset - 5, offset + cellsize * cellcount + 10, 40, neonpink);
		game.Draw();

		// END
		EndDrawing();
	}
	CloseWindow();
	return 0;
}