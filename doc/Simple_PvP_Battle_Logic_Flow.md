# Simple PvP Battle Logic Flow

Initially, for each Pokemon i = 0, 1:

- Set cooldown[i] to 0
- Set decision[i] to None

While both Pokemon are alive:

- For each Pokemon i = 0, 1:
  - If decision[i] is None and cooldown <= 0:
    - Ask for turn decision[i]
  - If at least one turn decision is None:
    - Process the decision of the other
  - Else if both decisions are Fast Attack:
    - Process both turn decisions
    - break
  - Else if decision[i] is Fast Attack and decision[1-i] is Charged Attack:
    - Process Pokemon i action
    - If Pokemon (1-i) is still alive, process decision[1-i]
    - break
  - Else if both decisions are Charged Attack:
    - Process Pokemon 0 action
    - If Pokemon 1 is still alive, process Pokemon 1 action
    - break
  - End If
- Reduce both cooldown by 1
