# Bitboard Chess Engine in C

Name: Apotheosis

Author: Spamdrew

# About
As my first real experience with engine development, this project is more of a learning experience for me than anything else. My goal in creating this engine is to hit 2500 ELO CCRL.

# How to build:
Windows/Linux (you must have some version of GCC installed):

`make` will build both the debug and the main binary

`make clean` will clean all object files

The release can be built by `sh release.sh`

MacOS:

Bro I have no idea, you're on your own

# UCI
This is not a complete chess program, you need a UCI compatible program to run it.

Cute Chess is one that I typically use, you can find it here: https://github.com/cutechess/cutechess

Alternatively, you can challenge it on Lichess at https://lichess.org/@/Apotheosis_Bot, assuming I am hosting the bot at the time the request is sent.

# Special Thanks
I need to thank some people who made this possible

### Crippa (https://github.com/crippa1337/svart)
My code was inspired by Crippa's awesome engine Svart in many places because of how readable the code is. Beyond that, Crippa helped me so much by explaining fundamental concepts to me and allowing me to get to where I am now much more quickly than I otherwise would have been able to do, especially with things relating to search. He is also the one who set up the OpenBench instance I used for so many of my SPRT tests.

### Archi (https://github.com/archishou/MidnightChessEngine)
Just like Crippa, Archi is the reason I understand many of the concepts I used in Apotheosis, and is entirely who I am crediting with my aspiration windows code which gained me 100 Elo. Archi helped me more than anyone else with my eval code.

### Ciekce (https://github.com/Ciekce/Polaris)
Ciekce was always there to help me when I needed it. He is more knowledgeable with C code than I am, and several times corrected my implementations of things. He is also a big part of why my tuner is even working in the first place, because he showed me how to implement the Adam optimizer I used and helped me test my tuner.

### Adam (https://github.com/Adam-Kulju/Willow)
Adam was someone I could always talk to, and his engine Willow was a big inspiration to me because it plays in a super fun and agressive way. Just like Archi, Adam helped me a lot with my eval code.

### Gedas (https://github.com/GediminasMasaitis/chess-dot-cpp)
Gedas is a big part of why my tuner works as well. I learned a lot from him, and he helped me be sure my implementation was correct. I also took heavy inspiration from his tuner which can be found at https://github.com/GediminasMasaitis/texel-tuner

### Analog Hors (https://github.com/analog-hors/tantabus)
Hors helped me so much early on in the development process, especially with move generation. Her amazing article on magic bitboards (https://analog-hors.github.io/site/magic-bitboards/) is the reason I understand them as well as I do, and her webperft program (https://analog-hors.github.io/webperft/) was immensely helpful for debugging my movegen code.

### The Chess Programming Wiki (https://www.chessprogramming.org/Main_Page)
The CPW was an immensely helpful resource throughout this whole project and is a big part of why I know as much as I do now about chess programming. Some articles are a bit outdated and incomplete but overall, it was an awesome resource.

### Andrew Grant (https://github.com/AndyGrant/Ethereal)
Andrew is a genius and the creator of OpenBench, a program which I used to test my engine almost from the beginning. His paper on texel tuning (https://github.com/AndyGrant/Ethereal/blob/master/Tuning.pdf) was an incredible resource for me, and it is what my tuner is based off. I didn't have too many personal interactions with the man, but his work was invaluable to the success of my engine.

### The Engine Programming Discord Server
Last but certainly not least, I would like to thank everyone in the Engine Programming server. I could always count on everyone there to provide me with great help on difficult topics, and I am forever grateful for every single person in that server. Without that amazing community, I am sure that Apotheosis would not even exist.
