// slashes prob mean to be found or TBD or for precautions zzz

#include <iostream>
#include <filesystem>
#include <raylib.h>
#include <deque> //deque is like add and removing thing (sa snek)
#include <raymath.h>
#include <vector>

using namespace std;

Color neonpink = {255, 97, 248, 255};
const int cellsize = 30;
const int cellcount = 25;
const int offset = 75;
const int borderThickness = 5;
const int playableMinCell = 1;
const int playableMaxCell = cellcount - 1;

double lastUpdateTime = 0; // time last update

filesystem::path FindAssetPath(const char *fileName)
{
	filesystem::path appDirectory = filesystem::path(GetApplicationDirectory());
	vector<filesystem::path> candidates = {
		appDirectory / fileName,
		appDirectory / "assets" / fileName,
		appDirectory / ".." / ".." / ".." / "1T Finals Graphics" / fileName,
		appDirectory / ".." / ".." / ".." / ".." / "1T Finals Graphics" / fileName};

	for (const filesystem::path &candidate : candidates)
	{
		filesystem::path normalizedCandidate = candidate.lexically_normal();
		if (filesystem::exists(normalizedCandidate))
		{
			return normalizedCandidate;
		}
	}

	return {};
}

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

bool eventTriggered(double interval) // its like, checking time after and update, basically like an update rate
{
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snek // also movement
{
public:
	deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
	Vector2 direction = {1, 0}; // MOVEMENT
	bool addSegment = false;

	void Draw()
	{
		for (unsigned int i = 0; i < body.size(); i++)
		{
			float x = body[i].x;
			float y = body[i].y;
			DrawEllipse(static_cast<int>(offset + x * cellsize), static_cast<int>(offset + y * cellsize), cellsize / 1.5f, cellsize / 1.5f, neonpink);
		}
	}
	// BODY
	void Update()
	{
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment == true) // add segment when eaten food
		{
			addSegment = false;
		}
		else
		{
			body.pop_back();
		}
	}
	void Reset()
	{
		body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
		direction = {1, 0};
		addSegment = false; // clear pending growth so a reset snake does not gain an unintended segment
	}
};

class Food
{

public:
	Vector2 position; // position x and position y, in cells (WALA NANG COORDS KASI GENERATE RANDOM NA)
	float x = 0, y = 0;
	Texture2D texture;

	Food(deque<Vector2> snakeBody)
	{
		position = GenerateRandomPos(snakeBody);
	}

	void Draw()
	{
		// position = GenerateRandomPos();
		DrawCircle(static_cast<int>(offset + position.x * cellsize), static_cast<int>(offset + position.y * cellsize), cellsize / 2.0f, RED);
	}

	Vector2 GenerateRandomCell()
	{
		float x = static_cast<float>(GetRandomValue(playableMinCell, playableMaxCell));
		float y = static_cast<float>(GetRandomValue(playableMinCell, playableMaxCell)); // keep food within the same playable rows as the snake
		return Vector2{x, y};
	}

	Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
	{
		Vector2 position = GenerateRandomCell();
		while (ElementInDeque(position, snakeBody)) // random position = segment of snek
		{
			position = GenerateRandomCell();
		}
		return position;
	}
};

class Game // basically put everything here for convenience
{
public:
	Snek snake = Snek();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;
	Sound nomnom = {};
	Sound wallboogsh = {};
	bool audioReady = false;

	Game()
	{
		InitAudioDevice();
		audioReady = IsAudioDeviceReady();
		if (!audioReady)
		{
			return;
		}
		filesystem::path nomnomPath = FindAssetPath("nomnom.mp3");
		filesystem::path wallCollisionPath = FindAssetPath("wallcollision.mp3");
		if (!nomnomPath.empty())
		{
			nomnom = LoadSound(nomnomPath.string().c_str()); // load sounds relative to the executable instead of a user-specific path
		}
		if (!wallCollisionPath.empty())
		{
			wallboogsh = LoadSound(wallCollisionPath.string().c_str());
		}
	}
	~Game()
	{
		if (audioReady)
		{
			if (nomnom.frameCount > 0)
			{
				UnloadSound(nomnom);
			}
			if (wallboogsh.frameCount > 0)
			{
				UnloadSound(wallboogsh);
			}
			CloseAudioDevice();
		}
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
			if (audioReady && nomnom.frameCount > 0)
			{
				PlaySound(nomnom);
			}
		}
	}

	void CheckCollisionWithEdges()
	{
		if (snake.body[0].x > playableMaxCell || snake.body[0].x < playableMinCell) // catch every position outside the playable cells
		{
			GameOver();
		}
		if (snake.body[0].y > playableMaxCell || snake.body[0].y < playableMinCell) // catch every position outside the playable cells
		{
			GameOver();
		}
	}
	void GameOver() // basically retries from original position if game ends
	{
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		running = false;
		score = 0;
		if (audioReady && wallboogsh.frameCount > 0)
		{
			PlaySound(wallboogsh);
		}
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
	InitWindow(2 * offset + cellsize * cellcount, 2 * offset + cellsize * cellcount, "snek geym");
	SetTargetFPS(60);

	Game game = Game();

	while (WindowShouldClose() == false)
	{
		// BEGIN
		BeginDrawing();

		// UPDATES
		if (eventTriggered(0.2))
		{
			game.Update();
		}

		Vector2 currentDirection = game.snake.direction;	 // validate all inputs against the direction from before this frame
		if (IsKeyPressed(KEY_UP) && currentDirection.y != 1) //&& function here basically stops the zoomies and only moves snek 1 by 1
		{
			game.snake.direction = {0, -1};
			game.running = true;
		}
		if (IsKeyPressed(KEY_DOWN) && currentDirection.y != -1)
		{
			game.snake.direction = {0, 1};
			game.running = true;
		}
		if (IsKeyPressed(KEY_LEFT) && currentDirection.x != 1)
		{
			game.snake.direction = {-1, 0};
			game.running = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && currentDirection.x != -1)
		{
			game.snake.direction = {1, 0};
			game.running = true;
		}
		// DRAW
		ClearBackground(BLACK);
		DrawRectangleLinesEx(Rectangle{static_cast<float>(offset - borderThickness), static_cast<float>(offset - borderThickness), static_cast<float>(cellsize * cellcount + borderThickness * 2), static_cast<float>(cellsize * cellcount + borderThickness * 2)}, borderThickness, neonpink);
		DrawText("CYBERSNEK", offset - 5, 20, 40, neonpink); // text, x, y, fontsize, color
		DrawText(TextFormat("%i", game.score), offset - 5, offset + cellsize * cellcount + 10, 40, neonpink);
		game.Draw();

		// END
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
