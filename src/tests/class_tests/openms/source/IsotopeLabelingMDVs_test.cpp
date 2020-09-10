// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2020.
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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------
//

#include <OpenMS/CONCEPT/ClassTest.h>
#include <OpenMS/ANALYSIS/QUANTITATION/IsotopeLabelingMDVs.h>
#include <assert.h>

using namespace OpenMS;


class IsotopeLabelingMDVs_test : public IsotopeLabelingMDVs
{
  public :
  
  void inverseMatrix_(std::vector<std::vector<double>>& correction_matrix,
                      std::vector<std::vector<double>>& correction_matrix_inversed)
    {
      IsotopeLabelingMDVs::inverseMatrix_(correction_matrix, correction_matrix_inversed);
    }
};


START_TEST(IsotopeLabelingMDVs, "$Id$")

IsotopeLabelingMDVs* ptr          = nullptr;
IsotopeLabelingMDVs* nullPointer  = nullptr;


START_SECTION((IsotopeLabelingMDVs()))
  ptr = new IsotopeLabelingMDVs();
  TEST_NOT_EQUAL(ptr, nullPointer)
END_SECTION

START_SECTION((~IsotopeLabelingMDVs()))
  delete ptr;
END_SECTION


START_SECTION(( void IsotopeLabelingMDVs::calculateMDV(
                                              Feature& measured_feature,
                                              Feature& normalized_featuremap,
                                              const String& mass_intensity_type,
                                              const String& feature_name) ))

  // case 1:  intensity with norm max and norm sum (x)  : intensity (peak area) not supplied
  // case 2:  peak apex with norm max and norm sum      : - Lactate1 & Lactate2 - peak_apex_int - norm_max
  //                                                      - Lactate1 & Lactate2 - peak_apex_int - norm_sum

  IsotopeLabelingMDVs                isotopelabelingmdvs;
  
  // From CHO_190316_Flux.xlsx provided by Douglas McCloskey
  std::vector<Peak2D::IntensityType> L1_peak_apex_int {3.61e+08, 1.20e+04, 1.02e+05, 2.59e+04};
  std::vector<Peak2D::IntensityType> L2_peak_apex_int {2.77e+07, 5.45e+04, 6.26e+05, 7.46e+04, 2.75e+04};

  std::vector<Peak2D::IntensityType> L1_norm_max {1.00e+00, 3.324e-05, 2.825e-04, 7.174e-05};
  std::vector<Peak2D::IntensityType> L1_norm_sum {9.9961e-01, 3.3228e-05, 2.8243e-04, 7.1717e-05};

  std::vector<Peak2D::IntensityType> L2_norm_max {1.00e+00, 1.967e-03, 2.259e-02, 2.693e-03, 9.927e-04};
  std::vector<Peak2D::IntensityType> L2_norm_sum {9.7252e-01, 1.9134e-03, 2.1978e-02, 2.6191e-03, 9.655e-04};
  

  // Lactate1 & Lactate2 - peak_apex_int - norm_max
  OpenMS::Feature               lactate_1_normmax;
  OpenMS::Feature               lactate_1_normalized_normmax;
  std::vector<OpenMS::Feature>  L1_subordinates_normmax;

  lactate_1_normmax.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_peak_apex_int[i]);
    L1_subordinates_normmax.push_back(sub);
  }
  lactate_1_normmax.setSubordinates(L1_subordinates_normmax);
  
  isotopelabelingmdvs.calculateMDV(lactate_1_normmax, lactate_1_normalized_normmax, "norm_max", "peak_apex_int");

  for(size_t i = 0; i < lactate_1_normalized_normmax.getSubordinates().size(); ++i)
  {
    TEST_REAL_SIMILAR(lactate_1_normalized_normmax.getSubordinates().at(i).getIntensity(), L1_norm_max.at(i));
  }

  OpenMS::Feature               lactate_2_normmax;
  OpenMS::Feature               lactate_2_normalized_normmax;
  std::vector<OpenMS::Feature>  L2_subordinates_normmax;

  lactate_2_normmax.setMetaValue("PeptideRef", "Lactate2");
  for (uint16_t i = 0; i < L2_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate2_"+std::to_string(219+i));
    sub.setMetaValue("peak_apex_int", L2_peak_apex_int[i]);
    L2_subordinates_normmax.push_back(sub);
  }
  lactate_2_normmax.setSubordinates(L2_subordinates_normmax);

  isotopelabelingmdvs.calculateMDV(lactate_2_normmax, lactate_2_normalized_normmax, "norm_max", "peak_apex_int");

  for(size_t i = 0; i < lactate_2_normalized_normmax.getSubordinates().size(); ++i)
  {
    TEST_REAL_SIMILAR(lactate_2_normalized_normmax.getSubordinates().at(i).getIntensity(), L2_norm_max.at(i));
  }


  // Lactate1 & Lactate2 - peak_apex_int - norm_sum
  OpenMS::Feature               lactate_1_normsum;
  OpenMS::Feature               lactate_1_normalized_normsum;
  std::vector<OpenMS::Feature>  L1_subordinates_normsum;

  lactate_1_normsum.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_peak_apex_int[i]);
    L1_subordinates_normsum.push_back(sub);
  }
  lactate_1_normsum.setSubordinates(L1_subordinates_normsum);

  isotopelabelingmdvs.calculateMDV(lactate_1_normsum, lactate_1_normalized_normsum, "norm_sum", "peak_apex_int");

  for(size_t i = 0; i < lactate_1_normalized_normsum.getSubordinates().size(); ++i)
  {
    TEST_REAL_SIMILAR(lactate_1_normalized_normsum.getSubordinates().at(i).getIntensity(), L1_norm_sum.at(i));
  }


  OpenMS::Feature lactate_2_normsum; OpenMS::Feature lactate_2_normalized_normsum;
  std::vector<OpenMS::Feature> L2_subordinates_normsum;

  lactate_2_normsum.setMetaValue("PeptideRef", "Lactate2");
  for (uint16_t i = 0; i < L2_peak_apex_int.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate2_"+std::to_string(219+i));
    sub.setMetaValue("peak_apex_int", L2_peak_apex_int[i]);
    L2_subordinates_normsum.push_back(sub);
    
  }
  lactate_2_normsum.setSubordinates(L2_subordinates_normsum);

  isotopelabelingmdvs.calculateMDV(lactate_2_normsum, lactate_2_normalized_normsum, "norm_sum", "peak_apex_int");

  for(size_t i = 0; i < lactate_2_normalized_normsum.getSubordinates().size(); ++i)
  {
    TEST_REAL_SIMILAR(lactate_2_normalized_normsum.getSubordinates().at(i).getIntensity(), L2_norm_sum.at(i));
  }

