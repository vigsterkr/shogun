/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soumyajit De, Pan Deng, Bjoern Esser, Viktor Gal
 */
#include <gtest/gtest.h>

#include <shogun/lib/common.h>

#include <shogun/lib/SGVector.h>
#include <shogun/lib/SGSparseMatrix.h>
#include <shogun/mathematics/Math.h>
#include <shogun/mathematics/eigen3.h>
#include <shogun/mathematics/linalg/linop/SparseMatrixOperator.h>
#include <shogun/mathematics/linalg/linsolver/ConjugateOrthogonalCGSolver.h>

using namespace shogun;
using namespace Eigen;

TEST(ConjugateOrthogonalCGSolver, solve)
{
	const int32_t size=10;
	SGSparseMatrix<complex128_t> m(size, size);
	auto A=std::make_shared<SparseMatrixOperator<complex128_t>>(m);

	// diagonal non-Hermintian matrix with random complex entries
	SGVector<complex128_t> diag(size);
	sg_rand->set_seed(100.0);
	for (index_t i=0; i<size; ++i)
	{
		float64_t real=sg_rand->std_normal_distrib();
		float64_t imag=sg_rand->std_normal_distrib();
		diag[i]=complex128_t(real, imag);
	}
	A->set_diagonal(diag);

	// vector b of the system
	SGVector<float64_t> b(size);
	for (index_t i=0; i<size; ++i)
		b[i]=sg_rand->std_normal_distrib();

	// Solve with COCG
	auto cocg_linear_solver
		=std::make_shared<ConjugateOrthogonalCGSolver>();
	const SGVector<complex128_t>& x=cocg_linear_solver->solve(A, b);

	const SGVector<complex128_t>& Ax=A->apply(x);

	Map<VectorXd> map_b(b.vector, b.vlen);
	Map<VectorXcd> map_Ax(Ax.vector, Ax.vlen);

	EXPECT_NEAR((map_b.cast<complex128_t>()-map_Ax).norm(), 0.0, 1E-10);



}
