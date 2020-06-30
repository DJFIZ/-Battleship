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
