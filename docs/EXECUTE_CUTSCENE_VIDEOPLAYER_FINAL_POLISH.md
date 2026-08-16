# EXECUTION TASK — VIDEO PLAYER + CUTSCENE FLOW + FINAL POLISH
## Dragon Asteroid Run

> Implement this task incrementally. Do not rewrite working systems. Keep the project buildable.

## 1. FINAL FLOW

```text
MAIN MENU
→ START GAME
→ OPENING CUTSCENE
→ GAMEPLAY BRIEFING
→ LEVEL 1
→ LEVEL 2
→ LEVEL 3
→ LEVEL 4
→ LEVEL 5
→ MID CUTSCENE
→ LEVEL 6
→ ENDING CUTSCENE
→ MISSION COMPLETE
→ MAIN ENCYCLOPEDIA UNLOCKED
→ EARTH — OUR KNOWN HOME
```

Videos already exist:

```text
assets/video/opening
assets/video/mid
assets/video/ending
```

Use the actual extensions already present. Do not generate, replace, or move them.

---

# 2. PHASE A — FIX VideoPlayer FIRST

Relevant files:

```text
src/core/VideoPlayer.h
src/core/VideoPlayer.cpp
```

The current VideoPlayer uses Media Foundation. Fix compilation before doing anything else.

### 2.1 Private Impl access

`VideoPlayer.h` currently uses:

```cpp
private:
    struct Impl;
    Impl* impl = nullptr;
```

Keep `Impl` private.

`MediaEngineEvents` in `VideoPlayer.cpp` accesses `VideoPlayer::Impl*` and `owner->finished`, causing private-access errors.

Fix this minimally with a valid friend/access solution.

Do NOT make `Impl` public.
Do NOT remove PImpl.
Do NOT rewrite the architecture.

### 2.2 Correct video window attribute

The current code uses:

```cpp
MF_MEDIA_ENGINE_VIDEO_WINDOW
```

This is incorrect.

Use:

```cpp
MF_MEDIA_ENGINE_PLAYBACK_HWND
```

for rendering to the existing game HWND.

Keep the existing HWND lookup if it works.

### 2.3 Remove unnecessary audio-category configuration

The current code uses:

```cpp
AudioCategory_ForegroundOnlyMedia
MF_MEDIA_ENGINE_AUDIO_CATEGORY
```

This is unnecessary for the current VideoPlayer requirements.

Remove that configuration rather than adding unnecessary dependencies.

Video must still play its embedded audio normally.

### 2.4 Fix GetCurrentTime

Incorrect:

```cpp
double currentTime = 0.0;
impl->engine->GetCurrentTime(&currentTime);
```

Correct:

```cpp
double currentTime = impl->engine->GetCurrentTime();
```

### 2.5 Fix GetDuration

Incorrect:

```cpp
double duration = 0.0;
impl->engine->GetDuration(&duration);
```

Correct:

```cpp
double duration = impl->engine->GetDuration();
```

### 2.6 VideoPlayer responsibilities

Keep the existing simple API:

```text
open(path)
play()
update()
isFinished()
isOpen()
close()
```

It must:
- open a video
- render it to the game window
- play embedded audio
- detect completion
- close safely

Do not add unnecessary features.

### 2.7 Build checkpoint

After fixing VideoPlayer:

1. Build.
2. Fix only errors directly related to VideoPlayer.
3. Confirm compilation succeeds.
4. Only then continue to cutscene integration.

Do not work on UI/gameplay while VideoPlayer is broken.

---

# 3. VIDEO PATH / RUNTIME

After compilation succeeds, test that:
- video opens
- video renders
- embedded audio plays
- completion is detected
- close works safely

Do not make unrelated path/refactoring changes unless a real runtime issue appears.

---

# 4. AUDIO RULE

For EVERY cutscene:

```text
GAME MUSIC
→ STOP / PAUSE

VIDEO
→ PLAY WITH EMBEDDED AUDIO

VIDEO FINISHES
→ VIDEO AUDIO STOPS

GAME MUSIC
→ RESUME / START
```

Never allow game music and video audio to overlap.

Reuse the existing audio system. Do not create another audio manager.

---

# 5. OPENING CUTSCENE

Required:

```text
MAIN MENU
→ START GAME
→ OPENING VIDEO
→ GAMEPLAY BRIEFING
→ LEVEL 1
```

Opening:

```text
assets/video/opening
```

When Start Game is selected:
- stop/pause game music
- open and play opening
- block gameplay input
- do not run Level 1 behind the video

After video:
- close VideoPlayer
- stop video audio
- restore gameplay music
- show Gameplay Briefing

Do not skip directly to Level 1.

---

# 6. GAMEPLAY BRIEFING

Keep it short and interactive, as if the player is inside the game.

Suggested content:

