# graphcompleter
 Solving abstract graph problems for JARVIS.

 Namely, this solves some problems in building an extrinsics inference graph.

 Sometimes, a camera won't have a valid path to the base camera, as in, the number of shared checkerboard frames won't exceed some threshold)

 This often will be a physical limitation of a setup rather than a matter of simply needing more frames.

 The graphcompleter does something similar to [NCams](https://github.com/CMGreenspon/NCams) and creates daisy-chains up to a user-defined maximum depth `k`.

 If it can't find a valid path, it will return the recommended upsampling factors to apply to each camera to achieve a valid graph. For instance, if you only have 20 shared frames between two cameras, if you have a validity threshold of 30 shared frames, the recommended upsampling factor for BOTH cameras will be 1.5.

 Valid paths are found greedily by camera index. In other words, if a camera can daisy-chain through camera 0 with edge weight 30, it will prompty accept that solution and move on, even if camera 12 is over there with edge weight 1 million. As JARVIS resamples cameras non-uniformly, raw shared frame counts beyond the "critical mass" are actually a poor measure of edge quality in any case. 

# Testing notes
 Tested in Windows, unfortunately I didn't have access to a linux environment for testing this (my poor laptop does not have the disk space for a VM). Hopefully the only problem that comes of this is that runtests.bat needs to be rewritten for other operating systems.

 To test:

 - `cd /graphcompleter/test/py/`
 - `python -m generateTestCases`
 - `cd /graphcompleter/`
 - `mkdir build`
 - `cmake -B build`
 - `cmake --build build`
 - `runtests` (only if Windows...)

# Contents
 `/src/` contains the source code. Once everything is tested to satisfaction, its files can be included and integrated into the main `calibrationtool` codebase.

 `/test/` contains the test cases, including a Python file located at `/test/py/generateTestCases.py`, which should generate the data required to run the tests when running `python -m generateTestCases`.

 Surface-level directory contains the `CMakeLists` for building the tests and the `runtests.bat` for executing them (on Windows...)

# Integration notes
 Make sure the test cases work (across platforms!) for this siloed version of graphcompleter before integrating!
