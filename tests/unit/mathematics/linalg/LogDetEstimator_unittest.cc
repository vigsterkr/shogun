/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sunil Mahendrakar, Soumyajit De, Heiko Strathmann, Bjoern Esser,
 *          Viktor Gal, Thoralf Klein, Shubham Shukla, Pan Deng
 */
#include <gtest/gtest.h>

#include <shogun/lib/common.h>

#include <shogun/mathematics/eigen3.h>
#include <shogun/mathematics/Math.h>
#include <shogun/mathematics/Random.h>
#include <shogun/mathematics/Statistics.h>
#include <shogun/lib/SGVector.h>
#include <shogun/lib/SGMatrix.h>
#include <shogun/lib/SGSparseMatrix.h>
#include <shogun/features/SparseFeatures.h>
#include <shogun/mathematics/linalg/linop/DenseMatrixOperator.h>
#include <shogun/mathematics/linalg/linop/SparseMatrixOperator.h>
#include <shogun/mathematics/linalg/ratapprox/logdet/opfunc/DenseMatrixExactLog.h>
#include <shogun/mathematics/linalg/ratapprox/logdet/opfunc/LogRationalApproximationIndividual.h>
#include <shogun/mathematics/linalg/ratapprox/logdet/opfunc/LogRationalApproximationCGM.h>
#include <shogun/mathematics/linalg/ratapprox/tracesampler/NormalSampler.h>
#include <shogun/mathematics/linalg/ratapprox/tracesampler/ProbingSampler.h>
#include <shogun/mathematics/linalg/eigsolver/DirectEigenSolver.h>
#include <shogun/mathematics/linalg/eigsolver/LanczosEigenSolver.h>
#include <shogun/mathematics/linalg/linsolver/DirectLinearSolverComplex.h>
#include <shogun/mathematics/linalg/linsolver/CGMShiftedFamilySolver.h>
#include <shogun/mathematics/linalg/ratapprox/logdet/LogDetEstimator.h>

using namespace shogun;
using namespace Eigen;

#if EIGEN_VERSION_AT_LEAST(3,1,0)
TEST(LogDetEstimator, sample)
{
	const index_t size=2;
	SGMatrix<float64_t> mat(size, size);
	mat(0,0)=2.0;
	mat(0,1)=1.0;
	mat(1,0)=1.0;
	mat(1,1)=3.0;

	auto op=std::make_shared<CDenseMatrixOperator<float64_t>>(mat);


	auto op_func = std::make_shared<CDenseMatrixExactLog>(op);


	auto trace_sampler=std::make_shared<CNormalSampler>(size);


	CLogDetEstimator estimator(trace_sampler, op_func);
	const index_t num_estimates=5000;
	SGVector<float64_t> estimates=estimator.sample(num_estimates);

	float64_t result=0.0;
	for (index_t i=0; i<num_estimates; ++i)
		result+=estimates[i];
	result/=num_estimates;

	EXPECT_NEAR(result, 1.60943791243410050384, 0.1);




}

#ifdef HAVE_LAPACK
TEST(LogDetEstimator, Sparse_sample_constructor)
{
	const index_t size=16;
	SGMatrix<float64_t> mat(size, size);
	mat.set_const(0.0);

	CSparseFeatures<float64_t> feat(mat);
	SGSparseMatrix<float64_t> sm=feat.get_sparse_feature_matrix();

	CLogDetEstimator estimator(sm);

	auto op=
		estimator.get_operator_function()->as<COperatorFunction<float64_t>>();

	auto tracer=
		estimator.get_trace_sampler()->as<CTraceSampler>();

	EXPECT_TRUE(op);
	EXPECT_TRUE(tracer);



}
#endif //HAVE_LAPACK
#endif // EIGEN_VERSION_AT_LEAST(3,1,0)

#ifdef USE_GPL_SHOGUN
TEST(LogDetEstimator, sample_ratapp_dense)
{
	const index_t size=2;
	SGMatrix<float64_t> mat(size, size);
	mat(0,0)=1.0;
	mat(0,1)=0.5;
	mat(1,0)=0.5;
	mat(1,1)=1000.0;

	float64_t accuracy=1E-5;
	auto op=std::make_shared<CDenseMatrixOperator<float64_t>>(mat);


	auto eig_solver=std::make_shared<CDirectEigenSolver>(op);


	auto linear_solver=std::make_shared<CDirectLinearSolverComplex>();


	auto op_func =
	    std::make_shared<CLogRationalApproximationIndividual>(
	        op, eig_solver,
	        linear_solver->as<CLinearSolver<complex128_t, float64_t>>(), accuracy);


	auto trace_sampler=std::make_shared<CNormalSampler>(size);


	CLogDetEstimator estimator(trace_sampler, op_func);
	const index_t num_estimates=10;
	SGVector<float64_t> estimates=estimator.sample(num_estimates);

	float64_t result=0.0;
	for (index_t i=0; i<num_estimates; ++i)
		result+=estimates[i];
	result/=num_estimates;






}

