## Installation  
sudo apt-get install build-essential  
sudo apt-get install freeglut3-dev  
sudo apt-get install libglew-dev  
sudo apt-get install sbcl  
sudo apt-get install libmuparser-dev  

[Optional]  
sudo apt-get install doxygen # for html documentation  
sudo apt-get install ffmpeg # for video export  
sudo apt-get install libsfml-dev # for BlinkyBlocks  



#ABC Algorithm
ABC Center is an algorithm developed in the lab to elect quickly a leader that is placed near the center of a huge set of robots. You can have a look at the original paper here.  
The goal of this exercise is to program a part of this algorithm, consisting in computing a diameter (BC): i.e. one of the longest shortest distance between to modules, and elect the center of this diameter as the center.  
Write the new code that:  
Let A be a robot identified in the XML description file by isA="true", and drawn in Red.  
Elect B (drawn in yellow) as the furthest module from A.  
Elect C (drawn in blue) as the furthest module from B.  
Elect the center of BC as the center of the set, turn it to brown.  
