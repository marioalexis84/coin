/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org> for  more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

/*!
  How to use OpenGL / GLX|WGL|AGL inside Coin
  ===========================================

  Creating portable OpenGL applications can be a complicated matter
  since you have to have both compile-time and run-time tests for
  OpenGL version, and what extensions are available. In addition, you
  might not have an entry point to the (extension) function in
  question on your build system.  The cc_glglue abstraction is here
  to relieve the application programmer for that burden.

  To use the cc_glglue interface, include Inventor/C/glue/gl.h.

  The cc_glglue interface is part of the public API of Coin, but is
  not documented on the public documentation pages at
  http://doc.coin3d.org yet. The status for client application usage
  is "unofficial, use at own risk, interface may change without
  warning for major version number upgrade releases".

  Coin programmer's responsibilities
  ----------------------------------

  o OpenGL calls that are part of OpenGL 1.0 can safely be used
    without any kind of checking.

  o Do _not_ use cc_glglue unless you are sure that you have a valid
    OpenGL context. cc_glglue implicitly assumes that this is the case
    for most of its functions. In short, only use OpenGL functions
    inside an SoGLRenderAction.

  o To get hold of a cc_glglue instance:
      const cc_glglue * cc_glglue_instance(int contextid);
    or
      const cc_glglue * cc_glglue_instance_from_context_ptr(void * ctx);

    See header file for more information about these.

  o Always check that the capability you want to use is supported.
    Queries for this is supported through the cc_glglue_has_*()
    functions.

  o cc_glglue has some functions for querying OpenGL/GLX version and
    extension availability. Usually you shouldn't need to use these
    unless you want to bypass cc_glglue or your function isn't
    supported by cc_glglue (in which case you should add it).

  o SoGLCacheContextElement also has some functions for querying
    OpenGL version and extension availability. These are public, so
    you can use them even in external code. However, use cc_glglue
    internally for consistency.

  What cc_glglue supplies
  -----------------------

  o cc_glglue supplies function pointer to OpenGL and GLX functions
    used in Coin that are _not_ part of OpenGL 1.0 and GLX 1.1.  Note
    that cc_glglue supplies OpenGL extension functions as if they were
    standard functions (i.e. without the EXT suffix).

  o In addition, the Inventor/system/gl.h file supplies OpenGL enums
    that might not otherwise be present in your system's GL headers.

  The following example accesses OpenGL 3D texturing. It works both on
  OpenGL >= 1.2 and on OpenGL drivers with the GL_EXT_texture3D
  extension.

  ------ 8< --------- [snip] --------------------- 8< --------- [snip] -----

  const cc_glglue * glw = cc_glglue_instance(SoGLCacheContextElement::get(state));
  if (cc_glglue_has_3d_textures(glw)) {
    cc_glglue_glTexImage3D(glw, GL_PROXY_TEXTURE_3D, 0, GL_RGBA,
                           64, 64, 64, 0,
                           GL_RGBA, GL_UNSIGNED_BYTE,
                           NULL);
  }
  else {
    // Implement a proper fallback or error handling.
  }

  ------ 8< --------- [snip] --------------------- 8< --------- [snip] -----
*/

/*!
  For the library/API doc, here's the environment variables
  influencing the OpenGL binding:

  - COIN_DEBUG_GLGLUE: set equal to "1" to make the wrapper
    initalization spit out lots of info about the underlying OpenGL
    implementation.

  - COIN_PREFER_GLPOLYGONOFFSET_EXT: when set to "1" and both
    glPolygonOffset() and glPolygonOffsetEXT() is available, the
    latter will be used. This can be useful to work around a
    problematic glPolygonOffset() implementation for certain SGI
    platforms.

  - COIN_FULL_INDIRECT_RENDERING: set to "1" to let Coin take
    advantage of OpenGL1.1+ and extensions even when doing
    remote/indirect rendering.

    We don't allow this by default now, for mainly two reasons: 1)
    we've seen NVidia GLX bugs when attempting this. 2) We generally
    prefer a "better safe than sorry" strategy.

    We might consider changing this strategy to allow it by default,
    and provide an envvar to turn it off instead -- if we can get
    confirmation that the assumed NVidia driver bug is indeed NVidia's
    problem.

  - COIN_FORCE_GL1_0_ONLY: set to "1" to disallow use of OpenGL1.1+
    and extensions under all circumstances.
*/


/*
  Useful resources:

   - About OpenGL 1.2, 1.3, 1.4:
     <URL:http://www.opengl.org/developers/documentation/OpenGL12.html>
     <URL:http://www.opengl.org/developers/documentation/OpenGL13.html>
     <URL:http://www.opengl.org/developers/documentation/OpenGL14.html>
     (explains all new features in depth)

   - The OpenGL Extension Registry:
     <URL:http://oss.sgi.com/projects/ogl-sample/registry/>

   - A great overview of what OpenGL driver capabilities are available
     for different cards, check out "3D Hardware Info" on
     <URL:http://www.delphi3d.net/>.

   - Brian Paul presentation "Using OpenGL Extensions" from SIGGRAPH '97:
     <URL:http://www.mesa3d.org/brianp/sig97/exten.htm>

   - Sun's man pages:
     <URL:http://wwws.sun.com/software/graphics/OpenGL/manpages>

   - IBM AIX GL man pages (try to find a "more official looking" link):
     <URL:http://molt.zdv.uni-mainz.de/doc_link/en_US/a_doc_lib/libs/openglrf/OpenGLXEnv.htm>

   - HP GL man pages:
     <URL:http://www.hp.com/workstations/support/documentation/manuals/user_guides/graphics/opengl/RefTOC.html>

   - An Apple Technical Q&A on how to do dynamic binding to OpenGL symbols:
     <URL:http://developer.apple.com/qa/qa2001/qa1188.html>

     Full documentation on all "Object File Image" functions, see:
     <URL:http://developer.apple.com/techpubs/macosx/DeveloperTools/MachORuntime/5rt_api_reference/_Object_Fil_e_Functions.html>
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> /* SHRT_MAX */

#ifdef HAVE_AGL
#include <AGL/AGL.h>
#include <OpenGL/CGLCurrent.h>  /* for CGLGetCurrentContext */
#endif /* HAVE_AGL */

#include <Inventor/C/glue/gl.h>
#include <Inventor/C/glue/glp.h>

#include <Inventor/C/glue/dl.h>
#include <Inventor/C/base/hash.h>
#include <Inventor/C/base/namemap.h>
#include <Inventor/C/errors/debugerror.h>

#include <Inventor/C/base/hash.h>
#include <Inventor/C/threads/threadsutilp.h>
#include <Inventor/C/tidbits.h>
#include <Inventor/C/tidbitsp.h>
#include <Inventor/C/glue/gl_wgl.h>
#include <Inventor/C/glue/gl_glx.h>
#include <Inventor/C/glue/gl_agl.h>

/* ********************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static cc_libhandle glglue_self_handle = NULL;
static SbBool glglue_tried_open_self = FALSE;

/* ********************************************************************** */

/* Sanity checks for enum extension value assumed to be equal to the
 * final / "proper" / standard OpenGL enum values. (If not, we could
 * end up with hard-to-find bugs because of mismatches with the
 * compiled values versus the run-time values.)
 *
 * This doesn't really _fix_ anything, it is just meant as an aid to
 * smoke out platforms where we're getting unexpected enum values.
 */

#ifdef GL_CLAMP_TO_EDGE_EXT
#if GL_CLAMP_TO_EDGE != GL_CLAMP_TO_EDGE_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_CLAMP_TO_EDGE_EXT */

#ifdef GL_CLAMP_TO_EDGE_SGIS
#if GL_CLAMP_TO_EDGE != GL_CLAMP_TO_EDGE_SGIS
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_CLAMP_TO_EDGE_SGIS */

#ifdef GL_MAX_3D_TEXTURE_SIZE_EXT
#if GL_MAX_3D_TEXTURE_SIZE != GL_MAX_3D_TEXTURE_SIZE_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_MAX_3D_TEXTURE_SIZE_EXT */

#ifdef GL_PACK_IMAGE_HEIGHT_EXT
#if GL_PACK_IMAGE_HEIGHT != GL_PACK_IMAGE_HEIGHT_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_PACK_IMAGE_HEIGHT_EXT */

#ifdef GL_PACK_SKIP_IMAGES_EXT
#if GL_PACK_SKIP_IMAGES != GL_PACK_SKIP_IMAGES_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_PACK_SKIP_IMAGES_EXT */

#ifdef GL_PROXY_TEXTURE_2D_EXT
#if GL_PROXY_TEXTURE_2D != GL_PROXY_TEXTURE_2D_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_PROXY_TEXTURE_2D_EXT */

#ifdef GL_PROXY_TEXTURE_3D_EXT
#if GL_PROXY_TEXTURE_3D != GL_PROXY_TEXTURE_3D_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_PROXY_TEXTURE_3D_EXT */

#ifdef GL_TEXTURE_3D_EXT
#if GL_TEXTURE_3D != GL_TEXTURE_3D_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_TEXTURE_3D_EXT */

#ifdef GL_TEXTURE_DEPTH_EXT
#if GL_TEXTURE_DEPTH != GL_TEXTURE_DEPTH_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_TEXTURE_DEPTH_EXT */

#ifdef GL_TEXTURE_WRAP_R_EXT
#if GL_TEXTURE_WRAP_R != GL_TEXTURE_WRAP_R_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_TEXTURE_WRAP_R_EXT */

#ifdef GL_UNPACK_IMAGE_HEIGHT_EXT
#if GL_UNPACK_IMAGE_HEIGHT != GL_UNPACK_IMAGE_HEIGHT_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_UNPACK_IMAGE_HEIGHT_EXT */

#ifdef GL_UNPACK_SKIP_IMAGES_EXT
#if GL_UNPACK_SKIP_IMAGES != GL_UNPACK_SKIP_IMAGES_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_UNPACK_SKIP_IMAGES_EXT */

#ifdef GL_FUNC_ADD_EXT
#if GL_FUNC_ADD != GL_FUNC_ADD_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_FUNC_ADD_EXT */

#ifdef GL_MIN_EXT
#if GL_MIN != GL_MIN_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_MIN_EXT */

#ifdef GL_MAX_EXT
#if GL_MAX != GL_MAX_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_MAX_EXT */

#ifdef GL_COLOR_TABLE_WIDTH_EXT
#if GL_COLOR_TABLE_WIDTH != GL_COLOR_TABLE_WIDTH_EXT
#error dangerous enum mismatch
#endif /* cmp */
#endif /* GL_COLOR_TABLE_WIDTH_EXT */

/* ********************************************************************** */

/* Resolve and return the integer value of an environment variable. */
static int
glglue_resolve_envvar(const char * txt)
{
  const char * val = coin_getenv(txt);
  return val ? atoi(val) : 0;
}

/* Returns whether or not COIN_GLGLUE_SILENCE_DRIVER_WARNINGS is set
   to a value > 0. If so, all known driver bugs will just be silently
   accepted and attempted worked around. */
static int
coin_glglue_silence_all_driver_warnings(void)
{
  static int d = -1;
  if (d == -1) { d = glglue_resolve_envvar("COIN_GLGLUE_SILENCE_DRIVER_WARNINGS"); }
  /* Note the inversion of the envvar value versus the return value. */
  return (d > 0) ? 0 : 1;
}

/* Return value of COIN_GLGLUE_NO_RADEON_WARNING environment variable. */
static int
coin_glglue_radeon_warning(void)
{
  static int d = -1;

  if (coin_glglue_silence_all_driver_warnings()) { return 0; }

  if (d == -1) { d = glglue_resolve_envvar("COIN_GLGLUE_NO_RADEON_WARNING"); }
  /* Note the inversion of the envvar value versus the return value. */
  return (d > 0) ? 0 : 1;
}

/* Return value of COIN_GLGLUE_NO_G400_WARNING environment variable. */
static int
coin_glglue_old_matrox_warning(void)
{
  static int d = -1;

  if (coin_glglue_silence_all_driver_warnings()) { return 0; }

  if (d == -1) { d = glglue_resolve_envvar("COIN_GLGLUE_NO_G400_WARNING"); }
  /* Note the inversion of the envvar value versus the return value. */
  return (d > 0) ? 0 : 1;
}

/* Return value of COIN_GLGLUE_NO_ELSA_WARNING environment variable. */
static int
coin_glglue_old_elsa_warning(void)
{
  static int d = -1;

  if (coin_glglue_silence_all_driver_warnings()) { return 0; }

  if (d == -1) { d = glglue_resolve_envvar("COIN_GLGLUE_NO_ELSA_WARNING"); }
  /* Note the inversion of the envvar value versus the return value. */
  return (d > 0) ? 0 : 1;
}

/* Return value of COIN_GLGLUE_NO_SUN_EXPERT3D_WARNING environment variable. */
static int
coin_glglue_sun_expert3d_warning(void)
{
  static int d = -1;

  if (coin_glglue_silence_all_driver_warnings()) { return 0; }

  if (d == -1) { d = glglue_resolve_envvar("COIN_GLGLUE_NO_SUN_EXPERT3D_WARNING"); }
  /* Note the inversion of the envvar value versus the return value. */
  return (d > 0) ? 0 : 1;
}

/* Return value of COIN_GLGLUE_NO_TRIDENT_WARNING environment variable. */
static int
coin_glglue_trident_warning(void)
{
  static int d = -1;

  if (coin_glglue_silence_all_driver_warnings()) { return 0; }

  if (d == -1) { d = glglue_resolve_envvar("COIN_GLGLUE_NO_TRIDENT_WARNING"); }
  /* Note the inversion of the envvar value versus the return value. */
  return (d > 0) ? 0 : 1;
}

/* Return value of COIN_DEBUG_GLGLUE environment variable. */
int
coin_glglue_debug(void)
{
  static int d = -1;
  if (d == -1) { d = glglue_resolve_envvar("COIN_DEBUG_GLGLUE"); }
  return (d > 0) ? 1 : 0;
}

/* Return value of COIN_PREFER_GLPOLYGONOFFSET_EXT environment variable. */
static int
glglue_prefer_glPolygonOffsetEXT(void)
{
  static int d = -1;
  if (d == -1) { d = glglue_resolve_envvar("COIN_PREFER_GLPOLYGONOFFSET_EXT"); }
  return (d > 0) ? 1 : 0;
}

/* doc in header file */
void *
cc_glglue_getprocaddress(const char * symname)
{
  void * ptr = NULL;

  ptr = coin_wgl_getprocaddress(symname);
  if (ptr) goto returnpoint;

  ptr = glxglue_getprocaddress(symname);
  if (ptr) goto returnpoint;

  ptr = aglglue_getprocaddress(symname);
  if (ptr) goto returnpoint;

  if (glglue_self_handle) {
    ptr = cc_dl_sym(glglue_self_handle, symname);
    if (ptr) goto returnpoint;
  }

returnpoint:
  if (coin_glglue_debug()) {
    cc_debugerror_postinfo("cc_glglue_getprocaddress", "%s==%p", symname, ptr);
  }
  return ptr;
}

