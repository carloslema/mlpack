/**
 * @file naive_bayes_classifier.hpp
 * @author Parikshit Ram (pram@cc.gatech.edu)
 * @author Shihao Jing (shihao.jing810@gmail.com)
 *
 * A Naive Bayes Classifier which parametrically estimates the distribution of
 * the features.  It is assumed that the features have been sampled from a
 * Gaussian PDF.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_NAIVE_BAYES_NAIVE_BAYES_CLASSIFIER_HPP
#define MLPACK_METHODS_NAIVE_BAYES_NAIVE_BAYES_CLASSIFIER_HPP

#include <mlpack/prereqs.hpp>

namespace mlpack {
namespace naive_bayes /** The Naive Bayes Classifier. */ {

/**
 * The simple Naive Bayes classifier.  This class trains on the data by
 * calculating the sample mean and variance of the features with respect to each
 * of the labels, and also the class probabilities.  The class labels are
 * assumed to be positive integers (starting with 0), and are expected to be the
 * last row of the data input to the constructor.
 *
 * Mathematically, it computes P(X_i = x_i | Y = y_j) for each feature X_i for
 * each of the labels y_j.  Alongwith this, it also computes the class
 * probabilities P(Y = y_j).
 *
 * For classifying a data point (x_1, x_2, ..., x_n), it computes the following:
 * arg max_y(P(Y = y)*P(X_1 = x_1 | Y = y) * ... * P(X_n = x_n | Y = y))
 *
 * Example use:
 *
 * @code
 * extern arma::mat training_data, testing_data;
 * NaiveBayesClassifier<> nbc(training_data, 5);
 * arma::vec results;
 *
 * nbc.Classify(testing_data, results);
 * @endcode
 */
template<typename MatType = arma::mat>
class NaiveBayesClassifier
{
 public:
  /**
   * Initializes the classifier as per the input and then trains it by
   * calculating the sample mean and variances.
   *
   * Example use:
   * @code
   * extern arma::mat training_data, testing_data;
   * extern arma::Row<size_t> labels;
   * NaiveBayesClassifier nbc(training_data, labels, 5);
   * @endcode
   *
   * @param data Training data points.
   * @param labels Labels corresponding to training data points.
   * @param classes Number of classes in this classifier.
   * @param incrementalVariance If true, an incremental algorithm is used to
   *     calculate the variance; this can prevent loss of precision in some
   *     cases, but will be somewhat slower to calculate.
   */
  NaiveBayesClassifier(const MatType& data,
                       const arma::Row<size_t>& labels,
                       const size_t classes,
                       const bool incrementalVariance = false);

  /**
   * Initialize the Naive Bayes classifier without performing training.  All of
   * the parameters of the model will be initialized to zero.  Be sure to use
   * Train() before calling Classify(), otherwise the results may be
   * meaningless.
   */
  NaiveBayesClassifier(const size_t dimensionality = 0,
                       const size_t classes = 0);

  /**
   * Train the Naive Bayes classifier on the given dataset.  If the incremental
   * algorithm is used, the current model is used as a starting point (this is
   * the default).  If the incremental algorithm is not used, then the current
   * model is ignored and the new model will be trained only on the given data.
   * Note that even if the incremental algorithm is not used, the data must have
   * the same dimensionality and number of classes that the model was
   * initialized with.  If you want to change the dimensionality or number of
   * classes, either re-initialize or call Means(), Variances(), and
   * Probabilities() individually to set them to the right size.
   *
   * @param data The dataset to train on.
   * @param incremental Whether or not to use the incremental algorithm for
   *      training.
   */
  void Train(const MatType& data,
             const arma::Row<size_t>& labels,
             const bool incremental = true);

  /**
   * Train the Naive Bayes classifier on the given point.  This will use the
   * incremental algorithm for updating the model parameters.  The data must be
   * the same dimensionality as the existing model parameters.
   *
   * @param point Data point to train on.
   * @param label Label of data point.
   */
  template<typename VecType>
  void Train(const VecType& point, const size_t label);

  /**
   * Classify the given point, using the training GaussianNB model. The predicted label is
   * returned.
   *
   * @param point Point to classify.
   */
  template<typename VecType>
  size_t Classify(const VecType& point) const;

  /**
   * Classify the given point using the training GaussianNB model
   * and also return estimates of the probability for
   * each class in the given vector.
   *
   * @param point Point to classify.
   * @param prediction This will be set to the predicted class of the point.
   * @param probabilities This will be filled with class probabilities for the
   *      point.
   */
  template<typename VecType>
  void Classify(const VecType& point,
                size_t& prediction,
                arma::vec& probabilities) const;

  /**
   * Classify the given points using the training GaussianNB model.
   * The predicted labels for each point are stored in the given vector.
   *
   * @code
   * arma::mat test_data; // each column is a test point
   * arma::Row<size_t> results;
   * ...
   * nbc.Classify(test_data, results);
   * @endcode
   *
   * @param data List of data points.
   * @param predictions Vector that class predictions will be placed into.
   */
  void Classify(const MatType& data,
                arma::Row<size_t>& predictions) const;

  /**
   * Classify the given points using the training GaussianNB model
   * and also return estimates of the probabilities for each class in the given matrix.
   * The predicted labels for each point are stored in the given vector.
   *
   * @code
   * arma::mat test_data; // each column is a test point
   * arma::Row<size_t> results;
   * arma::mat resultsProbs;
   * ...
   * nbc.Classify(test_data, results, resultsProbs);
   * @endcode
   *
   * @param data Set of points to classify.
   * @param predictions This will be filled with predictions for each point.
   * @param probabilities This will be filled with class probabilities for each
   *      point. Each row represents a point.
   */
  void Classify(const MatType& data,
                arma::Row<size_t>& predictions,
                arma::mat& probabilities) const;

  //! Get the sample means for each class.
  const MatType& Means() const { return means; }
  //! Modify the sample means for each class.
  MatType& Means() { return means; }

  //! Get the sample variances for each class.
  const MatType& Variances() const { return variances; }
  //! Modify the sample variances for each class.
  MatType& Variances() { return variances; }

  //! Get the prior probabilities for each class.
  const arma::vec& Probabilities() const { return probabilities; }
  //! Modify the prior probabilities for each class.
  arma::vec& Probabilities() { return probabilities; }

  //! Serialize the classifier.
  template<typename Archive>
  void Serialize(Archive& ar, const unsigned int /* version */);

 private:
  //! Sample mean for each class.
  MatType means;
  //! Sample variances for each class.
  MatType variances;
  //! Class probabilities.
  arma::vec probabilities;
  //! Number of training points seen so far.
  size_t trainingPoints;

  /**
   * Compute the unnormalized posterior log probability (log likelihood) of
   * given point.
   *
   * @param point Data point to compute posterior log probability of.
   * @param logLikelihoods Vector to store log likelihoods in.
   */
  template<typename VecType>
  void LogLikelihood(const VecType& point, arma::vec& logLikelihoods) const;

  /**
   * Compute the unnormalized posterior log probability of given points (log
   * likelihood). Results are returned as arma::mat, and each column represents
   * a point, each row represents log likelihood of a class.
   *
   * @param data Set of points to compute posterior log probability for.
   * @param logLikelihoods Matrix to store log likelihoods in.
   */
  void LogLikelihood(const MatType& data,
                     arma::mat& logLikelihoods) const;
};

} // namespace naive_bayes
} // namespace mlpack

// Include implementation.
#include "naive_bayes_classifier_impl.hpp"

#endif
