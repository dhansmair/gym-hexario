import gym
import numpy as np
from gym import spaces
import gym_hexario.game_module as game_module


LOCAL = 'local'
GLOBAL = 'global'
DISCRETE = 'discrete'
CONTINUOUS = 'continuous'
FEATURES = 'features'
PIXEL = 'pixels'
FULL = 'full'


class HexarioEnv(gym.Env):
    def __init__(self,
                 n_agents=1,
                 n_random_agents=0,
                 map_radius=10,
                 max_steps=500,
                 perspective=LOCAL,
                 observation_space=FEATURES,
                 observation_radius=5,
                 px_observation_dims=(84, 84),
                 action_space=DISCRETE,
                 velocity=0.1,
                 reward_timestep=0,
                 reward_capture=1,
                 reward_claim=0,
                 reward_kill=100):
        """
        OpenAI gym environment for the game Hexar.io

        :param n_agents:                        number of agents
        :param n_random_agents:                 number of random agents. These are environment internal and cannot be
                                                controlled
        :param map_radius:                      size of the arena
        :param max_steps:                       maximum number of steps the agents are allowed to take in one epoch
        :param perspective                      [local|global]
        :param observation_space:               [features|pixel]
        :param observation_radius:
        :param px_observation_dims:
        :param action_space:                    [discrete|continuous]
        :param reward_timestep:
        :param reward_capture:
        :param reward_claim:
        :param reward_kill:
        """

        # do some integrity checks
        assert n_agents > 0 and isinstance(
            n_agents, int), f'invalid value for parameter n_agents: {n_agents}'
        assert n_random_agents >= 0 and isinstance(
            n_random_agents, int), f'invalid value for parameter n_random_agents: {n_random_agents}'
        assert map_radius > 2 and isinstance(
            map_radius, int), f'invalid value for parameter map_radius: {map_radius}'
        assert max_steps > 0 and isinstance(
            max_steps, int), f'invalid value for parameter max_steps: {max_steps}'
        assert observation_radius == FULL or (observation_radius > 1 and isinstance(observation_radius, int)), \
            f'invalid argument observation_radius: {observation_radius}'
        assert perspective in (
            LOCAL, GLOBAL), f'invalid value for parameter perspective: {perspective}'
        assert observation_space in (FEATURES, PIXEL), \
            f'invalid value for parameter observation_space: {observation_space}'
        assert action_space in (
            DISCRETE, CONTINUOUS), f'invalid value for parameter action_space: {action_space}'
        assert isinstance(px_observation_dims, tuple) and len(px_observation_dims) == 2, \
            f'invalid value for parameter pixel_observation_dimensions: {px_observation_dims}'
        assert not (observation_space == FEATURES and action_space == CONTINUOUS), \
            f'invalid parameters: feature observation space and continuous action space are incompatible'

        # TODO maybe find a nice way to implement this
        # TODO the question is when to reset the agents
        assert n_agents == 1 or n_random_agents == 0, \
            f'currently, multiple agents with multiple random agents is not supported!'

        assert isinstance(velocity, float) and velocity > 0 and velocity <= 1, \
            f'invalid parameter velocity: {velocity}'

        self.n_agents = n_agents
        self.n_random_agents = n_random_agents
        self.map_radius = map_radius
        self.max_steps = max_steps
        self.perspective = perspective
        self.observation_radius = 2 * self.map_radius if observation_radius == FULL else observation_radius
        self.px_observation_dims = px_observation_dims
        self.velocity = velocity

        # these variable names should not be used because they are for gym en
        self.observation_type = observation_space
        self.action_type = action_space

        self.reward_timestep = reward_timestep
        self.reward_capture = reward_capture
        self.reward_claim = reward_claim
        self.reward_kill = reward_kill

        self.max_capture = self.__get_num_tiles()
        # keep track of the number of steps the agents already
        # have taken. Reset an agent when it has reached the max_steps
        self.__taken_steps = [0 for _ in range(n_agents)]

        # check the action spaces
        if self.action_type == DISCRETE:
            self.action_space = spaces.Discrete(6)
        else:
            self.action_space = spaces.Box(low=0., high=2 * np.pi, shape=(1,))

        # check the obs space
        if self.observation_type == FEATURES:
            if self.perspective == LOCAL:
                self.obs_size = 1 + 3 * self.observation_radius \
                    * (self.observation_radius - 1)
            else:
                self.obs_size = 1 + 3 * self.map_radius \
                                * (self.map_radius - 1)

            self.observation_space = spaces.Box(low=0., high=10.,
                                                shape=(self.obs_size,),
                                                dtype=np.float32)
            # override own private method
            self.__get_observation = self.__get_discrete_observation
        else:
            # screen pixels obs
            self.obs_size = self.px_observation_dims[0] * \
                self.px_observation_dims[1] * 3
            self.observation_space = spaces.Box(low=0, high=255, dtype=np.float32,
                                                shape=(*self.px_observation_dims, 3))
            # override own private method
            self.__get_observation = self.__get_pixel_observation

        # check number of agents and set reset and step accordingly
        if self.n_agents == 1:
            self.reset = self.__reset_single_agent
            self.step = self.__step_single_agent
        else:
            self.reset = self.__reset_multi_agent
            self.step = self.__step_multi_agent

        # access layer for the c++ implementation
        self.__game = game_module.setup(self.map_radius,
                                        self.n_agents + self.n_random_agents,
                                        self.perspective == LOCAL,
                                        self.observation_type == FEATURES,
                                        self.action_type == DISCRETE,
                                        self.observation_radius,
                                        self.px_observation_dims[0],
                                        self.px_observation_dims[1],
                                        self.velocity)

    def __del__(self):
        del self.__game

    def render(self, mode='human'):
        """displays the whole board on screen"""
        return game_module.show(self.__game)

    def reset(self, agents=None):
        """ gets overridden """
        raise NotImplemented

    def step(self, action):
        """ gets overridden """
        raise NotImplemented

    def __get_num_tiles(self):
        return 1 + 3 * self.map_radius * (self.map_radius - 1)

    def __get_observation(self, agent_id):
        """ gets overridden """
        raise NotImplementedError

    def __reset_single_agent(self):
        """ """
        self.__taken_steps[0] = 0
        game_module.reset_player(self.__game, 0)

        # reset the random agents
        for agent_id in range(1, self.n_random_agents + 1):
            game_module.reset_player(self.__game, agent_id)

        return self.__get_observation(0)

    def __reset_multi_agent(self, agents=()):
        """ """
        n_obs = []

        if agents == 'all':
            agents = [i for i in range(self.n_agents)]

        for agent_id in agents:
            self.__taken_steps[agent_id] = 0
            game_module.reset_player(self.__game, agent_id)

        # TODO what happens to the random agents?

        for agent_id in agents:
            n_obs.append(self.__get_observation(agent_id))

        return n_obs

    def __step_single_agent(self, action):
        """ """
        actions = np.array([action], dtype=np.float32).flatten()
        assert len(actions) == 1

        reward = self.reward_timestep

        captures_before = game_module.get_num_captures(self.__game, 0)
        claims_before = game_module.get_num_claims(self.__game, 0)
        kills_before = game_module.get_num_kills(self.__game, 0)

        if self.n_random_agents > 0:
            # decide the actions for the random agents
            actions = np.concatenate([actions,
                                     [self.__get_random_agent_action(i) for i in range(1, self.n_random_agents + 1)]])
            actions = actions.flatten().astype(np.float32)

        game_module.take_actions(self.__game, actions)
        self.__taken_steps[0] += 1

        # check which players are still in the game
        dones = [game_module.is_dead(self.__game, i) for i in range(1 + self.n_random_agents)]
        is_dead = dones[0]
        is_winner = game_module.is_winner(self.__game, 0)
        captures_after = game_module.get_num_captures(self.__game, 0)
        claims_after = game_module.get_num_claims(self.__game, 0)
        kills_after = game_module.get_num_kills(self.__game, 0)

        # check if the agent is the last one alive
        all_others_dead = True
        if self.n_random_agents > 0:
            all_others_dead = False not in dones[1:]

        if not is_dead:
            reward += self.reward_capture * (captures_after - captures_before)
            reward += self.reward_claim * max(claims_after - claims_before, 0)
            reward += self.reward_kill * max(kills_after - kills_before, 0)

        info = dict(
            num_kills=kills_after,
            num_captures=captures_after,
            num_claims=claims_after,
            coverage=captures_after / self.max_capture,
            success=is_winner,
            distances=game_module.get_distances(self.__game, 0),
            all_others_dead=all_others_dead
        )

        done = is_dead or is_winner or (
            self.__taken_steps[0] == self.max_steps)
        observation = self.__get_observation(0)

        return observation, reward, done, info

    def __step_multi_agent(self, actions):
        """ an action must be passed for every agent """
        assert len(actions) == self.n_agents, "Error: number of \
            given actions doesn't match number of agents in this game"

        if isinstance(actions, list):
            actions = np.array(actions, dtype=np.float32)

        assert isinstance(actions, np.ndarray), "Error: given actions \
            are in the wrong format"

        actions = actions.flatten()

        n_obs = []
        n_kills = []
        n_claims = []
        n_captures = []
        n_dones = []
        n_rewards = []
        n_infos = []

        # cache all the kills and captures
        for agent_id in range(self.n_agents):
            n_kills.append(game_module.get_num_kills(self.__game, agent_id))
            n_claims.append(game_module.get_num_claims(self.__game, agent_id))
            n_captures.append(
                game_module.get_num_captures(self.__game, agent_id))

        # take all the actions
        game_module.take_actions(self.__game, actions)

        # get the dones, kills, observations
        for agent_id in range(self.n_agents):
            self.__taken_steps[agent_id] += 1
            reward = self.reward_timestep

            is_dead = game_module.is_dead(self.__game, agent_id)
            is_winner = game_module.is_winner(self.__game, 0)

            kills = game_module.get_num_kills(self.__game, agent_id)
            claims = game_module.get_num_claims(self.__game, agent_id)
            captures = game_module.get_num_captures(self.__game, agent_id)

            if not is_dead:
                reward += self.reward_capture * \
                    (captures - n_captures[agent_id])
                reward += self.reward_claim * \
                    (claims - n_claims[agent_id])
                reward += self.reward_kill * (kills - n_kills[agent_id])

            info = dict(
                num_kills=kills,
                num_captures=captures,
                num_claims=claims,
                coverage=captures / self.max_capture,
                success=is_winner,
                distances=game_module.get_distances(self.__game, agent_id)
            )

            done = is_dead or is_winner or (
                self.__taken_steps[agent_id] == self.max_steps)
            obs = game_module.get_observation(self.__game, agent_id)

            n_dones.append(done)
            n_rewards.append(reward)
            n_obs.append(obs)
            n_infos.append(info)

        return n_obs, n_rewards, n_dones, n_infos

    def __get_discrete_observation(self, agent_id):
        """ """
        return game_module.get_observation(self.__game, agent_id)

    def __get_pixel_observation(self, agent_id):
        """ """
        observation = game_module.get_observation(self.__game, agent_id)

        return observation.reshape(
            self.px_observation_dims[1],
            self.px_observation_dims[0], 3)

    def __get_random_agent_action(self, random_agent_id):

        distances = game_module.get_distances(self.__game, random_agent_id)
        probs = np.array([1 if d > 1 else 0 for d in distances], dtype=np.float32)
        probs = probs / sum(probs)
        action = np.random.choice([0, 1, 2, 3, 4, 5], 1, p=probs)
        return action[0]