/* Global dictionary which stores the mappings from the context IDs to
   actual cc_glglue instances. */
static cc_hash * gldict = NULL;

static void
free_glglue_instance(unsigned long key, void * value, void * closure)
{
  cc_glglue * glue = (cc_glglue*) value;
  cc_hash_destruct(glue->glextdict);
  free(value);
}

/* Cleans up at exit. */
static void
glglue_cleanup(void)
{
  cc_hash_apply(gldict, free_glglue_instance, NULL);
  cc_hash_destruct(gldict);
  if (glglue_self_handle) cc_dl_close(glglue_self_handle);
}

/*
  Set the OpenGL version variables in the given cc_glglue struct
  instance.

  Note: this code has been copied from GLUWrapper.c, so if any changes
  are made, make sure they are propagated over if necessary.
*/
static void
glglue_set_glVersion(cc_glglue * w)
{
  char buffer[256];
  char * dotptr;

  /* NB: if you are getting a crash here, it's because an attempt at
   * setting up a cc_glglue instance was made when there is no current
   * OpenGL context. */
  if (coin_glglue_debug()) {
    cc_debugerror_postinfo("glglue_set_glVersion",
                           "glGetString(GL_VERSION)=='%s'", w->versionstr);
  }

  w->version.major = 0;
  w->version.minor = 0;
  w->version.release = 0;

  (void)strncpy(buffer, (const char *)w->versionstr, 255);
  buffer[255] = '\0'; /* strncpy() will not null-terminate if strlen > 255 */
  dotptr = strchr(buffer, '.');
  if (dotptr) {
    char * spaceptr;
    char * start = buffer;
    *dotptr = '\0';
    w->version.major = atoi(start);
    start = ++dotptr;

    dotptr = strchr(start, '.');
    spaceptr = strchr(start, ' ');
    if (!dotptr && spaceptr) dotptr = spaceptr;
    if (dotptr && spaceptr && spaceptr < dotptr) dotptr = spaceptr;
    if (dotptr) {
      int terminate = *dotptr == ' ';
      *dotptr = '\0';
      w->version.minor = atoi(start);
      if (!terminate) {
        start = ++dotptr;
        dotptr = strchr(start, ' ');
        if (dotptr) *dotptr = '\0';
        w->version.release = atoi(start);
      }
    }
    else {
      w->version.minor = atoi(start);
    }
  }

  if (coin_glglue_debug()) {
    cc_debugerror_postinfo("glglue_set_glVersion",
                           "parsed to major=='%d', minor=='%d', micro=='%d'",
                           w->version.major,
                           w->version.minor,
                           w->version.release);
  }
}

void
cc_glglue_glversion(const cc_glglue * w,
                    unsigned int * major,
                    unsigned int * minor,
                    unsigned int * release)
{
  *major = w->version.major;
  *minor = w->version.minor;
  *release = w->version.release;
}


SbBool
cc_glglue_glversion_matches_at_least(const cc_glglue * w,
                                     unsigned int major,
                                     unsigned int minor,
                                     unsigned int revision)
{
  if (w->version.major < major) return FALSE;
  else if (w->version.major > major) return TRUE;
  if (w->version.minor < minor) return FALSE;
  else if (w->version.minor > minor) return TRUE;
  if (w->version.release < revision) return FALSE;
  return TRUE;
}

SbBool
cc_glglue_glxversion_matches_at_least(const cc_glglue * w,
                                      int major,
                                      int minor)
{
  if (w->glx.version.major < major) return FALSE;
  else if (w->glx.version.major > major) return TRUE;
  if (w->glx.version.minor < minor) return FALSE;
  return TRUE;
}

int
coin_glglue_extension_available(const char * extensions, const char * ext)
{
  const char * start;
  int extlen;
  SbBool found = FALSE;

  assert(ext && "NULL string");
  assert((ext[0] != '\0') && "empty string");
  assert((strchr(ext, ' ') == NULL) && "extension name can't have spaces");

  start = extensions;
  extlen = strlen(ext);

  while (1) {
    const char * where = strstr(start, ext);
    if (!where) goto done;

    if (where == start || *(where - 1) == ' ') {
      const char * terminator = where + extlen;
      if (*terminator == ' ' || *terminator == '\0') {
        found = TRUE;
        goto done;
      }
    }

    start = where + extlen;
  }

done:
  if (coin_glglue_debug()) {
    cc_debugerror_postinfo("coin_glglue_extension_available",
                           "extension '%s' is%s present",
                           ext, found ? "" : " NOT");
  }

  return found ? 1 : 0;
}

int
cc_glglue_glext_supported(const cc_glglue * wrapper, const char * extension)
{
  unsigned long key = (unsigned long) cc_namemap_get_address(extension);

  void * result = NULL;
  if (cc_hash_get(wrapper->glextdict, key, &result)) {
    return result != NULL;
  }
  result = coin_glglue_extension_available(wrapper->extensionsstr, extension) ?
    (void*) 1 : NULL;
  cc_hash_put(wrapper->glextdict, key, result);

  return result != NULL;
}

#ifdef HAVE_DYNAMIC_LINKING

#define PROC(_func_) cc_glglue_getprocaddress(SO__QUOTE(_func_))

/* The OpenGL library which we dynamically pick up symbols from
   /could/ have all these defined. For the code below which tries to
   dynamically resolve the methods, we will assume that they are all
   defined. By doing this little "trick", can we use the same code
   below for resolving stuff dynamically as we need anyway to resolve
   in a static manner. */
#define GL_VERSION_1_1 1
#define GL_VERSION_1_2 1
#define GL_VERSION_1_3 1
#define GL_VERSION_1_4 1
#define GL_VERSION_1_5 1
#define GL_EXT_polygon_offset 1
#define GL_EXT_texture_object 1
#define GL_EXT_subtexture 1
#define GL_EXT_texture3D 1
#define GL_ARB_multitexture 1
#define GL_ARB_texture_compression 1
#define GL_EXT_paletted_texture 1
#define GL_ARB_imaging 1
#define GL_EXT_blend_minmax 1
#define GL_EXT_color_table 1
#define GL_EXT_color_subtable 1
#define GL_SGI_color_table 1
#define GL_SGI_texture_color_table 1
#define GL_ARB_vertex_buffer_object 1
#define GL_EXT_multi_draw_arrays 1
#define GL_NV_vertex_array_range 1
#define GL_NV_register_combiners 1
#define GL_NV_texture_rectangle 1
#define GL_NV_texture_shader 1
#define GL_ARB_depth_texture 1
#define GL_ARB_shadow 1
#define GL_EXT_texture_rectangle 1
#define GL_ARB_fragment_program 1


#else /* static binding */

#define PROC(_func_) (&_func_)

#endif /* static binding */


static void
glglue_resolve_symbols(cc_glglue * w)
{
  /* Note that there's a good reason why we use version checking
     *along* with dynamic resolving (if the platform allows it): the
     OpenGL library could (prematurely) include function symbols
     without having an actual valid implementation behind them. */

  /* Appeared in OpenGL v1.1. We store both the "real" function
     pointer and the extension pointer, in case we need to work around
     an SGI bug (see comments in cc_glglue_glPolygonOffset(). */
  w->glPolygonOffset = NULL;
  w->glPolygonOffsetEXT = NULL;
#ifdef GL_VERSION_1_1
  if (cc_glglue_glversion_matches_at_least(w, 1, 1, 0)) {
    w->glPolygonOffset = (COIN_PFNGLPOLYGONOFFSETPROC)PROC(glPolygonOffset);
  }
#endif /* GL_VERSION_1_1 */
#ifdef GL_EXT_polygon_offset
  if (cc_glglue_glext_supported(w, "GL_EXT_polygon_offset")) {
    w->glPolygonOffsetEXT = (COIN_PFNGLPOLYGONOFFSETPROC)PROC(glPolygonOffsetEXT);
  }
#endif /* GL_EXT_polygon_offset */



  /* Appeared in OpenGL v1.1. */
  w->glGenTextures = NULL;
  w->glBindTexture = NULL;
  w->glDeleteTextures = NULL;
#ifdef GL_VERSION_1_1
  if (cc_glglue_glversion_matches_at_least(w, 1, 1, 0)) {
    w->glGenTextures = (COIN_PFNGLGENTEXTURESPROC)PROC(glGenTextures);
    w->glBindTexture = (COIN_PFNGLBINDTEXTUREPROC)PROC(glBindTexture);
    w->glDeleteTextures = (COIN_PFNGLDELETETEXTURESPROC)PROC(glDeleteTextures);
  }
#endif /* GL_VERSION_1_1 */
#ifdef GL_EXT_texture_object
  if (!w->glGenTextures && cc_glglue_glext_supported(w, "GL_EXT_texture_object")) {
    w->glGenTextures = (COIN_PFNGLGENTEXTURESPROC)PROC(glGenTexturesEXT);
    w->glBindTexture = (COIN_PFNGLBINDTEXTUREPROC)PROC(glBindTextureEXT);
    w->glDeleteTextures = (COIN_PFNGLDELETETEXTURESPROC)PROC(glDeleteTexturesEXT);
  }
#endif /* GL_EXT_texture_object */

  /* Appeared in OpenGL v1.1. */
  w->glTexSubImage2D = NULL;
#ifdef GL_VERSION_1_1
  if (cc_glglue_glversion_matches_at_least(w, 1, 1, 0)) {
    w->glTexSubImage2D = (COIN_PFNGLTEXSUBIMAGE2DPROC)PROC(glTexSubImage2D);
  }
#endif /* GL_VERSION_1_1 */
#ifdef GL_EXT_subtexture
  if (!w->glTexSubImage2D && cc_glglue_glext_supported(w, "GL_EXT_subtexture")) {
    w->glTexSubImage2D = (COIN_PFNGLTEXSUBIMAGE2DPROC)PROC(glTexSubImage2DEXT);
  }
#endif /* GL_EXT_subtexture */

  /* These were introduced with OpenGL v1.2. */
  w->glTexImage3D = NULL;
  w->glCopyTexSubImage3D = NULL;
  w->glTexSubImage3D = NULL;
#ifdef GL_VERSION_1_2
  if (cc_glglue_glversion_matches_at_least(w, 1, 2, 0)) {
    w->glTexImage3D = (COIN_PFNGLTEXIMAGE3DPROC)PROC(glTexImage3D);
    w->glCopyTexSubImage3D = (COIN_PFNGLCOPYTEXSUBIMAGE3DPROC)PROC(glCopyTexSubImage3D);
    w->glTexSubImage3D = (COIN_PFNGLTEXSUBIMAGE3DPROC)PROC(glTexSubImage3D);
  }
#endif /* GL_VERSION_1_2 */
#ifdef GL_EXT_texture3D
  if (!w->glTexImage3D && cc_glglue_glext_supported(w, "GL_EXT_texture3D")) {
    w->glTexImage3D = (COIN_PFNGLTEXIMAGE3DPROC)PROC(glTexImage3DEXT);
    /* These are implicitly given if GL_EXT_texture3D is defined. */
    w->glCopyTexSubImage3D = (COIN_PFNGLCOPYTEXSUBIMAGE3DPROC)PROC(glCopyTexSubImage3DEXT);
    w->glTexSubImage3D = (COIN_PFNGLTEXSUBIMAGE3DPROC)PROC(glTexSubImage3DEXT);
  }
#endif /* GL_EXT_texture3D */

  /* Appeared in OpenGL v1.3. */
  w->glActiveTexture = NULL;
  w->glClientActiveTexture = NULL;
  w->glMultiTexCoord2f = NULL;
  w->glMultiTexCoord2fv = NULL;
  w->glMultiTexCoord3fv = NULL;
  w->glMultiTexCoord4fv = NULL;
#ifdef GL_VERSION_1_3
  if (cc_glglue_glversion_matches_at_least(w, 1, 3, 0)) {
    w->glActiveTexture = (COIN_PFNGLACTIVETEXTUREPROC)PROC(glActiveTexture);
    w->glClientActiveTexture = (COIN_PFNGLCLIENTACTIVETEXTUREPROC)PROC(glClientActiveTexture);
    w->glMultiTexCoord2f = (COIN_PFNGLMULTITEXCOORD2FPROC)PROC(glMultiTexCoord2f);
    w->glMultiTexCoord2fv = (COIN_PFNGLMULTITEXCOORD2FVPROC)PROC(glMultiTexCoord2fv);
    w->glMultiTexCoord3fv = (COIN_PFNGLMULTITEXCOORD3FVPROC)PROC(glMultiTexCoord3fv);
    w->glMultiTexCoord4fv = (COIN_PFNGLMULTITEXCOORD4FVPROC)PROC(glMultiTexCoord4fv);
  }
#endif /* GL_VERSION_1_3 */
#ifdef GL_ARB_multitexture
  if (!w->glActiveTexture && cc_glglue_glext_supported(w, "GL_ARB_multitexture")) {
    w->glActiveTexture = (COIN_PFNGLACTIVETEXTUREPROC)PROC(glActiveTextureARB);
    w->glClientActiveTexture = (COIN_PFNGLACTIVETEXTUREPROC)PROC(glClientActiveTextureARB);
    w->glMultiTexCoord2f = (COIN_PFNGLMULTITEXCOORD2FPROC)PROC(glMultiTexCoord2fARB);
    w->glMultiTexCoord2fv = (COIN_PFNGLMULTITEXCOORD2FVPROC)PROC(glMultiTexCoord2fvARB);
    w->glMultiTexCoord3fv = (COIN_PFNGLMULTITEXCOORD3FVPROC)PROC(glMultiTexCoord3fvARB);
    w->glMultiTexCoord4fv = (COIN_PFNGLMULTITEXCOORD4FVPROC)PROC(glMultiTexCoord4fvARB);
  }
#endif /* GL_ARB_multitexture */

  if (w->glActiveTexture) {
    if (!w->glMultiTexCoord2f ||
        !w->glMultiTexCoord2fv ||
        !w->glMultiTexCoord3fv ||
        !w->glMultiTexCoord4fv) {
      w->glActiveTexture = NULL; /* cc_glglue_has_multitexture() will return FALSE */
      cc_debugerror_postwarning("glglue_init",
                                "glActiveTexture found, but one or more of the other "
                                "multitexture functions were not found");
    }
  }
  w->maxtextureunits = 1; /* when multitexturing is not available */
  if (w->glActiveTexture) {
    GLint tmp;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &tmp);
    w->maxtextureunits = (int) tmp;
  }

  w->glCompressedTexImage1D = NULL;
  w->glCompressedTexImage2D = NULL;
  w->glCompressedTexImage3D = NULL;
  w->glCompressedTexSubImage1D = NULL;
  w->glCompressedTexSubImage2D = NULL;
  w->glCompressedTexSubImage3D = NULL;
  w->glGetCompressedTexImage = NULL;

