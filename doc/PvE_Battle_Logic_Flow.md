# PvE Battle Logic Flow

A heap is used to maintain some **TimelineEvent** :

| **Event Type** | **Description** |
| --- | --- |
| None | Default event. Ignored by the controller |
| Announce | The start of an attack. Notify all the Pokemon&#39;s opponents. |
| Free | The Pokemon is free for a new **Action**. |
| Fast | The activation of the Pokemon&#39;s Fast Attack. Deals damage to all its opponents. |
| Charged | The activation of the Pokemon&#39;s Charged attack. Deals damage to all its opponents. |
| Dodge | The Pokemon performs dodging. |
| Enter | The Pokemon enters the battle. |

An **Action** object is used to communicate between the controller and the strategy. The controller asks each Player what to do. The Player should response with an Action object. The controller will then execute the action (or buffer it), which is essentially queueing appropriate **TimelineEvent** objects accordingly. The types of Action are:

| **Action Type** | **Description** |
| --- | --- |
| None | Default action. Ignored by the controller |
| Wait | Wait and do nothing. Either finitely (`{value}` > 0) or infinitely (`{value}` <= 0). |
| Fast | Use a Fast Attack. |
| Charged | Use a Charged Attack. `{value}` is used to index which Charged Move to use.|
| Dodge | Perform a dodge. |
| Switch | Switch to another Pokemon in the current party indexed by `{value}`. |

The whole simulation battle process is basically an event loop:

- `Free` TimelineEvent produce Actions; 
- Actions produce {`Fast`, `Charged`, `Dodge`, ...} TimelineEvent and an ending `Free` TimelineEvent
- Until all Pokemon of one team faint, loop.

Initially,

- For each player:
  - Set current\_action to None
  - Set buffer\_action to None
  - Queue Enter event for head Pokemon

**Register\_and\_execute (player, action)**:

- Set current\_action to action
- Set current\_action.time to current time
- Call corresponding action register method

**handle\_event\_free** ():

- If the player&#39;s buffer\_action is None:
  - Call on\_free() to get response action
  - Register\_and\_execute that response action
- Else:
  - Register\_and\_execute buffer\_action
  - Set buffer\_action  to None
- If on\_clear is not NULL:
  - Call on\_clear() to get response action
  - Set buffer\_action to that response action

**handle\_event\_announce** ():

- For each rival player with on\_attack not NULL:
  - Call on\_attack() to get response action
  - If current\_action is None or Wait:
    - Register\_and\_execute the response action
  - Else:
    - Set buffer\_action to the response action

*more to be documented*
