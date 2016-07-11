// @HEADER
// ************************************************************************
//
//               Rapid Optimization Library (ROL) Package
//                 Copyright (2014) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact lead developers:
//              Drew Kouri   (dpkouri@sandia.gov) and
//              Denis Ridzal (dridzal@sandia.gov)
//
// ************************************************************************
// @HEADER

#ifndef ROL_QUADRATURE1D_HPP
#define ROL_QUADRATURE1D_HPP

namespace ROL {

template<class Real>
class Quadrature1D {
private:
  std::vector<Real> pts_;
  std::vector<Real> wts_;
  
protected:
  void set(const std::vector<Real> &pts, const std::vector<Real> &wts) {
    pts_.clear(); pts_.assign(pts.begin(),pts.end());
    wts_.clear(); wts_.assign(wts.begin(),wts.end());
  }

public:
  virtual ~Quadrature1D() {}
  Quadrature1D() {}

  void get(std::vector<Real> &pts, std::vector<Real> &wts) const {
    pts.clear(); pts.assign(pts_.begin(),pts_.end());
    wts.clear(); wts.assign(wts_.begin(),wts_.end());
  }

  void get(Teuchos::Array<Real> &pts, Teuchos::Array<Real> &wts) const {
    pts.clear(); pts.assign(pts_.begin(),pts_.end());
    wts.clear(); wts.assign(wts_.begin(),wts_.end());
  }

  virtual std::vector<std::vector<Real> > test(const bool printToStream = true,
                                               std::ostream &outStream = std::cout) const = 0;
};

}

#endif