#ifdef GL_VERSION_1_3
  if (cc_glglue_glversion_matches_at_least(w, 1, 3, 0)) {
    w->glCompressedTexImage1D = (COIN_PFNGLCOMPRESSEDTEXIMAGE1DPROC)PROC(glCompressedTexImage1D);
    w->glCompressedTexImage2D = (COIN_PFNGLCOMPRESSEDTEXIMAGE2DPROC)PROC(glCompressedTexImage2D);
    w->glCompressedTexImage3D = (COIN_PFNGLCOMPRESSEDTEXIMAGE3DPROC)PROC(glCompressedTexImage3D);
    w->glCompressedTexSubImage1D = (COIN_PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)PROC(glCompressedTexSubImage1D);
    w->glCompressedTexSubImage2D = (COIN_PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)PROC(glCompressedTexSubImage2D);
    w->glCompressedTexSubImage3D = (COIN_PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)PROC(glCompressedTexSubImage3D);
    w->glGetCompressedTexImage = (COIN_PFNGLGETCOMPRESSEDTEXIMAGEPROC)PROC(glGetCompressedTexImage);
  }
#endif /* GL_VERSION_1_3 */

#ifdef GL_ARB_texture_compression
  if ((w->glCompressedTexImage1D == NULL) &&
      cc_glglue_glext_supported(w, "GL_ARB_texture_compression")) {
    w->glCompressedTexImage1D = (COIN_PFNGLCOMPRESSEDTEXIMAGE1DPROC)PROC(glCompressedTexImage1DARB);
    w->glCompressedTexImage2D = (COIN_PFNGLCOMPRESSEDTEXIMAGE2DPROC)PROC(glCompressedTexImage2DARB);
    w->glCompressedTexImage3D = (COIN_PFNGLCOMPRESSEDTEXIMAGE3DPROC)PROC(glCompressedTexImage3DARB);
    w->glCompressedTexSubImage1D = (COIN_PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)PROC(glCompressedTexSubImage1DARB);
    w->glCompressedTexSubImage2D = (COIN_PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)PROC(glCompressedTexSubImage2DARB);
    w->glCompressedTexSubImage3D = (COIN_PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)PROC(glCompressedTexSubImage3DARB);
    w->glGetCompressedTexImage = (COIN_PFNGLGETCOMPRESSEDTEXIMAGEPROC)PROC(glGetCompressedTexImageARB);
  }
#endif /* GL_ARB_texture_compression */

  w->glColorTable = NULL;
  w->glColorSubTable = NULL;
  w->glGetColorTable = NULL;
  w->glGetColorTableParameteriv = NULL;
  w->glGetColorTableParameterfv = NULL;

#if defined(GL_VERSION_1_2) && defined(GL_ARB_imaging)
  if (cc_glglue_glversion_matches_at_least(w, 1, 2, 0) &&
      cc_glglue_glext_supported(w, "GL_ARB_imaging")) {
    w->glColorTable = (COIN_PFNGLCOLORTABLEPROC)PROC(glColorTable);
    w->glColorSubTable = (COIN_PFNGLCOLORSUBTABLEPROC)PROC(glColorSubTable);
    w->glGetColorTable = (COIN_PFNGLGETCOLORTABLEPROC)PROC(glGetColorTable);
    w->glGetColorTableParameteriv = (COIN_PFNGLGETCOLORTABLEPARAMETERIVPROC)PROC(glGetColorTableParameteriv);
    w->glGetColorTableParameterfv = (COIN_PFNGLGETCOLORTABLEPARAMETERFVPROC)PROC(glGetColorTableParameterfv);
  }
#endif /* GL_VERSION_1_2 && GL_ARB_imaging */

#if defined(GL_EXT_color_table)
  if ((w->glColorTable == NULL) &&
      cc_glglue_glext_supported(w, "GL_EXT_color_table")) {
    w->glColorTable = (COIN_PFNGLCOLORTABLEPROC)PROC(glColorTableEXT);
    w->glGetColorTable = (COIN_PFNGLGETCOLORTABLEPROC)PROC(glGetColorTableEXT);
    w->glGetColorTableParameteriv = (COIN_PFNGLGETCOLORTABLEPARAMETERIVPROC)PROC(glGetColorTableParameterivEXT);
    w->glGetColorTableParameterfv = (COIN_PFNGLGETCOLORTABLEPARAMETERFVPROC)PROC(glGetColorTableParameterfvEXT);
  }
#endif /* GL_EXT_color_table */

#if defined(GL_SGI_color_table)
  if ((w->glColorTable == NULL) &&
      cc_glglue_glext_supported(w, "GL_SGI_color_table")) {
    w->glColorTable = (COIN_PFNGLCOLORTABLEPROC)PROC(glColorTableSGI);
    w->glGetColorTable = (COIN_PFNGLGETCOLORTABLEPROC)PROC(glGetColorTableSGI);
    w->glGetColorTableParameteriv = (COIN_PFNGLGETCOLORTABLEPARAMETERIVPROC)PROC(glGetColorTableParameterivSGI);
    w->glGetColorTableParameterfv = (COIN_PFNGLGETCOLORTABLEPARAMETERFVPROC)PROC(glGetColorTableParameterfvSGI);
  }
#endif /* GL_SGI_color_table */

#if defined(GL_EXT_color_subtable)
  if ((w->glColorSubTable == NULL) &&
      cc_glglue_glext_supported(w, "GL_EXT_color_subtable")) {
    w->glColorSubTable = (COIN_PFNGLCOLORSUBTABLEPROC)PROC(glColorSubTableEXT);
  }
#endif /* GL_EXT_color_subtable */

  w->supportsPalettedTextures =
    cc_glglue_glext_supported(w, "GL_EXT_paletted_texture");
  /* FIXME: is paletted textures _really_ not supported through any
     non-extension mechanism for the later OpenGL spec versions?
     Investigate. 20031027 mortene. */

#ifdef GL_EXT_paletted_texture
  /* Note that EXT_paletted_texture defines glColorTableEXT et al
     "on it's own", i.e. it doesn't need the presence of
     EXT_color_table / SGI_color_table / OGL1.2+ + ARB_imaging. It
     only defines a *subset* of what EXT_color_table etc defines,
     though. */
  if ((w->glColorTable == NULL) &&
      cc_glglue_glext_supported(w, "GL_EXT_paletted_texture")) {
    w->glColorTable = (COIN_PFNGLCOLORTABLEPROC)PROC(glColorTableEXT);
    w->glColorSubTable = (COIN_PFNGLCOLORSUBTABLEPROC)PROC(glColorSubTableEXT);
    w->glGetColorTable = (COIN_PFNGLGETCOLORTABLEPROC)PROC(glGetColorTableEXT);
    w->glGetColorTableParameteriv = (COIN_PFNGLGETCOLORTABLEPARAMETERIVPROC)PROC(glGetColorTableParameterivEXT);
    w->glGetColorTableParameterfv = (COIN_PFNGLGETCOLORTABLEPARAMETERFVPROC)PROC(glGetColorTableParameterfvEXT);
  }
#endif /* GL_EXT_paletted_texture */

  /*
    Using the SGI_texture_color_table extension has been temporarily
    disabled, as it uses a different enum value for
    glColorTable(<target>,...), and seems to only support 2D
    textures. Quoting from the extension spec document:

        Accepted by the <cap> parameter of Enable, Disable, and
        IsEnabled, [...] and by the <target> parameter of
        ColorTableSGI, CopyColorTableSGI, GetColorTableSGI,
        ColorTableParameterfvSGI, ColorTableParameterivSGI,
        GetColorTableParameterfvSGI, GetColorTableParameterivSGI:

	TEXTURE_COLOR_TABLE_SGI		0x80BC

        Accepted by the <target> parameter of ColorTableSGI,
        GetColorTableParameterivSGI, and GetColorTableParameterfvSGI:

	PROXY_TEXTURE_COLOR_TABLE_SGI	0x80BD

    As paletted textures can only be supported through extensions, we
    should probably implement support for using this one in addition
    to EXT_paletted_texture.

    Note: our O2 supports this extension, but not
    EXT_paletted_texture, so it can be used for development and
    testing of support for this extension.

    20030129 mortene.
   */
#if 0
  w->supportsPalettedTextures = w->supportsPalettedTextures ||
    cc_glglue_glext_supported(w, "GL_SGI_texture_color_table");

#ifdef GL_SGI_texture_color_table
  /* Note that SGI_texture_color_table defines glColorTableEXT et al
     "on it's own", i.e. it doesn't need the presence of
     EXT_color_table / SGI_color_table / OGL1.2+ + ARB_imaging. It
     only defines a *subset* of what EXT_color_table etc defines,
     though. */
  if ((w->glColorTable == NULL) &&
      cc_glglue_glext_supported(w, "GL_SGI_texture_color_table")) {
    w->glColorTable = (COIN_PFNGLCOLORTABLEPROC)PROC(glColorTableSGI);
    w->glGetColorTable = (COIN_PFNGLGETCOLORTABLEPROC)PROC(glGetColorTableSGI);
    w->glGetColorTableParameteriv = (COIN_PFNGLGETCOLORTABLEPARAMETERIVPROC)PROC(glGetColorTableParameterivSGI);
    w->glGetColorTableParameterfv = (COIN_PFNGLGETCOLORTABLEPARAMETERFVPROC)PROC(glGetColorTableParameterfvSGI);
  }
#endif /* GL_SGI_texture_color_table */
#endif /* disabled */


  w->glBlendEquation = NULL;
  w->glBlendEquationEXT = NULL;

#if defined(GL_VERSION_1_4)
  if (cc_glglue_glversion_matches_at_least(w, 1, 4, 0)) {
    w->glBlendEquation = (COIN_PFNGLBLENDEQUATIONPROC)PROC(glBlendEquation);
  }
#endif /* GL_VERSION_1_4 */

  if (w->glBlendEquation == NULL) {
#if defined(GL_VERSION_1_2) && defined(GL_ARB_imaging)
    if (cc_glglue_glversion_matches_at_least(w, 1, 2, 0) &&
        cc_glglue_glext_supported(w, "GL_ARB_imaging")) {
      w->glBlendEquation = (COIN_PFNGLBLENDEQUATIONPROC)PROC(glBlendEquation);
    }
#endif /* GL_VERSION_1_2 && GL_ARB_imaging */
  }

#ifdef GL_EXT_blend_minmax
  if (cc_glglue_glext_supported(w, "GL_EXT_blend_minmax")) {
    w->glBlendEquationEXT = (COIN_PFNGLBLENDEQUATIONPROC)PROC(glBlendEquationEXT);
  }
#endif /* GL_EXT_blend_minmax */

  w->glVertexPointer = NULL; /* for cc_glglue_has_vertex_array() */
#if defined(GL_VERSION_1_1)
  if (cc_glglue_glversion_matches_at_least(w, 1, 1, 0)) {
    w->glVertexPointer = (COIN_PFNGLVERTEXPOINTERPROC) PROC(glVertexPointer);
    w->glTexCoordPointer = (COIN_PFNGLTEXCOORDPOINTERPROC) PROC(glTexCoordPointer);
    w->glNormalPointer = (COIN_PFNGLNORMALPOINTERPROC) PROC(glNormalPointer);
    w->glColorPointer = (COIN_PNFGLCOLORPOINTERPROC) PROC(glColorPointer);
    w->glIndexPointer = (COIN_PFNGLINDEXPOINTERPROC) PROC(glIndexPointer);
    w->glEnableClientState = (COIN_PFNGLENABLECLIENTSTATEPROC) PROC(glEnableClientState);
    w->glDisableClientState = (COIN_PFNGLDISABLECLIENTSTATEPROC) PROC(glDisableClientState);
    w->glInterleavedArrays = (COIN_PFNGLINTERLEAVEDARRAYSPROC) PROC(glInterleavedArrays);
    w->glDrawArrays = (COIN_PFNGLDRAWARRAYSPROC) PROC(glDrawArrays);
    w->glDrawElements = (COIN_PFNGLDRAWELEMENTSPROC) PROC(glDrawElements);
    w->glArrayElement = (COIN_PFNGLARRAYELEMENTPROC) PROC(glArrayElement);
  }
  if (w->glVertexPointer) {
    if (!w->glTexCoordPointer ||
        !w->glNormalPointer ||
        !w->glColorPointer ||
        !w->glIndexPointer ||
        !w->glEnableClientState ||
        !w->glDisableClientState ||
        !w->glInterleavedArrays ||
        !w->glDrawArrays ||
        !w->glDrawElements ||
        !w->glArrayElement) {
      w->glVertexPointer = NULL; /* cc_glglue_has_vertex_array() will return FALSE */
      cc_debugerror_postwarning("glglue_init",
                                "glVertexPointer found, but one or more of the other "
                                "vertex array functions were not found");
    }
  }
#endif /* GL_VERSION_1_1 */


#if defined(GL_VERSION_1_2)
  w->glDrawRangeElements = NULL;
  if (cc_glglue_glversion_matches_at_least(w, 1, 2, 0)) 
    w->glDrawRangeElements = (COIN_PFNGLDRAWRANGEELEMENTSPROC) PROC(glDrawRangeElements);   
#endif /* GL_VERSION_1_2 */


  /* Appeared in OpenGL v1.4 (but also in GL_EXT_multi_draw_array extension */
  w->glMultiDrawArrays = NULL;
  w->glMultiDrawElements = NULL;
#if defined(GL_VERSION_1_4)
  if (cc_glglue_glversion_matches_at_least(w, 1, 4, 0)) {
    w->glMultiDrawArrays = (COIN_PFNGLMULTIDRAWARRAYSPROC) PROC(glMultiDrawArrays);
    w->glMultiDrawElements = (COIN_PFNGLMULTIDRAWELEMENTSPROC) PROC(glMultiDrawElements);
  }
#endif /* GL_VERSION_1_4 */
#if defined(GL_EXT_multi_draw_arrays)
  if ((w->glMultiDrawArrays == NULL) && cc_glglue_glext_supported(w, "GL_EXT_multi_draw_arrays")) {
    w->glMultiDrawArrays = (COIN_PFNGLMULTIDRAWARRAYSPROC) PROC(glMultiDrawArraysEXT);
    w->glMultiDrawElements = (COIN_PFNGLMULTIDRAWELEMENTSPROC) PROC(glMultiDrawElementsEXT);
  }
#endif /* GL_EXT_multi_draw_arrays */

  w->glBindBuffer = NULL; /* so that cc_glglue_has_vertex_buffer_objects() works  */
