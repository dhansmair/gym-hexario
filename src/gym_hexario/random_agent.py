"""

"""
import numpy as np


class RandomAgent:
    """
    this agent behaves nearly randomly except that it is unlikely to hit the wall,
    so it won't kill itself
    """

    BORDER_CODE = 8.0
    ACTIONS = [0, 1, 2, 3, 4, 5]

    def __init__(self):
        #self.r = radius
        #self.actions = [0, 1, 2, 3, 4, 5]
        pass

    def choose_action(self, state):
        """
        select an action based on the state
        :param state:
        :return:
        """

        action_probs = []

        # indices are modulo-shifted
        for i in [3, 4, 5, 6, 1, 2]:
            val = state[i]

            if val == self.BORDER_CODE:
                action_probs.append(0)
            else:
                action_probs.append(1)

        action_probs = np.array(action_probs)
        action_probs = action_probs / action_probs.sum()
        action = np.random.choice(self.ACTIONS, 1, p=action_probs)

        return action[0]

        # collect the tiles along each direction axis
        #dirs = [[], [], [], [], [], []]
        #i = 1
        #for r in range(1, self.r):
        #    for d in range(6):
        #        dirs[(d - 2) % 6].append(state[i])
        #        i += r

        # get distance from the border in each direction
        # only if border is visible
        #distances = [self.r] * 6
        #for d in range(6):
        #    ray = dirs[d]
        #    if BORDER_CODE in ray:
        #        distances[d] = ray.index(BORDER_CODE)

        #distances = np.array(distances, dtype=np.float32)
        #probs = distances / sum(distances)
        #action = np.random.choice(self.actions, 1, p=probs)
        #return action[0]

    def choose_actions(self, states):
        """
        choose multiple actions, one for each state
        """
        return [self.choose_action(s) for s in states]

    def learn(self, states, actions, rewards, next_states, dones):
        pass