```text
THE ANOMALY HAS BEGUN.

Your journey starts here.

Explore the unknown.
Watch your surroundings.
Discover the phenomena you encounter.

Survive. Learn. Keep moving.
```

Show the ACTUAL controls already used by the game. Do not invent controls.

Example only if accurate:

```text
MOVE — W / A / S / D
```

Add a rounded:

```text
CONTINUE
```

button.

After Continue → Level 1.

Keyboard must work immediately. The player must not click the window first.

---

# 7. MID CUTSCENE

Required:

```text
LEVEL 5 COMPLETE
→ MID VIDEO
→ LEVEL 6
```

Video:

```text
assets/video/mid
```

Only start after Level 5 is actually completed.

During video:
- stop/pause game music
- play video + embedded audio
- block gameplay input
- do not run Level 5 behind it

After video:
- close VideoPlayer
- stop video audio
- restore game music
- enter Level 6

Do not enter Level 6 before the video finishes.

---

# 8. ENDING CUTSCENE

Required:

```text
LEVEL 6 COMPLETE
→ ENDING VIDEO
→ MISSION COMPLETE
```

Video:

```text
assets/video/ending
```

When Level 6 completes:
- stop gameplay
- stop/pause game music
- play ending
- block gameplay input

After video:
- close VideoPlayer
- stop video audio
- do not return directly to Main Menu
- show Mission Complete

---

# 9. MISSION COMPLETE

Keep it concise.

Suggested structure:

```text
MISSION COMPLETE

THE JOURNEY OF THE DRAGON

From the first anomaly,
the Dragon travelled through asteroid fields,
cosmic storms, comets, nebulae,
and unstable gravitational forces.

Each challenge revealed another piece
of the cosmic disturbance.

At the heart of the anomaly,
the Dragon restored the balance.

THE MISSION IS COMPLETE.
```

Then:

```text
BUT...

IS THE DRAGON'S STORY REALLY OVER?
```

Use a rounded Continue button.

Do not turn this into a long essay.

---

# 10. MAIN ENCYCLOPEDIA UNLOCK

After Mission Complete Continue:

```text
MAIN ENCYCLOPEDIA UNLOCKED
→ EARTH — OUR KNOWN HOME
```

If an Encyclopedia already exists:
- reuse it
- do not create a second encyclopedia
- preserve existing scan entries

Suggested final Earth message:

```text
EARTH — OUR KNOWN HOME

Earth is the planet we call home
and the only planet currently known
to support life.

After travelling across the universe,
perhaps the most important world to understand
is the one beneath our feet.
```

Keep it scientific, reflective, concise, and mysterious.

Do not claim that Earth is definitively the only life-bearing planet in the entire universe.

---

# 11. MAIN MENU LOGO

Asset:

```text
assets/ui/dragon-asteroid-run-title
```

Use the actual extension.

Current problem:
- stretched/distorted
- poor-looking resolution

Fix rendering only.

Requirements:
- preserve original aspect ratio
- never stretch X and Y independently
- preserve transparency
- keep it sharp
- position it cleanly

Scaling must use the same factor for X and Y.

Do not regenerate or replace the asset.

---

# 12. UI STYLE

Primary UI color:

```text
PURPLE
```

New/modified UI should use:
- rounded corners
- clean spacing
- centered text
- readable typography
- consistent margins
- consistent button dimensions

Avoid rigid sharp rectangles.

Do not redesign the entire UI.

---

# 13. BUTTON TEXT

Previous issue: text exceeded button bounds.

Fix all touched buttons:
- text centered horizontally and vertically
- enough padding
- no clipping
- no text outside the rounded rectangle

If text is too long, prefer increasing button width before making text very small.

---

# 14. KEYBOARD RESPONSIVENESS

Current issue: keyboard input sometimes requires mouse/cursor interaction first.

Fix active menu/overlay input so keyboard works immediately when the screen appears.

At minimum:
- Gameplay Briefing Continue works with keyboard
- Mission Complete Continue works with keyboard
- existing menu navigation remains functional
- gameplay keyboard controls remain unchanged

Use the existing input system.

Do not introduce a parallel input system.

---

# 15. VIDEO DISPLAY

Video must:
- fit the game window
- preserve its aspect ratio
- not stretch
- not unintentionally crop

If aspect ratios differ, prefer clean letterboxing rather than distortion.

Do not modify the source videos.

---

# 16. STATE / SCENE INTEGRATION

Use the existing state/scene architecture.

Conceptual states:

```text
MAIN_MENU
OPENING_CUTSCENE
GAMEPLAY_BRIEFING
LEVEL_1
LEVEL_2
LEVEL_3
LEVEL_4
LEVEL_5
MID_CUTSCENE
LEVEL_6
ENDING_CUTSCENE
MISSION_COMPLETE
MAIN_ENCYCLOPEDIA
```

Map them to the project's actual architecture.

Do not rename large amounts of existing code just for naming consistency.

---