#if defined(GL_VERSION_1_5)
  if (cc_glglue_glversion_matches_at_least(w, 1, 5, 0)) {   
    w->glBindBuffer = (COIN_PFNGLBINDBUFFERPROC) PROC(glBindBuffer);
    w->glDeleteBuffers = (COIN_PFNGLDELETEBUFFERSPROC) PROC(glDeleteBuffers);
    w->glGenBuffers = (COIN_PFNGLGENBUFFERSPROC) PROC(glGenBuffers);
    w->glIsBuffer = (COIN_PFNGLISBUFFERPROC) PROC(glIsBuffer);
    w->glBufferData = (COIN_PFNGLBUFFERDATAPROC) PROC(glBufferData);
    w->glBufferSubData = (COIN_PFNGLBUFFERSUBDATAPROC) PROC(glBufferSubData);
    w->glGetBufferSubData = (COIN_PFNGLGETBUFFERSUBDATAPROC) PROC(glGetBufferSubData);
    w->glMapBuffer = (COIN_PNFGLMAPBUFFERPROC) PROC(glMapBuffer);
    w->glUnmapBuffer = (COIN_PFNGLUNMAPBUFFERPROC) PROC(glUnmapBuffer);
    w->glGetBufferParameteriv = (COIN_PFNGLGETBUFFERPARAMETERIVPROC) PROC(glGetBufferParameteriv);
    w->glGetBufferPointerv = (COIN_PFNGLGETBUFFERPOINTERVPROC) PROC(glGetBufferPointerv);
  }
#endif /* GL_VERSION_1_5 */

#if defined(GL_ARB_vertex_buffer_object)
  if ((w->glBindBuffer == NULL) && cc_glglue_glext_supported(w, "GL_ARB_vertex_buffer_object")) {
    w->glBindBuffer = (COIN_PFNGLBINDBUFFERPROC) PROC(glBindBufferARB);
    w->glDeleteBuffers = (COIN_PFNGLDELETEBUFFERSPROC) PROC(glDeleteBuffersARB);
    w->glGenBuffers = (COIN_PFNGLGENBUFFERSPROC) PROC(glGenBuffersARB);
    w->glIsBuffer = (COIN_PFNGLISBUFFERPROC) PROC(glIsBufferARB);
    w->glBufferData = (COIN_PFNGLBUFFERDATAPROC) PROC(glBufferDataARB);
    w->glBufferSubData = (COIN_PFNGLBUFFERSUBDATAPROC) PROC(glBufferSubDataARB);
    w->glGetBufferSubData = (COIN_PFNGLGETBUFFERSUBDATAPROC) PROC(glGetBufferSubDataARB);
    w->glMapBuffer = (COIN_PNFGLMAPBUFFERPROC) PROC(glMapBufferARB);
    w->glUnmapBuffer = (COIN_PFNGLUNMAPBUFFERPROC) PROC(glUnmapBufferARB);
    w->glGetBufferParameteriv = (COIN_PFNGLGETBUFFERPARAMETERIVPROC) PROC(glGetBufferParameterivARB);
    w->glGetBufferPointerv = (COIN_PFNGLGETBUFFERPOINTERVPROC) PROC(glGetBufferPointervARB);
  }

#if defined(HAVE_GLX)  
  /* ARB_vertex_buffer_object does not work properly on Linux when
     using the Nvidia 44.96 driver (version 1.4.0). The VBO extension
     is therefore disabled for this driver. The issue was solved for
     the 53.28 driver (version 1.4.1). */
  if(!strcmp(w->vendorstr, "NVIDIA Corporation") && 
     !cc_glglue_glversion_matches_at_least(w, 1, 4, 1)) 
    w->glBindBuffer = NULL;  
#endif

#endif /* GL_ARB_vertex_buffer_object */

  if (w->glBindBuffer) {
    if (!w->glDeleteBuffers ||
        !w->glGenBuffers ||
        !w->glIsBuffer ||
        !w->glBufferData ||
        !w->glBufferSubData ||
        !w->glGetBufferSubData ||
        !w->glMapBuffer ||
        !w->glUnmapBuffer ||
        !w->glGetBufferParameteriv ||
        !w->glGetBufferPointerv) {
      w->glBindBuffer = NULL; /* so that cc_glglue_has_vertex_buffer_object() will return FALSE */
      cc_debugerror_postwarning("glglue_init",
                                "glBindBuffer found, but one or more of the other "
                                "vertex buffer object functions were not found");
    }
  }

  /* GL_NV_register_combiners */
  w->glCombinerParameterfvNV = NULL;
  w->glCombinerParameterivNV = NULL;
  w->glCombinerParameterfNV = NULL;
  w->glCombinerParameteriNV = NULL;
  w->glCombinerInputNV = NULL;
  w->glCombinerOutputNV = NULL;
  w->glFinalCombinerInputNV = NULL;
  w->glGetCombinerInputParameterfvNV = NULL;
  w->glGetCombinerInputParameterivNV = NULL;
  w->glGetCombinerOutputParameterfvNV = NULL;
  w->glGetCombinerOutputParameterivNV = NULL;
  w->glGetFinalCombinerInputParameterfvNV = NULL;
  w->glGetFinalCombinerInputParameterivNV = NULL;
  w->has_nv_register_combiners = FALSE;

#ifdef GL_NV_register_combiners  

  if (cc_glglue_glext_supported(w, "GL_NV_register_combiners")) {
    w->glCombinerParameterfvNV = (COIN_PFNGLCOMBINERPARAMETERFVNVPROC) PROC(glCombinerParameterfvNV);
    w->glCombinerParameterivNV = (COIN_PFNGLCOMBINERPARAMETERIVNVPROC) PROC(glCombinerParameterivNV);
    w->glCombinerParameterfNV = (COIN_PFNGLCOMBINERPARAMETERFNVPROC) PROC(glCombinerParameterfNV);
    w->glCombinerParameteriNV = (COIN_PFNGLCOMBINERPARAMETERINVPROC) PROC(glCombinerParameteriNV);
    w->glCombinerInputNV = (COIN_PFNGLCOMBINERINPUTNVPROC) PROC(glCombinerInputNV);
    w->glCombinerOutputNV = (COIN_PFNGLCOMBINEROUTPUTNVPROC) PROC(glCombinerOutputNV);
    w->glFinalCombinerInputNV = (COIN_PFNGLFINALCOMBINERINPUTNVPROC) PROC(glFinalCombinerInputNV);
    w->glGetCombinerInputParameterfvNV = (COIN_PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) PROC(glGetCombinerInputParameterfvNV);
    w->glGetCombinerInputParameterivNV = (COIN_PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) PROC(glGetCombinerInputParameterivNV);
    w->glGetCombinerOutputParameterfvNV = (COIN_PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) PROC(glGetCombinerOutputParameterfvNV);
    w->glGetCombinerOutputParameterivNV = (COIN_PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) PROC(glGetCombinerOutputParameterivNV);
    w->glGetFinalCombinerInputParameterfvNV = (COIN_PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) PROC(glGetFinalCombinerInputParameterfvNV);
    w->glGetFinalCombinerInputParameterivNV = (COIN_PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) PROC(glGetFinalCombinerInputParameterivNV);

    if (!(w->glCombinerParameterfvNV &&
          w->glCombinerParameterivNV &&
          w->glCombinerParameterfNV &&
          w->glCombinerParameteriNV &&
          w->glCombinerInputNV &&
          w->glCombinerOutputNV &&
          w->glFinalCombinerInputNV &&
          w->glGetCombinerInputParameterfvNV &&
          w->glGetCombinerInputParameterivNV &&
          w->glGetCombinerOutputParameterfvNV &&
          w->glGetCombinerOutputParameterivNV &&
          w->glGetFinalCombinerInputParameterfvNV &&
          w->glGetFinalCombinerInputParameterivNV)) {      
      cc_debugerror_postwarning("glglue_init","GL_NV_register_combiners found, but one or more of its "
                                "functions could not be bound.");
    }
    else {
      w->has_nv_register_combiners = TRUE;
    }
    
  } 
    
#endif /* GL_NV_register_combiners */
 
  
  /* GL_[NV/EXT]_texture_rectangle */
  w->has_ext_texture_rectangle = (cc_glglue_glext_supported(w, "GL_EXT_texture_rectangle") ||
                                  cc_glglue_glext_supported(w, "GL_NV_texture_rectangle")); 

  /* GL_NV_texture_shader */
  w->has_nv_texture_shader = cc_glglue_glext_supported(w, "GL_NV_texture_shader");

  /* GL_ARB_shadow */
  w->has_shadow = (cc_glglue_glext_supported(w, "GL_ARB_shadow") || 
                   cc_glglue_glversion_matches_at_least(w, 1, 4, 0));

  /* GL_ARB_depth_texture */
  w->has_depth_texture = (cc_glglue_glext_supported(w, "GL_ARB_depth_texture") ||
                          cc_glglue_glversion_matches_at_least(w, 1, 4, 0));
  
  /* GL_[ARB/EXT]_texture_env_combine */
  w->has_texture_env_combine = (cc_glglue_glext_supported(w, "GL_ARB_texture_env_combine") ||
                                cc_glglue_glext_supported(w, "GL_EXT_texture_env_combine") ||
                                cc_glglue_glversion_matches_at_least(w, 1, 3, 0));

  /* GL_ARB_fragment_program */
  w->glProgramStringARB = NULL;
  w->glBindProgramARB = NULL;
  w->glDeleteProgramsARB = NULL;
  w->glGenProgramsARB = NULL;
  w->glProgramEnvParameter4dARB = NULL;
  w->glProgramEnvParameter4dvARB = NULL;
  w->glProgramEnvParameter4fARB = NULL;
  w->glProgramEnvParameter4fvARB = NULL;
  w->glProgramLocalParameter4dARB = NULL;
  w->glProgramLocalParameter4dvARB = NULL;
  w->glProgramLocalParameter4fARB = NULL;
  w->glProgramLocalParameter4fvARB = NULL;
  w->glGetProgramEnvParameterdvARB = NULL;
  w->glGetProgramEnvParameterfvARB = NULL;
  w->glGetProgramLocalParameterdvARB = NULL;
  w->glGetProgramLocalParameterfvARB = NULL;
  w->glGetProgramivARB = NULL;
  w->glGetProgramStringARB = NULL;
  w->glIsProgramARB = NULL;
  w->has_arb_fragment_program = FALSE;
  
#ifdef GL_ARB_fragment_program
  if (cc_glglue_glext_supported(w, "GL_ARB_fragment_program")) {
   
   w->glProgramStringARB = (COIN_PFNGLPROGRAMSTRINGARBPROC) PROC(glProgramStringARB);
   w->glBindProgramARB = (COIN_PFNGLBINDPROGRAMARBPROC) PROC(glBindProgramARB);
   w->glDeleteProgramsARB = (COIN_PFNGLDELETEPROGRAMSARBPROC) PROC(glDeleteProgramsARB);
   w->glGenProgramsARB = (COIN_PFNGLGENPROGRAMSARBPROC) PROC(glGenProgramsARB);
   w->glProgramEnvParameter4dARB = (COIN_PFNGLPROGRAMENVPARAMETER4DARBPROC) PROC(glProgramEnvParameter4dARB); 
   w->glProgramEnvParameter4dvARB = (COIN_PFNGLPROGRAMENVPARAMETER4DVARBPROC) PROC(glProgramEnvParameter4dvARB);
   w->glProgramEnvParameter4fARB = (COIN_PFNGLPROGRAMENVPARAMETER4FARBPROC) PROC(glProgramEnvParameter4fARB);
   w->glProgramEnvParameter4fvARB = (COIN_PFNGLPROGRAMENVPARAMETER4FVARBPROC) PROC(glProgramEnvParameter4fvARB);
   w->glProgramLocalParameter4dARB = (COIN_PFNGLPROGRAMLOCALPARAMETER4DARBPROC) PROC(glProgramLocalParameter4dARB);
   w->glProgramLocalParameter4dvARB = (COIN_PFNGLPROGRAMLOCALPARAMETER4DVARBPROC) PROC(glProgramLocalParameter4dvARB);
   w->glProgramLocalParameter4fARB = (COIN_PFNGLPROGRAMLOCALPARAMETER4FARBPROC) PROC(glProgramLocalParameter4fARB);
   w->glProgramLocalParameter4fvARB = (COIN_PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) PROC(glProgramLocalParameter4fvARB);
   w->glGetProgramEnvParameterdvARB = (COIN_PFNGLGETPROGRAMENVPARAMETERDVARBPROC) PROC(glGetProgramEnvParameterdvARB);
   w->glGetProgramEnvParameterfvARB = (COIN_PFNGLGETPROGRAMENVPARAMETERFVARBPROC) PROC(glGetProgramEnvParameterfvARB);
   w->glGetProgramLocalParameterdvARB = (COIN_PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) PROC(glGetProgramLocalParameterdvARB);
   w->glGetProgramLocalParameterfvARB = (COIN_PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) PROC(glGetProgramLocalParameterfvARB);
   w->glGetProgramivARB = (COIN_PFNGLGETPROGRAMIVARBPROC) PROC(glGetProgramivARB);
   w->glGetProgramStringARB = (COIN_PFNGLGETPROGRAMSTRINGARBPROC) PROC(glGetProgramStringARB);
   w->glIsProgramARB = (COIN_PFNGLISPROGRAMARBPROC) PROC(glIsProgramARB);
 
   if (!(w->glProgramStringARB &&
         w->glBindProgramARB &&
         w->glDeleteProgramsARB &&
         w->glGenProgramsARB &&
         w->glProgramEnvParameter4dARB &&
         w->glProgramEnvParameter4dvARB &&
         w->glProgramEnvParameter4fARB &&
         w->glProgramEnvParameter4fvARB &&
         w->glProgramLocalParameter4dARB &&
         w->glProgramLocalParameter4dvARB &&
         w->glProgramLocalParameter4fARB &&
         w->glProgramLocalParameter4fvARB &&
         w->glGetProgramEnvParameterdvARB &&
         w->glGetProgramEnvParameterfvARB &&
         w->glGetProgramLocalParameterdvARB &&
         w->glGetProgramLocalParameterfvARB &&
         w->glGetProgramivARB &&
         w->glGetProgramStringARB &&
         w->glIsProgramARB)) {
     cc_debugerror_postwarning("glglue_init","GL_ARB_fragment_program found, but one or more of its "
                               "functions could not be bound."); 
   } else    
     w->has_arb_fragment_program = TRUE;
 } 
#endif /* GL_ARB_fragment_program */


  w->glVertexArrayRangeNV = NULL;
