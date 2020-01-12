# PvP Strategy

**Better move** : The move with the higher DPE

**Cheaper move** : The move with the lower energy cost

- If Fast move can kill the opponent:
  - Use Fast move

- Else If the better move is also the cheaper move
  - If have enough energy:
    - Use it
  - Else:
    - Use Fast move
  - End If

- Else If enough energy to use the better move:
  - If the opponent has any shield left:
    - Use the cheaper move to bait
  - Else:
    - Use the better move
  - End If
- Else If enough energy to use the cheaper move:
  - If the cheaper move can kill the opponent:
    - Use the cheaper move
  - Else if the opponent's next fast move can kill you:
    - Use the cheaper move
  - Else:
    - Use Fast move
  - End If
- Else:
  - Use Fast move
- End If
