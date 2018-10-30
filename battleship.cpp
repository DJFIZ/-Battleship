/**
battleship.cpp

This program runs the game of battleship, where a human player competes against 
a computer equipped with a functioning targeting algorithm. Multiple game types
for battleship exist in which the player can engage in: 

* CLASSIC  - Standard rules: players place their ships in the water and attempt 
  to locate where the enemy placed their ships. Each player gets to fire once 
  per turn. First one to sink all of the opponent's ships wins.
* MULTIFIRE - Standard rules with one exception: allows each player to fire once 
  for each ship they have afloat in their respective fleet. Results of firing
  will only be calculated once all shots have been fired.
* CRUISE MISSILES - Standard rules, but this time ships have an 80% chance to 
  shoot down a missile targetted at them.
* HARDCORE - combines all of the previously listed gamemodes; Standard rules, 
  but each player fires once for each ship afloat in their fleet AND ships have 
  a chance to shoot down incoming missiles.

The following code was formatted with readability in mind. Functions belonging 
to a specific class are grouped, with fuctions listed in relative order to how 
functions are called chronologically once the program is executed.

!!!For display purposes it may be required that the terminal be enlarged!!!

TODO: There seems to be a lot of code that is repeated. It is my hope to 
      eventually slim this program down. 
      (ie: the CheckDirection functions, EvalDirection functions, and the loops 
      within RandomPlace are extremely similar)

@author Dustin Ficenec, CISP 400
@version 1.1 - 10/30/18
*/
#include <iostream>
#include <string>
#include <vector> 
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <limits>
using namespace std;


enum Gametype{CLASSIC, MULTIFIRE, CRUISE_MISSILES, HARDCORE};
enum Gamestate{WAITING, PLAYING, USERWON, COMPWON};
enum SquareState{MISS, HIT, EMPTY, SHIP, SINK, SHOT_DOWN};
enum ShipState{AFLOAT, SUNK};
enum Player{USER, COMP};

//class prototypes
struct Square;
class Ship;
class AIOpponent;
class Game;


// Carries data of SquareState with the ability to print a symbol correlated 
// to that SquareState.
// Used in constructing a grid of Squares
struct Square{
  private:
    SquareState state;
  public:
    Square(){
      state = EMPTY;
    }
    void Print();
    //In-line getter & setter for state
    SquareState getSquareState() const {return state;}
    const void setSquareState(SquareState s) {state = s;}
};


// Specification C1 - OOP
// Ship manages all neccesary data correlated to each ship of BATTLESHIP
class Ship{
  private:
    string shipName;                      // name of the ship
    vector<pair<int, int> > shipCoords;   // set of coordinates where ship object is located on grid
    ShipState shipState;                  // state of the ship (either SUNK or AFLOAT)
    int shipSize,                         // number of spaces a ship occupies
        health;                           // amount of damage a ship can take before it is SUNK
  public:
    // Constructor Initializes all data variables based on the given arguments
    Ship(int s, string name){
      shipSize = s;
      health = s;
      shipName = name;
      shipState = AFLOAT;
    }
    void Damage();
    // Getters and setter for the various ship data
    void setShipState(ShipState s) {shipState = s;}
    ShipState getShipState() const {return shipState;}
    int getSize() const {return shipSize;}
    int getHealth(){return health;}
    string getName(){return shipName;}
    void setCoords(vector<pair<int, int> > coords){shipCoords = coords;}
    vector<pair<int,int> > const &getCoords() const {return shipCoords;}
};


// Essential set of functions for running the targeting algorithm that competes 
// against the human player
class AIOpponent{
  public:
    // Empty default constructor
    AIOpponent() = default;
    pair<int, int> EvaluateGrid(const Game& game, int s);
    bool EvalUp(const Game &game, int x, int y, int s);
    bool EvalDown(const Game &game, int x, int y, int s);
    bool EvalRight(const Game &game, int x, int y, int s);
    bool EvalLeft(const Game &game, int x, int y, int s);
    int SmallestShipAlive(const Game &game);
    void DisplayProbabilityGrid(int grid [10][10]);
    friend class Game;
};


// Extensive class which manages a vast majority of game functionality
// * Handles all persistent grids and ships belonging to the player and computer
// * Handles writing/recording actions to a log file with the current date 
//   and/or time
// * Manages the Gametype selected and all subsequent function changes resulting 
//   from the given Gametype
// * Displays all necessary output for the game to run
// * Takes input from the user in order to place ships and target squares on the 
//   grid(s)
class Game{
  private:
    Gamestate gameState;              // Helps keep track of what is currently happening in the game
    Gametype gameType;                // Determines what aspects of the game will be enabled/disabled
    ofstream file;                    // New file to be written to
    Square userShips[10][10];       // Grid showing where the player's ships are
    Square playerTargeting[10][10];   // Grid showing where the player has fired
    Square compShips[10][10];         // Grid showing where the computer's ships are
    Square compTargeting[10][10];     // Grid showing where the computer has fired
    vector<Ship> userFleet;         // Container of all ships belonging to the player
    vector<Ship> compFleet;           // Container of all ships belonging to the computer
    AIOpponent arty;                  // Computer opponent
  public:
    // Initializes a new game with the given Gametype
    explicit Game(Gametype gt){
      gameState = WAITING;
      gameType = gt;
      srand (time(nullptr));           // Seeds a random number generator for later use
      //Specification B2 - Log file to Disk
      file.open("log.txt");
      userFleet = vector<Ship>();
      compFleet = vector<Ship>();
    }
    void Initialize();
    bool NewGameMenu();
    void ConstructFleets();
    void Play();
    void PlayerTurn();
    void CompTurn();
    void CheckHit(pair <int, int> target, Player p);
    void ShipHit(int tarCol, int tarRow, Player p);
    int GetShip(pair<int, int> coords, Player p);
    bool IsFleetDestroyed(Player p);
    pair<int, int> PromptFire();
    bool CheckWin(Player p);
    void Forfeit(Player p);
    void RandomPlacement(Player p);
    void RandomPlace(int shipLoc,Player p);
    int getRandomDirection(int x, int y, int s);
    void ManualPlacement();
    bool CheckUp(int x, int y, int s);
    bool CheckDown(int x, int y, int s);
    bool CheckLeft(int x, int y, int s);
    bool CheckRight(int x, int y, int s);
    int DirectionMenu(int x, int y, int shipSize);
    void PromptPlacement(int shipLoc);
    void DisplayGrid(Square grid[10][10], Player p);
    void PlaceShip(vector<pair<int, int> > coords, Player p);
    bool ShootDownMissile();
    void LogStart();
    void LogGameType();
    void LogShipPlace(int shipLoc, Player p);
    void LogFire(int tarCol, int tarRow, Player p);
    void LogHit();
    void LogMiss();
    void LogShotDown();
    void LogDamage(int shipLoc, Player p);
    void LogSink(int shipLoc, Player p);
    void LogWin(Player p);
    void LogExit();
    string GetDate();
    string GetTime();
    void PrintShipP1(int shipLoc);
    void PrintShipP2(int shipLoc);
    void SinkShip(int shipLoc, Player p);
    int NumShipsAlive(Player p);
    friend class AIOpponent;
};


//function prototypes
void ProgramGreeting();
Gametype MainMenu();
string StrikeName(const string& str);



int main() {
  ProgramGreeting();
  bool playing = true;    // bool to enable continued play

  while(playing){
    Game game = Game(MainMenu());
    game.Initialize();
    game.Play();
    if(!game.NewGameMenu()){
      playing = false;
    }
  }

  return 0;
}


