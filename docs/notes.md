# Notes

Some notes, obv.

## Refactor

- consistent vector struct
- change all to smart pointers


## Architecture
- [ ] Winmain, winproc
- [ ] convert to be like the GCC book
- [ ] move large constructors to init methods
- [ ] Implement an IScreen interface
- [ ] make consistent style
- [ ] choose Vector standard
- [ ] WinRT and DX11
- [ ] update to use std::string vs char[]



## Path Generation
- Min 3 straight
- Max 8 straight
- Max reached, turn or branch
    - Left, Right, Both (branch)
- Cannot cross over any branch
    - If 2 tiles forward will cross, turn or branch

### Turnables
- Create pre-made set-ups
- Min 3 tile in and out from Turnable
- Rotate from center point



### Checkpoints
- Generate checkpoints every 20-40 tiles
- Checkpoints tally score
- Checkpoints remove any untaken paths...