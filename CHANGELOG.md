# Changelog

## [0.3.0](https://github.com/d06alexandrov/u-scope/compare/v0.2.4...v0.3.0) (2026-09-05)


### Features

* Bump Qt to version 6.11.0. ([8426f8a](https://github.com/d06alexandrov/u-scope/commit/8426f8a7b540d9ccf3e382213bd607d268c8cd46))
* Fix readers order in the menu. ([7249807](https://github.com/d06alexandrov/u-scope/commit/72498073a5df392b34af87945a2aad7fe1ff1849))

## [0.2.4](https://github.com/d06alexandrov/u-scope/compare/v0.2.3...v0.2.4) (2026-08-30)


### Bug Fixes

* Fix sliding window position reset during channel rescale. ([65f4e1d](https://github.com/d06alexandrov/u-scope/commit/65f4e1d4ab079941a5c5cc0201669434eeabd3e3))
* Send relative timestamps in graph data to ensure accuracy. ([050acd4](https://github.com/d06alexandrov/u-scope/commit/050acd4846a6ff8fb48e5c3b6762d5ce113fcecd))

## [0.2.3](https://github.com/d06alexandrov/u-scope/compare/v0.2.2...v0.2.3) (2026-08-20)


### Bug Fixes

* **ui:** Limit knobs size. ([c315019](https://github.com/d06alexandrov/u-scope/commit/c315019d32be53a2261790660ef31c57c8490e6b))

## [0.2.2](https://github.com/d06alexandrov/u-scope/compare/v0.2.1...v0.2.2) (2026-08-16)


### Bug Fixes

* Fix horizontal scale in maximum zoom out in stopped mode. ([1489f64](https://github.com/d06alexandrov/u-scope/commit/1489f64ae517bd0d2d90edcb793f91423819af9c))
* Remove explicit groupbox backgrounds to normalize windows layout. ([1d4e8c7](https://github.com/d06alexandrov/u-scope/commit/1d4e8c76279ca1bdf02589e87dfdc8a5d302bd78))
* Show proper vertical scale after channel reenabling. ([ac11fd3](https://github.com/d06alexandrov/u-scope/commit/ac11fd30a2a3b066dee09727f026740fbbef410e))
* **ui:** Hide new source menu before dialog popup. ([2decf00](https://github.com/d06alexandrov/u-scope/commit/2decf00015a12516b32dc518130bf35aa7b09766))

## [0.2.1](https://github.com/d06alexandrov/u-scope/compare/v0.2.0...v0.2.1) (2026-08-09)


### Bug Fixes

* Estimate timestamps of a serial data. ([cf2dfe4](https://github.com/d06alexandrov/u-scope/commit/cf2dfe4dc9e29adf3d4ad9727a69092c8d9f2d01))
* Set a proper title of main window. ([e8d0568](https://github.com/d06alexandrov/u-scope/commit/e8d0568d2fa4af658ba94d1e052511cd47d5fa11))

## [0.2.0](https://github.com/d06alexandrov/u-scope/compare/v0.1.1...v0.2.0) (2026-08-09)


### Features

* Add about dialog ([b992ebc](https://github.com/d06alexandrov/u-scope/commit/b992ebcd7fcab36ceeba94ba3deb2b51b0b70ada))
* Add channel enable/disable handling. ([7338319](https://github.com/d06alexandrov/u-scope/commit/7338319b96e13f727716e5feaef6fa8c48a0e9f8))
* Add independent vertical scale configuration. ([d322b89](https://github.com/d06alexandrov/u-scope/commit/d322b897ed747164a63ca97c2179de5e480dc684))
* Embed license into application. ([a9945e2](https://github.com/d06alexandrov/u-scope/commit/a9945e2d7cfe3daa0ffe664a822cec734422afde))
* Improve graph display near borders. ([d456c85](https://github.com/d06alexandrov/u-scope/commit/d456c8509069c0e7e031c53149cca60a6879b684))
* Make channel badges clickable. ([2de03ee](https://github.com/d06alexandrov/u-scope/commit/2de03eea3be823c85397d03914450544dc7b1fb4))
* **ui:** Add channel bar to display active channels. ([8cb5c79](https://github.com/d06alexandrov/u-scope/commit/8cb5c79e8954fd990984a6ddc058749662b97487))


### Bug Fixes

* **ci:** Prevent cpplinter comments posting on push events. ([4f80079](https://github.com/d06alexandrov/u-scope/commit/4f8007924f3ab41cbf9088f1af7a2c54fb6c2722))
* **ci:** Provide pull-requests permission to release workflow. ([7f23669](https://github.com/d06alexandrov/u-scope/commit/7f236690cb339a3db0e6b00e925f80301e0e81d6))
* Clear disabled channel data. ([a6bc9cb](https://github.com/d06alexandrov/u-scope/commit/a6bc9cbd70cc196cade88857882940b150f1026a))
* **dev:** Force software rendering of Qt Quick for developer environment. ([9d30531](https://github.com/d06alexandrov/u-scope/commit/9d30531355d2f3997e800b9d857d35393b4cae2b))
* Fix deploy for windows version. ([3039587](https://github.com/d06alexandrov/u-scope/commit/3039587f71705c38e3de28487c4fb5858f615923))
* Invert scale direction. ([95eb0b3](https://github.com/d06alexandrov/u-scope/commit/95eb0b3023ed948bbe91a4cbd758f8eb82f2d1fe))

## 0.1.1 (2026-07-31)


### Features

* **ci:** add release workflow ([ca6302c](https://github.com/d06alexandrov/u-scope/commit/ca6302ca0d3703e2853b3935c28312a32d105d0d))


### Miscellaneous Chores

* release 0.1.1 ([bdc2050](https://github.com/d06alexandrov/u-scope/commit/bdc20505c0669c4d3493153cfc36a4bf7c79ba27))