#if defined(GL_NV_vertex_array_range) && (defined(HAVE_GLX) || defined(HAVE_WGL))
  if (cc_glglue_glext_supported(w, "GL_NV_vertex_array_range")) {
    w->glVertexArrayRangeNV = (COIN_PFNGLVERTEXARRAYRANGENVPROC) PROC(glVertexArrayRangeNV);
    w->glFlushVertexArrayRangeNV = (COIN_PFNGLFLUSHVERTEXARRAYRANGENVPROC) PROC(glFlushVertexArrayRangeNV);                
#ifdef HAVE_GLX
    w->glAllocateMemoryNV = (COIN_PFNGLALLOCATEMEMORYNVPROC) PROC(glXAllocateMemoryNV);
    w->glFreeMemoryNV = (COIN_PFNGLFREEMEMORYNVPROC) PROC(glXFreeMemoryNV);
#endif /* HAVE_GLX */
#ifdef HAVE_WGL
    w->glAllocateMemoryNV = (COIN_PFNGLALLOCATEMEMORYNVPROC) PROC(wglAllocateMemoryNV);
    w->glFreeMemoryNV = (COIN_PFNGLFREEMEMORYNVPROC) PROC(wglFreeMemoryNV);
#endif /* HAVE_WGL */
    if (w->glVertexArrayRangeNV) {
      if (!w->glFlushVertexArrayRangeNV ||
          !w->glAllocateMemoryNV ||
          !w->glFreeMemoryNV) {
        w->glVertexArrayRangeNV = NULL;
        cc_debugerror_postwarning("glglue_init",
                                  "glVertexArrayRangeNV found, but one or more of the other "
                                  "vertex array functions were not found");

      }
    }
  }
#endif /* HAVE_GLX || HAVE_WGL */

  w->can_do_bumpmapping = FALSE;
  if (w->glActiveTexture &&
      (cc_glglue_glversion_matches_at_least(w, 1, 3, 0) ||
       (cc_glglue_glext_supported(w, "GL_ARB_texture_cube_map") &&
        w->has_texture_env_combine &&
        cc_glglue_glext_supported(w, "GL_ARB_texture_env_dot3")))) {
    w->can_do_bumpmapping = TRUE;
  }
  
  /* FIXME: We should be able to support more than one way to do order
     independent transparency (eg. by using fragment
     programming). This would demand a different combinations of
     extensions (and thus; a different codepath in
     SoGLRenderAction). (20031124 handegar) */
  w->can_do_sortedlayersblend = FALSE;
  if((w->has_nv_register_combiners &&
      w->has_ext_texture_rectangle &&
      w->has_nv_texture_shader &&
      w->has_depth_texture &&
      w->has_shadow) ||
     w->has_arb_fragment_program)
    w->can_do_sortedlayersblend = TRUE;
  
}

#undef PROC

static SbBool
glglue_check_trident_clampedge_bug(const char * vendor,
                                   const char * renderer,
                                   const char * version)
{
  return
    (strcmp(vendor, "Trident") == 0) &&
    (strcmp(renderer, "Blade XP/AGP") == 0) &&
    (strcmp(version, "1.2.1") == 0);
}

/* Give warnings on known faulty drivers. */
static void
glglue_check_driver(const char * vendor, const char * renderer,
                    const char * version)
{
#ifdef COIN_DEBUG
  /* Only spit out this in debug builds, as the bug was never properly
     confirmed. */
  if (coin_glglue_radeon_warning()) {
    if (strcmp(renderer, "Radeon 7500 DDR x86/SSE2") == 0) {
      cc_debugerror_postwarning("glglue_check_driver",
                                "We've had an unconfirmed bugreport that "
                                "this OpenGL driver ('%s') may crash upon "
                                "attempts to use 3D texturing. "
                                "We would like to get assistance to help "
                                "us debug the cause of this problem, so "
                                "please get in touch with us at "
                                "<coin-support@coin3d.org>. "
                                "This debug message can be turned off "
                                "permanently by setting the environment "
                                "variable COIN_GLGLUE_NO_RADEON_WARNING=1.",
                                renderer);

      /*
        Some additional information:

        The full driver information for the driver where this was
        reported is as follows:

        GL_VENDOR == 'ATI Technologies Inc.'
        GL_RENDERER == 'Radeon 7500 DDR x86/SSE2'
        GL_VERSION == '1.3.3302 Win2000 Release'

        The driver was reported to crash on MSWin with the
        SoGuiExamples/nodes/texture3 example. The reporter couldn't
        help us debug it, as he could a) not get a call-stack
        backtrace, and b) swapped his card for an NVidia card.

        Perhaps we should get hold of a Radeon card ourselves, to test
        and debug the problem.

        <mortene@sim.no>
      */
    }
  }
#endif /* COIN_DEBUG */

  if (coin_glglue_old_matrox_warning() &&
      (strcmp(renderer, "Matrox G400") == 0) &&
      (strcmp(version, "1.1.3 Aug 30 2001") == 0)) {
    cc_debugerror_postwarning("glglue_check_driver",
                              "This old OpenGL driver (\"%s\" \"%s\") has "
                              "known bugs, please upgrade.  "
                              "(This debug message can be turned off "
                              "permanently by setting the environment "
                              "variable COIN_GLGLUE_NO_G400_WARNING=1).",
                              renderer, version);
  }

  if (coin_glglue_old_elsa_warning() &&
      (strcmp(renderer, "ELSA TNT2 Vanta/PCI/SSE") == 0) &&
      (strcmp(version, "1.1.4 (4.06.00.266)") == 0)) {
    cc_debugerror_postwarning("glglue_check_driver",
                              "This old OpenGL driver (\"%s\" \"%s\") has "
                              "known bugs, please upgrade.  "
                              "(This debug message can be turned off "
                              "permanently by setting the environment "
                              "variable COIN_GLGLUE_NO_ELSA_WARNING=1).",
                              renderer, version);
  }

  /*
    The full driver information for the driver where this was reported
    is as follows:

    GL_VENDOR == 'Matrox Graphics Inc.'
    GL_RENDERER == 'Matrox G400'
    GL_VERSION == '1.1.3 Aug 30 2001'

    GL_VENDOR == 'ELSA AG (Aachen, Germany).'
    GL_RENDERER == 'ELSA TNT2 Vanta/PCI/SSE'
    GL_VERSION == '1.1.4 (4.06.00.266)'

    The driver was reported to crash on MSWin under following
    conditions, quoted verbatim from the problem report:

    ------8<---- [snip] -----------8<---- [snip] -----

    I observe a bit of strange behaviour on my NT4 systems. I have an
    appliction which uses the the following bit of code:

    // Define line width
    SoDrawStyle *drawStyle = new SoDrawStyle;
    drawStyle->lineWidth.setValue(3);
    drawStyle->linePattern.setValue(0x0F0F);
    root->addChild(drawStyle);

    // Define line connection
    SoCoordinate3 *coords = new SoCoordinate3;
    coords->point.setValues(0, 2, vert);
    root->addChild(coords);

    SoLineSet *lineSet = new SoLineSet ;
    lineSet->numVertices.set1Value(0, 2) ;
    root->addChild(lineSet);

    It defines a line with a dashed pattern. When the line is in a
    direction and the viewing direction is not parrallel to this line
    all works fine. In case the viewing direction is the same as the
    line direction one of my systems crashes [...]

    ------8<---- [snip] -----------8<---- [snip] -----

    <mortene@sim.no>

    UPDATE 20030116 mortene: as of this date, the most recent Matrox
    driver (version 5.86.032, from 2002-11-21) still exhibits the same
    problem, while the ELSA driver can be upgraded to a version that
    does not have the bug any more.
  */

  if (coin_glglue_sun_expert3d_warning() &&
      (strcmp(renderer, "Sun Expert3D, VIS") == 0) &&
      (strcmp(version, "1.2 Sun OpenGL 1.2.1 patch 109544-19 for Solaris") == 0)) {
    cc_debugerror_postwarning("glglue_check_driver",
                              "This OpenGL driver (\"%s\" \"%s\") has known "
                              "problems with dual screen configurations, "
                              "please upgrade.  "
                              "(This debug message can be turned off "
                              "permanently by setting the environment variable"
                              " COIN_GLGLUE_NO_SUN_EXPERT3D_WARNING=1).",
                              renderer, version);
  /*
    The full driver information for the driver where this was reported
    is as follows:

    GL_VENDOR == 'Sun Microsystems, Inc.'
    GL_RENDERER == 'Sun Expert3D, VIS'
    GL_VERSION == '1.2 Sun OpenGL 1.2.1 patch 109544-19 for Solaris'

    The driver was reported to fail when running on a Sun Solaris
    system with the XVR1000 graphics card. Quoted verbatim from the
    problem report:

    ------8<---- [snip] -----------8<---- [snip] -----

    [The client] works with two screens. One of the screen works as it
    should, while the otherone has erronious apperance (see uploaded
    image). The errors are the stripes on the texture (It should be
    one continious texture). The texture is wrapped on a rectangle
    (i.e. two large triangles). It is not only the OpenGl part of the
    window that is weired.  Some buttons are missing and other buttons
    have wrong colors++.

    ------8<---- [snip] -----------8<---- [snip] -----

    The error disappeared after a driver upgrade.

    <mortene@sim.no>
  */
  }

  if (coin_glglue_trident_warning() &&
      glglue_check_trident_clampedge_bug(vendor, renderer, version)) {
    cc_debugerror_postwarning("glglue_check_driver",
                              "This OpenGL driver (\"%s\" \"%s\" \"%s\") has "
                              "a known problem: it doesn't support the "
                              "GL_CLAMP_TO_EDGE texture wrapping mode. "
                              "(This debug message can be turned off "
                              "permanently by setting the environment variable"
                              " COIN_GLGLUE_NO_TRIDENT_WARNING=1).",
                              vendor, renderer, version);
    /*
      This problem manifests itself in the form of a glGetError()
      reporting GL_INVALID_ENUM if GL_TEXTURE_WRAP_[S|T] is attempted
      set to GL_CLAMP_TO_EDGE. GL_CLAMP_TO_EDGE was introduced with
      OpenGL v1.2.0, and the driver reports v1.2.1, so it is supposed
      to work.
    */
  }
}

/* We're basically using the Singleton pattern to instantiate and
   return OpenGL-glue "object structs". We're constructing one
   instance for each OpenGL context, though.  */
const cc_glglue *
cc_glglue_instance(int contextid)
{
  SbBool found;
  void * ptr;
  GLint gltmp;

  cc_glglue * gi = NULL;

  CC_SYNC_BEGIN(cc_glglue_instance);

  if (!gldict) {  /* First invocation, do initializations. */
    gldict = cc_hash_construct(16, 0.75f);
    coin_atexit((coin_atexit_f *)glglue_cleanup, 0);
  }

  found = cc_hash_get(gldict, (unsigned long) contextid, &ptr);

  if (!found) {
    /* Internal consistency checking. */
#ifdef HAVE_GLX
    /* Disabled this assert because GLX in Mesa version 3.4.2
       (GL_VENDOR "VA Linux Systems, Inc", GL_RENDERER "Mesa GLX
       Indirect", GL_VERSION "1.2 Mesa 3.4.2") returns NULL even
       though there really is a current context set up. (Reported by
       kintel.) */
/*     assert((glXGetCurrentContext() != NULL) && "must have a current GL context when instantiating cc_glglue"); */
#endif /* HAVE_GLX */
#ifdef HAVE_WGL
    assert((wglGetCurrentContext() != NULL) && "must have a current GL context when instantiating cc_glglue");
#endif /* HAVE_WGL */
#ifdef HAVE_AGL
    assert (CGLGetCurrentContext() != NULL);
    /* Note: We cannot use aglGetCurrentContext() here, since that
       only returns a value != NULL if the context has been set using
       aglSetCurrentContext(). */
#endif /* HAVE_AGL */

    gi = (cc_glglue*)malloc(sizeof(cc_glglue));
    /* clear to set all pointers and variables to NULL or 0 */
    memset(gi, 0, sizeof(cc_glglue));
    /* FIXME: handle out-of-memory on malloc(). 20000928 mortene. */

    /* create dict that makes a quick lookup for GL extensions */
    gi->glextdict = cc_hash_construct(256, 0.75f);

    ptr = gi;
    cc_hash_put(gldict, (unsigned long) contextid, ptr);

    if (!glglue_self_handle && !glglue_tried_open_self) {
      glglue_self_handle = cc_dl_open(NULL);
      glglue_tried_open_self = TRUE;
    }

    /* NB: if you are getting a crash here, it's because an attempt at
     * setting up a cc_glglue instance was made when there is no
     * current OpenGL context. */
    gi->versionstr = (const char *)glGetString(GL_VERSION);
    assert(gi->versionstr && "could not call glGetString() -- no current GL context?");
    assert(glGetError() == GL_NO_ERROR && "GL error when calling glGetString() -- no current GL context?");

    glglue_set_glVersion(gi);
    glxglue_init(gi);

    gi->vendorstr = (const char *)glGetString(GL_VENDOR);
    gi->vendor_is_SGI = strcmp((const char *)gi->vendorstr, "SGI") == 0;
    gi->rendererstr = (const char *)glGetString(GL_RENDERER);
    gi->extensionsstr = (const char *)glGetString(GL_EXTENSIONS);

    /* read some limits */
    glGetIntegerv(GL_MAX_LIGHTS, &gltmp);
    gi->max_lights = (int) gltmp;

    {
      GLfloat vals[2];
      glGetFloatv(GL_POINT_SIZE_RANGE, vals);

      // Matthias Koenig reported on coin-discuss that the OpenGL
      // implementation on SGI Onyx 2 InfiniteReality returns 0 for the
      // lowest pointsize, but it will still set the return value of
      // glGetError() to GL_INVALID_VALUE if this size is attempted
      // used. So the boundary range fix in the next line of code is a
      // workaround for that OpenGL implementation bug.
      //
      // 0.0f and lower values are explicitly disallowed, according to
      // the OpenGL 1.3 specification, Chapter 3.3.

      if (vals[0] <= 0.0f) { 
        vals[0] = vals[1] < 1.0f ? vals[1] : 1.0f;
      }
      gi->point_size_range[0] = vals[0];
      gi->point_size_range[1] = vals[1]; 
    }
    {
      GLfloat vals[2];
      glGetFloatv(GL_LINE_WIDTH_RANGE, vals);

      // Matthias Koenig reported on coin-discuss that the OpenGL
      // implementation on SGI Onyx 2 InfiniteReality returns 0 for the
      // lowest linewidth, but it will still set the return value of
      // glGetError() to GL_INVALID_VALUE if this size is attempted
      // used. This is a workaround for what looks like an OpenGL bug.
      
      if (vals[0] <= 0.0f) { 
        vals[0] = vals[1] < 1.0f ? vals[1] : 1.0f;
      }
      gi->line_width_range[0] = vals[0];
      gi->line_width_range[1] = vals[1];
    }

    if (coin_glglue_debug()) {
      cc_debugerror_postinfo("cc_glglue_instance",
                             "glGetString(GL_VENDOR)=='%s' (=> vendor_is_SGI==%s)",
                             gi->vendorstr,
                             gi->vendor_is_SGI ? "TRUE" : "FALSE");
      cc_debugerror_postinfo("cc_glglue_instance",
                             "glGetString(GL_RENDERER)=='%s'",
                             gi->rendererstr);
      cc_debugerror_postinfo("cc_glglue_instance",
                             "glGetString(GL_EXTENSIONS)=='%s'",
                             gi->extensionsstr);

      cc_debugerror_postinfo("cc_glglue_instance",
                             "Rendering is %sdirect.",
                             gi->glx.isdirect ? "" : "in");
    }

    glglue_check_driver(gi->vendorstr, gi->rendererstr, gi->versionstr);

    /* Resolve our function pointers. */
    glglue_resolve_symbols(gi);
  }
  else {
    gi = (cc_glglue *)ptr;
  }

  CC_SYNC_END(cc_glglue_instance);
  return gi;
}