//ProgramGreeting - USE ENLARGED TERMINAL WINDOW
void ProgramGreeting(){
  cout << " __________    _________________________________    ___________ _________ ___ ___ ______________" << endl
       << " \\______   \\  /  _  \\__    ___/\\__    ___/|    |    \\_   _____//   _____//   |   \\|   \\______   \\ " << endl
       << "  |    |  _/ /  /_\\  \\|    |     |    |   |    |     |    __)_ \\_____  \\/    ~    \\   ||     ___/ " << endl
       << "  |    |   \\/    |    \\    |     |    |   |    |___  |        \\/        \\    Y    /   ||    |    " <<endl
       << "  |________/\\____|____/____|     |____|   |________\\/_________/_________/\\___|___/|___||____|" << endl
       << "\nWelcome to BATTLESHIP, a classic game of wit and strategy."
       << " In this game, you will be pitted against the best battleship player"
       << " known to humankind: a computer. Good luck and godspeed." << endl
       << "\nPress Enter to Continue";
  // Waits for the user to press enter. Ignores all other input.
  cin.ignore(numeric_limits<streamsize>::max(),'\n'); 
}


// Prompts the user the select a Gametype through use of a numeric menu
// returns the Gametype selected
Gametype MainMenu(){
  Gametype gT = CLASSIC;    // initialized gametype to be returned
  int input;                // input from user
  bool valid = false;       // bool to ensure valid entry
  while(!valid){
    cout << "\n___________________"
         << "\n| SELECT GAME TYPE \\" 
         << "\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯"
         << "\n1. Classic"
         << "\n2. Multifire"
         << "\n3. Cruise Missiles"
         << "\n4. Hardcore"
         << "\n5. Game Type Descriptions"
         << "\n6. Exit" 
         << endl;
    cin >> input;
    cin.clear();
    cin.ignore( 256, '\n' );
    switch(input) {
      case 1: gT = CLASSIC;
        valid = true;
        break;
      // Specification A2 - MultiFire Game
      case 2: gT = MULTIFIRE;
        valid = true;
        break;
      // Specification A3 - Cruise Missiles
      case 3: gT = CRUISE_MISSILES;
        valid = true;
        break;
      case 4: gT = HARDCORE;
        valid = true;
        break;
      case 5:
      {
        cout << "\nCLASSIC - The name says it all. Place your ships in the water"
             << "and attempt to locate where the enemy placed their ships."
             << "Each player gets to fire once per turn. First one to sink all"
             << "of the opponent's ships wins." << endl
             << "\nMULTIFIRE - Classic, but with a twist. Each player gets to "
             << "fire once for each ship afloat in their fleet." << endl
             << "\nCRUISE MISSILES - Yet another take on the original, this game"
             << "type takes a more modern perspective. In this mode ships have "
             << "an 80% chance to shoot down incoming missiles." << endl
             << "\nHARDCORE - Absolute mayhem. Combines all of the features of"
             << "the previous game types into one explosive package." << endl;
        break;
      }
      case 6:
      {
        cout << "Goodbye!" << endl;
        exit(0);
      }
      // Bulletproof - prevents all entries other than numbers 1-6
      default: cout << "\nInvalid entry, please enter a number from 1 through 6"
                    << "(no decimals)." << endl;
        break;
    }
  }
  return gT;
}


// Returns the given str with a strike through it
// i.e. StrikeName("CARRIER") returns C̶A̶R̶R̶I̶E̶R
string StrikeName(const string& str) {
  string struck;
  for (auto ch : str) {
    struck.append(u8"\u0336");
    struck.push_back(ch);
  }
  return struck;
}


// Prints the SquareState of a Square.
// Each state is represented by a symbol.
void Square::Print(){
  switch(state){
    case HIT:   cout << " X ";
      break;
    case MISS:  cout << " O ";
      break;
    case EMPTY: cout << "   ";
      break;
    case SHIP:  cout << "!S!";
      break;
    case SINK:  cout << StrikeName(" X ");
      break;
    case SHOT_DOWN: cout << " + ";
      break;
  }
}


/*
  Below exists all functions for class Game.
  The amount of code is extensive, however it has been divided up logically
  in order to enhance readability.

  SECTION 1: PLACEMENT
  SECTION 2: PLAY
  SECTION 3: LOG
*/

/*
  SECTION 1: PLACEMENT

  This section deals with the initialization of the game and the placement of ships,
  both random and manual, for both the computer and the user.
*/


// Initializes the game by calling for the cration of all needed ships
// Prompts the user for what type of placement they wish to use for their
// fleet.
void Game::Initialize(){
  LogStart();
  LogGameType();
  ConstructFleets();
  
  bool choiceMade = false;    // ensures valid input
  int input;                  // input from user
  while(!choiceMade){
    cout << "\n___________________"
         << "\n| SELECT PLACEMENT \\"
         << "\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯"
         << "\n1. Random Placement"
         << "\n2. Manual Placement"
         << "\n3. Exit" 
         << endl;
    cin >> input;
    cin.clear();
    cin.ignore( 256, '\n' );
    switch(input) {
      case 1: { 
        RandomPlacement(USER);
        choiceMade = true;
        break;
      }
      case 2: { 
        ManualPlacement();
        choiceMade = true;
        break;
      }
      case 3: {
        cout << "Goodbye!" << endl;
        LogExit();
        exit(0);
      }
      //Bulletproof - Does not accept inputs other than 1,2, or 3
      default: cout << "\nInvalid entry, please enter either 1, 2, or 3." 
                    << endl;
        break;
    }
  }
  RandomPlacement(COMP);
}


//constructs the 5 ships needed for both players
void Game::ConstructFleets(){
userFleet.push_back(Ship(5, "CARRIER"));
userFleet.push_back(Ship(4, "BATTLESHIP"));
userFleet.push_back(Ship(3, "CRUISER"));
userFleet.push_back(Ship(3, "SUBMARINE"));
userFleet.push_back(Ship(2, "DESTROYER"));

compFleet.push_back(Ship(5, "CARRIER"));
compFleet.push_back(Ship(4, "BATTLESHIP"));
compFleet.push_back(Ship(3, "CRUISER"));
compFleet.push_back(Ship(3, "SUBMARINE"));
compFleet.push_back(Ship(2, "DESTROYER"));
}


//loops through the fleet belonging to Player p, randomly places each ship
void Game::RandomPlacement(Player p){
  if(p == USER){
    for(int i = 0; i < 5; i++){
      RandomPlace(i, p);
    }
  }
  else{
    // Specification B3 - Random Start
    for(int i = 0; i < 5; i++){
      RandomPlace(i, p);
    }
  }
}


