# gym-hexario
OpenAI gym multiagent environment for the game Hexar.io.
By now it only works on linux (and macOS eventually) because it requires the native library SDL2. 

### installing SDL2
Make sure SDL2 is present on your system. 
It can be installed with all common package managers, e.g.
```
sudo pacman -Syu sdl2
sudo apt-get install libsdl2-2.0
brew install sdl2
```

### installing gym-hexario
```
git clone https://github.com/dhansmair/gym-hexario.git
cd gym-hexario
pip3 install .

```