const cc_glglue *
cc_glglue_instance_from_context_ptr(void * ctx)
{
  /* The id can really be anything unique for the current context, but
     we should avoid a crash with the possible ids defined by
     SoGLCacheContextElement. It's a bit of a hack, this. */
  const int id = (int)((long)ctx);
  return cc_glglue_instance(id);
}

SbBool
cc_glglue_isdirect(const cc_glglue * w)
{
  return w->glx.isdirect;
}

/* Returns a flag which indicates whether or not to allow the use of
   OpenGL 1.1+ features and extensions.

   We default to *not* allowing this if rendering is indirect, as
   we've seen major problems with at least NVidia GLX when using
   OpenGL 1.1+ features. It can be forced on by an environment
   variable, though.

   (A better strategy *might* be to default to allow it, but to smoke
   out and warn if we detect NVidia GLX, and in addition to provide an
   environment variable that disables it.)
*/
static SbBool
glglue_allow_newer_opengl(const cc_glglue * w)
{
  static SbBool fullindirect = -1;
  static SbBool force1_0 = -1;
  static const char * COIN_FULL_INDIRECT_RENDERING = "COIN_FULL_INDIRECT_RENDERING";
  static const char * COIN_DONT_INFORM_INDIRECT_RENDERING = "COIN_DONT_INFORM_INDIRECT_RENDERING";

  if (fullindirect == -1) {
    fullindirect = (glglue_resolve_envvar(COIN_FULL_INDIRECT_RENDERING) > 0);
  }

  if (force1_0 == -1) {
    force1_0 = (glglue_resolve_envvar("COIN_FORCE_GL1_0_ONLY") > 0);
  }

  if (force1_0) return FALSE;

  if (!w->glx.isdirect && !fullindirect) {
    /* We give out a warning when the full OpenGL feature set is not
       used, in case the end user uses an application with a remote
       display, and that was not expected by the application
       programmer. */
    static int inform = -1;
    if (inform == -1) { inform = glglue_resolve_envvar(COIN_DONT_INFORM_INDIRECT_RENDERING); }
    if (inform == 0) {
      cc_debugerror_postinfo("glglue_allow_newer_opengl",
                             "\n\nFeatures of OpenGL version > 1.0 has been\n"
                             "disabled, due to the use of a remote display.\n\n"
                             "This is so because many common OpenGL drivers\n"
                             "have problems in this regard.\n\n"
                             "To force full OpenGL use, set the environment\n"
                             "variable %s=1 and re-run the application.\n\n"
                             "If you don't want this message displayed again,\n"
                             "set the environment variable %s=1.\n",
                             COIN_FULL_INDIRECT_RENDERING,
                             COIN_DONT_INFORM_INDIRECT_RENDERING);
    }

    return FALSE;
  }

  return TRUE;
}

/*!
  Whether glPolygonOffset() is availble or not: either we're on OpenGL
  1.1 or the GL_EXT_polygon_offset extension is available.

  Method then available for use:
  \li cc_glglue_glPolygonOffset
*/
SbBool
cc_glglue_has_polygon_offset(const cc_glglue * w)
{
  if (!glglue_allow_newer_opengl(w)) return FALSE;

  return (w->glPolygonOffset || w->glPolygonOffsetEXT) ? TRUE : FALSE;
}

/* Returns the glPolygonOffset() we're actually going to use. */
static COIN_PFNGLPOLYGONOFFSETPROC
glglue_glPolygonOffset(const cc_glglue * w)
{
  COIN_PFNGLPOLYGONOFFSETPROC poff = NULL;

  assert(w->glPolygonOffset ||  w->glPolygonOffsetEXT);

  poff = w->glPolygonOffset;

  /* Some SGI OpenGL 1.1 driver(s) seems to have a buggy
     implementation of glPolygonOffset(), according to pederb after
     some debugging he did for Fedem. These drivers'
     glPolygonOffsetEXT() actually seems to work better, so we prefer
     that if available. */
  if (w->vendor_is_SGI && w->glPolygonOffsetEXT &&
      cc_glglue_glversion_matches_at_least(w, 1, 1, 0) &&
      !cc_glglue_glversion_matches_at_least(w, 1, 2, 0)) {
    poff = w->glPolygonOffsetEXT;
  }

  /* Since we know glPolygonOffset() can be problematic, we also
     provide a way to prefer the EXT function instead through an
     environment variable "COIN_PREFER_GLPOLYGONOFFSET_EXT" (which
     could be handy for help debugging remote systems, at least). */
  if (w->glPolygonOffsetEXT && glglue_prefer_glPolygonOffsetEXT()) {
    poff = w->glPolygonOffsetEXT;
  }

  /* If glPolygonOffset() is not available (and the function pointer
     was not set by any of the bug workaround if-checks above), fall
     back on extension. */
  if (poff == NULL) { poff = w->glPolygonOffsetEXT; }

  return poff;
}

/*!
  Enable or disable z-buffer offsetting for the given primitive types.
*/
void
cc_glglue_glPolygonOffsetEnable(const cc_glglue * w,
                                SbBool enable, int m)
{
  COIN_PFNGLPOLYGONOFFSETPROC poff = glglue_glPolygonOffset(w);

  if (enable) {
    if (poff == w->glPolygonOffset) {
      if (m & cc_glglue_FILLED) glEnable(GL_POLYGON_OFFSET_FILL);
      else glDisable(GL_POLYGON_OFFSET_FILL);
      if (m & cc_glglue_LINES) glEnable(GL_POLYGON_OFFSET_LINE);
      else glDisable(GL_POLYGON_OFFSET_LINE);
      if (m & cc_glglue_POINTS) glEnable(GL_POLYGON_OFFSET_POINT);
      else glDisable(GL_POLYGON_OFFSET_POINT);
    }
    else { /* using glPolygonOffsetEXT() */
      /* The old pre-1.1 extension only supports filled polygon
         offsetting. */
      if (m & cc_glglue_FILLED) glEnable(GL_POLYGON_OFFSET_EXT);
      else glDisable(GL_POLYGON_OFFSET_EXT);

      if (coin_glglue_debug() && (m != cc_glglue_FILLED)) {
        static SbBool first = TRUE;
        if (first) {
          cc_debugerror_postwarning("cc_glglue_glPolygonOffsetEnable",
                                    "using EXT_polygon_offset, which only "
                                    "supports filled-polygon offsetting");
          first = FALSE;
        }
      }
    }
  }
  else { /* disable */
    if (poff == w->glPolygonOffset) {
      if (m & cc_glglue_FILLED) glDisable(GL_POLYGON_OFFSET_FILL);
      if (m & cc_glglue_LINES) glDisable(GL_POLYGON_OFFSET_LINE);
      if (m & cc_glglue_POINTS) glDisable(GL_POLYGON_OFFSET_POINT);
    }
    else { /* using glPolygonOffsetEXT() */
      if (m & cc_glglue_FILLED) glDisable(GL_POLYGON_OFFSET_EXT);
      /* Pre-1.1 glPolygonOffset extension only supported filled primitives.*/
    }
  }
}

void
cc_glglue_glPolygonOffset(const cc_glglue * w,
                          GLfloat factor,
                          GLfloat units)
{
  COIN_PFNGLPOLYGONOFFSETPROC poff = glglue_glPolygonOffset(w);

  if (poff == w->glPolygonOffsetEXT) {
    /* Try to detect if user actually attempted to specify a valid
       bias value, like the old glPolygonOffsetEXT() extension
       needs. If not, assume that the "units" argument was set up for
       the "real" glPolygonOffset() function, and use a default value
       that should work fairly ok under most circumstances. */
    SbBool isbias = (units > 0.0f) && (units < 0.01f);
    if (!isbias) units = 0.000001f;

    /* FIXME: shouldn't there be an attempt to convert the other way
       around too? Ie, if it *is* a "bias" value and we're using the
       "real" 1.1 glPolygonOffset() function, try to convert it into a
       valid "units" value? 20020919 mortene. */
  }

  poff(factor, units);
}

/*!
  Whether 3D texture objects are available or not: either we're on OpenGL
  1.1, or the GL_EXT_texture_object extension is available.

  Methods then available for use:

  \li cc_glglue_glGenTextures
  \li cc_glglue_glBindTexture
  \li cc_glglue_glDeleteTextures
*/
SbBool
cc_glglue_has_texture_objects(const cc_glglue * w)
{
  if (!glglue_allow_newer_opengl(w)) return FALSE;

  return w->glGenTextures && w->glBindTexture && w->glDeleteTextures;
}

void
cc_glglue_glGenTextures(const cc_glglue * w, GLsizei n, GLuint * textures)
{
  assert(w->glGenTextures);
  w->glGenTextures(n, textures);
}

void
cc_glglue_glBindTexture(const cc_glglue * w, GLenum target, GLuint texture)
{
  assert(w->glBindTexture);
  w->glBindTexture(target, texture);
}

void
cc_glglue_glDeleteTextures(const cc_glglue * w, GLsizei n, const GLuint * textures)
{
  assert(w->glDeleteTextures);
  w->glDeleteTextures(n, textures);
}

/*!
  Whether sub-textures are supported: either we're on OpenGL 1.2, or
  the GL_EXT_texture3D extension is available.

  Methods then available for use:

  \li cc_glglue_glTexImage3D
  \li cc_glglue_glTexSubImage3D
  \li cc_glglue_glCopyTexSubImage3D
*/
SbBool
cc_glglue_has_texsubimage(const cc_glglue * w)
{
  if (!glglue_allow_newer_opengl(w)) return FALSE;

  return w->glTexSubImage2D ? TRUE : FALSE;
}

void
cc_glglue_glTexSubImage2D(const cc_glglue * w,
                          GLenum target,
                          GLint level,
                          GLint xoffset,
                          GLint yoffset,
                          GLsizei width,
                          GLsizei height,
                          GLenum format,
                          GLenum type,
                          const GLvoid * pixels)
{
  assert(w->glTexSubImage2D);
  w->glTexSubImage2D(target, level, xoffset, yoffset,
                     width, height, format, type, pixels);
}

/*!
  Whether 3D textures are available or not: either we're on OpenGL
  1.2, or the GL_EXT_texture3D extension is available.

  Methods then available for use:

  \li cc_glglue_glTexImage3D
  \li cc_glglue_glTexSubImage3D
  \li cc_glglue_glCopyTexSubImage3D
*/
SbBool
cc_glglue_has_3d_textures(const cc_glglue * w)
{
  if (!glglue_allow_newer_opengl(w)) return FALSE;

  return
    w->glTexImage3D &&
    w->glCopyTexSubImage3D &&
    w->glTexSubImage3D;
}

SbBool
cc_glglue_has_2d_proxy_textures(const cc_glglue * w)
{
  if (!glglue_allow_newer_opengl(w)) return FALSE;

  /* FIXME: there are differences between the 1.1 proxy mechanisms and
     the GL_EXT_texture proxy extension; the 1.1 support considers
     mipmaps. I think. Check documentation in the GL spec. If that is
     correct, we can't really use them interchangeable versus each
     other like we now do in Coin code. 20030121 mortene. */
  return
    cc_glglue_glversion_matches_at_least(w, 1, 1, 0) ||
    cc_glglue_glext_supported(w, "GL_EXT_texture");
}

SbBool
cc_glglue_has_texture_edge_clamp(const cc_glglue * w)
{
  static int buggytrident = -1;

  if (!glglue_allow_newer_opengl(w)) return FALSE;

  if (buggytrident == -1) {
    buggytrident = glglue_check_trident_clampedge_bug(w->vendorstr,
                                                      w->rendererstr,
                                                      w->versionstr);
  }
  if (buggytrident) { return FALSE; }

  return
    cc_glglue_glversion_matches_at_least(w, 1, 2, 0) ||
    cc_glglue_glext_supported(w, "GL_EXT_texture_edge_clamp") ||
    cc_glglue_glext_supported(w, "GL_SGIS_texture_edge_clamp");
}

SbBool
cc_glglue_has_multitexture(const cc_glglue * w)
{
  if (!glglue_allow_newer_opengl(w)) return FALSE;
  return w->glActiveTexture != NULL;
}

int
cc_glglue_max_texture_units(const cc_glglue * w)
{
  if (!glglue_allow_newer_opengl(w)) return 1;
  return w->maxtextureunits; /* will be 1 when multitexturing is not available */
}


void
cc_glglue_glTexImage3D(const cc_glglue * w,
                       GLenum target,
                       GLint level,
                       GLenum internalformat,
                       GLsizei width,
                       GLsizei height,
                       GLsizei depth,
                       GLint border,
                       GLenum format,
                       GLenum type,
                       const GLvoid *pixels)
{
  assert(w->glTexImage3D);
  w->glTexImage3D(target, level, internalformat,
                  width, height, depth, border,
                  format, type, pixels);
}

void
cc_glglue_glTexSubImage3D(const cc_glglue * w,
                          GLenum target,
                          GLint level,
                          GLint xoffset,
                          GLint yoffset,
                          GLint zoffset,
                          GLsizei width,
                          GLsizei height,
                          GLsizei depth,
                          GLenum format,
                          GLenum type,
                          const GLvoid * pixels)
{
  assert(w->glTexSubImage3D);
  w->glTexSubImage3D(target, level, xoffset, yoffset,
                     zoffset, width, height, depth, format,
                     type, pixels);
}

void
cc_glglue_glCopyTexSubImage3D(const cc_glglue * w,
                              GLenum target,
                              GLint level,
                              GLint xoffset,
                              GLint yoffset,
                              GLint zoffset,
                              GLint x,
                              GLint y,
                              GLsizei width,
                              GLsizei height)
{
  assert(w->glCopyTexSubImage3D);
  w->glCopyTexSubImage3D(target,
                         level,
                         xoffset,
                         yoffset,
                         zoffset,
                         x,
                         y,
                         width,
                         height);
}

