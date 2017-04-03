# MayaMirrorSkinWeightsUV
Maya Plugin for in place mirroring of skin weights using UVs. 

Mirroring happens in place regardless of how character is currently posed. 
Requires that your meshes have a symmetrical UV set. 


## Command Arguments
```
import maya.cmds as mc
mc.mirrorSkinWeightsUV(help=True)

# For each vertex locates the mirrored vertex in UV space copies skinCluster weights to it.

Arguments:
	mesh { str }                         --name of the skinned mesh or (optionally) currently selected mesh.
Keyword Arguments:
	tolerance(tol) { number }            -- Sets the maximum allowable distance for a matched vertex. (default: 0.01)
	threshold(thr) { number }            -- Immediately matches a vertex if less than the specified value. (default: 0.000001)
	axisOnU(u) { number }                -- Sets the mirror uv axis. (default: 0.5)
	excludeMatch(exc) { string or list } -- Nodes excluded from mirroring (takes wildcards ie. "JOFace*")
	leftPrefix(lp) { str }               -- Specify left prefix. (Default: "Left")
	rightPrefix(rp) { str }              -- Specify right prefix. (Default "Right")
	uvSetName(uvs)  { str }              -- Specify name of uv set. (Default: current uv set)
	mirrorInverse(mi)  { bool }          -- Values are mirrored from high u to low u. Direction is inverted if true.
	help(h) { bool }                     -- Print help.
 # 
```

## To Build From Command Line
```
    # CMake 3.8.x
    # create build dir, cd to it and run (customize for your IDE and Maya)
    cmake -G "Visual Studio 14 Win64" -DMAYA_VERSION=2016.5 ../
    cmake --build . --conf
 ```

