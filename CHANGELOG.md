# Changelog

## [0.6.0](https://github.com/d06alexandrov/u-scope/compare/v0.5.0...v0.6.0) (2026-09-13)


### Features

* **ci:** Add workflow to create PR for merging main into develop after release. ([f4db1df](https://github.com/d06alexandrov/u-scope/commit/f4db1dfe2fd52394713f978f3f6e3be7c3eefe2b))
* **ui:** Add German translation. ([ec66e80](https://github.com/d06alexandrov/u-scope/commit/ec66e80fed4fd44613b224eb8a33f0fb84a4eda6))
* **ui:** Add Russian translation. ([8be3240](https://github.com/d06alexandrov/u-scope/commit/8be3240b64a5e4090dd22475757f572c95518709))
* **ui:** Add tooltip to display the exact received value on graph point hover. ([23c90eb](https://github.com/d06alexandrov/u-scope/commit/23c90ebd08a625fb2ac65fbd5608d8d92b457ad1))


### Bug Fixes

* **ci:** Prevent the creation of a release PR during the release. ([05efbf3](https://github.com/d06alexandrov/u-scope/commit/05efbf3ee1833231fe3ddfc03a87d57782618567))
* **ui:** Add missing translation strings for UI components. ([2592e2c](https://github.com/d06alexandrov/u-scope/commit/2592e2cad5201117301abcb4db182d7472088f3a))

## [0.5.0](https://github.com/d06alexandrov/u-scope/compare/v0.4.1...v0.5.0) (2026-09-11)


### Features

* Make configurable data bits, parity, stop bits and flow control serial port settings. ([55faae8](https://github.com/d06alexandrov/u-scope/commit/55faae8d9dd58e39cf27b78bf6d16109edcf413f))


### Bug Fixes

* **docs:** Fix a link to the github in github pages. ([083031f](https://github.com/d06alexandrov/u-scope/commit/083031f5f07180f9bbe2a110f11748890768be5d))
* Update dial's background when it's toggled. ([73df0e8](https://github.com/d06alexandrov/u-scope/commit/73df0e858ad67145dcd3054e99aadb0f77cd7634))

## [0.4.1](https://github.com/d06alexandrov/u-scope/compare/v0.4.0...v0.4.1) (2026-09-08)


### Bug Fixes

* **ci:** Keep original artifacts archives to preserve linux execution permission. ([6a8a355](https://github.com/d06alexandrov/u-scope/commit/6a8a35533b9973d1e07c4f12586a81c6b07dae25))

## [0.4.0](https://github.com/d06alexandrov/u-scope/compare/v0.3.0...v0.4.0) (2026-09-08)


### Features

* Add github pages with user guide. ([b613d1a](https://github.com/d06alexandrov/u-scope/commit/b613d1a510b8dceed0a7bd4f2bd0d9f37dc3331e))
* **ci:** Build self sufficient linux bundle. ([5a5b7e3](https://github.com/d06alexandrov/u-scope/commit/5a5b7e383ac83b9442cac378da562df7d221436c))
* Configurable non-trivial serial reader packets. ([ad34267](https://github.com/d06alexandrov/u-scope/commit/ad34267cf76503612e788897332a13c4ecc9c45b))


### Bug Fixes

* **ci:** Fix linux release CI. ([f3b6766](https://github.com/d06alexandrov/u-scope/commit/f3b67663bac838ca2018801625525974dfa0ba7d))

## [0.3.0](https://github.com/d06alexandrov/u-scope/compare/v0.2.4...v0.3.0) (2026-09-05)


### Features

* **ui:** Migrate to QtGraphs with hardware acceleration support. ([73a071c](https://github.com/d06alexandrov/u-scope/commit/73a071c2ee3082aa2a307d2c688283f138a73292))
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
