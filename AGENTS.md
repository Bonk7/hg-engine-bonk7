# Repository guide for coding agents

## Project layout

- `src/` and `include/` contain the C engine and headers. C targets ARM9 in Thumb mode; keep the existing low-level conventions and pay attention to memory, packed structures, overlays, and save compatibility.
- `asm/` and `armips/` contain ARM/Thumb assembly and ROM patching. `rom.ld`, `armhooks`, `bytereplacement`, `repoints`, `routinepointers`, `overlays.mk`, and `scripts/generate_ld.py` participate in linking or patch placement. Avoid moving offsets or repoints without understanding collisions and the ROM layout.
- `data/` is source game data: battle scripts, test scenarios, species/moves/items/trainers, text, learnsets, event data, and graphics. `rawdata/` contains extracted or binary source assets. `base/` and `build/` are build/extraction outputs; do not treat generated output as the authoritative source when a tracked source file exists.
- `tools/source/` contains tool sources and the `tools/source/nitrogfx` Git submodule. Do not change a submodule pointer unless the task requires it.
- `documentation/wiki/`, `README.md`, `CONFIG.md`, and `CONTRIBUTING.md` explain workflows and data formats. Read the focused page before changing a specialized data format.

## Change workflow

1. Read the relevant code, nearby data, documentation, and history (`git log`/`git blame`) before editing. Preserve existing local conventions and keep changes focused.
2. Check `git status` first. The working tree may contain user changes; never discard or overwrite unrelated changes. In particular, inspect submodule state separately.
3. For battle mechanic changes, add or update a scenario under `data/battle_tests/` that captures the behavior and expected result. Tests are C scenario files included by the test builder; `data/battle_tests/README.md` documents the schema and examples. Test fixes in commits commonly add regressions alongside implementation changes.
4. For C/header formatting, follow `.clang-format` and the repository pre-commit hook. CI uses `clang-format-18`; `CONTRIBUTING.md` describes enabling `.githooks` with `git config core.hooksPath .githooks`.
5. Use narrow builds and validation appropriate to the change. A normal ROM build is `make -j$(nproc)`. Battle scenarios require `make AUTO_TEST=Y -j$(nproc)` then `SDL_VIDEODRIVER=dummy scripts/run_tests.sh -j $(nproc)`; see `data/battle_tests/README.md`. The CI workflow is in `.github/workflows/build.yml`. Do not run a full ROM build unless the task calls for validation or the change warrants it.
6. Generated headers/data are build outputs where indicated by Makefile rules. Update their source inputs or generator instead of hand-editing generated artifacts. For NARC/archive changes, inspect `narcs.mk`, `Makefile`, and the relevant raw/source directory; archive ownership/rebuild behavior is intentional and some archives are rebuilt to preserve edits made by external tools under `base/`.

## Domain guidance

- Battle behavior spans C handlers, battle scripts, constants, and tests. Check related call sites and script command semantics before changing a shared constant or handler. Keep constants in their established header/category and update all affected uses when consolidating or renaming them.
- Species, move, item, trainer, dex, and other data tables often have fixed ordering, binary layouts, generated tables, or paired C/assembly configuration. Follow the associated documentation and Makefile dependencies; do not assume a filename is the only required update.
- Sprite and overworld assets may have structured companions such as JSON and palette files. Preserve naming, dimensions, palette/indexing, and archive mappings; inspect nearby assets and the relevant graphics makefile/rules.
- `include/config.h` and `armips/include/config.s` expose compile-time and assembly-time options. Some options must be changed in both places; consult `CONFIG.md` before altering configuration behavior.
- The project targets an English Pokémon HeartGold ROM (`IPKE`). The Makefile checks the ROM game code. Build inputs include the local ROM, so avoid replacing ROM or save files as part of routine work.

## Git and evidence

- Recent history includes merge commits, gameplay fixes, asset corrections, data fixes, and tool/build changes. Read the individual non-merge commits behind a merge when relevant; commit subjects alone may omit constraints captured in the diff.
- Before finishing, review `git diff` and `git status` so only intended source/documentation changes are reported. Do not stage or commit unless asked.
- Report what was changed and which relevant validation was run. If validation was not run, say so plainly.
