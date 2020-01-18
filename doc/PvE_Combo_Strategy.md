# PvE Combo Strategy

**Better move** : The move that yields higher cycle DPS paired with the Fast move

**Cheaper move** : The move with the lower energy cost

- If Fast move can kill the opponent:
  - Use Fast move

- Else If the better move is also the cheaper move
  - If have enough energy:
    - Use it
  - Else:
    - Use Fast move
  - End If

- Else If have enough energy for the better move:
  - Use it

- Else If the enemy's charged move is ready and can kill me
    OR 2 enemy's fast attacks can kill me:
  - If have enough energy for the cheaper move:
    - Use it
  - Else
    - Use Fast move
  - End If

- Else
  - Use Fast Move

- End If