END_SECTION


START_SECTION(( void IsotopeLabelingMDVs::isotopicCorrection(
                                              Feature& normalized_feature,
                                              Feature& corrected_feature,
                                              std::vector<std::vector<double>> correction_matrix) ))

  // case 1: validating matrix inverse (separately tested)
  // case 2: validating corrected results (corrected peak_apex_int)

  IsotopeLabelingMDVs                               isotopelabelingmdvs;
  OpenMS::Feature                                   lactate_1_normalized;
  OpenMS::Feature                                   lactate_1_corrected;
  std::vector<std::vector<double>>                  correction_matrix_inversed(4, std::vector<double>(4,0));
  // Correction Matrix extracted from "TOOLS FOR MASS ISOTOPOMER DATA EVALUATION IN 13C FLUX ANALYSIS,
  // Wahl et al, P.263, Table I"
  std::vector<std::vector<double>>                  correction_matrix_tBDMS {

    {0.8213, 0.1053, 0.0734, 0.0000},
    {0.8420, 0.0963, 0.0617, 0.0000},
    {0.8466, 0.0957, 0.0343, 0.0233},
    {0.8484, 0.0954, 0.0337, 0.0225}
  };

  // L1_norm_max, L1_peak_apex_int From CHO_190316_Flux.xlsx provided by Douglas McCloskey
  // L1_corrected self calculated
  std::vector<double>                               L1_norm_max       {1.00e+00, 3.324e-05, 2.825e-04, 7.174e-05};
  std::vector<double>                               L1_corrected      {-12.7699, 140.7289, -45.3788, -47.2081};
  std::vector<Peak2D::IntensityType>                L1_peak_apex_int  {3.61e+08, 1.20e+04, 1.02e+05, 2.59e+04};
  std::vector<OpenMS::Feature>                      L1_subordinates_normmax;

  
  lactate_1_normalized.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_norm_max.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_norm_max[i]);
    L1_subordinates_normmax.push_back(sub);
  }
  lactate_1_normalized.setSubordinates(L1_subordinates_normmax);

  isotopelabelingmdvs.isotopicCorrection(lactate_1_normalized, lactate_1_corrected, correction_matrix_tBDMS);

  for(size_t i = 0; i < lactate_1_corrected.getSubordinates().size(); ++i)
  {
    TEST_REAL_SIMILAR(lactate_1_corrected.getSubordinates().at(i).getIntensity(), L1_corrected[i]);
  }

