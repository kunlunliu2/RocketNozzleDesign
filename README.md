# RocketNozzleDesign
This repository contains one of my apps to generate the rocket Nozzle. The calculation and principles of design were based on the reference books: (1) rocket propulsion elements by G.P. Sutton and O. Biblarz, (2) Physics of rockets, by Selfert, Mille, and Summerfield, and (3) Fundamental of rocket propulsion, by D.P. Mishra. The aim of this effort is for benefiting the learning and practice of rocket design.  
  
How to use the apps?
  (1) compile the code using Qt.
  (2) or you can unzip the Executable.rar and run the rocketNozzle.exe in the folder.
  
As a test of the code, I back-engineered the rocket nozzle profiles using the published information for the Merline engine by SpaceX (https://en.wikipedia.org/wiki/SpaceX_Merlin), the NASA RS-25 engine (https://rocket.com/space/liquid-engines/rs-25-engine and https://en.wikipedia.org/wiki/RS-25) and the Chinese YF-77 engine (https://en.wikipedia.org/wiki/YF-77). These PNGs list the parameters to match the propulsion data for these engines. The code generates the nozzle profile to reach the propulsion targets. However, the converge and diverge angel of curves are determined by the test and high fidelity 3D simulation which is beyond the scope of this project. In this code, the converge and diverge of curves are following the testbooks (rocket propulsion elements, Physics of rockets and Fundamental of rocket propulsion).

In this version, I provide a one-dimensional heat transfer code to calculate the wall temperature of the nozzle. This one-dimensional conjugate heat transfer code gives a rough estimation of the metal and gas temperature. This function is needed for the conception study. 