// Randomly places the ship at index location shipLoc on the grid
// TODO: Modularize this code
void Game::RandomPlace(int shipLoc, Player p){
  int shipSize;                     // size of the given ship
  bool placed = false;              // bool ensuring valid placement
  bool isEmpty;                     // bool ensuring that the desired placement is empty
  vector<pair<int,int> > shipVec;   // container of coordinates of where the given ship
                                    // will be placed
  // Loops until a valid placement occurs
  // At the beginning of each iteration, random coordinates are generated from 1-10
  // A direction is then also randomly obtained.
  // With this direction, a check is performed to see if any of the sqaures in that direction
  // are already occupied. If all of the necessary squares are EMPTY, the ship is placed
  // at at those locations.
  while(!placed){
    int x = rand() % 10;
    int y = rand() % 10;
    if(p == USER){
      shipSize = userFleet[shipLoc].getSize();
      if(userShips[x][y].getSquareState() == EMPTY){
        isEmpty = true;
        switch (getRandomDirection(x,y, shipSize)){
          case 1: { //checks right
            for(int i = x; i < x + shipSize; i++){
              shipVec.push_back(make_pair(i, y));
              if(userShips[i][y].getSquareState() != EMPTY){
                isEmpty = false;
                break;
              }
            }
            break;
          }
          case 2: { //checks down
            for(int i = y; i < y + shipSize; i++){
              shipVec.push_back(make_pair(x, i));
              if(userShips[x][i].getSquareState() != EMPTY){
                isEmpty = false;
                break;
              }
            }
            break;
          }
          case 3: { //checks left
            for(int i = x; i > x - shipSize; i--){
              shipVec.push_back(make_pair(i, y));
              if(userShips[i][y].getSquareState() != EMPTY){
                isEmpty = false;
                break;
              }
            }
            break;
          }
          case 4: { //checks up
            for(int i = y; i > y - shipSize; i--){
              shipVec.push_back(make_pair(x, i));
              if(userShips[x][i].getSquareState() != EMPTY){
                isEmpty = false;
                break;
              }
            }
            break;
          }
          default: isEmpty = false;
            break;
        }
        if(isEmpty){
          userFleet[shipLoc].setCoords(shipVec);
          PlaceShip(shipVec, p);
          LogShipPlace(shipLoc, p);
          placed = true;
        }
        else{
          shipVec.clear();
        }
      }
    }
    else{
      //Evaluation for computer ships
      shipSize = compFleet[shipLoc].getSize();
      if(compShips[x][y].getSquareState() == EMPTY){
        isEmpty = true;
        switch (getRandomDirection(x,y, shipSize)){
          case 1: { //checks right
            for(int i = x; i < x + shipSize; i++){
              shipVec.push_back(make_pair(i, y));
              if(compShips[i][y].getSquareState() != EMPTY){
                isEmpty = false;
              }
            }
            break;
          }
          case 2: { //checks down
            for(int i = y; i < y + shipSize; i++){
              shipVec.push_back(make_pair(x, i));
              if(compShips[x][i].getSquareState() != EMPTY){
                isEmpty = false;
              }
            }
            break;
          }
          case 3: { //checks left
            for(int i = x; i > x - shipSize; i--){
              shipVec.push_back(make_pair(i, y));
              if(compShips[i][y].getSquareState() != EMPTY){
                isEmpty = false;
              }
            }
            break;
          }
          case 4: { //checks up
            for(int i = y; i > y - shipSize; i--){
              shipVec.push_back(make_pair(x, i));
              if(compShips[x][i].getSquareState() != EMPTY){
                isEmpty = false;
              }
            }
            break;
          }
          case 0: isEmpty = false;
        }
        // Places ship
        if(isEmpty == true){
          compFleet[shipLoc].setCoords(shipVec);
          PlaceShip(shipVec, p);
          placed = true;
          LogShipPlace(shipLoc, p);
        }
        else{
          shipVec.clear();
        }
      }
    }
  }
}


// Loops through all of the ships in the userFleet and prompts for placement
void Game::ManualPlacement(){
  for(int i = 0; i < 5; i++){
      PromptPlacement(i);
    }
}


// Prompts the user for placement coordinates and desired direction.
// Checks if the placement is valid, then places the ship if it is. 
// Reprompts if it isn't.
void Game::PromptPlacement(int shipLoc){
  SquareState tmpSS;                                // temporary SquareState variable to be used 
                                                    // for comparison
  pair<int, int> origin;                            // target coordinates to be returned
  string input;                                     // string input from the user
  int inX,                                          // integer conversion from the user's input
      inY;                                          // integer conversion from the user's input
  bool valid = false,                               // bool to ensure valid entry
       isNum;                                       // boot to ensure that second half of input 
                                                    // is a number
  vector<pair<int,int> > shipVec;                   // container of coordinates of where the given 
                                                    // ship will be placed
  int shipSize = userFleet[shipLoc].getSize();    // size of the given ship

  while(!valid){
    DisplayGrid(userShips, USER);
    cout << "\nWhere would you like to place your " 
         << userFleet[shipLoc].getName() 
         << " (ex: C5)?";
    cin.clear();
    getline(cin, input);
    //Bulletproof - checks to ensure input is of valid length
    if(input.length() > 3){
      cout << "\nInvalid entry. The correct format is format for entry is"
           << " LetterNumber, with no decimals (ex: C5)." 
           << endl;
    }
    else{
      //Bulletproof - check to ensure that second half of input is a number
      isNum = true;
      for(int i = 1; i < input.length(); i++){
        if(!isdigit(input[i])){
          isNum = false;
        }
      }
      // Bulletproof
      if(!isalpha(input[0]) || !isNum){
         cout << "\nInvalid entry. The correct format is format for entry is"
              << " LetterNumber, with no decimals (ex: C5)." 
              << endl;
      }
      else{
        // Bulletproof - handles both upper and lowercase letters
        // Conversions from ASCII chars to integers
        if(!isupper(input[0])){
          inX = (int)input[0] - 97;
        }
        else{
          inX = (int)input[0] - 65;
        }
        inY = stoi(input.substr(1,input.length() - 1)) - 1; 
        if(inX > 9 || inX < 0 || inY > 9 || inY < 0){
          cout << "\nInvalid entry, those coordinates are off the grid!" 
               << endl;
        }
        else{
          tmpSS = userShips[inY][inX].getSquareState();
          if(tmpSS != EMPTY){
            cout << "\nInvalid entry, you have already placed a ship there!" 
                 << endl;
          }
          else {
            // Prompts for direction then fills a vector with the relevant coordinates
            int dir = DirectionMenu(inY, inX, shipSize);
            switch(dir){
              case 1:
              {
                for(int i = inY; i < inY + shipSize; i++){
                  shipVec.push_back(make_pair(i, inX));
                }
                valid = true;
                break;
              }
              case 2:
              {
                for(int i = inX; i < inX + shipSize; i++){
                  shipVec.push_back(make_pair(inY, i));
                }
                valid = true;
                break;
              }
              case 3:
              {
                for(int i = inY; i > inY - shipSize; i--){
                  shipVec.push_back(make_pair(i, inX));
                }
                valid = true;
                break;
              }
              case 4:
              {
                for(int i = inX; i > inX - shipSize; i--){
                  shipVec.push_back(make_pair(inY, i));
                }
                valid = true;
                break;
              }
              case 99: break;
            }
          }
        }
      }
    }
  }
  cout << "PLACING";
  // Places ship at relevant locations
  PlaceShip(shipVec, USER);
  userFleet[shipLoc].setCoords(shipVec);
  LogShipPlace(shipLoc, USER);
}


// Menu for prompting for direction. Returns an int representing a direction.
// 1 = right; 2 = down; 3 = left; 4 = up
// returns 99 if user wishes to go back
int Game::DirectionMenu(int x, int y, int shipSize){
  int input,                // user input
        dir;                // direction to be returned
  bool validDir = false;    // bool to ensure valid direction
  string err = "\n Cannot place a ship in that direction!";
  while(!validDir){
    cout << "\n___________________"
         << "\n| SELECT DIRECTION \\" 
         << "\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯"
         << "\n1. Right"
         << "\n2. Down"
         << "\n3. Left"
         << "\n4. Up"
         << "\n5. Re-enter coordinates" 
         << endl;
    cin >> input;
    cin.clear();
    cin.ignore( 256, '\n' );
    // checks if given input is valid, sets dir equal to appropriate value
    switch(input) {
      case 1: 
      { 
        if(CheckRight(x, y, shipSize)){
          dir = 1;
          validDir = true;
        }
        else{
          cout << err << endl;
        }
        break;
      }
      case 2: 
      { 
        if(CheckDown(x, y, shipSize)){
          dir = 2;
          validDir = true;
        }
        else{
          cout << err << endl;
        }
        break;
      }
      case 3: 
      { 
        if(CheckLeft(x, y, shipSize)){
          dir = 3;
          validDir = true;
        }
        else{
          cout << err << endl;
        }
        break;
      }
      case 4: 
      { 
        if(CheckUp(x, y, shipSize)){
          dir = 4;
          validDir = true;
        }
        else{
          cout << err << endl;
        }
        break;
      }
      case 5: 
      { 
        dir = 99;
        validDir = true;
        break;
      }
      //Bulletproof - Does not accept inputs other than 1-5;
      default: cout << "\nInvalid entry, please enter a number 1-5 (no decimals)." 
                    << endl;
        break;
    }
  }
  return dir;
}