#ifdef HAVE_COLPACK
#ifdef HAVE_LAPACK
TEST(LogDetEstimator, sample_ratapp_probing_sampler)
{
	const index_t size=16;
	SGMatrix<float64_t> mat(size, size);
	mat.set_const(0.0);
	for (index_t i=0; i<size; ++i)
	{
		float64_t value=CMath::abs(sg_rand->std_normal_distrib())*1000;
		mat(i,i)=value<1.0?10.0:value;
	}

	mat(0,5)=mat(5,0)=1.0;
	mat(0,7)=mat(7,0)=1.0;
	mat(0,11)=mat(11,0)=1.0;
	mat(1,8)=mat(8,1)=1.0;
	mat(1,10)=mat(10,1)=1.0;
	mat(1,11)=mat(11,1)=1.0;
	mat(1,12)=mat(12,1)=1.0;
	mat(2,8)=mat(8,2)=1.0;
	mat(2,11)=mat(11,2)=1.0;
	mat(2,13)=mat(13,2)=1.0;
	mat(2,14)=mat(14,2)=1.0;
	mat(3,8)=mat(8,3)=1.0;
	mat(3,12)=mat(12,3)=1.0;
	mat(3,15)=mat(15,3)=1.0;
	mat(4,8)=mat(8,4)=1.0;
	mat(4,14)=mat(14,4)=1.0;
	mat(4,15)=mat(15,4)=1.0;
	mat(5,11)=mat(11,5)=1.0;
	mat(5,10)=mat(10,5)=1.0;
	mat(6,10)=mat(10,6)=1.0;
	mat(6,12)=mat(12,6)=1.0;
	mat(7,11)=mat(11,7)=1.0;
	mat(7,13)=mat(13,7)=1.0;
	mat(8,11)=mat(11,8)=1.0;
	mat(8,15)=mat(15,8)=1.0;
	mat(9,13)=mat(13,9)=1.0;
	mat(9,14)=mat(14,9)=1.0;

	float64_t actual_result=CStatistics::log_det(mat);
	float64_t accuracy=1E-5;

	CSparseFeatures<float64_t> feat(mat);
	SGSparseMatrix<float64_t> sm=feat.get_sparse_feature_matrix();

	auto op=std::make_shared<CSparseMatrixOperator<float64_t>>(sm);

	auto opd=std::make_shared<CDenseMatrixOperator<float64_t>>(mat);


	auto eig_solver=std::make_shared<CLanczosEigenSolver>(op);


	auto linear_solver=std::make_shared<CDirectLinearSolverComplex>();


	auto op_func =
	    std::make_shared<CLogRationalApproximationIndividual>(
	        opd, eig_solver,
	        (CLinearSolver<complex128_t, float64_t>*)linear_solver, accuracy);


	auto trace_sampler=std::make_shared<CProbingSampler>(op, 1, NATURAL, DISTANCE_TWO);


	CLogDetEstimator estimator(trace_sampler, op_func);
	const index_t num_estimates=1;
	SGVector<float64_t> estimates=estimator.sample(num_estimates);

	float64_t result=0.0;
	for (index_t i=0; i<num_estimates; ++i)
		result+=estimates[i];
	result/=num_estimates;

	EXPECT_NEAR(result, actual_result, 1.0);







}

TEST(LogDetEstimator, sample_ratapp_probing_sampler_cgm)
{
	const index_t size=16;
	SGMatrix<float64_t> mat(size, size);
	mat.set_const(0.0);
	for (index_t i=0; i<size; ++i)
	{
		float64_t value=CMath::abs(sg_rand->std_normal_distrib())*1000;
		mat(i,i)=value<1.0?10.0:value;
	}

	mat(0,5)=mat(5,0)=1.0;
	mat(0,7)=mat(7,0)=1.0;
	mat(0,11)=mat(11,0)=1.0;
	mat(1,8)=mat(8,1)=1.0;
	mat(1,10)=mat(10,1)=1.0;
	mat(1,11)=mat(11,1)=1.0;
	mat(1,12)=mat(12,1)=1.0;
	mat(2,8)=mat(8,2)=1.0;
	mat(2,11)=mat(11,2)=1.0;
	mat(2,13)=mat(13,2)=1.0;
	mat(2,14)=mat(14,2)=1.0;
	mat(3,8)=mat(8,3)=1.0;
	mat(3,12)=mat(12,3)=1.0;
	mat(3,15)=mat(15,3)=1.0;
	mat(4,8)=mat(8,4)=1.0;
	mat(4,14)=mat(14,4)=1.0;
	mat(4,15)=mat(15,4)=1.0;
	mat(5,11)=mat(11,5)=1.0;
	mat(5,10)=mat(10,5)=1.0;
	mat(6,10)=mat(10,6)=1.0;
	mat(6,12)=mat(12,6)=1.0;
	mat(7,11)=mat(11,7)=1.0;
	mat(7,13)=mat(13,7)=1.0;
	mat(8,11)=mat(11,8)=1.0;
	mat(8,15)=mat(15,8)=1.0;
	mat(9,13)=mat(13,9)=1.0;
	mat(9,14)=mat(14,9)=1.0;

	float64_t actual_result=CStatistics::log_det(mat);
	float64_t accuracy=1E-15;

	CSparseFeatures<float64_t> feat(mat);
	SGSparseMatrix<float64_t> sm=feat.get_sparse_feature_matrix();

	auto op=std::make_shared<CSparseMatrixOperator<float64_t>>(sm);


	auto eig_solver=std::make_shared<CLanczosEigenSolver>(op);


	auto linear_solver=std::make_shared<CCGMShiftedFamilySolver>();


	auto op_func = std::make_shared<CLogRationalApproximationCGM>(
	    op, eig_solver, linear_solver, accuracy);


	auto trace_sampler=std::make_shared<CProbingSampler>(op, 1, NATURAL, DISTANCE_TWO);


	CLogDetEstimator estimator(trace_sampler, op_func);
	const index_t num_estimates=10;
	SGVector<float64_t> estimates=estimator.sample(num_estimates);

	float64_t result=0.0;
	for (index_t i=0; i<num_estimates; ++i)
		result+=estimates[i];
	result/=num_estimates;

	EXPECT_NEAR(result, actual_result, 1E-3);






}