END_SECTION


START_SECTION(( void inverseMatrix_(
                          std::vector<std::vector<double>>& correction_matrix,
                          std::vector<std::vector<double>>& correction_matrix_inversed) ))

  IsotopeLabelingMDVs_test                          isotopelabelingmdvs;
  std::vector<std::vector<double>>                  correction_matrix_inversed(4, std::vector<double>(4, 0));
  // Correction Matrix extracted from "TOOLS FOR MASS ISOTOPOMER DATA EVALUATION IN 13C FLUX ANALYSIS,
  // Wahl et al, P.263, Table I"
  std::vector<std::vector<double>>                  correction_matrix_tBDMS {

    {0.8213, 0.1053, 0.0734, 0.0000},
    {0.8420, 0.0963, 0.0617, 0.0000},
    {0.8466, 0.0957, 0.0343, 0.0233},
    {0.8484, 0.0954, 0.0337, 0.0225}
  };

  isotopelabelingmdvs.inverseMatrix_(correction_matrix_tBDMS, correction_matrix_inversed);
  double corrected_value;

  for (size_t i = 0; i < correction_matrix_tBDMS.size(); ++i) {
    for (size_t j = 0; j < correction_matrix_tBDMS[0].size(); ++j) {
      corrected_value = 0.0;
      if (i == j) {
        for (size_t k = 0; k < correction_matrix_tBDMS.size(); ++k) {
          corrected_value += correction_matrix_tBDMS[i][k] * correction_matrix_inversed[k][j];
        }
        TEST_REAL_SIMILAR(corrected_value, 1.0);
      }
    }
  }

END_SECTION


START_SECTION(( void IsotopeLabelingMDVs::calculateIsotopicPurity(
                                              Feature& normalized_featuremap,
                                              Feature& featuremap_with_isotopic_purity,
                                              std::vector<double>& experiment_data,
                                              std::string& isotopic_purity_name) ))

  // case 1: calculating isotopic purity on 1_2_13C, U_13C sample experiment data

  IsotopeLabelingMDVs             isotopelabelingmdvs;
  OpenMS::Feature                 lactate_1_normalized;
  OpenMS::Feature                 lactate_1_with_isotopic_purity;
  
  // L1_norm_max From CHO_190316_Flux.xlsx provided by Douglas McCloskey
  // L1_1_2_13C_glucose_experiment, L1_U_13C_glucose_experiment & L1_isotopic_purity_ground_truth
  // from "High-resolution 13C metabolic flux analysis",Long et al, doi:10.1038/s41596-019-0204-0,
  // P.2869, Box 4
  std::vector<double>             L1_norm_max                     {1.00e+00, 3.324e-05, 2.825e-04, 7.174e-05};
  std::vector<double>             L1_1_2_13C_glucose_experiment   {0.5, 0.7, 98.8, 0.0, 0.0, 0.0};
  std::vector<double>             L1_U_13C_glucose_experiment     {0.5, 0.0, 0.1, 0.2, 3.6, 95.5};
  std::vector<double>             L1_isotopic_purity_ground_truth {99.6469, 99.2517};  // [1_2_13C, U_13C]

  std::string                     L1_1_2_13C_glucose          = "1_2-13C_glucose_experiment";
  std::string                     L1_U_13C_glucose            = "U-13C_glucose_experiment";

  std::vector<OpenMS::Feature>    L1_subordinates_normmax;

  
  lactate_1_normalized.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_norm_max.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_norm_max[i]);
    L1_subordinates_normmax.push_back(sub);
  }
  lactate_1_normalized.setSubordinates(L1_subordinates_normmax);

  isotopelabelingmdvs.calculateIsotopicPurity(lactate_1_normalized, lactate_1_with_isotopic_purity, L1_1_2_13C_glucose_experiment, L1_1_2_13C_glucose);
  TEST_REAL_SIMILAR( (double)(lactate_1_with_isotopic_purity.getMetaValue(L1_1_2_13C_glucose)) * 100, L1_isotopic_purity_ground_truth[0]);

  isotopelabelingmdvs.calculateIsotopicPurity(lactate_1_normalized, lactate_1_with_isotopic_purity, L1_U_13C_glucose_experiment, L1_U_13C_glucose);
  TEST_REAL_SIMILAR( (double)(lactate_1_with_isotopic_purity.getMetaValue(L1_U_13C_glucose)) * 100, L1_isotopic_purity_ground_truth[1]);

