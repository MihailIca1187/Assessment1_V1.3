//Mihail Ica

// Assessment1.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
#include <iostream>
#include <chrono>
using namespace std;
using namespace tle;

const float kGameSpeed = 50.0f; // Constant game speed variable used for variable timing.
const int kCubes = 12; // Constant variable used to define the number of cubes on the screen at any given time.

float Random(int from, int to) // Random function provided as part of the into to programming module that provides a random number from within a specified range.
{
	return from + rand() % (to - from);
}

auto start = std::chrono::high_resolution_clock::now(); // Method from chrono library for providing time.
const float kSphereSpeed = kGameSpeed;
const float kRotationSpeed = kSphereSpeed;





void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");
	myEngine->AddMediaFolder("./Media");

	/**** Set up your scene here ****/


	/* Variables related to the position of in-game objects */
	const float cubeMaxXPos = 100;
	const float cubeMinXPos = -100;
	const float cubeMaxZPos = 100;
	const float cubeMinZPos = -100;
	float sphereRadius = 10;
	const int marginOfError = 5;
	const int kCubeSpread = 10;

	const float cubeAttraction = 50; // Constant variable that provides the range of cube 'attraction' for the hyperState.


	IMesh* waterMesh = myEngine->LoadMesh("water.x");
	IModel* waterModel = waterMesh->CreateModel(0, -5, 0);

	IMesh* islandMesh = myEngine->LoadMesh("island.x");
	IModel* islandModel = islandMesh->CreateModel(0, -5, 0);

	IMesh* sphereMesh = myEngine->LoadMesh("sphere.x");
	IModel* playerSphere = sphereMesh->CreateModel(0, 10, 0);
	playerSphere->SetSkin("regularsphere.jpg");

	IMesh* cubeMesh = myEngine->LoadMesh("minicube.x");
	IModel* cubeModel[kCubes];

	IMesh* skyMesh = myEngine->LoadMesh("sky.x");
	IModel* skyModel = skyMesh->CreateModel(0, -960, 0);



	ICamera* camera = myEngine->CreateCamera(kManual, 0, 200, 0);
	camera->RotateLocalX(90);



	enum GameStates { playing, paused, gamewon, gameover };
	GameStates gameState = playing;

	enum CameraStates { topDown, isometric };
	CameraStates cameraState = topDown;

	/* Arrays holding the bounding volume coordinates for the cubes */

	float hitBoxMinX[kCubes];
	float hitBoxMaxX[kCubes];
	float hitBoxMinY[kCubes];
	float hitBoxMaxY[kCubes];
	float hitBoxMinZ[kCubes];
	float hitBoxMaxZ[kCubes];



	bool gamePaused = false;


	IFont* scoreFont = myEngine->LoadFont("Calibri", 32);
	IFont* gameStateFont = myEngine->LoadFont("Sherrif", 72);

	float sphereSpeed;
	float rotationSpeed;

	/* Arrays holding the cube coordinates */
	float cubeXPos[kCubes];
	float cubeZPos[kCubes];

	/* Variables used for vector calculations */

	float cubeVectorX = 0;
	float cubeVectorZ = 0;
	float cubeDistanceX = 0;
	float cubeDistanceZ = 0;
	float cubeToCubeDistance = 0;
	float cubeToSphereDistance = 0;

	int playerPoints = 0;
	int sphereProgress = 0;


	bool cubeToCubeKnock = false;

	float playerSphereXPos = playerSphere->GetX();
	float playerSphereYPos = playerSphere->GetY();
	float playerSphereZPos = playerSphere->GetZ();

	int cubesLeft = 0; // Variable used to keep track of the amount of cubes left on screen.

	for (int i = 0; i < kCubes; i++) //For loop repeats for the required number of cubes.
	{

		while (true) // While loop repeats as long as cubeToSphereDistance < (kCubeSpread + sphereRadius) is true.
		{
			cubeToCubeKnock = false; //Boolean used to keep track of cube to cube collision is set to false as no cubes have been generated yet.

			cubeXPos[i] = Random(cubeMinXPos, cubeMaxXPos); // Random coordinates are generated for the 'i' cube and placed in their respective arrays.
			cubeZPos[i] = Random(cubeMinZPos, cubeMaxZPos);

			cubeVectorX = playerSphereXPos - cubeXPos[i]; //Vector between playerSphere and the 'i' cube is calculated.
			cubeVectorZ = playerSphereZPos - cubeZPos[i];

			cubeDistanceX = cubeVectorX * cubeVectorX; // The individual coordinate distance is then calculated.
			cubeDistanceZ = cubeVectorZ * cubeVectorZ;

			cubeToSphereDistance = sqrt(cubeDistanceX + cubeDistanceZ); // Finally, the cubeToSphereDistance is calculated and then checked against the criteria previously specified.

			if (cubeToSphereDistance < (kCubeSpread + sphereRadius))
			{
				continue; // Should this criteria not be met, the loop is signaled to continue in order to continue generating coordinates.
			}

			for (int j = 0; j < i; j++) // For loop repeats for the required number of cubes. Since this loop checks the vector between two cubes, it only gets entered should there be more than one cube spawned.
			{
				cubeVectorX = cubeXPos[j] - cubeXPos[i]; // Vector between the spawned cube and the cube to be spawned is calculated.
				cubeVectorZ = cubeZPos[j] - cubeZPos[i];

				cubeDistanceX = cubeVectorX * cubeVectorX; // The individual coordinate distance is then calculated.
				cubeDistanceZ = cubeVectorZ * cubeVectorZ;

				cubeToCubeDistance = sqrt(cubeDistanceX + cubeDistanceZ); // Finally, the cubeToCubeDistance is calculated and then checked against the criteria previously specified.



				if (cubeToCubeDistance < kCubeSpread)
				{
					cubeToCubeKnock = true; // Boolean for keeping track of cube to cube collision is set to true.
					break; // Should this criteria not be met, the loop is signaled to continue in order to continue generating coordinates.

				}
			}

			if (cubeToCubeKnock)
			{
				continue; // Should cubeToCubeKnock be true, the loop is signaled to continue generating coordinates.
			}


			cubesLeft++; // For each cube generated, this variable gets iterated to reflect the number of cubes spawned.
			cubeModel[i] = cubeMesh->CreateModel(cubeXPos[i], 5, cubeZPos[i]); //The 'i' cube is then spawned and the loop is exited back to the 'for' loop in order to continue the process.
			break;
		}


	}

	/* Hyper cube mechanics including random coordinates */
	float hyperCubeXPos = Random(cubeMinXPos, cubeMaxXPos);
	float hyperCubeZPos = Random(cubeMinZPos, cubeMaxZPos);

	IModel* hyperCube = cubeMesh->CreateModel(hyperCubeXPos, hyperCubeZPos);
	hyperCube->SetSkin("hypercube.jpg");

	bool hyperCubeExists = true;
	float timeHyperCube = 0;
	bool startTimer = false;


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		float frameTime = myEngine->Timer();



		/**** Update your scene each frame here ****/

		float playerSphereXPos = playerSphere->GetX();
		float playerSphereYPos = playerSphere->GetY();
		float playerSphereZPos = playerSphere->GetZ();




		/* Bounding volume definition for hyper cube */

		float hyperCubeMinX = hyperCubeXPos - sphereRadius;
		float hyperCubeMaxX = hyperCubeXPos + sphereRadius;
		float hyperCubeMinZ = hyperCubeZPos - sphereRadius;
		float hyperCubeMaxZ = hyperCubeZPos + sphereRadius;

		if (hyperCubeExists && (playerSphereXPos > hyperCubeMinX) && (playerSphereXPos < hyperCubeMaxX)
			&& (playerSphereZPos > hyperCubeMinZ) && (playerSphereZPos < hyperCubeMaxZ))
		{
			cubeMesh->RemoveModel(hyperCube);
			hyperCubeExists = false;
			startTimer = true;
			playerSphere->SetSkin("hypersphere.jpg");
			start = std::chrono::high_resolution_clock::now(); // Chrono timer gets started in order to ensure that the hyper state is only active for 5 seconds.
		}

		if (startTimer)
		{
			auto finish = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = finish - start;
			elapsed.count();

			if (elapsed.count() > 5)
			{
				startTimer = false; // StartTimer is set to false should the time elapsed surpass 5 minutes.
				playerSphere->SetSkin("regularsphere.jpg");
			}

			for (int i = 0; i < kCubes; i++) // For loop, generating new bounding volumes for the hyper state.
			{
				hitBoxMinX[i] = cubeModel[i]->GetX() - sphereRadius - cubeAttraction;
				hitBoxMaxX[i] = cubeModel[i]->GetX() + sphereRadius + cubeAttraction;
				hitBoxMinZ[i] = cubeModel[i]->GetZ() - sphereRadius - cubeAttraction;
				hitBoxMaxZ[i] = cubeModel[i]->GetZ() + sphereRadius + cubeAttraction;

				if ((playerSphereXPos > hitBoxMinX[i]) && (playerSphereXPos < hitBoxMaxX[i]) // Collision is verified and the cubes within the previously updated 'hyper' bounding volumes move towards the cube.
					&& (playerSphereZPos > hitBoxMinZ[i]) && (playerSphereZPos < hitBoxMaxZ[i]))
				{
					float vectorX;
					float vectorZ;
					vectorX = playerSphere->GetX() - cubeModel[i]->GetX();
					vectorZ = playerSphere->GetZ() - cubeModel[i]->GetZ();

					float distanceX = vectorX * vectorX; // As previously, distance vector gets calculated for the playerSphere and cubes.
					float distanceZ = vectorZ * vectorZ;

					float distance = sqrt(distanceX + distanceZ);

					float directionX = vectorX / distance; // Additionally, the direction vector gets calculated in order to ensure the cubes move in the right direction.
					float directionZ = vectorZ / distance;

					cubeModel[i]->MoveX(directionX * cubeAttraction * frameTime); // Cubes move accordingly.
					cubeModel[i]->MoveZ(directionZ * cubeAttraction * frameTime);

				}
			}
		}



		for (int i = 0; i < kCubes; i++) // For loop, used for defining bounding volumes for cubes.
		{

			hitBoxMinX[i] = cubeModel[i]->GetX() - sphereRadius;
			hitBoxMaxX[i] = cubeModel[i]->GetX() + sphereRadius;
			hitBoxMinZ[i] = cubeModel[i]->GetZ() - sphereRadius;
			hitBoxMaxZ[i] = cubeModel[i]->GetZ() + sphereRadius;

			if ((playerSphereXPos > hitBoxMinX[i]) && (playerSphereXPos < hitBoxMaxX[i])
				&& (playerSphereZPos > hitBoxMinZ[i]) && (playerSphereZPos < hitBoxMaxZ[i]))
			{

				playerPoints += 10; //Should collision occur between playerSphere and any cube, the player gets awarded 10 points, sphere progress gets iterated and the cube in question is removed.
				sphereProgress += 1;
				cubeMesh->RemoveModel(cubeModel[i]);
				cubesLeft--;

				while (true) // Same cube spawning mechanics used in the scene setup. Wanted to turn this into a function, however, ran out of time unfortunately :( .
				{
					cubeToCubeKnock = false;

					cubeXPos[i] = Random(cubeMinXPos, cubeMaxXPos);
					cubeZPos[i] = Random(cubeMinZPos, cubeMaxZPos);

					cubeVectorX = playerSphereXPos - cubeXPos[i];
					cubeVectorZ = playerSphereZPos - cubeZPos[i];

					cubeDistanceX = cubeVectorX * cubeVectorX;
					cubeDistanceZ = cubeVectorZ * cubeVectorZ;

					cubeToSphereDistance = sqrt(cubeDistanceX + cubeDistanceZ);

					if (cubeToSphereDistance < (kCubeSpread + sphereRadius))
					{
						continue;
					}

					for (int j = 0; j < kCubes; j++)
					{
						if (j == i)
						{
							continue;
						}

						cubeVectorX = cubeXPos[j] - cubeXPos[i];
						cubeVectorZ = cubeZPos[j] - cubeZPos[i];

						cubeDistanceX = cubeVectorX * cubeVectorX;
						cubeDistanceZ = cubeVectorZ * cubeVectorZ;

						cubeToCubeDistance = sqrt(cubeDistanceX + cubeDistanceZ);



						if (cubeToCubeDistance < kCubeSpread)
						{
							cubeToCubeKnock = true;
							break;

						}
					}

					if (cubeToCubeKnock)
					{
						continue;
					}


					cubesLeft++;
					cubeModel[i] = cubeMesh->CreateModel(cubeXPos[i], 5, cubeZPos[i]);
					break;
				}


			}
		}


		/*Game State actions*/
		if (gameState == playing)
		{
			sphereSpeed = kSphereSpeed * frameTime;
			rotationSpeed = kRotationSpeed * frameTime;
		}

		else if (gameState == gameover)
		{
			sphereSpeed = 0;
			rotationSpeed = 0;
			stringstream gameOverText;
			gameOverText << "Game Over!";
			gameStateFont->Draw(gameOverText.str(), myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);

		}
		else if (gameState == gamewon)
		{

			sphereSpeed = 0;
			rotationSpeed = 0;
			stringstream gameWonText;
			gameWonText << "You won!";
			gameStateFont->Draw(gameWonText.str(), myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kBlue, kCentre);

		}
		else if (gameState == paused)
		{
			sphereSpeed = 0;
			rotationSpeed = 0;
			stringstream gamePausedText;
			gamePausedText << "Paused.";
			gameStateFont->Draw(gamePausedText.str(), myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kGreen, kCentre);
		}



		/*playerSphere movement controls*/
		if (myEngine->KeyHeld(Key_W))
		{
			playerSphere->MoveLocalZ(sphereSpeed);
		}
		if (myEngine->KeyHeld(Key_S))
		{
			playerSphere->MoveLocalZ(-sphereSpeed);
		}
		if (myEngine->KeyHeld(Key_D))
		{
			playerSphere->RotateY(rotationSpeed);
		}
		if (myEngine->KeyHeld(Key_A))
		{
			playerSphere->RotateY(-rotationSpeed);
		}

		/*camera movement controls*/

		if (cameraState == topDown)
		{
			if (myEngine->KeyHeld(Key_Up))
			{
				camera->MoveZ(sphereSpeed);
			}
			if (myEngine->KeyHeld(Key_Down))
			{
				camera->MoveZ(-sphereSpeed);
			}
			if (myEngine->KeyHeld(Key_Right))
			{
				camera->MoveX(sphereSpeed);
			}
			if (myEngine->KeyHeld(Key_Left))
			{
				camera->MoveX(-sphereSpeed);
			}
			if (myEngine->KeyHit(Key_2))
			{
				camera->ResetOrientation();
				camera->SetPosition(150, 150, -150);
				camera->RotateLocalY(-45);
				camera->RotateLocalX(45);
				cameraState = isometric;
			}
		}

		if (cameraState == isometric)
		{
			if (myEngine->KeyHit(Key_1))
			{
				camera->ResetOrientation();
				camera->SetPosition(0, 200, 0);
				camera->RotateLocalX(90);
				cameraState = topDown;
			}


		}

		/*Out of bounds checks*/
		if (playerSphereXPos > cubeMaxXPos + marginOfError)

		{
			gameState = gameover;

		}
		else if (playerSphereXPos < cubeMinXPos - marginOfError)
		{
			gameState = gameover;
		}
		else if (playerSphereZPos > cubeMaxZPos + marginOfError)
		{
			gameState = gameover;
		}
		else if (playerSphereZPos < cubeMinZPos + marginOfError)
		{
			gameState = gameover;
		}


		/*Score mechanics*/

		stringstream scoreText;
		scoreText << "Score: " << playerPoints;
		scoreFont->Draw(scoreText.str(), 1275, 0, kBlack, kRight, kTop);

		if (sphereProgress != 0) //If the sphere progress variable is not 0, it can then be used to ammend the scale of the sphere based on the number of cubes picked up.
		{
			if (sphereProgress / 4 != 0) // This is done by checking that the modulo of spear progress is not 0, then ammending the scale are resetting the sphere progress to 0;
			{
				playerSphere->SetY(playerSphere->GetY() * 1.2);
				playerSphere->Scale(1.2);
				sphereRadius *= 1.2;
				sphereProgress = 0;
			}
		}

		if (playerPoints == 120) // As specified in the requirements, a score of 120 points, or 12 picked cubes leads to a gamewon state.
		{
			gameState = gamewon;
		}



		/*Game pause and exit controls*/
		if (myEngine->KeyHit(Key_P) && gameState != gameover)
		{
			if (gamePaused == false)
			{
				gameState = paused;
				gamePaused = true;
			}
			else if (gamePaused == true)
			{
				gamePaused = false;
				gameState = playing;

			}

		}


		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
