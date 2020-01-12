# PvE Battle Logic Flow

A heap is used to maintain some **TimelineEvent** :

| **Event Type** | **Description** |
| --- | --- |
| Announce | The start of an attack. Notify all the Pokemon&#39;s opponents. |
| Free | The Pokemon is free for a new **Action**. |
| Fast | The activation of the Pokemon&#39;s Fast Attack. Damage all its opponents. |
| Charged | The activation of the Pokemon&#39;s Charged attack. Damage all its opponents. |
| Dodge | The Pokemon performs dodging. |
| Enter | The Pokemon enters the battle. |

An **Action** object is used to communicate between the controller and the strategy. The controller asks each Player what to do on some **TimelineEvent**. The Player strategy should response with an Action object. The controller will then execute the action or buffer it, which is essentially queueing **TimelineEvent** objects accordingly.

The types of Action are:

| **Action Type** | **Description** |
| --- | --- |
| Wait | Wait and do nothing. Either finitely (`{value}` > 0) or infinitely (`{value}` <= 0). |
| Fast | Use a Fast Attack. |
| Charged | Use a Charged Attack. `{value}` is used to index which Charged Move to use.|
| Dodge | Perform a dodge. |
| Switch | Switch to another Pokemon in the current party indexed by `{value}`. |

The whole simulation battle process is basically an event loop:

- Pop the TimelineEvent from heap
- `Free` TimelineEvent produces Action (by player strategy)
- Actions produce {`Fast`, `Charged`, `Dodge`, ...} and an tailing `Free` TimelineEvent
- Until all Pokemon of one team faint, repeat.

When a Pokemon faints:

- If there is an alive Pokemon in its current party, send it to battle

- Else If the party's revive policy is true, revive the party and rejoin

- Else If the player has a next party, send the next party in

- Else check whether the player's team is defeated
