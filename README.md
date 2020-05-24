# gym-hexario
OpenAI gym multiagent environment for the game Hexar.io.  
By now it only works on linux (and macOS eventually) because it requires the native library SDL2. 

### installing SDL2
Make sure SDL2 is present on your system. 
It can be installed with all common package managers, e.g.
```
sudo pacman -Syu sdl2               # arch based
sudo apt-get install libsdl2-2.0    # debian etc
brew install sdl2                   # macOS
```

### installing gym-hexario
```
git clone https://github.com/dhansmair/gym-hexario.git
cd gym-hexario
pip3 install .

```

### code example
```
import gym
import gym_hexario

# create the environment
n_agents = 2
env = gym.make('Hexario-v0', n_agents=n_agents)

# run 100 episodes with random actions
for _ in range(100):

    env.reset('all')
    dones = [False] * n_agents

    while False in dones:
        env.render()

        actions = [env.action_space.sample() for _ in range(n_agents)]
        states, rewards, dones, infos = env.step(actions)

    print('finished episode')
```

### configuration
The environment can be configured through various optional arguments for the gym.make(...) function.  
```
kwargs={
    'n_agents': 1,
    'map_radius': 20,                   # size of the board
    'max_steps': 500,
    'perspective': 'local',             # 'local' | 'global'
    'observation_space': 'features',    # 'features' | 'pixels'
    'observation_radius': 8,
    'action_space': 'discrete',         # 'discrete' | 'continuous'
    'reward_timestep': 0.0,
    'reward_capture': 1.0,
    'reward_claim': 0.0,
    'reward_kill': 100.0
}

env = gym.make('Hexario-v0', **kwargs)

# ...
```
