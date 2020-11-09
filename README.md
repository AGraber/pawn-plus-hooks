# pawn-plus-hooks

[![sampctl](https://shields.southcla.ws/badge/sampctl-pawn--plus--hooks-2f2f2f.svg?style=for-the-badge)](https://github.com/AGraber/pawn-plus-hooks)


## Installation

Simply install to your project:

```bash
sampctl package install AGraber/pawn-plus-hooks
```

Include in your code and begin using the library:

```pawn
#include <pp-hooks>
```

## Usage

If you're familiar with y_hooks then the syntax will be familiar:

```pawn
#include <pp-hooks>
hook OnPlayerEnterDynArea(playerid, areaid)
{
    new string[145];
    format(string, sizeof string, "You entered area id %d", areaid);
    SendClientMessage(playerid, -1, string);
}


#include <pp-hooks>
hook OnPlayerEnterDynArea(playerid, areaid)
{
    printf("playerid %d entered areaid %d", playerid, areaid);
}

hook native SetPlayerPos(playerid, Float:x, Float:y, Float:z)
{
    // Not recursive: calls next hook or the native
    return SetPlayerPos(playerid, x, y, z + 1.0);
}

hook ret OnPlayerUpdate(&ret, playerid)
{
    if(IsPlayerCheating(playerid))
    {
        // set return to 0 to desync
        ret = 0;
        // return non-zero to use that ret value
        return 1;
    }
}
```

You can change the default hook method (used by the `hook` and `hook public` macros) by defining `PP_DEFAULT_PUBLIC_HOOK_METHOD` to the desired method.

If the hook returns a non-zero value, the following hooks and the base callback won't be executed and that value (or the one at ret if it's a `ret` hook) will be returned.

# Thanks
- Y_Less: YSI macros used for reference
- IllidanS4: PawnPlus