void
cc_glglue_glActiveTexture(const cc_glglue * w,
                          GLenum texture)
{
  assert(w->glActiveTexture);
  w->glActiveTexture(texture);
}

void
cc_glglue_glClientActiveTexture(const cc_glglue * w,
                                GLenum texture)
{
  assert(w->glClientActiveTexture);
  w->glClientActiveTexture(texture);
}
void
cc_glglue_glMultiTexCoord2f(const cc_glglue * w,
                            GLenum target,
                            GLfloat s,
                            GLfloat t)
{
  assert(w->glMultiTexCoord2f);
  w->glMultiTexCoord2f(target, s, t);
}

void
cc_glglue_glMultiTexCoord2fv(const cc_glglue * w,
                             GLenum target,
                             const GLfloat * v)
{
  assert(w->glMultiTexCoord2fv);
  w->glMultiTexCoord2fv(target, v);
}

void
cc_glglue_glMultiTexCoord3fv(const cc_glglue * w,
                             GLenum target,
                             const GLfloat * v)
{
  assert(w->glMultiTexCoord3fv);
  w->glMultiTexCoord3fv(target, v);
}

void
cc_glglue_glMultiTexCoord4fv(const cc_glglue * w,
                             GLenum target,
                             const GLfloat * v)
{
  assert(w->glMultiTexCoord4fv);
  w->glMultiTexCoord4fv(target, v);
}

SbBool
cc_glue_has_texture_compression(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;

  return
    glue->glCompressedTexImage1D &&
    glue->glCompressedTexImage2D &&
    glue->glCompressedTexImage3D &&
    glue->glGetCompressedTexImage;
}

SbBool
cc_glue_has_texture_compression_2d(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->glCompressedTexImage2D && glue->glGetCompressedTexImage;
}

SbBool
cc_glue_has_texture_compression_3d(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->glCompressedTexImage3D && glue->glGetCompressedTexImage;
}

void
cc_glglue_glCompressedTexImage3D(const cc_glglue * glue,
                                 GLenum target,
                                 GLint level,
                                 GLenum internalformat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLsizei depth,
                                 GLint border,
                                 GLsizei imageSize,
                                 const GLvoid * data)
{
  assert(glue->glCompressedTexImage3D);
  glue->glCompressedTexImage3D(target,
                               level,
                               internalformat,
                               width,
                               height,
                               depth,
                               border,
                               imageSize,
                               data);
}

void
cc_glglue_glCompressedTexImage2D(const cc_glglue * glue,
                                 GLenum target,
                                 GLint level,
                                 GLenum internalformat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLint border,
                                 GLsizei imageSize,
                                 const GLvoid *data)
{
  assert(glue->glCompressedTexImage2D);
  glue->glCompressedTexImage2D(target,
                               level,
                               internalformat,
                               width,
                               height,
                               border,
                               imageSize,
                               data);
}

void
cc_glglue_glCompressedTexImage1D(const cc_glglue * glue,
                                 GLenum target,
                                 GLint level,
                                 GLenum internalformat,
                                 GLsizei width,
                                 GLint border,
                                 GLsizei imageSize,
                                 const GLvoid *data)
{
  assert(glue->glCompressedTexImage1D);
  glue->glCompressedTexImage1D(target,
                               level,
                               internalformat,
                               width,
                               border,
                               imageSize,
                               data);
}

void
cc_glglue_glCompressedTexSubImage3D(const cc_glglue * glue,
                                    GLenum target,
                                    GLint level,
                                    GLint xoffset,
                                    GLint yoffset,
                                    GLint zoffset,
                                    GLsizei width,
                                    GLsizei height,
                                    GLsizei depth,
                                    GLenum format,
                                    GLsizei imageSize,
                                    const GLvoid *data)
{
  assert(glue->glCompressedTexSubImage3D);
  glue->glCompressedTexSubImage3D(target,
                                  level,
                                  xoffset,
                                  yoffset,
                                  zoffset,
                                  width,
                                  height,
                                  depth,
                                  format,
                                  imageSize,
                                  data);
}

void
cc_glglue_glCompressedTexSubImage2D(const cc_glglue * glue,
                                    GLenum target,
                                    GLint level,
                                    GLint xoffset,
                                    GLint yoffset,
                                    GLsizei width,
                                    GLsizei height,
                                    GLenum format,
                                    GLsizei imageSize,
                                    const GLvoid *data)
{
  assert(glue->glCompressedTexSubImage2D);
  glue->glCompressedTexSubImage2D(target,
                                  level,
                                  xoffset,
                                  yoffset,
                                  width,
                                  height,
                                  format,
                                  imageSize,
                                  data);
}

void
cc_glglue_glCompressedTexSubImage1D(const cc_glglue * glue,
                                    GLenum target,
                                    GLint level,
                                    GLint xoffset,
                                    GLsizei width,
                                    GLenum format,
                                    GLsizei imageSize,
                                    const GLvoid *data)
{
  assert(glue->glCompressedTexSubImage1D);
  glue->glCompressedTexSubImage1D(target,
                                  level,
                                  xoffset,
                                  width,
                                  format,
                                  imageSize,
                                  data);
}

void
cc_glglue_glGetCompressedTexImage(const cc_glglue * glue,
                                  GLenum target,
                                  GLint level,
                                  void * img)
{
  assert(glue->glGetCompressedTexImage);
  glue->glGetCompressedTexImage(target,
                                level,
                                img);
}

SbBool
cc_glglue_has_paletted_textures(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;

  return glue->supportsPalettedTextures;
}

SbBool
cc_glglue_has_color_tables(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->glColorTable != NULL;
}

SbBool
cc_glglue_has_color_subtables(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->glColorSubTable != NULL;
}

void
cc_glglue_glColorTable(const cc_glglue * glue,
                       GLenum target,
                       GLenum internalFormat,
                       GLsizei width,
                       GLenum format,
                       GLenum type,
                       const GLvoid *table)
{
  assert(glue->glColorTable);
  glue->glColorTable(target,
                     internalFormat,
                     width,
                     format,
                     type,
                     table);
}

void
cc_glglue_glColorSubTable(const cc_glglue * glue,
                          GLenum target,
                          GLsizei start,
                          GLsizei count,
                          GLenum format,
                          GLenum type,
                          const GLvoid * data)
{
  assert(glue->glColorSubTable);
  glue->glColorSubTable(target,
                        start,
                        count,
                        format,
                        type,
                        data);
}

void
cc_glglue_glGetColorTable(const cc_glglue * glue,
                          GLenum target,
                          GLenum format,
                          GLenum type,
                          GLvoid *data)
{
  assert(glue->glGetColorTable);
  glue->glGetColorTable(target,
                        format,
                        type,
                        data);
}

void
cc_glglue_glGetColorTableParameteriv(const cc_glglue * glue,
                                     GLenum target,
                                     GLenum pname,
                                     GLint *params)
{
  assert(glue->glGetColorTableParameteriv);
  glue->glGetColorTableParameteriv(target,
                                   pname,
                                   params);
}

void
cc_glglue_glGetColorTableParameterfv(const cc_glglue * glue,
                                     GLenum target,
                                     GLenum pname,
                                     GLfloat *params)
{
  assert(glue->glGetColorTableParameterfv);
  glue->glGetColorTableParameterfv(target,
                                   pname,
                                   params);
}

SbBool
cc_glglue_has_blendequation(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;

  return glue->glBlendEquation || glue->glBlendEquationEXT;
}

void
cc_glglue_glBlendEquation(const cc_glglue * glue, GLenum mode)
{
  assert(glue->glBlendEquation || glue->glBlendEquationEXT);

  if (glue->glBlendEquation) glue->glBlendEquation(mode);
  else glue->glBlendEquationEXT(mode);
}

SbBool
cc_glglue_has_vertex_array(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->glVertexPointer != NULL;
}

void
cc_glglue_glVertexPointer(const cc_glglue * glue,
                          GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  assert(glue->glVertexPointer);
  glue->glVertexPointer(size, type, stride, pointer);
}

void
cc_glglue_glTexCoordPointer(const cc_glglue * glue,
                            GLint size, GLenum type,
                            GLsizei stride, const GLvoid * pointer)
{
  assert(glue->glTexCoordPointer);
  glue->glTexCoordPointer(size, type, stride, pointer);
}

void
cc_glglue_glNormalPointer(const cc_glglue * glue,
                          GLenum type, GLsizei stride, const GLvoid *pointer)
{
  assert(glue->glNormalPointer);
  glue->glNormalPointer(type, stride, pointer);
}

void
cc_glglue_glColorPointer(const cc_glglue * glue,
                         GLint size, GLenum type,
                         GLsizei stride, const GLvoid * pointer)
{
  assert(glue->glColorPointer);
  glue->glColorPointer(size, type, stride, pointer);
}

void
cc_glglue_glIndexPointer(const cc_glglue * glue,
                         GLenum type, GLsizei stride, const GLvoid * pointer)
{
  assert(glue->glIndexPointer);
  glue->glIndexPointer(type, stride, pointer);
}

void
cc_glglue_glEnableClientState(const cc_glglue * glue, GLenum array)
{
  assert(glue->glEnableClientState);
  glue->glEnableClientState(array);
}

void
cc_glglue_glDisableClientState(const cc_glglue * glue, GLenum array)
{
  assert(glue->glDisableClientState);
  glue->glDisableClientState(array);
}

void
cc_glglue_glInterleavedArrays(const cc_glglue * glue,
                              GLenum format, GLsizei stride, const GLvoid * pointer)
{
  assert(glue->glInterleavedArrays);
  glue->glInterleavedArrays(format, stride, pointer);
}

void
cc_glglue_glDrawArrays(const cc_glglue * glue,
                       GLenum mode, GLint first, GLsizei count)
{
  assert(glue->glDrawArrays);
  glue->glDrawArrays(mode, first, count);
}

void
cc_glglue_glDrawElements(const cc_glglue * glue,
                         GLenum mode, GLsizei count, GLenum type,
                         const GLvoid * indices)
{
  assert(glue->glDrawElements);
  glue->glDrawElements(mode, count, type, indices);
}

void
cc_glglue_glDrawRangeElements(const cc_glglue * glue,
                              GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type,
                              const GLvoid * indices)
{
  assert(glue->glDrawRangeElements);
  glue->glDrawRangeElements(mode, start, end, count, type, indices);
}

void
cc_glglue_glArrayElement(const cc_glglue * glue, GLint i)
{
  assert(glue->glArrayElement);
  glue->glArrayElement(i);
}

SbBool
cc_glglue_has_multitexture_vertex_array(const cc_glglue * glue)
{
  return glue->glMultiDrawArrays && glue->glMultiDrawElements;
}

void
cc_glglue_glMultiDrawArrays(const cc_glglue * glue, GLenum mode, const GLint * first,
                            const GLsizei * count, GLsizei primcount)
{
  assert(glue->glMultiDrawArrays);
  glue->glMultiDrawArrays(mode, first, count, primcount);
}

void
cc_glglue_glMultiDrawElements(const cc_glglue * glue, GLenum mode, const GLsizei * count,
                              GLenum type, const GLvoid ** indices, GLsizei primcount)
{
  assert(glue->glMultiDrawElements);
  glue->glMultiDrawElements(mode, count, type, indices, primcount);
}

SbBool
cc_glglue_has_nv_vertex_array_range(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->glVertexArrayRangeNV != NULL;
}

void
cc_glglue_glFlushVertexArrayRangeNV(const cc_glglue * glue)
{
  assert(glue->glFlushVertexArrayRangeNV);
  glue->glFlushVertexArrayRangeNV();
}

void
cc_glglue_glVertexArrayRangeNV(const cc_glglue * glue, GLsizei size, const GLvoid * pointer)
{
  assert(glue->glVertexArrayRangeNV);
  glue->glVertexArrayRangeNV(size, pointer);
}

void *
cc_glglue_glAllocateMemoryNV(const cc_glglue * glue,
                             GLsizei size, GLfloat readfreq,
                             GLfloat writefreq, GLfloat priority)
{
  assert(glue->glAllocateMemoryNV);
  return glue->glAllocateMemoryNV(size, readfreq, writefreq, priority);
}

void
cc_glglue_glFreeMemoryNV(const cc_glglue * glue, GLvoid * buffer)
{
  assert(glue->glFreeMemoryNV);
  glue->glFreeMemoryNV(buffer);
}

SbBool
cc_glglue_has_vertex_buffer_object(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;

  /* check only one function for speed. It's set to NULL when
     initializing if one of the other functions wasn't found */
  return glue->glBindBuffer != NULL;
}

void
cc_glglue_glBindBuffer(const cc_glglue * glue, GLenum target, GLuint buffer)
{
  assert(glue->glBindBuffer);
  glue->glBindBuffer(target, buffer);
}

void
cc_glglue_glDeleteBuffers(const cc_glglue * glue, GLsizei n, const GLuint *buffers)
{
  assert(glue->glDeleteBuffers);
  glue->glDeleteBuffers(n, buffers);
}

void
cc_glglue_glGenBuffers(const cc_glglue * glue, GLsizei n, GLuint *buffers)
{
  assert(glue->glGenBuffers);
  glue->glGenBuffers(n, buffers);
}

GLboolean
cc_glglue_glIsBuffer(const cc_glglue * glue, GLuint buffer)
{
  assert(glue->glIsBuffer);
  return glue->glIsBuffer(buffer);
}

void
cc_glglue_glBufferData(const cc_glglue * glue,
                       GLenum target,
                       intptr_t size, /* 64 bit on 64 bit systems */
                       const GLvoid *data,
                       GLenum usage)
{
  assert(glue->glBufferData);
  glue->glBufferData(target, size, data, usage);
}

void
cc_glglue_glBufferSubData(const cc_glglue * glue,
                          GLenum target,
                          intptr_t offset, /* 64 bit */
                          intptr_t size, /* 64 bit */
                          const GLvoid * data)
{
  assert(glue->glBufferSubData);
  glue->glBufferSubData(target, offset, size, data);
}

void
cc_glglue_glGetBufferSubData(const cc_glglue * glue,
                             GLenum target,
                             intptr_t offset, /* 64 bit */
                             intptr_t size, /* 64 bit */
                             GLvoid *data)
{
  assert(glue->glGetBufferSubData);
  glue->glGetBufferSubData(target, offset, size, data);
}

GLvoid *
cc_glglue_glMapBuffer(const cc_glglue * glue,
                      GLenum target, GLenum access)
{
  assert(glue->glMapBuffer);
  return glue->glMapBuffer(target, access);
}

GLboolean
cc_glglue_glUnmapBuffer(const cc_glglue * glue,
                        GLenum target)
{
  assert(glue->glUnmapBuffer);
  return glue->glUnmapBuffer(target);
}

