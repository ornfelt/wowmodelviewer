#ifndef PARTICLE_H
#define PARTICLE_H

#include <ostream> // iostream or <ostream> for std::ostream and std::endl

#ifdef _WIN32
#    ifdef BUILDING_WOW_DLL
#        define _PARTICLE_API_ __declspec(dllexport)
#    else
#        define _PARTICLE_API_ __declspec(dllimport)
#    endif
#else
#    define _PARTICLE_API_
#endif

class WoWModel;
class ParticleSystem;
class RibbonEmitter;

#include "animated.h"

#include <list>
#include "GL/glew.h"
#include "glm/glm.hpp"

class Bone;

struct Particle
{
  glm::vec3 pos, speed, down, origin, dir;
  glm::vec3 corners[4];
  glm::vec3 tpos;
  float size, life, maxlife;
  size_t tile;
  glm::vec4 color;
};

typedef std::list<Particle> ParticleList;

class ParticleEmitter
{
protected:
  ParticleSystem *sys;
public:
  ParticleEmitter(ParticleSystem *sys): sys(sys) {}
  virtual Particle newParticle(size_t anim, size_t time, float w, float l, float spd, float var, float spr, float spr2) = 0;
  virtual ~ParticleEmitter() {}
};

class PlaneParticleEmitter: public ParticleEmitter
{
public:
  PlaneParticleEmitter(ParticleSystem *sys): ParticleEmitter(sys) {}
  Particle newParticle(size_t anim, size_t time, float w, float l, float spd, float var, float spr, float spr2);
};

class SphereParticleEmitter: public ParticleEmitter
{
public:
  SphereParticleEmitter(ParticleSystem *sys): ParticleEmitter(sys) {}
  Particle newParticle(size_t anim, size_t time, float w, float l, float spd, float var, float spr, float spr2);
};

struct TexCoordSet
{
  glm::vec2 tc[4];
};

class _PARTICLE_API_ ParticleSystem
{
  float mid, slowdown, rotation;
  glm::vec3 pos, tpos;
  GLuint texture, texture2, texture3;
  ParticleEmitter *emitter;
  ParticleList particles;
  int order, ParticleType;
  size_t manim, mtime;
  int rows, cols;
  std::vector<TexCoordSet> tiles;
  void initTile(glm::vec2 *tc, int num);
  bool billboard;
  float rem;
  //bool transform;
  // unknown parameters omitted for now ...
  int32 flags;
  int16 EmitterType;
  Bone *parent;

public:
  int blend;
  WoWModel *model;
  float tofs;
  Animated<uint16> enabled;
  Animated<float> speed, variation, spread, lat, gravity, lifespan, rate, areal, areaw;
  //Animated<float>  deacceleration;
  glm::vec4 colors[3];
  float sizes[3];
  bool multitexture, doNotTrail;
  int particleColID;
  bool replaceParticleColors;
  // Start, Mid and End colours, for cases where the model's particle colours
  // are overridden by values from ParticleColor.dbc, indexed from CreatureDisplayInfo:
  typedef std::vector<glm::vec4> particleColorSet;
  // The particle will get its replacement colour set from 0, 1 or 2, depending on
  // whether its ParticleColorIndex is set to 11, 12 or 13:
  std::vector<particleColorSet> particleColorReplacements;

  ParticleSystem(): mid(0), emitter(0), rem(0)
  {
    multitexture = 0;
    particleColID = 0;
    replaceParticleColors = false;
    doNotTrail = false;
    blend = 0;
    order = 0;
    ParticleType = 0;
    manim = 0;
    mtime = 0;
    rows = 0;
    cols = 0;
    model = 0;
    parent = 0;
    texture = 0;
    texture2 = 0;
    texture3 = 0;
    slowdown = 0;
    rotation = 0;
    tofs = 0;
  }
  ~ParticleSystem() { delete emitter; }

  void init(GameFile * f, M2ParticleDef &mta, std::vector<uint32> & globals);
  void update(float dt);

  void setup(size_t anim, size_t time);
  void draw();

  friend class PlaneParticleEmitter;
  friend class SphereParticleEmitter;
  int BlendValueForMode(int mode);
  friend std::ostream& operator<<(std::ostream& out, const ParticleSystem& v)
  {
    out << "    <colors>" << v.colors[0].x << " " << v.colors[0].y << " " << v.colors[0].z << "</colors>" << std::endl;
    out << "    <colors>" << v.colors[1].x << " " << v.colors[1].y << " " << v.colors[1].z << "</colors>" << std::endl;
    out << "    <colors>" << v.colors[2].x << " " << v.colors[2].y << " " << v.colors[2].z << "</colors>" << std::endl;
    out << "    <sizes>" << v.sizes[0] << "</sizes>" << std::endl;
    out << "    <sizes>" << v.sizes[1] << "</sizes>" << std::endl;
    out << "    <sizes>" << v.sizes[2] << "</sizes>" << std::endl;
    out << "    <mid>" << v.mid << "</mid>" << std::endl;
    out << "    <slowdown>" << v.slowdown << "</slowdown>" << std::endl;
    out << "    <rotation>" << v.rotation << "</rotation>" << std::endl;
    out << "    <pos>" << v.pos.x << " " << v.pos.y << " " << v.pos.z << "</pos>" << std::endl;
    out << "    <texture>" << v.texture << "</texture>" << std::endl;
    out << "    <blend>" << v.blend << "</blend>" << std::endl;
    out << "    <order>" << v.order << "</order>" << std::endl;
    out << "    <ParticleType>" << v.ParticleType << "</ParticleType>" << std::endl;
    out << "    <manim>" << v.manim << "</manim>" << std::endl;
    out << "    <mtime>" << v.mtime << "</mtime>" << std::endl;
    out << "    <rows>" << v.rows << "</rows>" << std::endl;
    out << "    <cols>" << v.cols << "</cols>" << std::endl;
    out << "    <billboard>" << v.billboard << "</billboard>" << std::endl;
    out << "    <rem>" << v.rem << "</rem>" << std::endl;
    out << "    <flags>" << v.flags << "</flags>" << std::endl;
    out << "    <EmitterType>" << v.EmitterType << "</EmitterType>" << std::endl;
    out << "    <tofs>" << v.tofs << "</tofs>" << std::endl;
    return out;
  }

  static void useDoNotTrailInfo()
  {
    ParticleSystem::useDoNotTrail = true;
  }

  static bool useDoNotTrail;
};


struct RibbonSegment
{
  glm::vec3 pos, up, back;
  float len,len0;
};

class RibbonEmitter
{
  Animated<glm::vec3> color;
  AnimatedShort opacity;
  Animated<float> above, below;

  Bone *parent;

  glm::vec3 pos;

  size_t manim, mtime;
  float length, seglen;
  int numsegs;

  glm::vec3 tpos;
  glm::vec4 tcolor;
  float tabove, tbelow;

  GLuint texture;

  std::list<RibbonSegment> segs;

public:
  WoWModel *model;

  void init(GameFile * f, ModelRibbonEmitterDef &mta, std::vector<uint32> & globals);
  void setup(size_t anim, size_t time);
  void draw();
};



#endif
