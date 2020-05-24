from gym.envs.registration import register
"""
all possible parameters and their default values:

'n_agents': 1,
'map_radius': 10,
'partially_observable': False,
'discrete_obs_space': True,
'discrete_action_space': True,
'obs_discrete_radius': 10,
'obs_continuous_width': 210,
'obs_continuous_height': 160
"""

register(
    id='Hexario-v0',
    entry_point='gym_hexario.envs:HexarioEnv',
    kwargs={
        'n_agents': 1,
        'map_radius': 20,
        'max_steps': 500,
        'perspective': 'local',
        'observation_space': 'features',
        'observation_radius': 8,
        'action_space': 'discrete',
        'reward_timestep': 0.0,
        'reward_capture': 1.0,
        'reward_claim': 0.0,
        'reward_kill': 100.0
    }
)

register(
    id='Hexario-v1',
    entry_point='gym_hexario.envs:HexarioEnv',
    kwargs={
        'n_agents': 1,
        'map_radius': 50,
        'max_steps': 500,
        'perspective': 'local',
        'observation_space': 'features',
        'observation_radius': 8,
        'action_space': 'discrete',
        'reward_timestep': 0.0,
        'reward_capture': 1.0,
        'reward_claim': 0.0,
        'reward_kill': 100.0
    }
)

register(
    id='Hexario-v2',
    entry_point='gym_hexario.envs:HexarioEnv',
    kwargs={
        'n_agents': 1,
        'n_random_agents': 3,
        'map_radius': 20,
        'max_steps': 500,
        'perspective': 'local',
        'observation_space': 'features',
        'observation_radius': 8,
        'action_space': 'discrete',
        'reward_timestep': 0.0,
        'reward_capture': 1.0,
        'reward_claim': 0.0,
        'reward_kill': 100.0
    }
)
