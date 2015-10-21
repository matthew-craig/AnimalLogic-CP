========================================================================
    CONSOLE APPLICATION : ClosestPointOnMesh Project Overview
========================================================================

Approach:

There are a number of possible approaches to determining the closest point on
a mesh from a given point. Broadly these fall into the following categories:

* Space Partitioning e.g. r-tree, quadtree, kd-tree, bsp etc.
* GJK or Gilbert-Johnson-Keerthi 
* Spatial Hashing

I decided to take a spatial hashing approach for a couple of reasons:

* There are lots of space partitioning approaches on the internet and the same
is true of GJK, so its more likely you could just find that code lying around

* GJK is the most complicated and error prone to implement so I didn't choose
to attempt it for a weekend technical assignment. GJK I suspect would struggle
in the support function for larger meshes, there is research on replacing this
with a hill climbing approach to be more optimal which would be interesting to
pursue if it's not already common place
https://graphics.stanford.edu/courses/cs468-01-fall/Papers/cameron.pdf

* The complexity of spatial hashing is O(n) for the hashmap creation and then
O(n*c*p) where c is the no of cells tested and p the points per cell. This
should scale well for larger meshes

The approach I took is detailed in the following paper:
http://matthias-mueller-fischer.ch/publications/tetraederCollision.pdf

Assumptions/Pre-requisites:

Tools Used:
Visual Studio 2015

Libraries Used:
Boost v1.59.0 - used for supporting data containers most notably unordered_multimap
http://www.boost.org/
Tinyobjloader - used for mesh loading from Maya
https://github.com/syoyo/tinyobjloader

Running:

I've included the executable in the git repo look in the Data folder it should run as is.

To build the solution please ensure the following steps are taken:
1. Unzip boost_1_59_0.7z into boost_1_59_0
2. Switch to boost_1_59_0 folder in a visual studio command prompt and type
    bootstrap
    .\b2
    or to build 64 bit libs
    .\b2 variant=release address-model=64
3. git clone tinyobjloader in the root folder from the root folder git clone https://github.com/syoyo/tinyobjloader.git
4. When building the solution ensure either Debug x86 or Release x86|x64 is selected

Note that the executable looks to load 2 obj files simplesphere.obj and largesphere.obj
that should be present in the Data directory. Note that the largesphere submitted to
the git repo is significantly smaller than what I tested with, I used a file containing
6389762 verts this takes roughly 2 minutes to load (must be loaded with x64 exe) 
and produced the following stats:

Spatial Hashmap
searched 569366 points
 0.05s wall, 0.02s user + 0.00s system = 0.02s CPU (28.9%)
closest point to query point is x -1.11517 y 5.46004 z -1.02395

Brute Force
searched 6397147 points
 0.22s wall, 0.17s user + 0.05s system = 0.22s CPU (97.9%)
closest point to query point is x -1.11517 y 5.46004 z -1.02395

The one in the git repo is hardly a large file I just included it so the file can be
found I'd encourage you to replace it with any larger file though please ensure you
run x64 for larger files.

You may note that for smaller meshes just brute forcing the solution is faster, larger
meshes are required to note the performance of spatial hashing as shown above it is 4x
faster (prior to any further optimizations) for a mesh with ~6.5 million points

Improvements:

* Switch loader to binary format that can rapidly load larger files
* As per the comments in populate_buckets store indice lookup to allow quick
  retrieval of polys later to determine exact mesh hit location using barycentric
  coordinates.
* Profile and refine it is likely there are further simple gains to be had from
  looking at object creation costs, data alignment etc. I had limited time to
  do any optimization so this is really just comparing at the big O level.
* Control the number of buckets created by unordered_multimap its likely hurting performance
  need to profile to confirm

Additional Notes:

We used a technique almost identical to this on MechWarrior Online to scorch
the Mechs with lasers some reference:
https://www.youtube.com/watch?v=hTfaomwVUzg right at the start
https://youtu.be/7xz4n05ntrk?t=117 freeze frames for a moment to see the glow

We used this technique to find the point on the mech to apply a damage mask to
and to write to a glow map. The glow map was reduced down by a shader over time
to fade. The size of the brush could be controlled by the game logic and was tied
to the amount of damage dealt. The damage and glow brushes could be different for
different weapon sets.

As you can see there are many mechs in a game capable of firing often 8-12 lasers
each so there were many of these hits and performance was a concern, this technique
never caused any issues other than some memory concerns initially as we used larger
glowmaps these were reduced later as the glow could be quite blurry for game standards
and hold up fine it did look nicer originally though the first link above shows
when it was more precise at the cost of additional memory.

/////////////////////////////////////////////////////////////////////////////