void
cc_glglue_glGetBufferParameteriv(const cc_glglue * glue,
                                 GLenum target,
                                 GLenum pname,
                                 GLint * params)
{
  assert(glue->glGetBufferParameteriv);
  glue->glGetBufferParameteriv(target, pname, params);
}

void
cc_glglue_glGetBufferPointerv(const cc_glglue * glue,
                              GLenum target,
                              GLenum pname,
                              GLvoid ** params)
{
  assert(glue->glGetBufferPointerv);
  glue->glGetBufferPointerv(target, pname, params);
}


SbBool
cc_glglue_can_do_bumpmapping(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->can_do_bumpmapping;
}

SbBool
cc_glglue_can_do_sortedlayersblend(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->can_do_sortedlayersblend;
}

int 
cc_glglue_get_max_lights(const cc_glglue * glue)
{
  return glue->max_lights;
}

const float * 
cc_glglue_get_line_width_range(const cc_glglue * glue)
{
  return glue->line_width_range;
}

const float * 
cc_glglue_get_point_size_range(const cc_glglue * glue)
{
  return glue->point_size_range;
}

/* GL_NV_register_combiners functions */
SbBool
cc_glglue_has_nv_register_combiners(const cc_glglue * glue)
{  
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_nv_register_combiners;
}

void 
cc_glglue_glCombinerParameterfvNV(const cc_glglue * glue,
                                  GLenum pname,
                                  const GLfloat *params)
{
  glue->glCombinerParameterfvNV(pname, params);
}

void 
cc_glglue_glCombinerParameterivNV(const cc_glglue * glue,
                                  GLenum pname,
                                  const GLint *params)
{
  glue->glCombinerParameterivNV(pname, params);
}

void 
cc_glglue_glCombinerParameterfNV(const cc_glglue * glue,
                                 GLenum pname,
                                 GLfloat param)
{
  glue->glCombinerParameterfNV(pname, param);
}

void 
cc_glglue_glCombinerParameteriNV(const cc_glglue * glue,
                                 GLenum pname,
                                 GLint param)
{
  glue->glCombinerParameteriNV(pname, param);
}

void 
cc_glglue_glCombinerInputNV(const cc_glglue * glue,
                            GLenum stage,
                            GLenum portion,
                            GLenum variable,
                            GLenum input,
                            GLenum mapping,
                            GLenum componentUsage)
{
  glue->glCombinerInputNV(stage, portion, variable, input, mapping, componentUsage);
}

void 
cc_glglue_glCombinerOutputNV(const cc_glglue * glue,
                             GLenum stage,
                             GLenum portion, 
                             GLenum abOutput,
                             GLenum cdOutput,
                             GLenum sumOutput,
                             GLenum scale,
                             GLenum bias,
                             GLboolean abDotProduct,
                             GLboolean cdDotProduct,
                             GLboolean muxSum)
{
  glue->glCombinerOutputNV(stage, portion, abOutput, cdOutput, sumOutput, scale, bias,
                           abDotProduct, cdDotProduct, muxSum);
}

void 
cc_glglue_glFinalCombinerInputNV(const cc_glglue * glue,
                                 GLenum variable,
                                 GLenum input,
                                 GLenum mapping,
                                 GLenum componentUsage)
{
  glue->glFinalCombinerInputNV(variable, input, mapping, componentUsage);
}

void 
cc_glglue_glGetCombinerInputParameterfvNV(const cc_glglue * glue,
                                          GLenum stage,
                                          GLenum portion,
                                          GLenum variable,
                                          GLenum pname,
                                          GLfloat *params)
{
  glue->glGetCombinerInputParameterfvNV(stage, portion, variable, pname, params);
}

void 
cc_glglue_glGetCombinerInputParameterivNV(const cc_glglue * glue,
                                          GLenum stage,
                                          GLenum portion,
                                          GLenum variable,
                                          GLenum pname,
                                          GLint *params)
{
  glue->glGetCombinerInputParameterivNV(stage, portion, variable, pname, params);
}

void 
cc_glglue_glGetCombinerOutputParameterfvNV(const cc_glglue * glue,
                                           GLenum stage,
                                           GLenum portion, 
                                           GLenum pname,
                                           GLfloat *params)
{
  glue->glGetCombinerOutputParameterfvNV(stage, portion, pname, params);
}

void 
cc_glglue_glGetCombinerOutputParameterivNV(const cc_glglue * glue,
                                           GLenum stage,
                                           GLenum portion, 
                                           GLenum pname,
                                           GLint *params)
{
  glue->glGetCombinerOutputParameterivNV(stage, portion, pname, params);
}

void 
cc_glglue_glGetFinalCombinerInputParameterfvNV(const cc_glglue * glue,
                                               GLenum variable,
                                               GLenum pname,
                                               GLfloat *params)
{
  glue->glGetFinalCombinerInputParameterfvNV(variable, pname, params);
}

void 
cc_glglue_glGetFinalCombinerInputParameterivNV(const cc_glglue * glue,
                                               GLenum variable,
                                               GLenum pname,
                                               GLint *params)
{
  glue->glGetFinalCombinerInputParameterivNV(variable, pname, params);
}


/* GL_NV_texture_rectangle (identical to GL_EXT_texture_rectangle) */
SbBool 
cc_glglue_has_nv_texture_rectangle(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_ext_texture_rectangle;
}

/* GL_EXT_texture_rectangle */
SbBool 
cc_glglue_has_ext_texture_rectangle(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_ext_texture_rectangle;
}

/* GL_NV_texture_shader */
SbBool
cc_glglue_has_nv_texture_shader(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_nv_texture_shader;
}

/* GL_ARB_shadow */
SbBool 
cc_glglue_has_arb_shadow(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_shadow;
}

/* GL_ARB_depth_texture */
SbBool
cc_glglue_has_arb_depth_texture(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_depth_texture;
}

/* GL_ARB_fragment_program */
SbBool
cc_glglue_has_arb_fragment_program(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_arb_fragment_program;
}

/* GL_EXT_texture_env_combine || GL_ARB_texture_env_combine || OGL 1.4 */
SbBool 
cc_glglue_has_texture_env_combine(const cc_glglue * glue)
{
  if (!glglue_allow_newer_opengl(glue)) return FALSE;
  return glue->has_texture_env_combine;
}

/*!
  Returns current X11 display the OpenGL context is in. If none, or if
  the glXGetCurrentDisplay() method is not available (it was
  introduced with GLX 1.3), returns \c NULL.
*/
void *
cc_glglue_glXGetCurrentDisplay(const cc_glglue * w)
{
  return w->glx.glXGetCurrentDisplay ? w->glx.glXGetCurrentDisplay() : NULL;
}

/*** Offscreen buffer handling. *********************************************/

/*
  Below is a stand-alone example that can be compiled and linked with
  the Coin library for testing that the context handling interface
  works:
 */
/*
  #include <Inventor/C/glue/gl.h>
  #include <assert.h>
  #include <stdio.h>

  int
  main(void)
  {
    void * ctx = cc_glglue_context_create_offscreen(128, 128);
    assert(ctx);
    SbBool ok = cc_glglue_context_make_current(ctx);
    assert(ok);

    const GLubyte * str = glGetString(GL_VERSION);
    assert(str && "could not call glGetString() -- no current GL context?");
    assert(glGetError() == GL_NO_ERROR && "GL error when calling glGetString() -- no current GL context?");

    (void)fprintf(stdout, "glGetString(GL_VERSION)=='%s'\n", str);
    (void)fprintf(stdout, "glGetString(GL_VENDOR)=='%s'\n", glGetString(GL_VENDOR));
    (void)fprintf(stdout, "glGetString(GL_RENDERER)=='%s'\n", glGetString(GL_RENDERER));

    (void)fprintf(stdout, "glGenTextures=='%p'\n",
                  cc_glglue_getprocaddress("glGenTextures"));

    (void)fprintf(stdout, "glGenTexturesEXT=='%p'\n",
                  cc_glglue_getprocaddress("glGenTexturesEXT"));

    cc_glglue_context_reinstate_previous(ctx);
    cc_glglue_context_destruct(ctx);
    return 0;
  }
*/

void *
cc_glglue_context_create_offscreen(unsigned int width, unsigned int height)
{
#ifdef HAVE_GLX
  return glxglue_context_create_offscreen(width, height);
#elif defined(HAVE_AGL)
  return aglglue_context_create_offscreen(width, height);
#elif defined(HAVE_WGL)
  return wglglue_context_create_offscreen(width, height);
#else
  assert(FALSE && "unimplemented");
  return NULL;
#endif
}

SbBool
cc_glglue_context_make_current(void * ctx)
{
#ifdef HAVE_GLX
  return glxglue_context_make_current(ctx);
#elif defined(HAVE_AGL)
  return aglglue_context_make_current(ctx);
#elif defined(HAVE_WGL)
  return wglglue_context_make_current(ctx);
#else
  assert(FALSE && "unimplemented");
  return FALSE;
#endif
}

void
cc_glglue_context_reinstate_previous(void * ctx)
{
#ifdef HAVE_GLX
  glxglue_context_reinstate_previous(ctx);
#elif defined(HAVE_AGL)
  aglglue_context_reinstate_previous(ctx);
#elif defined(HAVE_WGL)
  wglglue_context_reinstate_previous(ctx);
#else
  assert(FALSE && "unimplemented");
#endif
}

void
cc_glglue_context_destruct(void * ctx)
{
#ifdef HAVE_GLX
  glxglue_context_destruct(ctx);
#elif defined(HAVE_AGL)
  aglglue_context_destruct(ctx);
#elif defined(HAVE_WGL)
  wglglue_context_destruct(ctx);
#else
  assert(FALSE && "unimplemented");
#endif
}

void
cc_glglue_context_max_dimensions(unsigned int * width, unsigned int * height)
{
  void * ctx;
  SbBool ok;
  const char * vendor;
  GLint size[2];
  static unsigned int dim[2] = { 0, 0 };

  if (dim[0] > 0) { /* value cached */
    *width = dim[0];
    *height = dim[1];
    return;
  }


  /* FIXME: the below calls *can* fail, due to e.g. lack of resources,
     or no usable visual for the GL context. Should handle
     gracefully. (Which is straightforward to do here, simply return
     dimensions of [0,0], but we also need to handle the exception in
     the callers.) 20031202 mortene. */

  ctx = cc_glglue_context_create_offscreen(32, 32);
  assert(ctx);
  ok = cc_glglue_context_make_current(ctx);
  assert(ok);

  glGetIntegerv(GL_MAX_VIEWPORT_DIMS, size);

  vendor = (const char *)glGetString(GL_VENDOR);
  if (strcmp(vendor, "NVIDIA Corporation") == 0) {

    /* NVIDIA seems to have a bug where max render size is limited
       by desktop resolution (at least for their Linux X11
       drivers), not the texture maxsize returned by OpenGL. So we
       use a workaround by limiting max size to the lowend
       resolution for desktop monitors.

       According to pederb, there are versions of the NVidia
       drivers where the offscreen buffer also has to have
       dimensions that are 2^x, so we limit further down to these
       dimension settings to be sure.
    */

    /* FIXME: should make a stand-alone test-case (not dependent
       on Coin, only GL, GLX & X11) that demonstrates this problem
       for a) submitting to <linux-bugs@nvidia.com>, and b) to
       test which versions of the NVidia drivers are affected --
       as it is now, we shrink the max to <512,512> on all
       versions (even if we're under MSWin). 20030812 mortene.
    */

    size[0] = cc_min(size[0], 512);
    size[1] = cc_min(size[1], 512);
  }

  cc_glglue_context_reinstate_previous(ctx);
  cc_glglue_context_destruct(ctx);

  /* FIXME: if we're on e.g. GLX and are going to use pbuffers, we
     should check the GLX_MAX_PBUFFER_WIDTH, GLX_MAX_PBUFFER_HEIGHT
     and GLX_MAX_PBUFFER_PIXELS values to see if they limit us
     further.

     Similar limits probably also exists for WGL and AGL pbuffers.

     20030812 mortene.
  */

  *width = (unsigned int) size[0];
  *height = (unsigned int) size[1];

  /* FIXME: increase the below limit somewhat (to e.g. 2048x2048)
     after implementing the check for pbuffer limits, as described in
     the above FIXME note. 1024x1024 is really too small -- it will
     cause unnecessary additional rendering passes for most (?) usage,
     which causes a big performance hit. 20031202 mortene. */

  /* Limit the maximum tilesize to 1024x1024 pixels.
  
     This is done to work around a problem with some OpenGL drivers: a
     huge value is returned for the maximum offscreen OpenGL canvas,
     where the driver obviously does not take into account the amount
     of memory needed to actually allocate such a large buffer.
  
     This problem has at least been observed with the MS Windows XP
     software OpenGL renderer, which reports a maximum viewport size
     of 16k x 16k pixels.
  */
  *width = cc_min(*width, 1024);
  *height = cc_min(*height, 1024);


  if (coin_glglue_debug()) {
    cc_debugerror_postinfo("cc_glglue_context_max_dimensions",
                           "max dimensions==<%d, %d>", width, height);
  }

  /* cache values for next invocation */

  dim[0] = *width;
  dim[1] = *height;
}

SbBool
cc_glglue_context_can_render_to_texture(void * ctx)
{
#if defined(HAVE_AGL)
  return aglglue_context_can_render_to_texture(ctx);
#elif defined(HAVE_WGL)
  return wglglue_context_can_render_to_texture(ctx);
#endif
  /* GLX */
  return FALSE; 
}


void
cc_glglue_context_bind_pbuffer(void * ctx)
{
/* FIXME: Implement for GLX. kyrah 20031123. */
#if defined(HAVE_AGL)
  aglglue_context_bind_pbuffer(ctx);
#elif defined(HAVE_WGL)
  wglglue_context_bind_pbuffer(ctx);
#else
  assert(FALSE && "unimplemented");
#endif
}

void
cc_glglue_context_release_pbuffer(void * ctx)
{
  /* FIXME: Implement for GLX. kyrah 20031123. */
#if defined(HAVE_AGL)
  aglglue_context_release_pbuffer(ctx);
#elif defined(HAVE_WGL)
  wglglue_context_release_pbuffer(ctx);
#else
  assert(FALSE && "unimplemented");
#endif
}

SbBool
cc_glglue_context_pbuffer_is_bound(void * ctx)
{
  /* FIXME: Implement for GLX. kyrah 20031123. */
#if defined(HAVE_AGL)
  return aglglue_context_pbuffer_is_bound(ctx);
#elif defined(HAVE_WGL)
  return wglglue_context_pbuffer_is_bound(ctx);
#else
  assert(FALSE && "unimplemented");
  return FALSE;
#endif
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/*** </Offscreen buffer handling.> ******************************************/
