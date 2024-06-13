
# Mars Engine

A small homemade game engine using OpenGL




## Features

- Graphics
- Some shaders
- Shanalotte


## How to use

To run the engine, you simply need to clone the repository and build the project with CMake. To clone the reposiory and all its submodules use this command:

```bash
  git clone --recursive-submodules -j8 https://github.com/MarsAstro/MarsEngine.git
```

The engine relies on symbolic links to ensure files in the assets and shaders folders follow along with the build so that they are available to load via relative paths. Symbolic links are deactivated by default on Windows, to activate them [follow this guide](https://neacsu.net/posts/win_symlinks/).

If you can't or won't activate symbolic links, edit the CMakeLists.txt file to directly copy the files instead:

```CMakeLists
#Symbolic link between source asset/shader files for easier development
[...] REMOVE CODE FOLLOWING THIS COMMENT

#Replace symlink custom commands with this line if symbolic links don't work on your machine
[...] UNCOMMENT CODE FOLLOWING THIS COMMENT
```
Note that this means you need to do a clean rebuild, or manually replace the files, whenever you want to update the content of the assets and shaders folders. Symbolic links are strongly recommended for this reason.
## FAQ
#### When will this game engine replace all other game engines?

Around 2027

#### What was your mother's maiden name?

Mama Roach

#### Why are you so cool and handsome?

No comment