TEST(LogDetEstimator, sample_ratapp_big_diag_matrix)
{
	float64_t difficulty=2;
	float64_t accuracy=1E-5;
	float64_t min_eigenvalue=0.001;

	// create a sparse matrix
	const index_t size=100;
	SGSparseMatrix<float64_t> sm(size, size);
	auto op=std::make_shared<CSparseMatrixOperator<float64_t>>(sm);


	// set its diagonal
	SGVector<float64_t> diag(size);
	for (index_t i=0; i<size; ++i)
	{
		diag[i]=CMath::pow(CMath::abs(sg_rand->std_normal_distrib()), difficulty)
			+min_eigenvalue;
	}
	op->set_diagonal(diag);

	auto eig_solver=std::make_shared<CLanczosEigenSolver>(op);


	auto linear_solver=std::make_shared<CCGMShiftedFamilySolver>();


	auto op_func = std::make_shared<CLogRationalApproximationCGM>(
	    op, eig_solver, linear_solver, accuracy);


	auto trace_sampler=std::make_shared<CProbingSampler>(op);


	CLogDetEstimator estimator(trace_sampler, op_func);
	const index_t num_estimates=1;
	SGVector<float64_t> estimates=estimator.sample(num_estimates);

	float64_t result=0.0;
	for (index_t i=0; i<num_estimates; ++i)
		result+=estimates[i];
	result/=num_estimates;

	// test the log-det samples
	sm=op->get_matrix_operator();
	float64_t actual_result=CStatistics::log_det(sm);
	EXPECT_NEAR(result, actual_result, 1.0);






}

TEST(LogDetEstimator, sample_ratapp_big_matrix)
{
	float64_t difficulty=2;
	float64_t accuracy=1E-5;
	float64_t min_eigenvalue=0.001;

	// create a sparse matrix
	const index_t size=100;
	SGSparseMatrix<float64_t> sm(size, size);

	// set its diagonal
	SGVector<float64_t> diag(size);
	for (index_t i=0; i<size; ++i)
	{
		sm(i,i)=CMath::pow(CMath::abs(sg_rand->std_normal_distrib()), difficulty)
			+min_eigenvalue;
	}
	// set its subdiagonal
	float64_t entry=min_eigenvalue/2;
	for (index_t i=0; i<size-1; ++i)
	{
		sm(i,i+1)=entry;
		sm(i+1,i)=entry;
	}

	auto op=std::make_shared<CSparseMatrixOperator<float64_t>>(sm);


	auto eig_solver=std::make_shared<CLanczosEigenSolver>(op);


	auto linear_solver=std::make_shared<CCGMShiftedFamilySolver>();
	//linear_solver->set_iteration_limit(2000);


	auto op_func = std::make_shared<CLogRationalApproximationCGM>(
	    op, eig_solver, linear_solver, accuracy);


	auto trace_sampler=std::make_shared<CProbingSampler>(op);


	CLogDetEstimator estimator(trace_sampler, op_func);
	const index_t num_estimates=1;
	SGVector<float64_t> estimates=estimator.sample(num_estimates);

	float64_t result=0.0;
	for (index_t i=0; i<num_estimates; ++i)
		result+=estimates[i];
	result/=num_estimates;

	// test the log-det samples
	sm=op->get_matrix_operator();
	float64_t actual_result=CStatistics::log_det(sm);
	EXPECT_NEAR(result, actual_result, 1.0);






}
#endif // HAVE_LAPACK
#endif // HAVE_COLPACK
#endif //USE_GPL_SHOGUN
