// ::gyronimo:: - gyromotion for the people, by the people -
// An object-oriented library for gyromotion applications in plasma physics.
// Copyright (C) 2021 Paulo Rodrigues, Rogerio Jorge.

#ifndef EQUILIBRIUM_STELLNA_QS
#define EQUILIBRIUM_STELLNA_QS

#include <gyronimo/fields/IR3field_c1.hh>
#include <metric_stellna_qs.hh>
using namespace gyronimo;

//! Quasi-symmetric stellarator equilibrium field in near-axis coordinates.
/*!
    Following IR3Field rules, the magnetic field is normalised by a `m_factor`
    matching its reference value `Bref` in [T]. In turn, 'axis_length()'
    is in [m]. The coordinates are set by the `metric_stellna` object and the
    contravariant field components have dimensions of [m^{-1}]. Being an
    **equilibrium** field, `t_factor` is set to one.
*/
class equilibrium_stellna_qs : public IR3field_c1{
 public:
  equilibrium_stellna_qs(const metric_stellna_qs *g);
  virtual ~equilibrium_stellna_qs() override {};

  virtual IR3 contravariant(const IR3& position, double time) const override;
  virtual dIR3 del_contravariant(const IR3& position, double time) const override;
  virtual IR3 partial_t_contravariant(const IR3& position, double time) const override {return {0.0,0.0,0.0};};
  virtual IR3 covariant(const IR3& position, double time) const override;
  virtual dIR3 del_covariant(const IR3& position, double time) const override;
  virtual IR3 partial_t_covariant(const IR3& position, double time) const override {return {0.0,0.0,0.0};};
  virtual double magnitude(const IR3& position, double time) const override;
  virtual IR3 del_magnitude(const IR3& position, double time) const override;

  private:
    const metric_stellna_qs *metric_;
};

#endif