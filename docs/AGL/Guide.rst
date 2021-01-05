AGL Guide
=========

Accessing
---------
You can access all AGL features, including builtins using namespace `agl`. All the base files
can be included with

``
#include <AOLGuiLibrary/AOLGuiLibrary.h>
``

Setup
-----
The library needs to be set up before using. Two most important functions must be called first:
:cpp:func:`agl::debug::init` and :cpp:func:`agl::set_default_font`.