//Checks to see if a ship of size 's' can fit vertically above point (x,y)
bool Game::CheckUp(int x, int y, int s){
  bool isEmpty = true;
  if(y - (s - 1) < 0){
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(userShips[x][y - n].getSquareState() != EMPTY){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}

//Checks to see if a ship of size 's' can fit vertically below point (x,y)
bool Game::CheckDown(int x, int y, int s){
  bool isEmpty = true;
  if(y + (s - 1) > 9){ 
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(userShips[x][y + n].getSquareState() != EMPTY){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}

//Checks to see if a ship of size 's' can fit horizontally left of point (x,y)
bool Game::CheckLeft(int x, int y, int s){
  bool isEmpty = true;
  if(x - (s - 1) < 0){ 
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(userShips[x - n][y].getSquareState() != EMPTY){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}

//Checks to see if a ship of size 's' can fit horizontally right of point (x,y)
bool Game::CheckRight(int x, int y, int s){
  bool isEmpty = true;
  if(x + (s - 1) > 9){ 
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(userShips[x + n][y].getSquareState() != EMPTY){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}


// Checks if a given point (x,y) is too close to any of the grid borders.
// Generates a random integer representing a valid direction 
// (A direction that doesnt result in a ship being placed off the grid).
// Returns this random integer.
// 1 = right; 2 = down; 3 = left; 4 = up
int Game::getRandomDirection(int x, int y, int s){
  int rng = 0;
  //left
  if(x < s - 1){
    // Grid location: left|top
    if(y < s - 1){
      do{
        rng = rand() % 3 + 1;
      }while(rng == 3 || rng == 4);
    }
    // Grid location: left|bottom
    else if(y > 10 - s){
      do{
        rng = rand() % 3 + 1;
      }while(rng == 2 || rng == 3);
    }
    else{
      do{
        rng = rand() % 3 + 1;
      }while(rng == 3);
    }
  }
  //right
  else if(x > 10 - s){
    // Grid location: right|top
    if(y < s - 1){
      do{
        rng = rand() % 3 + 1;
      }while(rng == 1 || rng == 4);
    }
    // Grid location: right|bottom
    else if(y > 10 - s){
      do{
        rng = rand() % 3 + 1;
      }while(rng == 1 || rng == 2);
    }
    else{
      do{
        rng = rand() % 3 + 1;
      }while(rng == 1);
    }
  }
  // Grid location: top|middle
  else if(y < s - 1){
    do{
      rng = rand() % 3 + 1;
    }while(rng == 4);
  }
  // Grid location: bottom|middle
  else if(y > 10 - s){
    do{
      rng = rand() % 3 + 1;
    }while(rng == 2);
  }
  // Grid location:  middle|middle
  else{
    rng = rand() % 3 + 1;
  }
  return rng;
}


// Sets a ships coordinates equal to the given vector coords
// Iterates through the given container of coordinates, coords and
// changes the SquareState of that grid point to SHIP
void Game::PlaceShip(vector<pair<int, int> > coords, Player p){
  if(p == USER){
    for(const pair<int, int> &coord: coords){
      userShips[coord.first][coord.second].setSquareState(SHIP);
    }
  }
  else{
    for(const pair<int, int> &coord: coords){
      compShips[coord.first][coord.second].setSquareState(SHIP);
    }
  }
}


/*
  SECTION 2: PLAY

  The following code deals with the actual playing of the game. 
  All functions in section 2 are executed while the user is playing.
*/


// Outlines what happens in the course of playing the game.
// Each side takes a turn, with the user always going first.
// After each side takes a turn, the game checks to see if either
// has won.
// After both turns are concluded, the game waits for the enter
// key to be pressed, giving the player time to analyze what occurred.
void Game::Play(){
  gameState = PLAYING;
  while(gameState == PLAYING){
    //DisplayGrid(compTargeting); //uncomment for debugging
    PlayerTurn();
    if(CheckWin(USER) || CheckWin(COMP)) {
      break;
    }
    else {
      CompTurn();
      CheckWin(COMP);
    }
    cout << "\nPress Enter to Continue";
    cin.ignore(std::numeric_limits<streamsize>::max(),'\n');
  }
}


// Outlines everything that occurs during a players turn.
// 1. Grids showing the locations of the player's ships and locations of the 
//    player's shots are displayed.
// 2. Checks what Gametype is selected. 
//    If MULTIFIRE or HARDCORE have been chosen, a targetList is generated by 
//    prompting the player a number of times equal to the number of ships they 
//    have alive.
//    If niether of those Gametypes are active, the user is prompted for only 
//    one target.
// 3. For each target prompted for, the target coordinates are checked to see
//    if the resulting impact is a HIT, MISS, or if the missile was SHOT_DOWN.
void Game::PlayerTurn(){
  cout << "\n___________________"
       << "\n| YOUR SHIPS       \\"
       << "\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯" 
       << endl;
  DisplayGrid(userShips, USER);
  cout << "\n___________________"
       << "\n| TARGETTING GRID  \\"
       << "\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯" 
       << endl;
  DisplayGrid(playerTargeting, COMP);
  cout << "\n(Type ff to forfeit.)"<< endl;
  if(gameType == MULTIFIRE || gameType == HARDCORE){
    vector<pair<int, int> > targetList;     // container for up to several targetting solutions
    int numShips = NumShipsAlive(USER);   // number of ships the player has AFLOAT
    for(int i = 0; i < numShips; i++){
      targetList.push_back(PromptFire());
    }
    for(int i = 0; i < numShips; i++){
      CheckHit(targetList[i], USER);
    }
  }
  else{
    CheckHit(PromptFire(), USER);
  }
}


// Outlines everything that occurs during the computer's turn.
// 1. Checks what Gametype is selected. 
//    If MULTIFIRE or HARDCORE have been chosen, a targetList is generated by 
//    evaluating the grid, compTargeting, a number of times equal to the number 
//    of ships the computer has alive 
//    If niether of those Gametypes are active, the grid is only evaluated once.
// 2. For each target evaluated, the target coordinates are checked to see
//    if the resulting impact is a HIT, MISS, or if the missile was SHOT_DOWN.
void Game::CompTurn(){
  pair<int, int> compTarget;              // firing solution to be generated by the AI's grid evaluation
  if(gameType == MULTIFIRE || gameType == HARDCORE){
    vector<pair<int, int> > targetList;   // container for up to several targetting solutions
    int numShips = NumShipsAlive(COMP);   // number of ships the coputer has AFLOAT
    for(int i = 0; i < numShips; i++){
      compTarget = arty.EvaluateGrid(*this, arty.SmallestShipAlive(*this));
      compTargeting[compTarget.second][compTarget.first].setSquareState(MISS);
      targetList.push_back(compTarget);
    }
    for(int i = 0; i < numShips; i++){
      CheckHit(targetList[i], COMP);
    }
  }
  else{
    compTarget = arty.EvaluateGrid(*this, arty.SmallestShipAlive(*this));
    CheckHit(compTarget, COMP);
  }
}


// Loops through the given player/comp's fleet to see if every ship is SUNK
// returns true if all ships are SUNK, false if at least one ship is AFLOAT
bool Game::IsFleetDestroyed(Player p){
  bool isDestroyed = true;
  for(int i = 0; i < 5; i++){
    if(p == USER){
      if(userFleet[i].getShipState() == AFLOAT){
        isDestroyed = false;
      }
    }
    else
      if(compFleet[i].getShipState() == AFLOAT){
        isDestroyed = false;
      }
  }
  return isDestroyed;
}

// Loops through the given Player p's fleet to see how many ships are alive
// returns the number of ships alive in said fleet
int Game::NumShipsAlive(Player p){
  int numShips = 0;   // number of ships afloat; initialized to 0 for later incrementation
  if(p == USER){
    for(int i = 0; i < 5; i++){
      if(userFleet[i].getShipState() == AFLOAT){
        numShips++;
      }
    }
  }
  else{
    for(int i = 0; i < 5; i++){
      if(compFleet[i].getShipState() == AFLOAT){
        numShips++;
      }
    }
  }
  return numShips;
}


// Reduces a ships health by 1
// Sets the ships state to SUNK if health is reduced to 0
void Ship::Damage(){
  health--;
  if(health == 0){
    shipState = SUNK;
  }
}


// Prompts the user for targeting coordinates and returns those coordinates as a 
// pair of integers. Used pair to allow for ease of returning two variables.
pair<int, int> Game::PromptFire(){
  SquareState tmpSS;        // temporary SquareState variable to be used for comparison
  pair<int, int> target;    // target coordinates to be returned
  string input;             // string input from the user
  int inX,                  // integer conversion from the user's input
      inY;                  // integer conversion from the user's input
  bool valid = false,       // bool to ensure valid entry
       isNum;               // boot to ensure that second half of input is a number
  while(!valid){
    cout << "\nWhere would you like to fire (ex: C5)?";
    cin.clear();
    getline(cin, input);
    // Specification C3 - Secret Option
    if(input == "~"){
      cout << "\n___________________"
           << "\n| COMPUTER'S SHIPS \\"
           << "\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯" 
           << endl;
      DisplayGrid(compShips, COMP);
    }
    // User selection to forfeit the game
    else if(input == "ff" || input == "FF"){
      //set to (99,99) to indicate escape sequence
      target = make_pair(99, 99);
      valid = true;
    }
    // User selection to force AI to forfeit
    else if(input == "YOUSUNKMYBATTLESHIP"){
      target = make_pair(999,999);
      valid = true;
    }
    else{
      //Bulletproof - checks to ensure input is of valid length
      if(input.length() > 3){
        cout << "\nInvalid entry. The correct format is format for entry is"
             << " LetterNumber, with no decimals (ex: C5)." 
             << endl;
      }
      else{
        //Bulletproof - check to ensure that second half of input is a number
        isNum = true;
        for(int i = 1; i < input.length(); i++){
          if(!isdigit(input[i])){
            isNum = false;
          }
        }
        // Specification B1 - Validate Input
        if(!isalpha(input[0]) || !isNum){
         cout << "\nInvalid entry. The correct format is format for entry is"
              << " LetterNumber, with no decimals (ex: C5)." 
              << endl;
        }
        else{
          // Bulletproof - handles both upper and lowercase letters
          // Conversions from ASCII chars to integers
          if(!isupper(input[0])){
            inX = (int)input[0] - 97;
          }
          else{
            inX = (int)input[0] - 65;
          }
          inY = stoi(input.substr(1,input.length() - 1)) - 1; 
          if(inX > 9 || inX < 0 || inY > 9 || inY < 0){
            cout << "\nInvalid entry, those coordinates are off the grid!" 
                 << endl;
          }
          else{
            // Specification A1 - Adv Input Validation
            tmpSS = playerTargeting[inY][inX].getSquareState();
            if(tmpSS != EMPTY){
              // Check for Gametypes CRUISE_MISSILES or HARDCORE. SHOT_DOWN can still be fired upon.
              if(tmpSS != SHOT_DOWN){
              cout << "\nInvalid entry, you have already fired on those coordinates!" 
                   << endl;
              }
              else {
              target = make_pair(inX, inY);
              valid = true;
              }
            }
            else {
              target = make_pair(inX, inY);
              valid = true;
            }
          }
        }
      }
    }
  }
  return target;
}


// Checks to see if the given target coordinates result in a hit or miss. 
// Changes the appropriate grids and ships to display the outcome of this check.
void Game::CheckHit(pair<int, int> target, Player p){
  SquareState tmpSS;            // temporary SquareState variable to be used for comparison
  int tarRow = target.first;    
  int tarCol = target.second;   // coordinates of the given target
  LogFire(tarCol, tarRow, p);
  if(tarRow == 99)
  {
    Forfeit(USER);
  }
  else if(tarRow == 999){
    Forfeit(COMP);
  }
  else{
    if(p == USER){
      tmpSS = compShips[tarCol][tarRow].getSquareState();
      if(tmpSS == SHIP || tmpSS == SHOT_DOWN){
        if(gameType == CRUISE_MISSILES || gameType == HARDCORE) {
          if(ShootDownMissile()){
            cout << "Your missile was SHOT DOWN!" 
                 << endl;
            playerTargeting[tarCol][tarRow].setSquareState(SHOT_DOWN);
            LogShotDown();
          }
          else {
            ShipHit(tarCol, tarRow, p);
          }
        }
        else{
          ShipHit(tarCol, tarRow, p);
        }
      }
      else{
        // Conversion from ints to identifiable grid coordinates (Will be used a lot)
        cout << "\nYour shot at (" 
             << char(tarRow + 65) << ", " 
             << tarCol + 1 << ")"
             << " was a MISS!" << endl;
        playerTargeting[tarCol][tarRow].setSquareState(MISS);
        LogMiss();
      }
    }
    else{
      cout << "\nComputer fired at (" 
           << char(tarRow + 65) << ", " 
           << tarCol + 1 << ")."
           << endl;
      tmpSS = userShips[tarCol][tarRow].getSquareState();
      if(tmpSS == SHIP || tmpSS == SHOT_DOWN){
        if(gameType == CRUISE_MISSILES || gameType == HARDCORE) {
          if(ShootDownMissile()){
            cout << "The missile was SHOT DOWN!" 
                 << endl;
            compTargeting[tarCol][tarRow].setSquareState(SHOT_DOWN);
            userShips[tarCol][tarRow].setSquareState(SHOT_DOWN);
            LogShotDown();
          }
          else {
           ShipHit(tarCol, tarRow, p);
          }
        }
        else{
          ShipHit(tarCol, tarRow, p);
        }
      }
      else{
        cout << "It was a MISS!" << endl;
        compTargeting[tarCol][tarRow].setSquareState(MISS);
        userShips[tarCol][tarRow].setSquareState(MISS);
        LogMiss();
      }
    }
  }
}


// Upon a ship being hit, prints to inform the user.
// Also evaluates whether a ship was SUNK as a result from the hit.
// Modifies respective grids to reflect these changes
void Game::ShipHit(int tarCol, int tarRow, Player p){
  int shipLoc = GetShip(make_pair(tarCol, tarRow), p);    // index location of ship
  LogHit();
  if(p == USER){
    cout << "\nYour shot at (" 
         << char(tarRow + 65) << ", " 
         << tarCol + 1 << ")"
         << " was a HIT!" << endl;
    playerTargeting[tarCol][tarRow].setSquareState(HIT);
    compFleet[shipLoc].Damage();
    LogDamage(shipLoc, p);
    if(compFleet[shipLoc].getShipState() == SUNK){
      cout << "\nYOU SUNK THE ENEMY'S " 
           << compFleet[shipLoc].getName() << "!" 
           << endl;
      SinkShip(shipLoc, p);
      LogSink(shipLoc, p);
    }
  }
  else{
    cout << "It was a HIT!" << endl;
    compTargeting[tarCol][tarRow].setSquareState(HIT);
    userShips[tarCol][tarRow].setSquareState(HIT);
    userFleet[shipLoc].Damage();
    LogDamage(shipLoc, p);
    if(userFleet[shipLoc].getShipState() == SUNK){
      cout << "\nTHE ENEMY SUNK YOUR " 
           << userFleet[shipLoc].getName() << "!" 
           << endl;
    SinkShip(shipLoc, p);
    LogSink(shipLoc, p);
    } 
  }
}


// Performs check to see if incoming missile was shot down (80% chance)
bool Game::ShootDownMissile(){
  bool shotDown = false;
  int rng = rand() % 10 + 1;    // random number 1-10 generated to simulate chance
  if(rng <= 8){
    shotDown = true;
  }
  return shotDown;
}



// Checks the given coords to see which ship is contained at that location. 
// Returns the index of where the ship is located in its repective fleet vector.
int Game::GetShip(pair<int, int> coords, Player p) {
  int shipLoc = 99;                   // index location of ship. Initialized to 99 for debugging purposes
  vector<pair<int, int> > shipVec;    // container of coordinates of where ship is located
  // Loops through each ship, checking if any of its coordinates match the coordinates given
  for (int i = 0; i < 5; i++) {
    if(p == USER){
      shipVec = compFleet[i].getCoords();
    }
    else{
      shipVec = userFleet[i].getCoords();
    }
    for (auto &n : shipVec) {
      if(coords.first == n.first && coords.second == n.second){
        shipLoc = i;
      }
    }
  }
  return shipLoc;
}


//Displays the given grid
/* EX:
          1   2   3   4   5   6   7   8   9  10
      -----------------------------------------
    A |   |   |   |   |   |   |   |   |   |   |
      -----------------------------------------
    B |   |   |   |   |   |   |   |   |   |   |
      -----------------------------------------
    C |   |   |   |   |   |   | O |   |   |   |
      -----------------------------------------
    D |   |   |   |   |   |   |   |   |   |   |
      -----------------------------------------
    E |   |   |   |   | X |   |   |   |   |   |
      -----------------------------------------
    F |   |   |   |   |   |   |   |   |   |   |
      -----------------------------------------
    G |   |   |   |   |   |   |   |   |   |   |
      -----------------------------------------
    H |   |   |   |   |   |   |   |   |   |   |
      -----------------------------------------
    I |   |   |   |   |   |   |   |   |   |   |
      -----------------------------------------
    J |   |   |   |   |   |   | O |   |   |   |
      -----------------------------------------
*/
void Game::DisplayGrid(Square grid[10][10], Player p){
  string line = "  -----------------------------------------";
  cout << "\n    1   2   3   4   5   6   7   8   9  10" << endl;
  cout << line << endl;
  for (int row = 0; row < 10; row++) {
    for (int col = 0; col < 10; col++) {
      if (col == 0){
        switch(row){
        case 0: cout<< "A ";
          break;
        case 1: cout<< "B ";
          break;
        case 2: cout<< "C ";
          break;
        case 3: cout<< "D ";
          break;
        case 4: cout<< "E ";
          break;
        case 5: cout<< "F ";
          break;
        case 6: cout<< "G ";
          break;
        case 7: cout<< "H ";
          break;
        case 8: cout<< "I ";
          break;
        case 9: cout<< "J ";
          break;
        }
        cout << "|";
      }
      grid[col][row].Print();
      if (col < 10) {
        cout << "|";
      }
    }
    // Displays the status of the user's ships after each grid line
    if(p == USER){
    switch(row){
        case 0: 
          PrintShipP1(0);
          break;
        case 1: 
          PrintShipP2(0);
          break;
        case 2: 
          PrintShipP1(1);
          break;
        case 3: 
          PrintShipP2(1);
          break;
        case 4: 
          PrintShipP1(2);
          break;
        case 5: 
          PrintShipP2(2);
          break;
        case 6: 
          PrintShipP1(3);
          break;
        case 7: 
          PrintShipP2(3);
          break;
        case 8: 
          PrintShipP1(4);
          break;
        case 9: 
          PrintShipP2(4);
          break;
        }
    }
    else{
      cout << "\n";
      if (row < 10) {
        cout << line << endl;
      }
    }
  }
}


// PrintShip is divided into two parts. P1 prints the name of the ship as well as 
// the top line of the squares that represent the ship and a line on the grid
// ex print:
//
// DESTROYER
//  ---------
void Game::PrintShipP1(int shipLoc){
  cout << "          ";
  string shipName = userFleet[shipLoc].getName();   // name of the ship to be printed
  if(userFleet[shipLoc].getShipState() == SUNK){
    cout << StrikeName(shipName);
  }
  else{
    cout << shipName;
  }
  cout << "\n  -----------------------------------------" 
       << "            ";
  for(int n = 0; n < userFleet[shipLoc].getSize(); n++){
    cout << "----";
  }
  cout << "-" << endl;
}


// P2 of PrintShip prints the bottom parts of the squares and a line on the grid. 
// The squares are filled X's, representing how damaged a ship is. 
// Blank squares represent health points (ie. 5 blank squares = 5 health).
// X's that have been struck are indicative of a SUNK ship
// ex print:
//
//    | X |   |
//    ---------
void Game::PrintShipP2(int shipLoc){
  cout << "            ";
  int shipSize = userFleet[shipLoc].getSize();        // size of the given ship
  int shipHealth = userFleet[shipLoc].getHealth();    // health of the given ship
  for(int i = 0; i < shipSize; i++){
    cout << "|";
    if(shipHealth == 0){
      cout << StrikeName(" X ");
    }
    else if(i < shipSize - shipHealth){
      cout << " X ";
    }
    else{
      cout << "   ";
    }
  }
  cout << "|\n  -----------------------------------------" 
       << "            ";
  for(int n = 0; n < shipSize; n++){
    cout << "----";
  }
  cout << "-" << endl;
}


// Loops through a ships coordinate locations, setting the SquareState
// of each relevant grid square to SINK.
void Game::SinkShip(int shipLoc, Player p){
  int x,y;                            // variables used to contain coordinates;
                                      // solely used to enhance readability.
  vector<pair<int, int> > shipVec;    // container of ships coordinates
  if(p == USER){
    shipVec = compFleet[shipLoc].getCoords();
    for (auto &i : shipVec) {
      x = i.first;
      y = i.second;
      playerTargeting[x][y].setSquareState(SINK);
    }
  }
  else{
    shipVec = userFleet[shipLoc].getCoords();
    for (auto &i : shipVec) {
      x = i.first;
      y = i.second;
      compTargeting[x][y].setSquareState(SINK);
      userShips[x][y].setSquareState(SINK);
    }
  }
}


// Checks to see if either player has won the game
// Returns true if Player 'p' won; false if not.
bool Game::CheckWin(Player p){
  bool win = false;   //bool to be returned
  if(p == USER){
    if(IsFleetDestroyed(COMP)){
      gameState = USERWON;
      LogWin(USER);
      cout << "\nYOU HAVE DESTROYED ALL OF THE ENEMY'S SHIPS!"
           << "\nYOU WIN!" << endl;
      win = true;
    }
  }
  else {
    if(IsFleetDestroyed(USER)){
      gameState = USERWON;
      LogWin(COMP);
      cout << "\nALL FRIENDLY SHIPS HAVE BEEN DESTROYED!"
           << "\nTHE COMPUTER WINS!" << endl;
      win = true;
    }
  }
  return win;
}


//Sinks all Player 'p's ships in order to end the current game
void Game::Forfeit(Player p){
  if(p == USER) {
    for(int i = 0; i < 5; i++){
      userFleet[i].setShipState(SUNK);
    }
  }
  else {
    for(int i = 0; i < 5; i++){
      compFleet[i].setShipState(SUNK);
    }
  }
}


// Menu displayed once a game has concluded.
// Prompts the user if they would like to play again
// Returns true if yes, false if no
bool Game::NewGameMenu(){
  bool playAgain;             // bool to be returned
  bool choiceMade = false;    // bool to ensure valid input
  int input;                  // input from user

  while(!choiceMade){
    cout << "\n___________________"
         << "\n| PLAY AGAIN?      \\"
         << "\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯"
         << "\n1. Yes"
         << "\n2. No"
         << endl;
    cin >> input;
    cin.clear();
    cin.ignore( 256, '\n' );
    switch(input) {
      case 1: 
      { 
        playAgain = true;
        choiceMade = true;
        break;
      }
      case 2: 
      { 
        playAgain = false;
        choiceMade = true;
        cout << "Goodbye!" << endl;
        LogExit();
        break;
      }
      //Bulletproof - Does not accept inputs other than 1 or 2
      default: cout << "\nInvalid entry, please enter either 1 or 2." 
                    << endl;
        break;
    }
  }
  return playAgain;
}


/*
  SECTION 3:LOG

  The following functions all deal with writing to the file, log.txt
*/

// Writes to log.txt whenever a new game is started.
void Game::LogStart(){
  file << "BATTLESHIP \n--------------------------- \nNew game started on " 
       << GetDate() << " at " << GetTime() << "." 
       << endl;
}

// Writes to log.txt what Gametype was selected.
void Game::LogGameType(){
  file << "Game Type ";
  switch(gameType){
    case CLASSIC: file << "CLASSIC";
      break;
    case MULTIFIRE: file << "MULTIFIRE";
      break;
    case CRUISE_MISSILES: file << "CRUISE MISSILES";
      break;
    case HARDCORE: file << "HARDCORE";
      break;
  }
  file << " was selected." << endl;
}

// Writes to log.txt whenever the program is exited.
void Game::LogExit(){
  file << "Game exited on " << GetDate() << " at " << GetTime() << "." << endl;
  file.close();
}

//Writes to log.txt when and where a ship was placed on a grid
void Game::LogShipPlace(int shipLoc, Player p){
  vector<pair<int, int> > shipVec;    // container of ships coordinates
  int shipSize;                       // size of given ship
  file << "\n" << GetTime();
  if(p == USER){
    shipSize = userFleet[shipLoc].getSize();
    shipVec = userFleet[shipLoc].getCoords();
    file << " Player's " << userFleet[shipLoc].getName();
  }
  else{
    shipSize = compFleet[shipLoc].getSize();
    shipVec = compFleet[shipLoc].getCoords();
    file << " Computer's " << compFleet[shipLoc].getName();
  }
  file << " placed at the following coordinates: " 
       << endl;
  for(int i = 0; i < shipSize; i++){
    file << "(" << char(shipVec[i].second + 65) << ", " 
         << shipVec[i].first + 1 << ")" 
         << endl; 
  }
}

// Writes to log.txt whenever the player or computer fires. 
// Also writes the point(x,y) fired upon
void Game::LogFire(int tarCol, int tarRow, Player p){
  file << "\n" << GetTime(); 
  if(p == USER){
    file << " Player";
  }
  else{
    file << " Computer";
  }
  file << " fired at (" 
       << char(tarRow + 65) << ", " 
       << tarCol + 1 << ").";
}

//Writes to log.txt when a shot is determined to be a hit
void Game::LogHit(){
  file << " It was a HIT." << endl;
}

//Writes to log.txt when a shot is determined to be a miss
void Game::LogMiss(){
  file << " It was a MISS." << endl;
}

void Game::LogShotDown(){
  file << " The missile was SHOT DOWN." << endl;
}

//Writes to log.txt when a ship is damaged
void Game::LogDamage(int shipLoc, Player p){
  if(p == USER){
    file << "The computer's " << compFleet[shipLoc].getName()       << " was damaged." 
         << " Health reduced to " 
         << compFleet[shipLoc].getHealth() 
         << "/" 
         << compFleet[shipLoc].getSize() 
         << "." 
         << endl;
  }
  else{
    file << "The player's " << userFleet[shipLoc].getName() << " was damaged." 
    << " Health reduced to " 
    << userFleet[shipLoc].getHealth() 
    << "/" 
    << userFleet[shipLoc].getSize() 
    << "." 
    << endl;
  }
}

//Writes to log.txt when a ship is sunk
void Game::LogSink(int shipLoc, Player p){
  if(p == USER){
    file << "The computer's " << compFleet[shipLoc].getName() << " was sunk." 
         << endl;
  }
  else{
    file << "The player's " << userFleet[shipLoc].getName() << " was sunk." 
         << endl;
  }
}

//Writes to log.txt whenever the user or computer wins.
void Game::LogWin(Player p){
  if(p == USER){
    file << "\n" << GetTime() << " The Player WON!" << endl;
  }
  else{
    file << "\n" << GetTime() << " The Computer WON!" << endl;
  }
}


// Gets the current date.
string Game::GetDate(){
  string timeStr;   //string to store the date
  time_t currentTime = time(nullptr);
  tm* timePtr = localtime(&currentTime);
  timeStr = 
         to_string(timePtr->tm_mday) + "/"
       + to_string((timePtr->tm_mon) + 1) + "/"
       + to_string((timePtr->tm_year)+1900);
  return timeStr;
}


// Gets the current time.
string Game::GetTime(){
  string timeStr;   //string to store the time
  time_t currentTime = time(nullptr);
  tm* timePtr = localtime(&currentTime);
  timeStr =
         to_string(timePtr->tm_hour) + ":"
       + to_string(timePtr->tm_min) +":"
       + to_string(timePtr->tm_sec);
  return timeStr;
}


/*
  Below exists all functions used for the AIOpponent class
*/

// Main AI function: creates a 2D array filled with integers representing the 
// probability that a ship is contained at its location.
// Assumes standard distribution of ships, with each coordinate as likely to 
// contain a ship as the next when looking at an empty board.
// TODO: Optimize algorithm
pair<int, int> AIOpponent::EvaluateGrid(const Game &game, int s ){
  SquareState tmpSS;        // temporary SquareState variable to be used for comparison
  int high = 0;             // value used for comparison to find the most weighted gridpoint
  int highX, highY;         // variables used to store location of the most weighted gridpoint
  int tmp[10][10] = {0};    // temporary grid of integers to represent how much weight  
                            // each square has
  // Loops through all of the spaces on the grid, incrementing values in grid tmp based on the
  // probability that a part of a ship is contained at those coordinates;
  for(int x = 0; x < 10; x++){
    for(int y = 0; y < 10; y++){
      tmpSS = game.compTargeting[x][y].getSquareState();    
      if(tmpSS == EMPTY){
        // Iterates from the argument s (representing the size of the smallest player ship AFLOAT)
        // to 5, the largest possible ship size. At each iteration, the AI checks if a ship of size i
        // can fit vertically or horizontally in the four possible directions from the current point. 
        // Weight values are then incremented by 1 for each time a ship is able to fit in the spots of
        // the given directions.
        // ex: Assuming an empty board - at x = 0, y = 0, only CheckRight and CheckDown will return true.
        //     From this, only the squares to the right and squares down will be incremented by 1 for up 
        //     to i spaces away. 
        for(int i = s; i <= 5; i++){
          if(EvalUp(game, x, y, i)){
            for(int n = 0; n < i - 1; n++){
              tmp[x][y-n] += 1;
            }
          }
          if(EvalDown(game, x, y, i)){
            for(int n = 0; n < i - 1; n++){
              tmp[x][y+n] += 1;
            }
          }
          if(EvalLeft(game, x, y, i)){
            for(int n = 0; n < i - 1; n++){
              tmp[x - n][y] += 1;
            }
          }
          if(EvalRight(game, x, y, i)){
            for(int n = 0; n < i - 1; n++){
              tmp[x + n][y] += 1;
            }
          }
        }
      } 
      // If a square is a HIT, special weighting is applied to the immediate surrounding coordinates.
      // This will check if the additional wighting can be applied UP TO 2 spaces away, but may not attempt
      // to apply weighting at all if a ship cannot fit in that direction.
      else if(tmpSS == HIT){
        if(EvalUp(game, x, y, 3)){
          tmp[x][y-1] += 100;
          tmp[x][y-2] += 50;
        }
        else if(EvalUp(game, x, y, 2)){
          tmp[x][y-1] += 100;
        }
        if(EvalDown(game, x, y, 3)){
          tmp[x][y+1] += 100;
          tmp[x][y+2] += 50;
        }
        else if(EvalDown(game, x, y, 2)){
          tmp[x][y+1] += 100;
        }
        if(EvalLeft(game, x, y, 3)){
          tmp[x-1][y] += 100;
          tmp[x-2][y] += 50;
        }
        else if(EvalLeft(game, x, y, 2)){
          tmp[x-1][y] += 100;
        }
        if(EvalRight(game, x, y, 3)){
          tmp[x+1][y] += 100;
          tmp[x+2][y] += 50;
        }
        else if(EvalRight(game, x, y, 2)){
          tmp[x+1][y] += 100;
        }
      }  
      // A square with a state of SHOT_DOWN is given immense weight to ensure that the square is retargeted.
      else if(tmpSS == SHOT_DOWN){
        tmp[x][y] += 1000;
      }
    }
  }
  // Loops through the grid again to ensure no shots are performed on squares of HIT, MISS, or SINK,
  // as these squares may have recieved weight in the previous loops.
  for(int x = 0; x < 10; x++){
    for(int y = 0; y < 10; y++){
      tmpSS = game.compTargeting[x][y].getSquareState();
      // Specification C2 - Prohibit AI wasted shots
      if(tmpSS == HIT || tmpSS == MISS || tmpSS == SINK){
        tmp[x][y] = 0;
      }
      if(tmp[x][y] > high){
        high = tmp[x][y];
        highX = x;
        highY = y;
      }
    }
  }
  //DisplayProbabilityGrid(tmp);  //uncomment for debugging
  return make_pair(highY, highX);
}


// Used solely in debugging
// Displays the weight each square is given by the AI
// Would have used DisplayGrid, but requirements are different
// i.e. DisplayProbabilityGrid prints numbers not symbols
// and takes in a 2D array of ints, not squares.
// Thought about implementing a conversion constructor, but decided
// that solution was less efficient than constructing a new function
/* EX:
    COMP PROBABILITY GRID
        1   2   3   4   5   6   7   8   9  10
      -----------------------------------------
    A | 8 | 15| 20| 23| 24| 24| 23| 20| 15| 8 |
      -----------------------------------------
    B | 15| 22| 27| 30| 31| 31| 30| 27| 22| 15|
      -----------------------------------------
    C | 20| 27| 32| 35| 36| 36| 35| 32| 27| 20|
      -----------------------------------------
    D | 23| 30| 35| 38| 39| 39| 38| 35| 30| 23|
      -----------------------------------------
    E | 24| 31| 36| 39| 40| 40| 39| 36| 31| 24|
      -----------------------------------------
    F | 24| 31| 36| 39| 40| 40| 39| 36| 31| 24|
      -----------------------------------------
    G | 23| 30| 35| 38| 39| 39| 38| 35| 30| 23|
      -----------------------------------------
    H | 20| 27| 32| 35| 36| 36| 35| 32| 27| 20|
      -----------------------------------------
    I | 15| 22| 27| 30| 31| 31| 30| 27| 22| 15|
      -----------------------------------------
    J | 8 | 15| 20| 23| 24| 24| 23| 20| 15| 8 |
      -----------------------------------------
*/
void AIOpponent::DisplayProbabilityGrid(int grid [10][10]){
  cout << "\nCOMP PROBABILITY GRID";
  cout << "\n    1   2   3   4   5   6   7   8   9  10" << endl;
  cout << "  -----------------------------------------" << endl;
  for (int row = 0; row < 10; row++) {
    for (int col = 0; col < 10; col++) {
      if (col == 0){
        switch(row){
        case 0: cout<< "A ";
          break;
        case 1: cout<< "B ";
          break;
        case 2: cout<< "C ";
          break;
        case 3: cout<< "D ";
          break;
        case 4: cout<< "E ";
          break;
        case 5: cout<< "F ";
          break;
        case 6: cout<< "G ";
          break;
        case 7: cout<< "H ";
          break;
        case 8: cout<< "I ";
          break;
        case 9: cout<< "J ";
          break;
        }
        cout << "|";
      }
      if(grid[col][row] < 100){
        cout << " ";
      }
      cout  << grid[col][row];
      if(grid[col][row] < 10){
        cout << " ";
      }
      if (col < 10) {
        cout << "|";
      }
    }
    cout << "\n";
    if (row < 9) {
      cout << "  -----------------------------------------" << endl;
    }
  }
    cout << "  -----------------------------------------\n" << endl;
}


//Checks to see if a ship of size 's' can fit vertically above point (x,y)
bool AIOpponent::EvalUp(const Game &game, int x, int y, int s){
  bool isEmpty = true;
  if(y - (s - 1) < 0){
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(game.compTargeting[x][y-n].getSquareState() == MISS 
      || game.compTargeting[x][y-n].getSquareState() == SINK){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}


//Checks to see if a ship of size 's' can fit vertically below point (x,y)
bool AIOpponent::EvalDown(const Game &game, int x, int y, int s){
  bool isEmpty = true;
  if(y + (s - 1) > 9){ 
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(game.compTargeting[x][y+n].getSquareState() == MISS 
      || game.compTargeting[x][y+n].getSquareState() == SINK){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}


//Checks to see if a ship of size 's' can fit horizontally to the right of point (x,y)
bool AIOpponent::EvalRight(const Game &game, int x, int y, int s){
  bool isEmpty = true;
  if(x + (s - 1) > 9){ 
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(game.compTargeting[x + n][y].getSquareState() == MISS 
      || game.compTargeting[x + n][y].getSquareState() == SINK){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}


//Checks to see if a ship of size 's' can fit horizontally to the left of point (x,y)
bool AIOpponent::EvalLeft(const Game &game, int x, int y, int s){
  bool isEmpty = true;
  if(x - (s - 1) < 0){ 
    isEmpty = false;
  } else{
    for(int n = 1; n < s - 1; n++){
      if(game.compTargeting[x - n][y].getSquareState() == MISS 
      || game.compTargeting[x - n][y].getSquareState() == SINK){
        isEmpty = false;
        break;
      }
    }
  }
  return isEmpty;
}


//Used by AI to determine the size of the smallest ship afloat in the player's fleet
int AIOpponent::SmallestShipAlive(const Game &game){
  int small = 5;    // variable to hold the size of the smallest ship
  int tmpInt;       // temporary int used to make comparisons to small
  for(int i = 0; i < 5; i++){
    if(game.userFleet[i].getShipState() == AFLOAT){
      tmpInt = game.userFleet[i].getSize();
      if(tmpInt < small){
        small = tmpInt;
      }
    }
  }
  return small;
}