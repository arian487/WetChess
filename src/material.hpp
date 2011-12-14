/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;

protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;

  Colour m_kd;
  Colour m_ks;
  Colour old_mkd;

private:

  double m_shininess;
};


#endif