# 17. DO NOT BREAK EXISTING GAMEPLAY

Do not modify unless integration requires it:

- Dragon movement
- collision
- obstacle spawning
- score
- questions
- level mechanics
- Level 5 gravity
- Level 6 black hole mechanics
- existing encyclopedia data
- existing assets
- existing gameplay audio
- level progression

This task is:

```text
INTEGRATION + BUG FIX + FINAL POLISH
```

not a gameplay rewrite.

---

# 18. IMPLEMENTATION ORDER

Follow this order:

## Phase A — Compilation
1. Inspect VideoPlayer.h/cpp.
2. Fix private Impl access.
3. Fix `MF_MEDIA_ENGINE_VIDEO_WINDOW`.
4. Remove unnecessary audio-category configuration.
5. Fix `GetCurrentTime()`.
6. Fix `GetDuration()`.
7. Build.

## Phase B — Runtime
8. Test opening video.
9. Confirm rendering.
10. Confirm embedded audio.
11. Confirm completion.
12. Confirm cleanup.

## Phase C — Opening
13. Start Game → Opening.
14. Opening → Briefing.
15. Briefing → Level 1.
16. Verify audio.

## Phase D — Mid
17. Level 5 complete → Mid.
18. Mid → Level 6.
19. Verify audio.

## Phase E — Ending
20. Level 6 complete → Ending.
21. Ending → Mission Complete.
22. Mission Complete → Main Encyclopedia.

## Phase F — UI
23. Fix logo aspect ratio.
24. Fix button text.
25. Fix keyboard responsiveness.
26. Check purple/rounded consistency.

---

# 19. ERROR HANDLING

If a video fails to open:
- do not crash
- log a useful error
- safely continue according to the game flow

Fallbacks:

```text
Opening failure → Gameplay Briefing / Level 1
Mid failure → Level 6
Ending failure → Mission Complete
```

Do not leave the game permanently black.

---

# 20. TOKEN / AGENT EFFICIENCY

VERY IMPORTANT:

Do not read/rewrite the entire repository.

First locate only:

```text
VideoPlayer.h
VideoPlayer.cpp
main/game entry
game state/scene system
audio/music system
main menu
level transition
encyclopedia
```

Edit only relevant files.

Do not:
- refactor unrelated code
- rename unrelated files
- replace working classes
- add unnecessary libraries
- generate assets
- regenerate videos
- change gameplay mechanics
- spend tokens explaining the whole repository

Reuse existing functions/classes whenever possible.

Priority:

```text
BUILD
→ RUNTIME
→ CUTSCENE FLOW
→ AUDIO
→ UI POLISH
```

If something already works, leave it alone.

---

# 21. TEST CHECKLIST

## VideoPlayer
- [ ] Project compiles
- [ ] No VideoPlayer compilation errors
- [ ] Video opens
- [ ] Video renders
- [ ] Video audio plays
- [ ] Completion detected
- [ ] Video closes safely

## Opening
- [ ] Start Game opens opening video
- [ ] Game music does not overlap video
- [ ] Opening finishes
- [ ] Video audio stops
- [ ] Briefing appears
- [ ] Keyboard works immediately
- [ ] Continue enters Level 1

## Levels
- [ ] Level 1 unchanged
- [ ] Level 2 unchanged
- [ ] Level 3 unchanged
- [ ] Level 4 unchanged
- [ ] Level 5 unchanged

## Mid
- [ ] Starts only after Level 5
- [ ] Game music stops
- [ ] Video audio plays
- [ ] Gameplay blocked
- [ ] Video finishes
- [ ] Game music resumes
- [ ] Level 6 starts

## Ending
- [ ] Starts only after Level 6
- [ ] Game music stops
- [ ] Video audio plays
- [ ] Gameplay blocked
- [ ] Video finishes
- [ ] Mission Complete appears
- [ ] Journey summary appears
- [ ] Hanging question appears
- [ ] Continue works

## Encyclopedia
- [ ] Main Encyclopedia unlocks
- [ ] Existing entries remain
- [ ] Earth entry exists
- [ ] Earth message is readable

## Main Menu
- [ ] Logo is not stretched
- [ ] Aspect ratio preserved
- [ ] Logo remains sharp
- [ ] Main menu remains functional

## Input
- [ ] Keyboard works immediately on active UI
- [ ] Mouse still works
- [ ] Gameplay keyboard controls still work

---

# 22. FINAL REPORT

When finished, report only:

```text
BUILD: PASS / FAIL
VideoPlayer: PASS / FAIL
Opening: PASS / FAIL
Mid: PASS / FAIL
Ending: PASS / FAIL
Mission Complete: PASS / FAIL
Main Encyclopedia: PASS / FAIL
Main Menu Logo: PASS / FAIL
Keyboard Input: PASS / FAIL

Files changed:
- ...

Remaining problems:
- ...
```

Do not claim PASS unless it was actually tested.
