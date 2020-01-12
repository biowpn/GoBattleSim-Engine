# PvE Dodge Strategy

**on_free** :

- Based on enemy current action, calculate damage_time
- If damage_time < time_free OR (has dodged this attack):
  - Predict damage_time for next enemy attack and assign it to damage_time
- Time_till_damage = damage_time – current_time
- If time_till_damage > cmove_duration and have enough energy:
  - Use cmove
- Else If time_till_damage > fmove_duration:
  - Use fmove
- Else:
  - Wait
- End If

**on_attack** :

- Calculate time_till_damage
- If time_till_damage > cmove_duration and have enough energy:
  - Use cmove
- Else If time_till_damage > fmove_duration:
  - Use fmove
- Else:
  - Dodge, no earlier than dodge window open
- End If
