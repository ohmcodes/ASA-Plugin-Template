# ASA Plugin Template
Simple template for ARK: Survival Ascended server plugins using [AsaApi](https://github.com/ArkServerApi/AsaApi).

RUN (DEPRECATED)
`git pull --recurse-submodules`
`git clone --recursive`

`vcpkg x-update-baseline [options] [--add-initial-baseline] [--dry-run]`

## SETUP

`<path> - depends on you`

- Create a repo using this template
- clone `<path>/plugin-name`
- clone AsaApi `https://github.com/ArkServerApi/AsaApi` to `<path>AsaApi`
- if you want the official plugins like `ArkShop` and `Permissions (mandatory)`
- clone it here `https://github.com/ArkServerApi/ASA-Plugins` to `<path>/ASA-Plugins`
- this repo is setup this way it will fetch automatically the library and includes you needed
- for 1st time clone please build both `AsaApi` and `ASA-Plugins` (release build)
- if you want to double check settings go to
  - Go to Project > <ProjectName> properties
  - VC++ Directories (includes Directories, Library Directories)
  - C/C++ > General (Additional Include Directories)
  - Linker > Input 

# Enjoy!



