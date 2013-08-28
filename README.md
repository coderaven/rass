RASS
=====

Raven Automated Server Setup - by Raven G. Duran MUST BSIT 3R1 
RASS automatically sets up your APACHE web and BIND dns servers.

- Created for personal use to automate everything when I'm adding a new domain to my web and dns servers.
- Easily extendable to add Mail server lookup
- Configuration style is based from our Linux IT Subject (Credits to sir Jeyran Labrador)

Just follow the interactive commandline application and you're good to go!

How to run
----------
- Download Rass (Zipped)
- Extract the Zipped file
- Open your command line and go to the directory where you have extracted RASS
- then type in your command line: sudo ./rass
- that's it! Enjoy :D


Screenshot
----------
![Screenshot](http://i.imgur.com/2JL4fb9.png)


How to modify and compile RASS
------------------------------
if you want to modify RASS for good, then follow these:
- Go to RASS directory
- Modify rass.cpp and add to it your code
- on the command line on the same directory as RASS, type and enter: g++ rass.cpp -o rass
- type and enter to run RASS: ./rass
- ENJOY :D
