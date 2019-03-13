#include <iostream>
using namespace std;

#include <windows.h>
#include <chrono>

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

float fFOV = 3.14159 / 4.0;
float fDepth = 16.0f;

int main()
{
	// Create screen buffer
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map;
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#......#########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";


	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();
		// Controls 
		// Handle CCW Rotation
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (.8f) *fElapsedTime;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (.8f) * fElapsedTime;
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerX += sinf(fPlayerA)*5.0f*fElapsedTime;
			fPlayerY += cosf(fPlayerA)*5.0f*fElapsedTime;
			
			// Collision detection walking forwards
			if (map[(int)fPlayerY*nMapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX -= sinf(fPlayerA)*5.0f*fElapsedTime;
				fPlayerY -= cosf(fPlayerA)*5.0f*fElapsedTime;
			}


		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerX -= sinf(fPlayerA)*5.0f*fElapsedTime;
			fPlayerY -= cosf(fPlayerA)*5.0f*fElapsedTime;

			// Collision detection, walking backwards
			if (map[(int)fPlayerY*nMapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX += sinf(fPlayerA)*5.0f*fElapsedTime;
				fPlayerY += cosf(fPlayerA)*5.0f*fElapsedTime;
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			// for each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool bHitWall = false;

			float fEyeX = sinf(fRayAngle); // unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);


			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY+ fEyeY * fDistanceToWall);

				// test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true; // just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// ray is inbounds so test to see if the ray cell is a wall block
					if (map[nTestY * nMapWidth + nTestX] == '#')
					{
						bHitWall = true;
					}
				}
			}

			// calculate distance to ceiling and floor
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			if (fDistanceToWall <= fDepth / 4.0f)	nShade = 0x2588;
			else if (fDistanceToWall < fDepth / 3.0f)	nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.0f)	nShade = 0x2592;
			else if (fDistanceToWall < fDepth)	nShade = 0x2591;
			else nShade = ' ';




			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling) // interestingly if you remove this = so it is just <, the walls stop dislplaying, are black
					screen[y*nScreenWidth + x] = ' ';
				else if (y> nCeiling && y< nFloor)
					screen[y*nScreenWidth + x] = nShade;
				else 
				{
					// shade floor based on distance
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25) nShade = '#';
					else if (b < 0.5) nShade = 'X';
					else if (b < 0.75) nShade = '.';
					else if (b < 0.9) nShade = '-';
					else  nShade = ' ';
					screen[y*nScreenWidth + x] = nShade;
				}
			}
		}
		
		screen[nScreenWidth*nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);




	}





	return 0;
}

/* Questions to answer
Why is the screen height and width different from the map? 

*/