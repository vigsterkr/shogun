/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soumyajit De, Heiko Strathmann, Bjoern Esser
 */

#include <shogun/lib/config.h>

#include <shogun/base/Parameter.h>
#include <shogun/lib/SGMatrix.h>
#include <shogun/lib/SGVector.h>
#include <shogun/mathematics/linalg/LinalgNamespace.h>
#include <shogun/mathematics/linalg/linop/DenseMatrixOperator.h>
#include <shogun/mathematics/linalg/linop/SparseMatrixOperator.h>
#include <shogun/mathematics/linalg/linsolver/LinearSolver.h>
#include <shogun/mathematics/linalg/ratapprox/logdet/opfunc/LogRationalApproximationIndividual.h>
#include <typeinfo>

using namespace Eigen;
namespace shogun
{

	CLogRationalApproximationIndividual::CLogRationalApproximationIndividual()
	    : CRationalApproximation(nullptr, nullptr, 0, OF_LOG)
	{
		init();
}

CLogRationalApproximationIndividual::CLogRationalApproximationIndividual(
	std::shared_ptr<CMatrixOperator<float64_t>> linear_operator, std::shared_ptr<CEigenSolver> eigen_solver,
	std::shared_ptr<CLinearSolver<complex128_t, float64_t>> linear_solver,
	float64_t desired_accuracy)
	: CRationalApproximation(
	      linear_operator, eigen_solver, desired_accuracy, OF_LOG)
{
	init();

	m_linear_solver=linear_solver;

}

void CLogRationalApproximationIndividual::init()
{
	m_linear_solver=NULL;

	SG_ADD((std::shared_ptr<CSGObject>*)&m_linear_solver, "linear_solver",
		"Linear solver for complex systems");
}

CLogRationalApproximationIndividual::~CLogRationalApproximationIndividual()
{

}

float64_t
CLogRationalApproximationIndividual::compute(SGVector<float64_t> sample) const
{
	SG_DEBUG("Entering..\n");
	REQUIRE(sample.vector, "Sample is not initialized!\n");
	REQUIRE(m_linear_operator, "Operator is not initialized!\n");

	// this enum will save from repeated typechecking for all jobs
	enum typeID {DENSE=1, SPARSE, UNKNOWN} operator_type=UNKNOWN;

	// create a complex copy of the matrix linear operator
	std::shared_ptr<CMatrixOperator<complex128_t>> complex_op=NULL;
	if (typeid(*m_linear_operator)==typeid(CDenseMatrixOperator<float64_t>))
	{
		operator_type=DENSE;

		auto op
			=m_linear_operator->as<CDenseMatrixOperator<float64_t>>();

		REQUIRE(op->get_matrix_operator().matrix, "Matrix is not initialized!\n");

		// create complex dense matrix operator
		complex_op=op->as<CDenseMatrixOperator<complex128_t>>();
	}
	else if (typeid(*m_linear_operator)==typeid(CSparseMatrixOperator<float64_t>))
	{
		operator_type=SPARSE;

		auto op
			=m_linear_operator->as<CSparseMatrixOperator<float64_t>>();

		REQUIRE(op->get_matrix_operator().sparse_matrix, "Matrix is not initialized!\n");

		// create complex sparse matrix operator
		complex_op=op->as<CSparseMatrixOperator<complex128_t>>();
	}
	else
	{
		// something weird happened
		SG_ERROR("Unknown MatrixOperator given!\n");
	}

	SGVector<complex128_t> agg(sample.vlen);
	// create num_shifts number of jobs for current sample vector
	for (index_t i=0; i<m_num_shifts; ++i)
	{
		// create a deep copy of the operator
		std::shared_ptr<CMatrixOperator<complex128_t>> shifted_op=NULL;

		switch(operator_type)
		{
		case DENSE:
			shifted_op=std::make_shared<CDenseMatrixOperator<complex128_t>>
				(*(complex_op->as<CDenseMatrixOperator<complex128_t>>()));
			break;
		case SPARSE:
			shifted_op=std::make_shared<CSparseMatrixOperator<complex128_t>>
				(*(complex_op->as<CSparseMatrixOperator<complex128_t>>()));
			break;
		default:
			break;
		}

		SGVector<complex128_t> diag=shifted_op->get_diagonal();
		for (index_t j=0; j<diag.vlen; ++j)
			diag[j]-=m_shifts[i];
		shifted_op->set_diagonal(diag);

		SGVector<complex128_t> vec = m_linear_solver->solve(shifted_op, sample);
		// multiply with the weight using Eigen3 and take negative
		// (see CRationalApproximation for the formula)
		Map<VectorXcd> v(vec.vector, vec.vlen);
		v *= m_weights[i];
		v = -v;
		// aggregate the result
		agg += vec;
	}
	float64_t result =
		linalg::dot(sample, m_linear_operator->apply(agg.get_imag()));
	result *= m_constant_multiplier;
	return result;
}
}
