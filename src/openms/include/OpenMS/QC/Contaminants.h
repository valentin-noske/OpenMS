// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2018.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Chris Bielow $
// $Authors:  Dominik Schmitz, Chris Bielow$
// --------------------------------------------------------------------------

#pragma once

#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/FORMAT/FASTAFile.h>
#include <OpenMS/METADATA/PeptideIdentification.h>
#include <unordered_set>
#include <OpenMS/QC/QCBase.h>


namespace OpenMS
{
  /**
   * @brief This class is a metric for the QualityControl-ToppTool.
   *
   * This class checks whether a peptide is a contaminant or not and adds that result to the first hit of the peptideidentification.
   */
  class OPENMS_DLLAPI Contaminants:
      QCBase
  {
  public:
    /// structure for storing results
    struct ContaminantsSummary
    {
      ///(# contaminants in assigned/ #peptides in assigned)
      double assigned_contaminants_ratio;
      ///(# contaminants in unassigned/ #peptides in unassigned)
      double unassigned_contaminants_ratio;
      ///(# all contaminants/ #peptides in all)
      double all_contaminants_ratio;
      ///(intensity of contaminants in assigned/ intensity of peptides in assigned)
      double assigned_contaminants_intensity_ratio;
      ///(features without peptideidentification or with peptideidentifications but without hits, all features)
      std::pair<Int64, Int64> empty_features;
    };
    ///Constructor
    Contaminants() = default;
    ///Destructor
    virtual ~Contaminants() = default;
    /**
     * @brief Checks if the peptides are in the contaminant database.
     * "is_contaminant" identification is added to the first hit of the peptideidentification of each feature and to the first hit of all unsignedpeptideidentifications
     * @param features input FeatureMap with peptideidentifications of features
     * @param contaminants vector of FASTAEntries that need to be digested to check whether a peptide is a contaminant or not
     * @exception Exception::MissingInformation if the contaminants database is empty
     * @exception Exception::MissingInformation if no enzyme is given
     * @warning LOG_WARN if the FeatureMap is empty
     */
    void compute(FeatureMap& features, const std::vector<FASTAFile::FASTAEntry>& contaminants);
    /// returns results
    const std::vector<Contaminants::ContaminantsSummary>& getResults();
    /**
     * @brief Returns the input data requirements of the compute(...) function
     * @return Status for POSTFDRFEAT and CONTAMINANTS
     */
    Status requires() const override;
  private:
    /// container that stores results
    std::vector<Contaminants::ContaminantsSummary> results_;
    /// unordered set that contains the contaminant sequences
    std::unordered_set<String> digested_db_;
    /**
     * @brief
     * checks if the peptide is in the contaminant database
     * @param key String that will be the key for searching in the unordered set
     * @param f Feature with peptideidentification to store the result "is_contaminant = 0/1" in the first hit of each
     * @param total counter of all checked peptides
     * @param cont counter of all checked peptides that are contamiants
     * @param sum_total intensity of all checked peptides
     * @param sum_cont intensity of all checked peptides that are contaminants
     */
    void compare_(const String& key, Feature& f, Int64& total, Int64& cont, double& sum_total, double& sum_cont);
  };
} // namespace OpenMS