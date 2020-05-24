"""
a small demo with two random agents
"""

import gym
import gym_hexario

# create an environment
n_agents = 2
env = gym.make('Hexario-v0', n_agents=n_agents)

for _ in range(100):

    env.reset('all')
    dones = [False] * n_agents

    while False in dones:
        env.render()

        actions = [env.action_space.sample() for _ in range(n_agents)]
        states, rewards, dones, infos = env.step(actions)

    print('finished episode')