END_SECTION


START_SECTION(( IsotopeLabelingMDVs::calculateMDVAccuracy(
                                              Feature& normalized_feature,
                                              Feature& feature_with_accuracy_info,
                                              std::vector<double>& fragment_isotopomer_measured,
                                              std::vector<double>& fragment_isotopomer_theoretical) ))

  // case 1: calculating accuracy given theoretical and measured values

  IsotopeLabelingMDVs             isotopelabelingmdvs;
  OpenMS::Feature                 lactate_1_normalized;
  OpenMS::Feature                 lactate_1_with_accuracy_info;
  
  // L1_norm_max From CHO_190316_Flux.xlsx provided by Douglas McCloskey
  // accoa_C23H37N7O17P3S_MRM_theoretical_13, accoa_C23H37N7O17P3S_MRM_measured_13,
  // fad_C27H32N9O15P2_EPI_theoretical_48 & fad_C27H32N9O15P2_EPI_measured_48 are extracted from
  // "MID Max: LC–MS/MS Method for Measuring the Precursor and Product Mass Isotopomer Distributions
  // of Metabolic Intermediates and Cofactors for Metabolic Flux Analysis Applications, McCloskey et al",
  // DOI: 10.1021/acs.analchem.5b03887, Supporting Information: Table S-2
  std::vector<double>             L1_norm_max                             {1.00e+00, 3.324e-05, 2.825e-04, 7.174e-05};

  std::vector<double>             accoa_C23H37N7O17P3S_MRM_theoretical_13 {0.69, 0.202, 0.084, 0.019, 0.004, 0.001};
  std::vector<double>             accoa_C23H37N7O17P3S_MRM_measured_13    {0.627, 0.253, 0.096, 0.02, 0.004, 0.001};

  std::vector<double>             fad_C27H32N9O15P2_EPI_theoretical_48    {0.695, 0.233,  0.059,  0.011,  0.002, 0.0};
  std::vector<double>             fad_C27H32N9O15P2_EPI_measured_48       {0.638, 0.355, 0.1, 0.0, 0.0, 0.0};
  std::vector<double>             Average_accuracy_groundtruth            {0.02388, 0.0345}; // [accoa_13, fad_48]


  std::vector<OpenMS::Feature>    L1_subordinates_normmax;
  
  lactate_1_normalized.setMetaValue("PeptideRef", "Lactate1");
  for (uint16_t i = 0; i < L1_norm_max.size(); ++i)
  {
    OpenMS::Feature sub;
    sub.setMetaValue("native_id", "Lactate1_"+std::to_string(117+i));
    sub.setMetaValue("peak_apex_int", L1_norm_max[i]);
    L1_subordinates_normmax.push_back(sub);
  }
  lactate_1_normalized.setSubordinates(L1_subordinates_normmax);

  isotopelabelingmdvs.calculateMDVAccuracy(lactate_1_normalized, lactate_1_with_accuracy_info, accoa_C23H37N7O17P3S_MRM_measured_13, accoa_C23H37N7O17P3S_MRM_theoretical_13);
  TEST_REAL_SIMILAR( lactate_1_with_accuracy_info.getMetaValue("average_accuracy"), Average_accuracy_groundtruth[0] );
  lactate_1_with_accuracy_info.clearMetaInfo();

  isotopelabelingmdvs.calculateMDVAccuracy(lactate_1_normalized, lactate_1_with_accuracy_info, fad_C27H32N9O15P2_EPI_theoretical_48, fad_C27H32N9O15P2_EPI_measured_48);
  TEST_REAL_SIMILAR( lactate_1_with_accuracy_info.getMetaValue("average_accuracy"), Average_accuracy_groundtruth[1] );

END_SECTION

END_TEST
