// @HEADER
// ***********************************************************************
//
//          Tpetra: Templated Linear Algebra Services Package
//                 Copyright (2008) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
// @HEADER

#include "Tpetra_Details_idot.hpp"
#ifdef HAVE_TPETRACORE_MPI
#  include "Teuchos_DefaultMpiComm.hpp"
#else
#  include "Teuchos_DefaultSerialComm.hpp"
#endif // HAVE_TPETRACORE_MPI
#include "Tpetra_TestingUtilities.hpp"
#include "Tpetra_Map.hpp"

namespace { // (anonymous)

using Teuchos::Comm;
using Teuchos::outArg;
using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::REDUCE_MIN;
using Teuchos::reduceAll;
using std::endl;

typedef Tpetra::Map<> map_type;
typedef Tpetra::MultiVector<> mv_type;
typedef Tpetra::Vector<> vec_type;
typedef vec_type::scalar_type SC;
typedef vec_type::mag_type mag_type;
typedef map_type::local_ordinal_type LO;
typedef map_type::global_ordinal_type GO;
typedef Teuchos::ScalarTraits<SC> STS;

/// \brief Test Tpetra::Details::idot.
///
/// \param out [out] Output stream; valid (writeable) only on Process
///   0 in the input communicator.
/// \param comm [in] Communicator over which to do the test.
void
testIdot (bool& success,
          std::ostream& out,
          const RCP<const Comm<int> >& comm)
{
  const SC ZERO = STS::zero ();
  const SC TWO = STS::one () + STS::one ();
  const SC THREE = TWO + STS::one ();
  const int numProcs = comm->getSize ();

  // lclSuccess: Local success status.  0 means a failure happened on
  //   the calling process.
  // gblSuccess [in/out] Global success status.  0 means a failure
  //   happened on some process in the input communicator.
  int lclSuccess = 1; // to be updated below
  int gblSuccess = 0; // output argument (see below)

  const LO lclNumRows = 10;
  const GO gblNumRows =
    static_cast<GO> (lclNumRows) * static_cast<GO> (numProcs);
  const GO indexBase = 0;
  RCP<const map_type> map =
    rcp (new map_type (gblNumRows, lclNumRows, indexBase, comm));

  out << "Test Tpetra::Vector inputs and raw pointer output" << endl;
  {
    vec_type x (map);
    vec_type y (map);
    const SC valX = TWO;
    const SC valY = THREE;
    x.putScalar (valX);
    y.putScalar (valY);

    SC result = ZERO;
    auto req = Tpetra::Details::idot (&result, x, y);
    req->wait ();
    const SC N = static_cast<SC> (static_cast<mag_type> (gblNumRows));
    const SC expectedResult = N * valX * valY;

    TEST_EQUALITY( expectedResult, result );

    lclSuccess = success ? 1 : 0; // input argument
    gblSuccess = 0; // output argument
    reduceAll<int, int> (*comm, REDUCE_MIN, lclSuccess, outArg (gblSuccess));
    TEST_EQUALITY( gblSuccess, 1 );
    success = (gblSuccess != 0);
  }

  out << "Test Tpetra::Vector inputs and rank-0 Kokkos::View output" << endl;
  {
    vec_type x (map);
    vec_type y (map);
    const SC valX = TWO;
    const SC valY = THREE;
    x.putScalar (valX);
    y.putScalar (valY);

    Kokkos::View<vec_type::dot_type, vec_type::device_type> result ("result");
    auto result_h = Kokkos::create_mirror_view (result);
    result_h() = ZERO;
    Kokkos::deep_copy (result, result_h);

    auto req = Tpetra::Details::idot (result, x, y);
    req->wait ();
    const SC N = static_cast<SC> (static_cast<mag_type> (gblNumRows));
    const SC expectedResult = N * valX * valY;

    Kokkos::deep_copy (result_h, result);

    TEST_EQUALITY( expectedResult, result_h() );

    lclSuccess = success ? 1 : 0; // input argument
    gblSuccess = 0; // output argument
    reduceAll<int, int> (*comm, REDUCE_MIN, lclSuccess, outArg (gblSuccess));
    TEST_EQUALITY( gblSuccess, 1 );
    success = (gblSuccess != 0);
  }

  out << "Test Tpetra::MultiVector inputs and raw pointer output" << endl;
  {
    constexpr size_t numVecs = 3;
    mv_type x (map, numVecs);
    mv_type y (map, numVecs);
    const SC valX = TWO;
    const SC valY = THREE;
    x.putScalar (valX);
    y.putScalar (valY);

    SC results[numVecs];
    for (size_t k = 0; k < numVecs; ++k) {
      results[k] = ZERO;
    }
    auto req = Tpetra::Details::idot (results, x, y);
    req->wait ();
    const SC N = static_cast<SC> (static_cast<mag_type> (gblNumRows));
    const SC expectedResult = N * valX * valY;

    for (size_t k = 0; k < numVecs; ++k) {
      TEST_EQUALITY( expectedResult, results[k] );
    }

    lclSuccess = success ? 1 : 0; // input argument
    gblSuccess = 0; // output argument
    reduceAll<int, int> (*comm, REDUCE_MIN, lclSuccess, outArg (gblSuccess));
    TEST_EQUALITY( gblSuccess, 1 );
    success = (gblSuccess != 0);
  }
}


TEUCHOS_UNIT_TEST( idot, basic )
{
  out << "Testing Tpetra::Details::idot" << endl;
  Teuchos::OSTab tab1 (out);

#ifdef HAVE_TPETRACORE_MPI
  RCP<const Comm<int> > comm = rcp (new Teuchos::MpiComm<int> (MPI_COMM_WORLD));
#else
  RCP<const Comm<int> > comm = rcp (new Teuchos::SerialComm<int> ());
#endif // HAVE_TPETRACORE_MPI

  testIdot (success, std::cerr, comm);
  // Just to make sure that if the test fails, we still print
  // something that the unit test framework recognizes.
  TEST_ASSERT( success );
}

} // namespace (anonymous)
