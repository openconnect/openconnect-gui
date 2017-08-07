### Building release version and create a package 

The project use ['git-flow'](https://github.com/petervanderdoes/gitflow-avh) branching model.
The main development activities can be find in 'develop', in 'feature/...' or other similar named branch.
The 'master' branch contains release/delivered version and it's not supposed to do there any manual changes.

Note: nice graphical illustration how [gitflow branching work](http://nvie.com/posts/a-successful-git-branching-model/).

#### Version scheme
Project use git-flow technique i.e. the master contains stable/released versions
and main programming activities are in 'develop' or 'feature/*' branches.
Version string auto=generated on compilation from Git info into following format:

    <major>.<minor>.<patch>[-rev_count-sha1][-dirty]

#### Building packages

Always check if you have commited all changes or move work-in-progress work into stash!!
Following steps illustrate how to create application:

    $ git clone https://github.com/openconnect/openconnect-gui
    $ cd openconnect-gui
    $ git flow init -d

To build a release package, start a release process with target tag:

    $ git flow release start X.Y.Z

Now review released changes in `CHANGELOG.md`, update planned release version in `CMakeLists.txt`
and commit all changes. If all is done, continue with:

    $ git flow release finish X.Y.Z
    $ git checkout master
    $ git push master
    $ git push --tags

And then, continue with compilation process:

    $ cd ..
    $ md build-release
    $ cd build-release
    $ cmake -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles" ..\openconnect-gui
    $ mingw32-make -j5
    $ mingw32-make package



