# gym-hexario

OpenAI gym environment for the game Hexar.io


## requirements
- python 3
- native graphics library SDL2 to be on your machine
- ... TODO include SDL2 sources 

### installing SDL2

#### arch-based
```
sudo pacman -Syu sdl2
```
#### debian-based
```
sudo apt-get install libsdl2-2.0
```
#### macOS
```
brew install sdl2
```
#### windows
```
?
```


## dependencies
- gym
- numpy
- ... TODO check which specific versions of dependencies are needed

## installation
```
git clone git@gitlab2.cip.ifi.lmu.de:hansmair/gym-hexario.git
cd gym-hexario
pip3 install .

```

## environments

| name       | agents | observability | action space | observation space
| ---------- | ------ | ------------- | ------------ | ---------------- 
| Hexario-v0 | 1      | full          | {0,..,5}     | discrete
| Hexario-v1 | 1      | full          | [0;1)        | pixel (210x160x3)

... TODO specify environments

## uninstall
```
pip uninstall gym-hexario
```

## todo