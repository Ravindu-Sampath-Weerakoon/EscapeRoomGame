// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

// The oder is important 
// -first include standard headers
// -then include project specific headers
// -glut.h must be included after framework.h to avoid conflicts

#include <iostream>
#include <stdio.h>
#include "framework.h"
#include "GraphicsUtils.h"
#include <glut.h>


#endif //PCH_H
