# Repository Guidelines

## Project Structure & Modules
- Source in `src/` with components: `cli/`, `templates/`, `utils/`, `config/`, `testing/`, `documentation/`; main entry `src/main.cpp`.
- Tests in `tests/`: `unit/`, `integration/`, `platform/`, `edge_cases/` (helpers in `tests/common/`).
- Build configuration in `cmake/` (modules, presets); optional Xmake in `xmake.lua` and `xmake/`.
- Docs in `docs/` (MkDocs via `mkdocs.yml`) and Doxygen via `Doxyfile`; examples in `examples/`.

## Build, Test, and Development
- CMake (recommended): `cmake --preset debug`, then `cmake --build --preset debug --parallel`, run tests with `ctest --preset default --output-on-failure`.
- Windows helper: `scripts/build.ps1 -Config debug -Tests` (supports `-Clean`, `-Install`).
- Xmake (alternative): `xmake f -m debug && xmake && xmake run cpp-scaffold`.
- Tools: format `cmake --build --target format`; static analysis `cmake --build --target tidy`; Doxygen `cmake --build --target docs`; MkDocs `pip install -r requirements.txt && mkdocs serve`.

## Coding Style & Naming
- clang-format (Google-based) with 4-space indent and 100-column limit; root `.clang-format` is authoritative.
- Naming: files `snake_case`, classes `PascalCase`, functions `camelCase`, constants `SCREAMING_SNAKE_CASE`. Prefer `.h`/`.cpp` extensions.

## Testing Guidelines
- Framework: GoogleTest via CTest. Run `ctest --preset default` or filter with `ctest -R <name>`.
- Place tests as `tests/<area>/test_<topic>.cpp`. Aim for good coverage; use `coverage` target when available (CI uses `codecov.yml`).

## Commit & Pull Requests
- Conventional Commits (seen in history): `feat:`, `fix:`, `docs:`, `test:`, `ci:`, `build:`, `refactor:`, `chore:`. Keep subject concise; include rationale and “Closes #<id>` when applicable.
- PRs must: describe changes, link issues, update docs/tests, include CLI output or screenshots when helpful, and ensure presets build and tests pass.

## Security & Contributor Notes
- Do not commit secrets; review `SECURITY.md`. Prefer `cmake --preset debug-sanitized` for ASan/UBSan checks.
- Keep changes focused; do not rename public targets or reorganize modules without discussion. Prefer presets and existing CMake/Xmake modules; update relevant docs alongside code.

