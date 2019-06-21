/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal, Pan Deng, Soumyajit De, Bjoern Esser
 */
#include <gtest/gtest.h>

#include <shogun/lib/common.h>

#include <shogun/lib/SGVector.h>
#include <shogun/lib/SGSparseMatrix.h>
#include <shogun/mathematics/eigen3.h>
#include <shogun/mathematics/Math.h>
#include <shogun/mathematics/Random.h>
#include <shogun/mathematics/linalg/linop/SparseMatrixOperator.h>
#include <shogun/mathematics/linalg/linsolver/DirectSparseLinearSolver.h>

using namespace shogun;
using namespace Eigen;

TEST(DirectSparseLinearSolver, solve)
{
	const index_t size=100000;
	SGSparseMatrix<float64_t> sm(size, size);
	auto A=std::make_shared<CSparseMatrixOperator<float64_t>>(sm);
	SGVector<float64_t> diag(size);
	float64_t difficulty=5;

	for (index_t i=0; i<size; ++i)
		diag[i]=CMath::pow(CMath::abs(sg_rand->std_normal_distrib()), difficulty)+0.0001;
	A->set_diagonal(diag);

	auto linear_solver=std::make_shared<CDirectSparseLinearSolver>();
	SGVector<float64_t> b(size);
	b.set_const(0.5);

	const SGVector<float64_t>& x=linear_solver->solve(A, b);
	SGVector<float64_t> Ax=A->apply(x);
	Map<VectorXd> map_Ax(Ax.vector, Ax.vlen);
	Map<VectorXd> map_b(b.vector, b.vlen);

	EXPECT_NEAR((map_Ax-map_b).norm(), 0.0, 1E-10);



}
